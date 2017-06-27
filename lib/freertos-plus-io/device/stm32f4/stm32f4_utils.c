/******************************************************************************
 |
 | 	FILENAME:  stm32f4_utils.c
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
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* IO library includes */
#include <freertos-plus-io/include/FreeRTOS_IO.h>
#include <freertos-plus-io/include/FreeRTOS_ssp.h>
#include <freertos-plus-io/include/IOUtils_Common.h>

#include "stm32f4_utils.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
/*
 * clears dma status flags
 */
portBASE_TYPE prv_cleardmastatusflags(DMA_Stream_TypeDef *dmaystreamx)
{
	switch((uint32_t)dmaystreamx)
	{

	case (uint32_t)DMA1_Stream0:
	case (uint32_t)DMA2_Stream0:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF0 | DMA_FLAG_TCIF0);
	}
	break;
	case (uint32_t)DMA1_Stream1:
	case (uint32_t)DMA2_Stream1:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF1 | DMA_FLAG_TCIF1);
	}
	break;
	case (uint32_t)DMA1_Stream2:
	case (uint32_t)DMA2_Stream2:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF2 | DMA_FLAG_TCIF2);
	}
	break;
	case (uint32_t)DMA1_Stream3:
	case (uint32_t)DMA2_Stream3:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF3 | DMA_FLAG_TCIF3);
	}
	break;
	case (uint32_t)DMA1_Stream4:
	case (uint32_t)DMA2_Stream4:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF4 | DMA_FLAG_TCIF4);
	}
	break;
	case (uint32_t)DMA1_Stream5:
	case (uint32_t)DMA2_Stream5:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF5 | DMA_FLAG_TCIF5);
	}
	break;
	case (uint32_t)DMA1_Stream6:
	case (uint32_t)DMA2_Stream6:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF6 | DMA_FLAG_TCIF6);
	}
	break;
	case (uint32_t)DMA1_Stream7:
	case (uint32_t)DMA2_Stream7:
	{
		DMA_ClearFlag(dmaystreamx, DMA_FLAG_HTIF7 | DMA_FLAG_TCIF7);
	}
	break;
	}

	return pdTRUE;
}

/*
 * retrieve dma descriptor
 *
 * \param[in] pxPeripheral
 * \param[in] ppdma - the table of descriptors
 * \param[out] desc - the descriptor for the current peripheral
 */
portBASE_TYPE prv_getdmadesc(Peripheral_Descriptor_t pxPeripheral,
							 DMA_Descriptions_t **ppdma,
							 DMA_Descriptions_t *desc)
{
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t* const)pxPeripheral;
	SPI_TypeDef* const spibase =
			(SPI_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	portBASE_TYPE xReturn = pdFAIL;

	/* iterate through and match the base address */
	for(uint32_t i = 0 ;
			(*ppdma)[i].baseaddress ; ++i)
	{
		if ((*ppdma)[i].baseaddress == spibase)
		{
			*desc = (*ppdma)[i];
			xReturn = pdPASS;

			break;
		}
	}

	/* !!! freaking amateur, you didnt configure your board correctly !!! */
	configASSERT(xReturn);

	return xReturn;
}
