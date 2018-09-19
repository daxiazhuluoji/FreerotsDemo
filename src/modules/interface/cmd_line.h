/**
 * Copyright (C) 2018 
 * All Rights Reserved
 * 
 * Filename     : cmd_line.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-03 10:40
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef _CMD_LINE_H_
#define _CMD_LINE_H_
// Include file for Command line interprenter



#ifndef NULL
#define NULL 0
#endif
#define CMD_TBL_LEN (sizeof (cmd_tbl) / sizeof (cmd_tbl [0]))
#define MAX_CMD_LEN 15
#define CMD_ARGU_NUM 4

#define printf_small printf

enum 
{
	CMD_LINE_TEST,
	ADDR_PAIR,
	SEND_CMD,
};

// Command structure
struct cmd_st
{
	char *st_cmdstr;
	unsigned char id;
};

extern const struct cmd_st cmd_tbl[];

extern unsigned char cmdid_search (char *, unsigned char);
extern char *strupr (char *);
extern void cmd_proc (char *);
extern void cmdb_init (void);
extern void cmdb_prompt (void);
extern char * cmdb_scan (void);
extern void cmdline_test (void);


#endif

