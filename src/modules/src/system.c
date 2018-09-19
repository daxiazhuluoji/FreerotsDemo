/**
 * Copyright (C) 2018 
 * All Rights Reserved
 * 
 * Filename     : system.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-04 09:16
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */


#define DEBUG_MODULE "SYS"

#include <stdbool.h>

/* FreeRtos includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "debug.h"

#include "system.h"
#include "platform.h"
#include "AppFreeRTOSConfig.h"
#include "led.h"
//#include "uart1.h"
#include "uart2.h"
#include "key.h"
#include "AppFreeRTOSConfig.h"
#include "lcd1602.h"
#include "at24lc32.h"

/* Private variable */
static bool isInit = false;

/* System wide synchronisation */
xSemaphoreHandle canStartMutex;

/* Private functions */
static void systemTask(void *arg);

volatile uint32_t longulHighFrequencyTimerTicks; 

void TIM3_Int_Init(uint16_t arr,uint16_t psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStucture;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseStucture.TIM_Period = arr;
	TIM_TimeBaseStucture.TIM_Prescaler = psc;
	TIM_TimeBaseStucture.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStucture.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStucture);

	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_Cmd(TIM3,ENABLE);
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{
		longulHighFrequencyTimerTicks ++;
	}
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
}

void configureTimerForRunTimeStats(void)
{
	longulHighFrequencyTimerTicks = 0;
	/*
	 *longulHighFrequencyTimerTicks must be set 10~20 times
	 *to SysTick,then set TIMER3 clock 1MHz,and set counter
	 *50 to make longulHighFrequencyTimerTicks 20 KHz as 50us period.
	 */
	TIM3_Int_Init(50-1,72-1);
}

/* Public functions */
void systemLaunch(void)
{
	xTaskCreate(systemTask, 
			appConfigSYSTEM_TASK_NAME,
			appConfigSYSTEM_TASK_STACKSIZE, 
			NULL,
			appConfigSYSTEM_TASK_PRI, NULL);
}

// This must be the first module to be initialized!
void systemInit(void)
{
	if(isInit)
		return;

	canStartMutex = xSemaphoreCreateMutex();
	xSemaphoreTake(canStartMutex, portMAX_DELAY);

	ledInit();
	//uart1Init(115200);
	uart2Init(115200);
	keyInit();
	LCD1602Init();
	at24lc32Init();

	isInit = true;
}

bool systemTest(void)
{
	bool pass = isInit;

	pass &= ledTest();
	pass &= uart2Test();
	pass &= keyTest();
	pass &= LCD1602Test();
	pass &= at24lc32Test();

	return pass;
}

static uint8_t pcWriteBuffer[500] = {0};
/* Private functions implementation */
void systemTask(void *arg)
{
	bool pass = true;

#if (appConfigGET_RUN_STA == 1)
	static uint32_t i;
	static uint8_t runStaSwitch = 0;
#endif
	const portTickType xFrequency = 1000;  
	static portTickType xLastWakeTime;  
	xLastWakeTime = xTaskGetTickCount();  

	//Init the high-levels modules
	systemInit();
	pass &= systemTest();

	//Start the firmware
	if(pass)
	{
		systemStart();
		app_printf("systemStart\r\n");	
	}

	for (;;)
	{
		vTaskDelayUntil( &xLastWakeTime,xFrequency );  
		
		app_printf("t:%02d\r\n",i++);	
		runStaSwitch ^= 0x01;
#if (appConfigSYS_STA_LED_ON == 1)
		ledSet(LED_GREEN,runStaSwitch);
#endif
		if (0 == runStaSwitch)
		{
#if (appConfigGET_RUN_STA == 1)
			app_printf("name\t\tsta\tPrio\tStack\tNum\r\n");
			vTaskList((char *)&pcWriteBuffer);
			app_printf("%s\r\n", pcWriteBuffer);
#endif
		}
		else
		{
#if (appConfigGET_RUN_STA == 1)
			app_printf("name\t\truntime\t\tpercentage\r\n");
			vTaskGetRunTimeStats((char *)&pcWriteBuffer);
			app_printf("%s\r\n", pcWriteBuffer);
#endif
		}
	}
}


/* Global system variables */
void systemStart(void)
{
	xSemaphoreGive(canStartMutex);
#if 0
#ifndef DEBUG
	watchdogInit();
#endif
#endif
}

void systemWaitStart(void)
{
	//This permits to guarantee that the system task is initialized before other
	//tasks waits for the start event.
	while(!isInit)
		vTaskDelay(2);

	xSemaphoreTake(canStartMutex, portMAX_DELAY);
	xSemaphoreGive(canStartMutex);
}

void vApplicationIdleHook( void )
{
#if 0
	static uint32_t tickOfLatestWatchdogReset = M2T(0);

	portTickType tickCount = xTaskGetTickCount();

	if (tickCount - tickOfLatestWatchdogReset > M2T(WATCHDOG_RESET_PERIOD_MS))
	{
		tickOfLatestWatchdogReset = tickCount;
		watchdogReset();
	}

	// Enter sleep mode. Does not work when debugging chip with SWD.
	// Currently saves about 20mA STM32F405 current consumption (~30%).
#ifndef DEBUG
	{ __asm volatile ("wfi"); }
#endif
#endif
}

