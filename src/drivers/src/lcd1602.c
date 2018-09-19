/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : sys_lcd1602.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-03 10:37
 * Email        : dianjun.li@yeah.net
 * Description  : 
 * 
 */
 
 
/* Library includes. */
#include "stm32f10x.h"

/* Standard includes. */
#include "stdio.h"
#include "stdbool.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* app includes. */
#include "lcd1602.h"
#include "delay.h"
#include "led.h"
#include "debug.h"

static bool isInit = false;

QueueHandle_t xLcdQueue;

u8 changed_string[10]="0";
volatile uint8_t LCDWork = 0;

void  lcd1602_pin_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA , ENABLE);	 

	/*--------------------LCD1602 gpio init----------------------*/
	/*LCD1602_RS--PB8*/
	/*LCD1602_D7--PB9*/
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_R/W--PC13, EN--PC14, D0--PC15, D1-PC0, D2-PC1, D3-PC2, D4-PC3*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15|GPIO_Pin_0|\
								   GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_D5--PA0, D6-PA1*/
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*end*/
	/*------------------LCD1602 gpio init end---------------------*/

}
/*****************************************************
 * Fuction Name: void Set_STM32_To_LCD1602_Pin_Output(void)
 * Function Describe:
 ******************************************************/
void Set_STM32_To_LCD1602_Pin_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*LCD1602_D0--PC15, D1-PC0, D2-PC1, D3-PC2, D4-PC3*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_0|GPIO_Pin_1\
								   |GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_D5--PA0, D6-PA1*/
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_D7--PB9*/
	GPIO_InitStructure.GPIO_Pin= GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	/*end*/
}
/****************************************************
 * Fuction Name: void Set_STM32_To_LCD1602_Pin_Input(void)
 * Function Describe:
 *****************************************************/
void Set_STM32_To_LCD1602_Pin_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*LCD1602_D0--PC15, D1-PC0, D2-PC1, D3-PC2, D4-PC3*/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15|GPIO_Pin_0|GPIO_Pin_1\
								   |GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_D5--PA0, D6-PA1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/*end*/

	/*LCD1602_D7--PB9*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	/*end*/

}


/*********************************************
 * Fuction Name: void LCD1602_EN_High_Pulse(void)
 * Function Describe:  
 **********************************************/
void LCD1602_EN_High_Pulse(void)
{
	LCD1602_EN_LOW;

	/*delay  > 30ns*/

	LCD1602_EN_HIGH;

	/*delay > 150ns*/

	LCD1602_EN_LOW;

}


/***********************************************************
 * Fuction Name: u8 LCD1602_Read(LCD1602_RS_T reg_type)
 * Function Describe: Read the value of command reg and the data reg
 *************************************************************/
u8 LCD1602_Read(LCD1602_RS_T reg_type)
{
	u8 data = 0x00;
	u8 i;

	Set_STM32_To_LCD1602_Pin_Input();

	LCD1602_RD_OPT;

	if(Cmd_Reg == reg_type)
	{
		LCD1602_CMD_REG;
	}
	else
	{
		LCD1602_DATA_REG;
	}

	LCD1602_EN_HIGH;

	//Delay_ms(1);
	for(i = 0; i < 8; i++)
	{
		switch(i)
		{
			case 0:
				data |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15) << 0;
				break;

			case 1:
				data |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_0) << 1;
				break;

			case 2:
				data |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1) << 2;
				break;

			case 3:
				data |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) << 3;
				break;

			case 4:
				data |= GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) << 4;
				break;

			case 5:
				data |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) << 5;
				break;

			case 6:
				data |= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) << 6;
				break;

			case 7:
				data |= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) << 7;
				break;

			default:
				data = 0x00;
				break;
		}
	}

	LCD1602_EN_LOW;

	Set_STM32_To_LCD1602_Pin_Output();

	return data;

}

/*********************************************************
 * Fuction Name:   void LCD1602_Busy_Wait(void)
 * Function Describe: When you want to read or write data to LCD1602,
 *								   you must confirm LCD1602 is idle. 
 *								   LCD1602 DB7 = '1' indicates it is busy
 ***********************************************************/
void LCD1602_Busy_Wait(void)
{
	u8 data;
	while(1)
	{
		data = LCD1602_Read(Cmd_Reg);
		//app_printf("data %x\r\n", data);
		if(!(data & LCD1602_BUSY_FLAG))
		{
			break;
		}
	}
}


/************************************************************
 *Fuction Name: void LCD1602_Write(LCD1602_RS_T reg_type, u8 data)
 *Fuction Describe: reg_type : 0--write data to LCD1602 command reg
 *												 1--write data to LCD1602 data reg
 *							   data: the value you want to write
 *							   No return value.
 *************************************************************/
void LCD1602_Write(LCD1602_RS_T reg_type, u8 data)
{
	u8 i;

	LCD1602_WR_OPT;

	if(Cmd_Reg == reg_type)
	{
		LCD1602_CMD_REG;
	}
	else
	{
		LCD1602_DATA_REG;
	}

	//LCD1602_EN_LOW;//20150527
	//LCD1602_EN_HIGH;//20150527

	//Delay_ms(1);
	/*put the data on the LCD1602 data bus*/
	for(i = 0; i < 8; i++)
	{
		switch(i)
		{
			case 0:
				((data & 0x01) == 0x01) ? LCD1602_DB0_SET : LCD1602_DB0_RESET;
				break;

			case 1:
				((data & 0x01) == 0x01) ? LCD1602_DB1_SET : LCD1602_DB1_RESET;
				break;

			case 2:
				((data & 0x01) == 0x01) ? LCD1602_DB2_SET : LCD1602_DB2_RESET;
				break;

			case 3:
				((data & 0x01) == 0x01) ? LCD1602_DB3_SET : LCD1602_DB3_RESET;
				break;

			case 4:
				((data & 0x01) == 0x01) ? LCD1602_DB4_SET : LCD1602_DB4_RESET;
				break;

			case 5:
				((data & 0x01) == 0x01) ? LCD1602_DB5_SET : LCD1602_DB5_RESET;
				break;

			case 6:
				((data & 0x01) == 0x01) ? LCD1602_DB6_SET : LCD1602_DB6_RESET;
				break;

			case 7:
				((data & 0x01) == 0x01) ? LCD1602_DB7_SET : LCD1602_DB7_RESET;
				break;

			default:
				break;

		}

		data >>= 1;

	}
	/*end*/

	//LCD1602_EN_LOW;//20150527
	//Delay_ms(1);

	LCD1602_EN_High_Pulse();


}

/****************************************************************************
 * Fuction Name:   void LCD1602_Display(u8 *pdata, u8 pos_row, u8 pos_col, u8 len)
 * Function Describe: 'pdata' -- a pointer which aims at the data you want display on LCD1602
 *								   'pos_row' -- the range of the variable is 1~2, corresponding to 1602 two
 *														 display row. Row 1: 0x00~0x27; Row 2: 0x40~0x67
 *								   'pos_col' -- the display column of LCD1602, the range of the variable is 
 *														0x00~0x27
 *									'len' -- the length of the data you want to diaplay
 *****************************************************************************/
void LCD1602_Display(const u8 *pdata, u8 pos_row, u8 pos_col, u8 len)
{
	u8 i;
	const u8 *pdata_tmp = pdata;

	//app_printf("Enter LCD1602_Display\r\n");

	if(1 == pos_row)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Cmd_Reg, 0x80 + pos_col);
	}
	else if(2 == pos_row)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Cmd_Reg, 0x80 + 0x40 + pos_col);
	}
	else
	{;}

	for(i = 0; i < len; i++)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Data_Reg, *pdata_tmp++);

	}

	//app_printf("OUT LCD1602_Display\r\n");

}

/****************************************************************************

 *****************************************************************************/

//void LCD_printf(const u8 *pdata, u8 pos_row)
void LCD_printf(const u8 *pdata, u8 pos_row)
{
	u8 i;
	//	u8 len;
	u8 Date;
	u8 Col_offset;
	u8 Disp_Buffer[16]={
		' ',' ',' ',' ',
		' ',' ',' ',' ',
		' ',' ',' ',' ',
		' ',' ',' ',' '};

	const u8 *pdata_tmp = pdata;

	if (1 != LCDWork)
	{
		return;	
	}

#if 1 //居中模式
	for(i=0;i<16;i++)
	{
		if(*(pdata_tmp+i) =='\0')// End of String
		{
			break;
		}
	}

	Col_offset = (16-i)/2;//居中对齐控制

	//Copy to buffer
	for(i=Col_offset;i<16;i++)
	{
		Date=*pdata_tmp++;
		if(Date == '\0')break;
		Disp_Buffer[i]=Date;
	}

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, 0x80 + 0x40*(pos_row-1));
	for(i = 0; i<16; i++)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Data_Reg, Disp_Buffer[i]);
	}

#else

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, 0x80 + 0x40*(pos_row-1));

	for(i = 16; i >0; i--)
	{
		LCD1602_Busy_Wait();
		Date=*pdata_tmp++;
		if(Date == '\0')break;
		LCD1602_Write(Data_Reg, Date);
	}

	while(i)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Data_Reg, ' ');
		i--;
	}
#endif

}

/****************************************************************************
 * Fuction Name:   void LCD1602_Display(u8 *pdata, u8 pos_row, u8 pos_col, u8 len)
 * Function Describe: 'pdata' -- a pointer which aims at the data you want display on LCD1602
 *								   'pos_row' -- the range of the variable is 1~2, corresponding to 1602 two
 *														 display row. Row 1: 0x00~0x27; Row 2: 0x40~0x67
 *								   'pos_col' -- the display column of LCD1602, the range of the variable is 
 *														0x00~0x27
 *									'len' -- the length of the data you want to diaplay
 *****************************************************************************/
void LCD1602_Display_uChar(u8 *pdata, u8 pos_row, u8 pos_col, u8 len)
{
	u8 i;
	const u8 *pdata_tmp = pdata;

	//app_printf("Enter LCD1602_Display\r\n");  

	if(1 == pos_row)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Cmd_Reg, 0x80 + pos_col);
	}
	else if(2 == pos_row)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Cmd_Reg, 0x80 + 0x40 + pos_col);
	}
	else
	{;}

	for(i = 0; i < len; i++)
	{
		LCD1602_Busy_Wait();
		LCD1602_Write(Data_Reg, *pdata_tmp++);

	}

	//app_printf("OUT LCD1602_Display\r\n");

}
/******************************************
 * Fuction Name:   void LCD1602_Clr_Screen(void)
 * Function Describe:
 *******************************************/
void LCD1602_Clr_Screen(void)
{
	if (1 != LCDWork)
	{
		return;
	}

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DISP_CLR);
}

uint8_t isLCDWork(void)
{
	u8 data;
	volatile uint8_t counter = 0;

	lcd1602_pin_init();

	while(1)
	{
		data = LCD1602_Read(Cmd_Reg);
		if(!(data & LCD1602_BUSY_FLAG))
		{
			app_printf("has lcd:%d\r\n",counter);
			return 1;
		}

		if (counter >= 10)
		{
			app_printf("no lcd\r\n");
			return 0;
		}
		my_delay_ms(10);
		counter++;
	}
}


/************************************
 * Fuction Name:   void LCD1602Init(void)
 * Function Describe:
 *************************************/
void LCD1602Init(void)
{
	if (!isLCDWork())
		return;

	LCDWork = 1;

	LCD1602_EN_LOW;
	LCD1602_RD_OPT;//R/W = 1
	LCD1602_CMD_REG;//RS = 0

	/*No need to check whether LCD1602 busy or not*/
	my_delay_ms(15);
	LCD1602_Write(Cmd_Reg, DISP_MODE);
	my_delay_ms(5);
	LCD1602_Write(Cmd_Reg, DISP_MODE);
	my_delay_ms(5);
	LCD1602_Write(Cmd_Reg, DISP_MODE);
	my_delay_ms(5);
	/*end*/

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DISP_MODE);

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DISP_CURSOR_OFF);

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DISP_CLR);

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DP_CURSOR_INCR_SCREEN_FIXED);

	LCD1602_Busy_Wait();
	LCD1602_Write(Cmd_Reg, DISP_ON_CURSOR_OFF);

	LCD1602_Clr_Screen();
	LCD_printf("FreertosDemo",1);	
	LCD_printf("v0.1",2);	

	isInit = true;
}

bool LCD1602Test(void)
{
	return isInit;
}

void NUM_CHANGE_STRING(u32 roll_num_change)  // 将32位的数转换成字符串
{  
	u8 i=0;
	u8 judge;

	changed_string[0]=(roll_num_change&0xf0000000)>>28;
	changed_string[1]=(roll_num_change&0x0f000000)>>24;
	changed_string[2]=(roll_num_change&0x00f00000)>>20;
	changed_string[3]=(roll_num_change&0x000f0000)>>16;
	changed_string[4]=(roll_num_change&0x0000f000)>>12;
	changed_string[5]=(roll_num_change&0x00000f00)>>8;
	changed_string[6]=(roll_num_change&0x000000f0)>>4;
	changed_string[7]=(roll_num_change&0x0000000f);
	for(i=0;i<8;i++)
	{
		if(changed_string[i]>=10)
		{
			judge=changed_string[i]-10;
			switch(judge)
			{
				case 0: changed_string[i]='A';break;
				case 1: changed_string[i]='B';break;
				case 2: changed_string[i]='C';break;
				case 3: changed_string[i]='D';break;
				case 4: changed_string[i]='E';break;
				case 5: changed_string[i]='F';break;
				default: 	                    break;
			}
		}
		else
		{
			judge=changed_string[i];
			switch(judge)
			{
				case 0: changed_string[i]='0';break;
				case 1: changed_string[i]='1';break;
				case 2: changed_string[i]='2';break;
				case 3: changed_string[i]='3';break;
				case 4: changed_string[i]='4';break;
				case 5: changed_string[i]='5';break;
				case 6: changed_string[i]='6';break;
				case 7: changed_string[i]='7';break;
				case 8: changed_string[i]='8';break;
				case 9: changed_string[i]='9';break;
				default:                      break;
			}
		}
	}
}


void Number_to_String(u8* pString,u32 Number,u8 Disp_Format)
{
	//u8 Digital;
	u8 i;
	//static u32 Divider;
	//static u32 Number_D;
	//Divider =10;
	//Number_D=Number;
	if((Disp_Format == 'H')||(Disp_Format == 'h'))
	{

		pString[0]=(Number&0xf0000000)>>28;
		pString[1]=(Number&0x0f000000)>>24;
		pString[2]=(Number&0x00f00000)>>20;
		pString[3]=(Number&0x000f0000)>>16;
		pString[4]=(Number&0x0000f000)>>12;
		pString[5]=(Number&0x00000f00)>>8;
		pString[6]=(Number&0x000000f0)>>4;
		pString[7]=(Number&0x0000000f);

		for(i=0;i<8;i++)
		{
			if(pString[i]<10)
			{
				pString[i]+='0';
			}
			else
			{
				pString[i]=(pString[i]-10)+'A';
			}
		}

	}

#if 0
	if((Disp_Format == 'D')||(Disp_Format == 'd'))
	{
		for(i=0;i<16;i++)
		{
			if(Divider>Number_D)break;
			Divider *=10;
		}

		for(i=0;i<16;i++)
		{
			Divider /=10;
			pString[i]=(u8)(Number_D/Divider)+'0';
			Number_D -= Divider;
			if(Divider==1)break;
		}
	}


#endif
}
//

void Num2Decmial_String(u32 dat)
{
	u32 dat_temp=dat;
	u8 loop;

	for(loop=0;loop<10;loop++)
	{
		changed_string[9-loop]=(dat_temp%10)+0x30;
		dat_temp=dat_temp/10;
	}

}

void vLcdPrintTask (void *pvParameters)
{
	while(1)
	{
		vTaskDelay(500);
		//app_printf("vLcdPrintTask\r\n");
	}
}

