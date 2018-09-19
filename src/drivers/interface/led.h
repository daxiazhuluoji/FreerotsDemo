/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : led.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:59
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
#ifndef __LED_H__
#define __LED_H__

#include <stdbool.h>

//Led polarity configuration constant
#define LED_POL_POS 0
#define LED_POL_NEG 1

//Hardware configuration
#define LED_GPIO_PERIF   ( RCC_APB2Periph_GPIOB )

#define LED_GPIO_PORT    GPIOB
//blue
#define LED_GPIO_BLUE  GPIO_Pin_7
#define LED_POL_BLUE   LED_POL_POS
//green
#define LED_GPIO_GREEN GPIO_Pin_6
#define LED_POL_GREEN  LED_POL_POS
//red
#define LED_GPIO_RED   GPIO_Pin_5
#define LED_POL_RED    LED_POL_POS

#define LINK_LED         LED_GREEN_L
#define CHG_LED          LED_BLUE_L
#define LOWBAT_LED       LED_RED_R
#define LINK_DOWN_LED    LED_RED_L
#define SYS_LED          LED_RED_R
#define ERR_LED1         LED_RED_L
#define ERR_LED2         LED_RED_R

#define LED_NUM 3

typedef enum {
	LED_BLUE = 0, 
	LED_GREEN, 
	LED_RED, 
	}led_t;

void ledInit(void);
bool ledTest(void);

// Clear all configured LEDs
void ledClearAll(void);

// Set all configured LEDs
void ledSetAll(void);

// Procedures to set the status of the LEDs
void ledSet(led_t led, bool value);

void ledTask(void *param);

//Legacy functions
#define ledSetRed(VALUE) ledSet(LED_RED, VALUE)
#define ledSetGreen(VALUE) ledSet(LED_GREEN, VALUE)

#endif 

