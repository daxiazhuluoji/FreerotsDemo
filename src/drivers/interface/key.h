/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : key.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:59
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef __KEY_H
#define __KEY_H	 

#include "stm32f10x.h"

typedef enum{
	KeyCode_NoKey=0x00,
	KeyCode_Program=0x01,
	KeyCode_Down=0x02,
	KeyCode_Up=0x03,
	KeyCode_PressHold=0x04,
}Key_t;

#define KEY_Up_Detect   1//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4)	 
#define KEY_Down_Detect   1//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)	 
#define KEY_Program_Detect    GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0)	

void keyInit(void);
bool keyTest(void);
Key_t getKeyValue(TickType_t xBlockTime);
Key_t getKeyValueMaxWait(void);

#endif

