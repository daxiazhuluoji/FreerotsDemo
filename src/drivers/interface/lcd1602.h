/**
 * Copyright (C) 2018 daxiazhuluoji
 * All Rights Reserved
 * 
 * Filename     : sys_lcd1602.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-03 10:33
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */


#ifndef __SYS_LCD1602_H_
#define __SYS_LCD1602_H_

#include "stdint.h"
#include "stdbool.h"

//16*2 display, 5*7 dot array, 8 data interface
#define DISP_MODE 									0x38

//all the data pointer and the display on the screen are cleared
#define DISP_CLR										0x01

#define DISP_CURSOR_OFF							0x08

#define DISP_ON_CURSOR_OFF 					0x0c

//the data pointer is cleared
#define DP_CLR											0x02
#define DP_CURSOR_INCR_SCREEN_FIXED	0x06


#define LCD1602_EN_HIGH GPIO_SetBits(GPIOC, GPIO_Pin_14)
#define LCD1602_EN_LOW  GPIO_ResetBits(GPIOC, GPIO_Pin_14)

#define LCD1602_RD_OPT  GPIO_SetBits(GPIOC, GPIO_Pin_13)
#define LCD1602_WR_OPT  GPIO_ResetBits(GPIOC, GPIO_Pin_13)

#define LCD1602_DATA_REG GPIO_SetBits(GPIOB, GPIO_Pin_8)
#define LCD1602_CMD_REG  GPIO_ResetBits(GPIOB, GPIO_Pin_8)

#define LCD1602_DB0_SET   GPIO_WriteBit(GPIOC,GPIO_Pin_15, Bit_SET)
#define LCD1602_DB0_RESET GPIO_WriteBit(GPIOC,GPIO_Pin_15, Bit_RESET)

#define LCD1602_DB1_SET   GPIO_WriteBit(GPIOC,GPIO_Pin_0, Bit_SET)
#define LCD1602_DB1_RESET GPIO_WriteBit(GPIOC,GPIO_Pin_0, Bit_RESET)

#define LCD1602_DB2_SET   GPIO_WriteBit(GPIOC,GPIO_Pin_1, Bit_SET)
#define LCD1602_DB2_RESET GPIO_WriteBit(GPIOC,GPIO_Pin_1, Bit_RESET)

#define LCD1602_DB3_SET   GPIO_WriteBit(GPIOC,GPIO_Pin_2, Bit_SET)
#define LCD1602_DB3_RESET GPIO_WriteBit(GPIOC,GPIO_Pin_2, Bit_RESET)

#define LCD1602_DB4_SET   GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_SET)
#define LCD1602_DB4_RESET GPIO_WriteBit(GPIOC,GPIO_Pin_3, Bit_RESET)

#define LCD1602_DB5_SET   GPIO_WriteBit(GPIOA,GPIO_Pin_0, Bit_SET)
#define LCD1602_DB5_RESET GPIO_WriteBit(GPIOA,GPIO_Pin_0, Bit_RESET)

#define LCD1602_DB6_SET   GPIO_WriteBit(GPIOA,GPIO_Pin_1, Bit_SET)
#define LCD1602_DB6_RESET GPIO_WriteBit(GPIOA,GPIO_Pin_1, Bit_RESET)

#define LCD1602_DB7_SET   GPIO_WriteBit(GPIOB,GPIO_Pin_9, Bit_SET)
#define LCD1602_DB7_RESET GPIO_WriteBit(GPIOB,GPIO_Pin_9, Bit_RESET)

#define LCD1602_BUSY_FLAG 0x80

typedef enum
{
	Cmd_Reg = 0,
	Data_Reg
}LCD1602_RS_T;

void LCD1602Init(void);
bool LCD1602Test(void);
void LCD1602_Clr_Screen(void);

void LCD_printf(const uint8_t *pdata, uint8_t pos_row);
void Number_to_String(uint8_t* pString,uint32_t Number,uint8_t Disp_Format);

#endif

