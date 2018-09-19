/**
 * Copyright (C) 2018 daxiazhuluoji
 * All Rights Reserved
 * 
 * Filename     : AT24LC32.c
 * Author       : daxiazhuluoji
 * Last modified: 2018-08-16 15:58
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#include "stm32f10x.h"
#include "AT24LC32.h"
#include "stdio.h"
#include "debug.h"
#include "string.h"

static bool isInit = false;

void I2C1_delay(void) 
{ 
	//uint16_t i = 150;  //17K
	uint16_t i = 15;   //113K 
	//uint16_t i = 5;    //250K
	//uint16_t i = 2;      //333K
	//uint16_t i = 1;      //400~500K
	
	while (i --);
} 

uint8_t I2C1_Start(void) 
{ 
	SDA1_H; 
	SCL1_H; 
	I2C1_delay(); 
	if (!SDA1_read)
		return false; //SDA线为低电平则总线忙,退出 
	SDA1_L; 
	I2C1_delay(); 
	if (SDA1_read)
		return false; //SDA线为高电平则总线出错,退出 
	SDA1_L; 
	I2C1_delay(); 
	return true; 
} 

void I2C1_Stop(void) 
{ 
	SCL1_L; 
	I2C1_delay(); 
	SDA1_L; 
	I2C1_delay(); 
	SCL1_H; 
	I2C1_delay(); 
	SDA1_H; 
	I2C1_delay(); 
} 

void I2C1_Ack(void) 
{ 
	SCL1_L; 
	I2C1_delay(); 
	SDA1_L; 
	I2C1_delay(); 
	SCL1_H; 
	I2C1_delay(); 
	SCL1_L; 
	I2C1_delay(); 
} 

void I2C1_NoAck(void) 
{ 
	SCL1_L; 
	I2C1_delay(); 
	SDA1_H; 
	I2C1_delay(); 
	SCL1_H; 
	I2C1_delay(); 
	SCL1_L; 
	I2C1_delay(); 
}

uint8_t I2C1_WaitAck(void)  
{ 
	SCL1_L; 
	I2C1_delay(); 
	SDA1_H; 
	I2C1_delay(); 
	SCL1_H; 
	I2C1_delay(); 
	if (SDA1_read)
	{
		SCL1_L; 
		return false; 
	}
	SCL1_L; 
	return true; 
} 

void I2C1_SendByte(uint8_t SendByte)  
{ 
	uint8_t i=8; 
	while (i--)
	{
		SCL1_L; 
		I2C1_delay(); 
		if (SendByte&0x80)
			SDA1_H;
		if(!(SendByte&0x80))
			SDA1_L;    
		SendByte<<=1; 
		I2C1_delay(); 
		SCL1_H; 
		I2C1_delay(); 
	} 
	SCL1_L; 
} 

uint8_t I2C1_ReceiveByte(void)  
{  
	uint8_t i=8; 
	uint8_t ReceiveByte=0; 

	SDA1_H; 
	while (i--)
	{
		ReceiveByte<<=1;       
		SCL1_L; 
		I2C1_delay(); 
		SCL1_H; 
		I2C1_delay(); 
		if (SDA1_read)
		{
			ReceiveByte|=0x01; 
		}
	} 
	SCL1_L; 
	return ReceiveByte; 
} 

uint8_t I2C1_BufferWrite(uint8_t* pBuffer, uint16_t length,u16 WriteAddress, uint8_t DeviceAddress)
{
	if(!I2C1_Start())
	{
		return false;
	}

	//app_printf("I2C1_BufferWrite\r\n");
	I2C1_SendByte(DeviceAddress);    
	if(!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	I2C1_SendByte((WriteAddress>>8)&0x00FF);  //write addr high 8 bits
	if(!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	I2C1_SendByte((WriteAddress>>0)&0x00FF); //write addr low 8 bits
	if(!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	while(length--)
	{
		I2C1_SendByte(*pBuffer);
		if(!I2C1_WaitAck())
		{
			I2C1_Stop(); 
			return false;
		}
		pBuffer++;
	}
	I2C1_Stop();

	AT24C32_WRITE_PAGE_DLY();

	return true;
} 

void at24lc32Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//EEPROM SCL and SDA
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	isInit = true;
} 

void at24lc32EraseOperation(uint16_t start_addr,uint16_t end_addr)
{
	uint8_t  data_temp[AT24C32_PGE_SZE];
	uint8_t  page_start_location;
	uint8_t  page_remaining_len;
	uint16_t write_len;
	uint8_t loop=0;

	if((start_addr>end_addr)
			||(start_addr > AT24LC32_END_ADDR)
			||(end_addr > AT24LC32_END_ADDR))
	{
		//app_printf("Exception E\r\n");
		while(1);
	}

	memset(data_temp,0xFF,sizeof(data_temp));

	for(;start_addr<=end_addr;)
	{
		page_start_location = start_addr % AT24C32_PGE_SZE;
		write_len = end_addr + 1 - start_addr;

		if(0 == page_start_location)
		{
			if(write_len > AT24C32_PGE_SZE)
			{
				write_len = AT24C32_PGE_SZE;
			}
		}
		else
		{
			page_remaining_len = AT24C32_PGE_SZE - page_start_location;

			if(write_len > page_remaining_len)
			{
				write_len = page_remaining_len;
			}
		}

		if(true == I2C1_BufferWrite(data_temp,write_len,start_addr,AT24C32_I2C_ADDR))
		{
			start_addr += write_len;
		}
		else
		{
			loop++;
			if(loop>=10)
			{
				//exception
				app_printf("Exception F\r\n");
				while(1);
			}
		}
		//app_printf("start_addr=%x,end_addr=%x\r\n",start_addr,end_addr);
		//app_printf("write_len=%d\r\n",write_len);
	}
}

void at24lc32WriteData(uint16_t WriteAddress,
		uint8_t* pBuffer,u16 DataLength,uint8_t DeviceAddress)
{
	uint16_t dataWriteToFistPagaLen = 0;
	uint16_t hasNotWriteDataLen = DataLength;
	uint16_t currentWriteAddr = WriteAddress;
	uint16_t currentWriteDataLen = 0;
	uint8_t *pDataToWrite = pBuffer;
	
	if (0 == hasNotWriteDataLen)
	{
		//app_printf("hasNotWriteDataLen = 0\r\n");	
		return;
	}
	
	pDataToWrite = pBuffer;
	dataWriteToFistPagaLen = hasNotWriteDataLen > (AT24C32_PGE_SZE - (WriteAddress % AT24C32_PGE_SZE))?\
							 AT24C32_PGE_SZE - (WriteAddress % AT24C32_PGE_SZE):hasNotWriteDataLen;  

	//app_printf("currentWriteAddr:%x dataWriteToFistPagaLen:%d hasNotWriteDataLen:%d\r\n",currentWriteAddr,dataWriteToFistPagaLen,hasNotWriteDataLen);
	I2C1_BufferWrite(pDataToWrite,dataWriteToFistPagaLen,currentWriteAddr,DeviceAddress);   
	currentWriteAddr += dataWriteToFistPagaLen;
	//while(currentWriteAddr % AT24C32_PGE_SZE);
	pDataToWrite += dataWriteToFistPagaLen;
	hasNotWriteDataLen -= dataWriteToFistPagaLen;

	while (hasNotWriteDataLen > 0)
	{
		currentWriteDataLen = hasNotWriteDataLen > AT24C32_PGE_SZE?\
							  AT24C32_PGE_SZE:hasNotWriteDataLen; 
		app_printf("currentWriteAddr:%x currentWriteDataLen:%d hasNotWriteDataLen:%d\r\n",currentWriteAddr,currentWriteDataLen,hasNotWriteDataLen);

		I2C1_BufferWrite(pDataToWrite,currentWriteDataLen,currentWriteAddr,DeviceAddress);   
		currentWriteAddr += currentWriteDataLen;
		pDataToWrite += currentWriteDataLen;
		hasNotWriteDataLen -= currentWriteDataLen;
	}
} 

uint8_t at24lc32ReadData(uint16_t ReadAddress,uint8_t* pBuffer,uint16_t DataLength,uint8_t DeviceAddress)
{       
	if (!I2C1_Start())
		return false;

	I2C1_SendByte(DeviceAddress|0x00);
	if (!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	I2C1_SendByte((uint8_t)(ReadAddress>>8));   //设置数据地址      
	if (!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	I2C1_SendByte((uint8_t)(ReadAddress));   //设置数据地址      
	if (!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	if (!I2C1_Start())
		return false;

	I2C1_SendByte(DeviceAddress|0x01);   //设置读地址      
	if (!I2C1_WaitAck())
	{
		I2C1_Stop(); 
		return false;
	}

	while (DataLength)
	{
		*pBuffer = I2C1_ReceiveByte();
		if (DataLength == 1)
			I2C1_NoAck();
		else 
			I2C1_Ack(); 
		pBuffer++;
		DataLength--;
	}
	I2C1_Stop();

	return true;
}



#define TEST_DATA_LEN 13
uint8_t Test[TEST_DATA_LEN] = {0};
uint8_t Check[TEST_DATA_LEN] = {0};

bool at24lc32Test(void) 
{
	uint16_t i = 0;
	uint16_t testAddr = 0x490;
	bool testResult = false;

	if (false == isInit)
		return isInit;

	for (i=0;i<TEST_DATA_LEN;i++)
	{
		Test[i] = i;
	}

	at24lc32WriteData(testAddr,Test,TEST_DATA_LEN,AT24C32_I2C_ADDR);
#if 0
	for (i=0;i<TEST_DATA_LEN;i++)
	{
		if (0 == i%32)
		{
			app_printf("\r\n");
		}
		app_printf("s:%02x ",Test[i]);
	}
	app_printf("\r\n");
#endif

#if 0
	at24c32EraseOperation(0x326,0x326 + 255);
	at24lc32ReadData(testAddr,Check,sizeof(Check),AT24C32_I2C_ADDR);
	for (i=0;i<TEST_DATA_LEN;i++)
	{
		if (0 == i%32)
		{
			app_printf("\r\n");
		}
		app_printf("e:%02x ",Check[i]);
	}
	app_printf("\r\n");
	at24lc32WriteData(0x326,&Test[0x326 - 0x321],256,AT24C32_I2C_ADDR);
#endif

	memset(Check,0,sizeof(Check));
	at24lc32ReadData(testAddr,Check,sizeof(Check),AT24C32_I2C_ADDR);
#if 0
	for (i=0;i<TEST_DATA_LEN;i++)
	{
		if (0 == i%32)
		{
			app_printf("\r\n");
		}
		app_printf("r:%02x ",Check[i]);
	}
	app_printf("\r\n");
#endif

	if (0 == memcmp((const void*)Test,(const void*)Check,TEST_DATA_LEN))
	{
		//app_printf("eeprom test success1\r\n");	
		testResult = true;
	}
	else
	{
		//app_printf("eeprom test fail\r\n");	
		testResult = false;
	}

	return testResult;
} 

