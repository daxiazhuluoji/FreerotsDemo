
#ifndef _AT_OPTION_H_
#define _AT_OPTION_H_

#include "stm32f10x.h"

#define ATSTR_OK "OK\r\n"
#define ATSTR_ERROR "ERROR\r\n"
#define ATSTR_ATBTIRQ_0 "AT+BTIRQ=0\r\n"
#define ATSTR_ATBTIRQ_1 "AT+BTIRQ=1\r\n"
#define ATSTR_ATGWID "AT+GWID=1234567890\r\n"


#define BTCD_HIGH GPIO_SetBits(GPIOC,GPIO_Pin_6)
#define BTCD_LOW GPIO_ResetBits(GPIOC,GPIO_Pin_6)

typedef enum
{
	BTCDLOW,
	BTCDHIGH,
}BtCDPinSta_t;

uint8_t getAtStr(uint8_t* AtStrTemp);
BaseType_t ATOption(BtCDPinSta_t BTCDPinSta,
		signed char* sendATCmd,
		signed char* expectRsp,
		TickType_t xTicksToWaitATRsp);

#endif

