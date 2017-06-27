/******************************************************************************
 |
 |  	FILENAME:  stm32f4_utils.h
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
 |	    DATE:		Nov 29, 2016
 |
 ******************************************************************************/
#ifndef FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_STM32F4_UTILS_H_
#define FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_STM32F4_UTILS_H_

/* Includes ------------------------------------------------------------------*/

/* Namespace declaration -----------------------------------------------------*/

/* Class definition ----------------------------------------------------------*/
portBASE_TYPE prv_cleardmastatusflags(DMA_Stream_TypeDef *dmaystreamx);
portBASE_TYPE prv_getdmadesc(Peripheral_Descriptor_t pxPeripheral,
							 DMA_Descriptions_t **ppdma,
							 DMA_Descriptions_t *desc);


#endif /* FREERTOS_PLUS_IO_DEVICE_STM32F4_SUPPORTEDBOARDS_STM32F4_UTILS_H_ */
