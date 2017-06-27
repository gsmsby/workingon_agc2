/******************************************************************************
 |
 | 	FILENAME:  nucleo-f429zi.c
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
 |	    DATE:		Nov 23, 2016
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* IO library includes. */
#include <freertos-plus-io/include/FreeRTOS_IO.h>
#include <freertos-plus-io/include/FreeRTOS_ssp.h>
#include <freertos-plus-io/include/IOUtils_Common.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
portBASE_TYPE boardCONFIGURE_UART_PINS(const uint8_t cPeripheralNumber)
{
	portBASE_TYPE ret = pdPASS;

	switch(cPeripheralNumber)
	{
	case 2:
	{
		GPIO_InitTypeDef gpio;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

		GPIO_StructInit(&gpio);
		gpio.GPIO_Speed = GPIO_Speed_25MHz;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		gpio.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_Init(GPIOD, &gpio);

		GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_USART2);
		GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_USART2);
	}
	break;
	default:
	{
		/* umm, no one figured out what pins they want */
		configASSERT(pdFALSE);
		ret = pdFAIL;
	}
	}

	return ret;
}

portBASE_TYPE boardCONFIGURE_SSP_PINS(const uint8_t cPeripheralNumber)
{
	portBASE_TYPE ret = pdPASS;

	switch(cPeripheralNumber)
	{
	case 3:
	{
		GPIO_InitTypeDef gpio;

		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

		GPIO_StructInit(&gpio);
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_OType = GPIO_OType_PP;
		gpio.GPIO_Mode = GPIO_Mode_AF;
		gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
		gpio.GPIO_Pin = GPIO_Pin_4;

		/* NSS */
		GPIO_Init(GPIOA, &gpio);

		/* MOSI, SCK, MISO */
		gpio.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
		GPIO_Init(GPIOB, &gpio);

		GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_SPI3);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI3);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI3);
		GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI3);
	}
	break;

	default:
	{
		/* umm, no one figured out what pins they want */
		configASSERT(pdFALSE);
		ret = pdFAIL;
	}
	}

	return ret;
}
