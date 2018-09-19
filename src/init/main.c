/**
 * Copyright (C) 2018 
 * All Rights Reserved
 * 
 * Filename     : main.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-29 11:19
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
/* Library includes. */
#include "stm32f10x.h"

/* Standard includes. */
#include <stdio.h>
#include <stdarg.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* app includes. */
#include "platform.h"
#include "system.h"

int main(void)
{
	volatile unsigned int i;

	platformInit();
	systemLaunch();

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	   idle task. */
	return 0;
}


/*-----------------------------------------------------------*/

/* retarget. */
int fputc(int ch, FILE *f)
{      
#if 1
	while((USART2->SR&0X40)==0);   
	USART2->DR = (u8) ch;      
	return ch;
#else 
	xSerialPutChar(NULL,ch,portMAX_DELAY);
	return ch;
#endif
}
/*-----------------------------------------------------------*/

