/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : key.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:59
 * Email        : lidianjun@yeah.net
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
#include "debug.h"
#include "key.h"
#include "delay.h"
#include "AppFreeRTOSConfig.h"
#include "system.h"

static bool isInit = false;

QueueHandle_t xKeyValueQueue;
void vKeyTask(void *pvParameters)
{	 
	static u8 All_Key_Release_Flag=1;
	Key_t key_value = KeyCode_NoKey;

	static portTickType xLastWakeTime;  
	const portTickType xFrequency = pdMS_TO_TICKS(10); //10ms 

	systemWaitStart();

	xLastWakeTime = xTaskGetTickCount();  
	
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime,xFrequency );  
		
		if((KEY_Program_Detect == 0)&&(All_Key_Release_Flag==1))
		{
			vTaskDelay(10);//ȥ���� 
			if(KEY_Program_Detect == 0)
			{
				All_Key_Release_Flag=0;
				key_value = KeyCode_Program;
				//app_printf("KeyCode_Program\r\n");
				//return KeyCode_Program;
			}
		}

		if((KEY_Down_Detect == 0)&&(All_Key_Release_Flag==1))
		{
			vTaskDelay(10);//ȥ���� 
			if(KEY_Down_Detect == 0)
			{
				All_Key_Release_Flag=0;
				key_value = KeyCode_Down;
				//app_printf("KeyCode_Down\r\n");
				//return KeyCode_Down;
			}
		}

		if((KEY_Up_Detect==0)&&(All_Key_Release_Flag==1))
		{
			vTaskDelay(10);//ȥ���� 
			if(KEY_Up_Detect==0)
			{
				All_Key_Release_Flag=0;
				key_value = KeyCode_Up;
				//app_printf("KeyCode_Up\r\n");
				//return KeyCode_Up;
			}
		}

		if((KEY_Down_Detect)&&(KEY_Up_Detect)&&(KEY_Program_Detect))//3���������ͷ�,�ж�Ϊ��������
		{
			All_Key_Release_Flag=1;
			//return KeyCode_NoKey;
		}
		else
		{
			//return KeyCode_PressHold;
		}
		
		if (KeyCode_NoKey != key_value)  
		{
			xQueueSend(xKeyValueQueue,
					(void *) &key_value,
					portMAX_DELAY); 
			key_value = KeyCode_NoKey;
		}
	}
}

void keyInit(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//��ʹ��JTAG�ӿڣ��ͷ� PB3  ʹ��SWD���Խӿ�

	//GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_1|GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;

	GPIO_Init(GPIOB, &GPIO_InitStructure);	 

	xKeyValueQueue = xQueueCreate(10,sizeof(Key_t)); 

	xTaskCreate( vKeyTask, 
			appConfigKEY_TASK_NAME, 
			appConfigKeySTACK_SIZE, 
			NULL, 
			appConfigKEY_TASK_PRIORITY, 
			( TaskHandle_t * ) NULL );

	isInit = true;
}

bool keyTest(void)
{
	return isInit;
}

Key_t getKeyValue(TickType_t xBlockTime)
{
	Key_t keyValueTemp = KeyCode_NoKey;
	xQueueReceive(xKeyValueQueue,         
				(void *)&keyValueTemp,
				xBlockTime);

	return keyValueTemp;
}

Key_t getKeyValueMaxWait(void)
{
	Key_t keyValueTemp = KeyCode_NoKey;
	xQueueReceive(xKeyValueQueue,         
				(void *)&keyValueTemp,
				portMAX_DELAY);

	return keyValueTemp;
}

