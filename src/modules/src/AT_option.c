/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : AT_option.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-03 10:34
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
/* Library includes. */
#include "stm32f10x.h"

/* Standard includes. */
#include "stdio.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* app includes. */
#include "app_fifo.h"
#include "serial.h"
#include "debug.h"
#include "string.h"
#include "AT_option.h"


extern SemaphoreHandle_t xUartInSemaphore; 
extern app_fifo_t uart_com_queue_rx;


uint8_t getAtStr(uint8_t* AtStrTemp)
{
	uint8_t AT_rsp_len = 0;

	while(1)
	{
		app_fifo_out(&uart_com_queue_rx,
				&AtStrTemp[AT_rsp_len],
				sizeof(uint8_t));

		AT_rsp_len ++;
		if ((AT_rsp_len >= UART_RX_QUEUE_LEN)
				||(('\r' == AtStrTemp[AT_rsp_len - 2])
					&&('\n' == AtStrTemp[AT_rsp_len - 1])))
		{
			break;
		}
	}

	return AT_rsp_len; 
}

BaseType_t ATOption(BtCDPinSta_t BTCDPinSta,
		signed char* sendATCmd,
		signed char* expectRsp,
		TickType_t xTicksToWaitATRsp)
{
	BaseType_t xResult = pdFAIL;
	uint8_t AT_cmd_buffer_temp[UART_RX_QUEUE_LEN] = {0};
	volatile uint8_t AT_str_len = 0;

	if (BTCDLOW == BTCDPinSta)
	{
		BTCD_LOW;
	}
	else
	{
		BTCD_HIGH;
	}

    vTaskDelay(5);

	ATSerialPutStr(sendATCmd);
	if (pdTRUE == xSemaphoreTake(xUartInSemaphore,xTicksToWaitATRsp)) 
	{
		AT_str_len = getAtStr(AT_cmd_buffer_temp);
		//app_printf("AT_str_len:%d\r\n",AT_str_len);
		if (AT_str_len >= 4)
		{
			app_printf("AT<-:%s",(char *)AT_cmd_buffer_temp);//there is "\r\n" in cmd str 
				
			if (('\r' == AT_cmd_buffer_temp[AT_str_len - 2])
					&&('\n' == AT_cmd_buffer_temp[AT_str_len - 1]))
			{
				if (0 == strcmp((const char*)AT_cmd_buffer_temp,(const char*)expectRsp))
				{
					xResult = pdPASS;
					//app_printf("AT get rsp\r\n");
				}
				else if (0 == strcmp((const char*)AT_cmd_buffer_temp,ATSTR_ERROR))
				{
					xResult = pdFAIL;
					app_printf("AT cmd error\r\n");
				}
				else
				{
					xResult = pdFAIL;
					app_printf("unexpeced tRsp\r\n");
				}		
			}
		}
		else
		{
			xResult = pdFAIL;
			app_printf("AT get AT rsp str none\r\n");
		}
	}
	else
	{
		app_printf("AT get semaphore fail\r\n");
		xResult = pdFAIL;
	}

	BTCD_HIGH;
	vTaskDelay(10);
	return xResult;
}

