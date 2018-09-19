/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : debug.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:57
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "stdio.h"
#include "stdint.h"
#include "eprintf.h"

extern uint8_t logTaskInitFlag;

void myPrintTest(void);
void vTask_printf(char *format, ...);

#define DEBUG_OUT_EN
#define APP_LOG_EN
//#define PY_MONITOR_TERMINAL //use python idle teiminal

#ifdef DEBUG_OUT_EN
		#define DEBUG_OUT(format,...) if ( logTaskInitFlag ) {\
			vTask_printf(""format"",##__VA_ARGS__);}\
			else {\
			printf(""format"",##__VA_ARGS__);}
#else
	#define DEBUG_OUT(format,...)  
#endif

#ifdef APP_LOG_EN
#ifndef DEBUG_OUT_EN
#error hasn't define DEBUG_OUT_EN
#endif
    #define app_printf(format,...) DEBUG_OUT(""format"",##__VA_ARGS__)
#else
	#define app_printf(format,...)  
#endif

#ifdef PY_MONITOR_TERMINAL
#ifndef DEBUG_OUT_EN
#error hasn't define DEBUG_OUT_EN
#endif
	#define app_py_printf(format,...) DEBUG_OUT("log:"format"",##__VA_ARGS__)
#else
	#define app_py_printf(format,...)  
#endif
	
#endif


