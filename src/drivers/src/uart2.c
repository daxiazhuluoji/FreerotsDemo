/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : uart2.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:53
 * Email        : lidianjun@yeah.net
 * Description  : 
 *        The print performance of DMA print is CHAR_PRINT_PER_MS Bytes per micro second. 
 *                                                   -----
 *           \                                           |
 *            \fifo_in                              vTask_printf()    
 *            _\|                                        |
 *        printFifoBuf(PRINT_FIFO_QUEUE_LEN)             |
 *        (printFifoQueue)                               |
 *               \                                   -----
 *                \fifo_out                              |
 *                _\|                                    |
 *              logTempBuffer(DMA_PRINT_ONE_TIME_LEN)    |
 *              (DMAPrint source addr)                vLogTask() 
 *                 /                                     |
 *                /printf/DMAPrint                       |
 *              |/_                                      |
 *          uartport                                 -----
 */
 

/* Library includes. */
#include "stm32f10x.h"

/* Standard includes. */
#include <stdio.h>
#include <stdarg.h>
#include "stdint.h"
#include "string.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* app includes. */
#include "app_fifo.h"
#include "delay.h"
#include "debug.h"
#include "AppFreeRTOSConfig.h"
#include "eprintf.h"
#include "uart2.h"

/*total str len for task print if log task has't be scheduled*/
#define PRINT_FIFO_QUEUE_LEN 512 
static uint8_t printFifoBuf[PRINT_FIFO_QUEUE_LEN] = {0};
/*str len for DMA print one time*/
#define DMA_PRINT_ONE_TIME_LEN 32
static uint8_t logTempBuffer[DMA_PRINT_ONE_TIME_LEN + 1] = {0};
uint32_t logTempIndex = 0;
/*
 * about 10 Bytes can be print per micro second 
 * at 115200 bps using USART in theory,but slowly than 10 Bps indeed.
 * */
#define CHAR_PRINT_PER_MS 4

static app_fifo_t printFifoQueue;
uint8_t logTaskInitFlag = 0;

static bool isInit = false;

SemaphoreHandle_t xPritfSemaphore; 


static void DMAPrint(DMA_Channel_TypeDef*DMA_CHx,uint32_t logLen)
{
	DMA_CHx->CCR&=~(1<<0); //disable DMA
	DMA_CHx->CNDTR=logLen; //set data len
	DMA_CHx->CCR|=1<<0;    //enable DMA
}

int putcToPrintFifo(int c)
{
	uint32_t len = 0;

	len = app_fifo_avail(&printFifoQueue);
	//app_printf("fifo_in avail len:%d\r\n",len);
	if (len > 0)
	{
		app_fifo_in(&printFifoQueue,(const uint8_t*)&c,1);
	}
	else
	{
		/*some print data will be lost*/
		printf("printfifo full!!!\r\n");	
	}
	return 1;
}

void vTask_printf(char *format, ...)
{
	va_list ap;

	//vTaskSuspendAll();
	xSemaphoreTake(xPritfSemaphore,portMAX_DELAY); 
	va_start(ap, format);
	evprintf(putcToPrintFifo, format, ap);
	va_end(ap);
	xSemaphoreGive(xPritfSemaphore); 
	//xTaskResumeAll();
}

void vLogTask (void *pvParameters)
{
	uint32_t len;
	static portTickType xLastWakeTime;  
	const portTickType xFrequency = DMA_PRINT_ONE_TIME_LEN / CHAR_PRINT_PER_MS;  

	xLastWakeTime = xTaskGetTickCount();  
	logTaskInitFlag = 1;
	for (;;)
	{
		vTaskDelayUntil( &xLastWakeTime,xFrequency );  
		logTempIndex = 0;

		xSemaphoreTake(xPritfSemaphore,portMAX_DELAY); 
		while(1)
		{
			len = app_fifo_out(&printFifoQueue,
					(uint8_t*)(&logTempBuffer[logTempIndex]),
					sizeof(uint8_t));

			if (len <= 0)
			{
				//read all char from printf fifo
				break;
			}
			logTempIndex ++;
			if (logTempIndex >= DMA_PRINT_ONE_TIME_LEN)
			{
				//more than DMA can print one time
				break;
			}
		}

		if (logTempIndex > 0)
		{
			len = app_fifo_avail(&printFifoQueue);
			//printf("fifi_out avail len:%d\r\n",len);
			//printf("TempIndex:%d\r\n",logTempIndex);
#if (appConfigUART_USE_DMA == 0)
			/*add '\0' at the and of the string avoid extra print.*/
			logTempBuffer[logTempIndex] = '\0';
			printf("%s",(char*)logTempBuffer);
			memset(logTempBuffer,0,sizeof(logTempBuffer));
#else
			DMAPrint(DMA1_Channel7,logTempIndex);
#endif
		}
		xSemaphoreGive(xPritfSemaphore); 
	}
}

/*-----------------------------------------------------------*/
void uart2Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef appConfigUART_USE_DMA
	DMA_InitTypeDef DMA_InitStructure;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
#ifdef appConfigUART_USE_DMA
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE); 
	my_delay_ms(50); //for DMA ready,ldj 2016,07,20
#endif

	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//USART2_RX	  GPIOA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; //�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;         //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	 //�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); 
	/*if use simulate key for start burn,disable USART_IT_RXNE of USART2*/
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 

#ifdef appConfigUART_USE_DMA
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);   //ʹ�ܴ���DMA����
#endif
	USART_Cmd(USART2, ENABLE);                    

#ifdef appConfigUART_USE_DMA
	//��Ӧ��DMA����
	DMA_DeInit(DMA1_Channel7);                                    //����2 TX��Ӧ����DMAͨ��7
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR;  //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)logTempBuffer;    //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;            //���ݴ��䷽���ڴ浽����
	//DMA_InitStructure.DMA_BufferSize = sizeof(logBuffDMA);      //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_BufferSize = 0;                         //��ʼֵ����Ϊ0��ÿ��д��ǰ����������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //ͨ��������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;           //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = 8;                 //�������ݿ��Ϊ8λ
	DMA_InitStructure.DMA_MemoryDataSize = 8;                     //�ڴ����ݿ��Ϊ8λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                 //��������������ģʽ����ѭ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;         //DMAͨ�� xӵ�������ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��x����Ϊ���ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);  

	//DMA_Cmd(DMA1_Channel7, ENABLE);  //��ʽ����DMA����
#endif

	app_fifo_init(&printFifoQueue,\
			printFifoBuf,PRINT_FIFO_QUEUE_LEN);

	xPritfSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xPritfSemaphore); 


	xTaskCreate( vLogTask, 
			appConfigLOG_TASK_NAME, 
			appConfigLogSTACK_SIZE, 
			NULL, 
			appConfigLOG_TASK_PRIORITY, 
			( TaskHandle_t * ) NULL );

	isInit =  true;
}

bool uart2Test(void)
{
	return isInit;
}

void uart2SendData(uint32_t size, uint8_t* data)
{
	uint32_t i;

	if (!isInit)
		return;

	for(i = 0; i < size; i++)
	{
		while (!(UART2_TYPE->SR & USART_FLAG_TXE));
		UART2_TYPE->DR = (data[i] & 0x00FF);
	}
}

int uart2Putchar(int ch)
{
	uart2SendData(1, (uint8_t *)&ch);

	return (unsigned char)ch;
}

