/******************************************************************************
 |
 |  	FILENAME:  nucleo-f429zi.hpp
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
/*
 * FreeRTOS+IO V1.0.1 (C) 2012 Real Time Engineers ltd.
 *
 * FreeRTOS+IO is an add-on component to FreeRTOS.  It is not, in itself, part
 * of the FreeRTOS kernel.  FreeRTOS+IO is licensed separately from FreeRTOS,
 * and uses a different license to FreeRTOS.  FreeRTOS+IO uses a dual license
 * model, information on which is provided below:
 *
 * - Open source licensing -
 * FreeRTOS+IO is a free download and may be used, modified and distributed
 * without charge provided the user adheres to version two of the GNU General
 * Public license (GPL) and does not remove the copyright notice or this text.
 * The GPL V2 text is available on the gnu.org web site, and on the following
 * URL: http://www.FreeRTOS.org/gpl-2.0.txt
 *
 * - Commercial licensing -
 * Businesses and individuals who wish to incorporate FreeRTOS+IO into
 * proprietary software for redistribution in any form must first obtain a low
 * cost commercial license - and in-so-doing support the maintenance, support
 * and further development of the FreeRTOS+IO product.  Commercial licenses can
 * be obtained from http://shop.freertos.org and do not require any source files
 * to be changed.
 *
 * FreeRTOS+IO is distributed in the hope that it will be useful.  You cannot
 * use FreeRTOS+IO unless you agree that you use the software 'as is'.
 * FreeRTOS+IO is provided WITHOUT ANY WARRANTY; without even the implied
 * warranties of NON-INFRINGEMENT, MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. Real Time Engineers Ltd. disclaims all conditions and terms, be they
 * implied, expressed, or statutory.
 *
 * 1 tab == 4 spaces!
 *
 * http://www.FreeRTOS.org
 * http://www.FreeRTOS.org/FreeRTOS-Plus
 *
 */
#ifndef FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_NUCLEO_F429ZI_H_
#define FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_NUCLEO_F429ZI_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <freertos-plus-io/include/FreeRTOS_DriverInterface.h>
#include <stm32f4xx_usart.h>
#include <stm32f4xx_spi.h>
#include <stm32f4xx_i2c.h>

/* Namespace declaration -----------------------------------------------------*/
/* required for freertos-plus-io */
#define boardAVAILABLE_DEVICES_LIST											\
{																			\
	{(const int8_t * const)"/USART1/", eUSART_TYPE, (void *) USART1},		\
	{(const int8_t * const)"/USART2/", eUSART_TYPE, (void *) USART2},		\
	{(const int8_t * const)"/USART3/", eUSART_TYPE, (void *) USART3},		\
	{(const int8_t * const)"/UART4/", eUART_TYPE, (void *) UART4},			\
	{(const int8_t * const)"/UART5/", eUART_TYPE, (void *) UART5},			\
	{(const int8_t * const)"/USART6/", eUSART_TYPE, (void *) USART6},		\
	{(const int8_t * const)"/UART7/", eUART_TYPE, (void *) UART7},			\
	{(const int8_t * const)"/UART8/", eUART_TYPE, (void *) UART8},			\
																			\
	{(const int8_t * const)"/SSP1/", eSSP_TYPE, (void *) SPI1},				\
	{(const int8_t * const)"/SSP2/", eSSP_TYPE, (void *) SPI2},				\
	{(const int8_t * const)"/SSP3/", eSSP_TYPE, (void *) SPI3},				\
	{(const int8_t * const)"/SSP4/", eSSP_TYPE, (void *) SPI4},				\
	{(const int8_t * const)"/SSP5/", eSSP_TYPE, (void *) SPI5},				\
	{(const int8_t * const)"/SSP6/", eSSP_TYPE, (void *) SPI6},				\
																			\
	{(const int8_t * const)"/I2C2/", eI2C_TYPE, (void *) I2C1}				\
}

/* required for stm32f4 freertos-plus-io port */
/*
 * Used to describe DMA number, stream and channels
 */
typedef struct xDMA_DESCRIPTION
{
	void *baseaddress;
	DMA_Stream_TypeDef const * rxdmastream;
	uint32_t rxdmachannel;
	DMA_Stream_TypeDef const * txdmastream;
	uint32_t txdmachannel;
} DMA_Descriptions_t;

/*	BASEADD | UART_RX DY_SX | UART_RX_CHAN | DMA_TX_DY_SX | UART_TX_CHAN	*/
#define stm32f4DMA_UART_ASSIGNMENTS											\
{																			\
	{USART2, DMA1_Stream5, DMA_Channel_4, DMA1_Stream6, DMA_Channel_4},		\
	{UART4, DMA1_Stream2, DMA_Channel_4, DMA1_Stream4, DMA_Channel_4},		\
	{UART5, DMA1_Stream0, DMA_Channel_4, DMA1_Stream7, DMA_Channel_4},		\
	{UART7, DMA1_Stream3, DMA_Channel_5, DMA1_Stream1, DMA_Channel_5},		\
	{0, 0, 0, 0, 0}															\
}

#define stm32f4DMA_SPI_ASSIGNMENTS											\
{																			\
	{SPI1, DMA2_Stream0, DMA_Channel_3, DMA2_Stream3, DMA_Channel_3},		\
	{SPI2, DMA1_Stream3, DMA_Channel_0, DMA1_Stream4, DMA_Channel_0},		\
	{SPI3, DMA1_Stream2, DMA_Channel_0, DMA1_Stream7, DMA_Channel_0},		\
	{SPI4, DMA2_Stream0, DMA_Channel_4, DMA2_Stream1, DMA_Channel_4},		\
	{0, 0, 0, 0, 0}															\
}																			\

/*******************************************************************************
 * These define the number of peripherals available on the microcontroller -
 * not the number of peripherals that are supported by the software
 ******************************************************************************/
#define boardNUM_SSPS				6 /* SSP1 to SSP6. */
#define boardNUM_UARTS				8 /* UART1 to UART8. */
#define boardNUM_I2CS				3 /* I2C1 to I2C2. */

/*******************************************************************************
 * Map the FreeRTOS+IO interface to the STM32F4xx specific functions.
 ******************************************************************************/
portBASE_TYPE vFreeRTOS_nucleof429zi_PopulateFunctionPointers(const Peripheral_Types_t ePeripheralType, Peripheral_Control_t * const pxPeripheralControl);
#define boardFreeRTOS_PopulateFunctionPointers \
		vFreeRTOS_nucleof429zi_PopulateFunctionPointers
/*******************************************************************************
 * IRQ Handlers that need to be mapped to peripheral irq's
 ******************************************************************************/
void UART_IRQHandler(USART_TypeDef *usartbase,
		const unsigned portBASE_TYPE uartnum);
void SPI_IRQHandler(SPI_TypeDef* spibase, const unsigned portBASE_TYPE spinum);
/*******************************************************************************
 * Configure port UART port pins to be correct for the wiring of the
 * base board.
 ******************************************************************************/
portBASE_TYPE boardCONFIGURE_UART_PINS(const uint8_t cPeripheralNumber);
/*******************************************************************************
 * Configure port SSP port pins to be correct for the wiring of the
 * base board.
 ******************************************************************************/
portBASE_TYPE boardCONFIGURE_SSP_PINS(const uint8_t cPeripheralNumber);
/*******************************************************************************
 * Configure port I2C port pins to be correct for the wiring of the
 * base board.
 ******************************************************************************/

/*******************************************************************************
 * Additional ioctl settings
 ******************************************************************************/
/* SPI */
#define ioctlSET_SPI_NSS					204
#define ioctlSET_SPI_BAUDRATEPRESCALER		205
#define ioctlSET_SPI_CRCPOLY				206
#define ioctlSET_SPI_DIRECTION				207

/*******************************************************************************
 * Set the default settings
 ******************************************************************************/
#define boardDEFAULT_READ_MUTEX_TIMEOUT		500	//ms

/* UART */
#define boardDEFAULT_UART_BAUD		115200

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_NUCLEO_F429ZI_H_ */
