/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : delay.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:57
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef __DELAY_H
#define __DELAY_H 			   
#include "stm32f10x.h"

extern uint32_t SystemCoreClock;  /*!< System Clock Frequency (Core Clock) */
extern void delay_4cycles(uint32_t cycles);

#define my_delay_ms(s) delay_4cycles((s) * (SystemCoreClock / 1000 / 8)) 		//1.13ms indeed
#define my_delay_us(s) delay_4cycles((s) * (SystemCoreClock / 1000 / 1000 / 8)) //about 113u

#endif

