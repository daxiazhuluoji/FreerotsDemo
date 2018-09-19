/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : AT24LC32.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:58
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef AT24C16_H
#define AT24C16_H

#include "stm32f10x.h"
#include "delay.h"
#include "stdbool.h"


#define SCL1_H           GPIO_SetBits(GPIOB,GPIO_Pin_10);
#define SCL1_L           GPIO_ResetBits(GPIOB,GPIO_Pin_10);
#define SDA1_H           GPIO_SetBits(GPIOB,GPIO_Pin_11);
#define SDA1_L           GPIO_ResetBits(GPIOB,GPIO_Pin_11);
#define SCL1_read        GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10) 
#define SDA1_read        GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)

#define AT24C32_I2C_ADDR         (0x50<<1)//0xA0
#define AT24C32_PGE_SZE	         32U
#define AT24C32_PGE_NUM	         128U
#define AT24C32_WRITE_PAGE_DLY() my_delay_ms(15)

#define AT24LC32_START_ADDR 0x0000
#define AT24LC32_END_ADDR   0x0FFF

extern void at24lc32Init(void);
extern void at24lc32EraseOperation(uint16_t start_addr,uint16_t end_addr);
extern void at24lc32WriteData(uint16_t WriteAddress,uint8_t* pBuffer,uint16_t DataLength,uint8_t DeviceAddress);
extern uint8_t at24lc32ReadData(uint16_t ReadAddress,uint8_t* pBuffer,uint16_t DataLength,uint8_t DeviceAddress);
extern bool at24lc32Test(void); 

#endif

