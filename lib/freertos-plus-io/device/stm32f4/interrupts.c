/******************************************************************************
 |
 | 	FILENAME:  interrupts.c
 |
 |	Copyright 2016 Adara Systems Ltd. as an unpublished work.
 |	All Rights Reserved.
 |
 |	The information contained herein is confidential property of Adara Systems
 |	Ltd. The use, copying, transfer or disclosure of such information is
 |	prohibited except by express written agreement with Adara Systems Ltd.
 |
 |  	DESCRIPTION:
 |
 |  	PUBLIC FUNCTIONS:
 |
 |
 |  	NOTES:
 |
 |  	AUTHOR(S):  Roque
 |	    DATE:		Nov 25, 2016
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include <FreeRTOS_IO_BSP.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* USART / UART IRQ Handlers */
void USART2_IRQHandler(void);

/* SPI IRQ Handlers */
void SPI3_IRQHandler(void);

void USART2_IRQHandler()
{
	traceISR_ENTER();
	UART_IRQHandler(USART2, 2);
	traceISR_EXIT();
}

void SPI3_IRQHandler()
{
	traceISR_ENTER();
	SPI_IRQHandler(SPI3, 3);
	traceISR_EXIT();
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
