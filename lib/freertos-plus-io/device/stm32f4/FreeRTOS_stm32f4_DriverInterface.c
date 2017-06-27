/******************************************************************************
 |
 | 	FILENAME:  FreeRTOS_stm32f4_DriverInterface.c
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

/* FreeRTOS IO library includes. */
#include <freertos-plus-io/include/FreeRTOS_DriverInterface.h>
#include <freertos-plus-io/include/FreeRTOS_i2c.h>
#include <freertos-plus-io/include/FreeRTOS_ssp.h>
#include <freertos-plus-io/include/FreeRTOS_uart.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
portBASE_TYPE
vFreeRTOS_nucleof429zi_PopulateFunctionPointers(
			const Peripheral_Types_t ePeripheralType,
			Peripheral_Control_t * const pxPeripheralControl
		)
{
	portBASE_TYPE xret = pdFALSE;

	switch(ePeripheralType)
	{
	case eUART_TYPE:
	case eUSART_TYPE:
	{
#if ioconfigINCLUDE_UART == 1
		xret = FreeRTOS_UART_open(pxPeripheralControl);
#endif
	}
	break;

	case eSSP_TYPE:
	{
#if ioconfigINCLUDE_SSP == 1
		xret = FreeRTOS_SSP_open(pxPeripheralControl);
#endif
	}
	break;

	case eI2C_TYPE:
	{
#if ioconfigINCLUDE_I2C == 1
		xret = FreeRTOS_UART_open(pxPeripheralControl);
#endif
	}
	break;

	default:
		configASSERT(xret);
	}

	/*
	 * Just to prevent compiler warnings should FreeRTOSIOConfig.h be
	 * configured to exclude the above FreeRTOS_nnn_open() calls.
	 */
	( void ) pxPeripheralControl;

	return xret;
}
