/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : delay.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:57
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

/* app includes. */
#include "delay.h"
#include "debug.h"
#include "AppFreeRTOSConfig.h"

void delay_4cycles(uint32_t cycles)
{
  uint32_t nloop;

  nloop = cycles + 1; 
  while (--nloop > 0)
    __NOP();
};

