/**
 * Copyright (C) 2018
 * All Rights Reserved
 * 
 * Filename     : uart2.h
 * Author       : daxiazhuluoji
 * Last modified: 2018-09-04 17:54
 * Email        : lidianjun@yeah.net
 * Description  : 
 * 
 */
 
 
#ifndef UART2_H_
#define UART2_H_

#include <stdbool.h>
#include "eprintf.h"

#define UART2_TYPE             USART2
#define UART2_PERIF            RCC_APB1Periph_USART2
#define ENABLE_UART2_RCC       RCC_APB1PeriphClockCmd
#define UART2_IRQ              USART2_IRQn

#define UART2_DMA_IRQ          DMA1_Channel2_IRQn
#define UART2_DMA_IT_TC        DMA1_IT_TC2
#define UART2_DMA_CH           DMA1_Channel7

#define UART2_GPIO_PERIF       RCC_AHB1Periph_GPIOA
#define UART2_GPIO_PORT        GPIOA
#define UART2_GPIO_TX_PIN      GPIO_Pin_2
#define UART2_GPIO_RX_PIN      GPIO_Pin_3
#define UART2_GPIO_AF_TX_PIN   GPIO_PinSource2
#define UART2_GPIO_AF_RX_PIN   GPIO_PinSource3
#define UART2_GPIO_AF_TX       GPIO_AF_USART2
#define UART2_GPIO_AF_RX       GPIO_AF_USART2

/**
 * Initialize the UART.
 */
void uart2Init(uint32_t bound);
//void uart2Init(const uint32_t baudrate);

/**
 * Test the UART status.
 *
 * @return true if the UART is initialized
 */
bool uart2Test(void);

/**
 * Sends raw data using a lock. Should be used from
 * exception functions and for debugging when a lot of data
 * should be transfered.
 * @param[in] size  Number of bytes to send
 * @param[in] data  Pointer to data
 */
void uart2SendData(uint32_t size, uint8_t* data);

/**
 * Send a single character to the serial port using the uartSendData function.
 * @param[in] ch Character to print. Only the 8 LSB are used.
 *
 * @return Character printed
 */
int uart2Putchar(int ch);

/**
 * Uart printf macro that uses eprintf
 * @param[in] FMT String format
 * @param[in] ... Parameters to print
 *
 * @note If UART Crtp link is activated this function does nothing
 */
#define uart2Printf(FMT, ...) eprintf(uart2Putchar, FMT, ## __VA_ARGS__)

#endif /* UART2_H_ */
