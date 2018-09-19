/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : led.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:59
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
/* Library includes. */
#include "stm32f10x.h"
#include "stdbool.h"

/* app includes. */
#include "led.h"
#include "delay.h"

static bool isInit = false;

static GPIO_TypeDef* led_port[] = {
  [LED_BLUE] = LED_GPIO_PORT, 
  [LED_GREEN] = LED_GPIO_PORT, 
  [LED_RED] = LED_GPIO_PORT,
};

static unsigned int led_pin[] = {
  [LED_BLUE] = LED_GPIO_BLUE, 
  [LED_GREEN] = LED_GPIO_GREEN, 
  [LED_RED]   = LED_GPIO_RED,
};

static int led_polarity[] = {
  [LED_BLUE] = LED_POL_BLUE, 
  [LED_GREEN] = LED_POL_GREEN, 
  [LED_RED] = LED_POL_RED,
};

void ledInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	if (isInit)
		return;

    // Enable GPIO
	RCC_APB2PeriphClockCmd(LED_GPIO_PERIF, ENABLE);	 

	//Initialize the LED pins as an output
	GPIO_InitStructure.GPIO_Pin = \
		LED_GPIO_BLUE | LED_GPIO_GREEN | LED_GPIO_RED ;	 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);

    //Turn off the LED:s
	ledClearAll();

	isInit = true;
}

bool ledTest(void)
{
	return isInit;
}

void ledClearAll(void)
{
  int i;

  for (i = 0; i < LED_NUM; i++)
  {
    //Turn off the LED:s
    ledSet((led_t)i, 0);
  }
}


void ledSetAll(void)
{
  int i;

  for (i = 0; i < LED_NUM; i++)
  {
    //Turn on the LED:s
    ledSet((led_t)i, 1);
  }
}

void ledSet(led_t led, bool value)
{
  if (led > LED_NUM)
    return;

  if (led_polarity[led] == LED_POL_NEG)
    value = !value;
  
  if(value)
    GPIO_SetBits(led_port[led], led_pin[led]);
  else
    GPIO_ResetBits(led_port[led], led_pin[led]); 
}



