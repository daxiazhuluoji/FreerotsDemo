//=========================================================================
//                                                                        =
// Program description      : Serial command line interprenter            =
//                            The program accepts commands with arguments =
//                            from the internal UART, and executes them.  =
//                                                                        =
//                            The original code was graciously shared by  =
//                            Jon Ward (jonw@keil.com) and written for    =
//                            Keil compiler.                              =
//                                                                        =
//                            All I did was port it to SDCC.              =
//                            I hope you find it useful!                  =
//                                                                        =
//                            Sten D. Iversen                             =
//                            sten.iversen@post.tele.dk                   =
//                                                                        =
//                            Legal Stuff:                                =
//                                                                        =
//                            This program is free software; you can      =
//                            redistribute it and/or modify itunder the   =
//                            terms of the GNU General Public License     =
//                            as published by the Free Software Foundation=
//                            either version 2, or (at your option) any   =
//                            later version.                              =
//                                                                        =
//                            This program is distributed in the hope that=
//                            it will be useful,                          =
//                            but WITHOUT ANY WARRANTY; without even the  =
//                            implied warranty of MERCHANTABILITY or      =
//                            FITNESS FOR A PARTICULAR PURPOSE.  See the  =
//                            GNU General Public License for more details.=
//                                                                        =
//                            You should have received a copy of the      =
//                            GNU General Public License along with this  =
//                            program; if not, write to the Free Software =
//                            Foundation, 59 Temple Place - Suite 330,    =
//                            Boston, MA 02111-1307, USA.                 =
//                                                                        =
//                            In other words, you are welcome to use,     =
//                            share and improve this program.             =
//                            You are forbidden to forbid anyone else     =
//                            to use, share and improve what you give them=
//                            Help stamp out software-hoarding!           =
//                                                                        =
// Compiler                 : SDCC                                        =
// Compiler version         : >= 2.2.2                                    =
// Compiler options         : --stack-after-data                          =
//                                                                        =
// Version / Release Date   : 1.0 / 2001-03-22                            =
// Version Programmer/Text  : SDI / Initial Version, the code is portet   =
//                                  to fit the SDCC compiler.             =
//=========================================================================

// Includes from standard lib.
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/* app includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "cmd_line.h"
#include "serial.h"
#include "debug.h"


/* The Tx task will transmit the sequence of characters at a pseudo random
interval.  This is the maximum and minimum block time between sends. */
#define cmdlineTX_MAX_BLOCK_TIME		( ( TickType_t ) 0x96 )
#define cmdlineTX_MIN_BLOCK_TIME		( ( TickType_t ) 0x32 )
#define cmdlineOFFSET_TIME				( ( TickType_t ) 3 )

/* We should find that each character can be queued for Tx immediately and we
don't have to block to send. */
#define cmdlineNO_BLOCK					( ( TickType_t ) 0 )

/* The Rx task will block on the Rx queue for a long period. */
#define cmdlineRX_BLOCK_TIME			( ( TickType_t ) 0xffff )


// Arrays and simple types
char cmdbuf[1 + MAX_CMD_LEN];                    // command buffer
unsigned char cmd_argu[CMD_ARGU_NUM];


// command index
unsigned char cmdndx;

const struct cmd_st cmd_tbl[] = 
{
	{"CLT",  CMD_LINE_TEST},
	{"PAIR", ADDR_PAIR},
	{"SEND", SEND_CMD},
};

extern void Address_Pair_Operation(unsigned char,unsigned char,unsigned char,unsigned char);
extern void Send_Cmd_Operation(unsigned char,unsigned char,unsigned char);
extern void Get_Status_Operation(unsigned char,unsigned char,unsigned char);


//  This is a support function for the printf_small function
#if 0
char putchar(char c)
{
	serial_putchar(c);
}
#endif

//  This function searches the cmd_tbl for a specific command and returns the 
//  ID associated with that command or CID_LAST if theres no matching command.
unsigned char cmdid_search (char *cmdstr, unsigned char len)
{
	struct cmd_st const *ctp;

	for (ctp = cmd_tbl; ctp < &cmd_tbl[CMD_TBL_LEN]; ctp++)
	{
		if (strncmp(ctp->st_cmdstr, cmdstr, len) == 0)
			return (ctp->id);
	}

	return -1;//(CID_LAST);
}


//  No programmer's library would be complete without strupr!
char *strupr (char *src)
{
	char *s;

	for (s = src; *s != '\0'; s++)
	{
		*s = toupper (*s);
	}

	return (src);
}

#if 0
unsigned char get_decimal(char *src)
{
	char *s;
	unsigned char val;

	val = 0;
	s = src;
	while (*s != '\0' && *s != ' ')
	{
		if (*s > '9' || *s < '0')
			return -1;
		else
			val = val * 10 + (*s -'0');
		s++;
	}
	return val;
}
#endif

//#ifdef HS6206_USER_INTERFACE
#if 1
unsigned char get_hexadecimal(char *src)
{
	char *s;
	unsigned char val;

	val = 0;
	s = src;

	while (*s != '\0' && *s != ' ')
	{
		if (*s <= '9' && *s >= '0')
			val = val * 0x10 + (*s -'0');
		else if (*s <= 'f' && *s >= 'a')
			val = val * 0x10 + 10 + (*s -'a');
		else if (*s <= 'F' && *s >= 'A')
			val = val * 0x10 + 10 +(*s -'A');
		else
			return -1;
		s++;
	}

	return val;
}
#endif

void init_cmd_argu(void)
{
	char i;
	for (i = 0; i < CMD_ARGU_NUM; i++)
		cmd_argu[i] = 0;
}

#if 0
void get_cmd_argu(char *argstr_buf)
{
	xdata char *argsep;
	unsigned char i = 0;

	argsep = argstr_buf;
	while (argsep != NULL)
	{
		while (*argsep == ' ') argsep++;
		cmd_argu[i++] = get_decimal(argsep);
		argsep = strchr (argsep, ' ');
	}
}
#endif

#if 1
void get_cmd_argu_hex(char *argstr_buf)
{
	char *argsep;
	unsigned char i = 0;

	argsep = argstr_buf;
	while (argsep != NULL)
	{
		while (*argsep == ' ') argsep++;
		cmd_argu[i++] = get_hexadecimal(argsep);
		argsep = strchr (argsep, ' ');
	}
}
#endif

void cmd_line_test_func(unsigned char cmd_argu0, unsigned char cmd_argu1, unsigned char cmd_argu2, unsigned char cmd_argu3)
{
	app_printf("param:%x %x %x %x\r\n",cmd_argu0,cmd_argu1,cmd_argu2,cmd_argu3);

}


#if 1
//  This function processes the cmd command.
void cmd_proc (char *cmd)
{
	char argstr_buf[1 + MAX_CMD_LEN];
	char *argsep;
	unsigned char id;

	strupr(cmd);

	//Next, find the end of the first thing in the
	//buffer. Since the command ends with a space,
	//we'll look for that. NULL-Terminate the command
	//and keep a pointer to the arguments.
	argsep = strchr(cmd, ' ');

	if (argsep == NULL)
	{
		argstr_buf[0] = '\0';
	}
	else
	{
		strcpy (argstr_buf, argsep + 1);
		*argsep = '\0';
	}

	//Search for a command ID, then switch on it.
	//you should invoke each function here.
	id = cmdid_search(cmd, argsep - cmd + 1);

	// clear command argument array
	init_cmd_argu();

	get_cmd_argu_hex(argstr_buf);


	switch (id)
	{
		case CMD_LINE_TEST:
			//cmd_line_test_func(cmd_argu[0], cmd_argu[1], cmd_argu[2], cmd_argu[3]);
			break;
		case ADDR_PAIR:
			//Address_Pair_Operation(cmd_argu[0], cmd_argu[1], cmd_argu[2], cmd_argu[3]);
			break;
		case SEND_CMD:
			//Send_Cmd_Operation(cmd_argu[0], cmd_argu[1], cmd_argu[2]);
			break;
		default : 
			printf_small("unknow command\r\n");
			break;
	} 
}
#endif

//Initialize the command buffer and index.
void cmdb_init (void)
{
	cmdndx = 0;
	cmdbuf [0] = '\0';
}


//Prompt for a command.
void cmdb_prompt (void)
{
	printf_small("CMD>");
}



/* The Rx task will block on the Rx queue for a long period. */
#define comRX_BLOCK_TIME			( ( TickType_t ) 0xffff )

//Build a command string. This functions gets characters from the serial 
//buffer and appends them to the end of the command buffer. If no character is 
//available the function returns NULL. If a newline character is read, a pointer to 
//the buffer is returned. Do NOT reinvoke this function before extracting the 
//command from the buffer.
char* cmdb_scan (void)
{
	const xComPortHandle xPort = NULL;
	char c;
	signed char  cByteRxed;

	while(1)
	{
		if(pdFALSE == xSerialGetChar( xPort, &cByteRxed, cmdlineRX_BLOCK_TIME ) )
		{
			break;                                          //Exit NOW
		}

		c = cByteRxed;

		if (c == '\r')                                    //Newline?
		{
			printf_small("\r\n");                         //Output it and
			return (cmdbuf);                              //Return the buffer address
		}

		if ((c == '\b') && (cmdndx != 0))                 //Backspace?
		{
			printf_small("\b \b");
			cmdbuf [--cmdndx] = '\0';
			continue;
		}

		if (!isprint (c))                                 //Printable character?
		{
			continue;
		}

		if (cmdndx >= MAX_CMD_LEN)                        //Past buffer length?
		{
			continue;
		}

		putchar(c);                           //Output char use this as a sort of echo
		cmdbuf[cmdndx++] = (unsigned char)c;             //Add to the buffer
		cmdbuf[cmdndx]   = '\0';                         //NULL-Terminate the buffer
	}

	return (NULL);
}


//The main function should look something like this.
void vcmdlineTask (void *pvParameters)
{
	char *cmd;
	
	while (1)
	{
		cmdb_init ();		                                    //init a new command instance
		cmdb_prompt ();		                                  //prompt for a command

		for (cmd = NULL; cmd == NULL; cmd = cmdb_scan ())	  //get command
		{
			//do something useful here e.g. watchdog.
		}

		cmd_proc(cmd);		                                  //process the command
	}
}

/*
 *
 *
 *
 *
 */
void vStartCmdLineTasks( UBaseType_t uxPriority, uint32_t ulBaudRate )
{
	/* Initialise the com port then spawn the Rx and Tx tasks. */
	xSerialPortInitMinimal( ulBaudRate, 30 );

	/* The Tx task is spawned with a lower priority than the Rx task. */
	xTaskCreate( vcmdlineTask, "CmdLine", cmdlineSTACK_SIZE, NULL, uxPriority, ( TaskHandle_t * ) NULL );
}


