/******************************************************************************
 |
 | 	FILENAME:  FreeRTOS_stm32f4_ssp.c
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
 |	    DATE:		Nov 28, 2016
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* IO library includes */
#include <freertos-plus-io/include/FreeRTOS_IO.h>
#include <freertos-plus-io/include/FreeRTOS_ssp.h>
#include <freertos-plus-io/include/IOUtils_Common.h>
#include "stm32f4_utils.h"
#include <stdlib.h>
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define MAXINDEX boardNUM_SSPS + 1
/* Private variables ---------------------------------------------------------*/
/* A structure is maintained for each possible Tx session on each possible SSP
port, and each possible Rx session on each possible SSP port.  These structures
maintain the state of the Tx or Rx transfer, on the assumption that a Tx or
Rx cannot complete immediately. */
static Transfer_Control_t *pxTxTransferControlStructs[MAXINDEX] = {NULL};
static Transfer_Control_t *pxRxTransferControlStructs[MAXINDEX] = {NULL};

/* Circular buffer data structure */
typedef struct
{
	int32_t tsize;
} prvSTM32F4_CircularBuffer_t;
static prvSTM32F4_CircularBuffer_t pxRxCircularBuffer[MAXINDEX];

/* Writing to the SSP will also cause bytes to be received.  If the only
purpose of writing is to send data, and whatever is received during the
send can be junked, then ulRecieveActive[ x ] will be set to false.  When the
purpose of sending is to solicit incoming data, and Rxed data should be
stored, then ulReceiveActive[ x ] will be set to true. */
static volatile uint32_t ulReceiveActive[MAXINDEX] = {pdFALSE};

/* Maintain a structure that holds the configuration of each SSP port.  This
allows a single configuration parameter to be changed at a time, as the
configuration for all the other parameters can be read from the stored
structure rather than queried from the peripheral itself. */
static SPI_InitTypeDef xSSPConfigurations[MAXINDEX];

/* The CMSIS interrupt number definitions for the SSP ports. */
static const IRQn_Type xIRQ[] = { 0, SPI1_IRQn,  SPI2_IRQn, SPI3_IRQn,
		SPI4_IRQn, SPI5_IRQn, SPI6_IRQn};

/* DMA descriptor */
const DMA_Descriptions_t xDMASPIDescriptors[] =
		stm32f4DMA_SPI_ASSIGNMENTS;

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
static size_t prv_initdmatx(DMA_Descriptions_t const * const desc,
		   	   	   	   	    uint8_t **ppucBuffer, const size_t xTotalBytes);
/*
 * open and init spi port
 */
portBASE_TYPE FreeRTOS_SSP_open(Peripheral_Control_t * const pxPeripheralControl)
{
	portBASE_TYPE xret = pdFAIL;
	SPI_TypeDef * const spibase =
			(SPI_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	const int8_t pnum = diGET_PERIPHERAL_NUMBER(pxPeripheralControl);

	if (pnum < MAXINDEX)
	{
		/* create transfer control structures */
		vIOUtilsCreateTransferControlStructure(
				&(pxPeripheralControl->pxTxControl));
		vIOUtilsCreateTransferControlStructure(
				&(pxPeripheralControl->pxRxControl));

		/* successful creation */
		if ((pxPeripheralControl->pxTxControl) &&
				(pxPeripheralControl->pxRxControl))
		{
			pxPeripheralControl->read = FreeRTOS_SSP_read;
			pxPeripheralControl->write = FreeRTOS_SSP_write;
			pxPeripheralControl->ioctl = FreeRTOS_SSP_ioctl;
			pxPeripheralControl->pxTxControl->pvTransferState = NULL;
			pxPeripheralControl->pxTxControl->ucType = ioctlUSE_POLLED_TX;
			pxPeripheralControl->pxRxControl->pvTransferState = NULL;
			pxPeripheralControl->pxRxControl->ucType = ioctlUSE_POLLED_RX;

			taskENTER_CRITICAL();
			{
				boardCONFIGURE_SSP_PINS(pnum);

				/* set up the default spi config */
				SPI_StructInit(&(xSSPConfigurations[pnum]));
				xSSPConfigurations[pnum].SPI_Mode = SPI_Mode_Master;
				xSSPConfigurations[pnum].SPI_NSS = SPI_NSS_Soft;
				xSSPConfigurations[pnum].SPI_BaudRatePrescaler =
						SPI_BaudRatePrescaler_256;
				SPI_Init(spibase, &(xSSPConfigurations[pnum]));
				SPI_Cmd(spibase, ENABLE);

				/* clear rx data */
				SPI_ReceiveData(spibase);
			}
			taskEXIT_CRITICAL();
			xret = pdPASS;
		}
		else
		{
			if (pxPeripheralControl->pxTxControl)
			{
				vPortFree(pxPeripheralControl->pxTxControl);
				pxPeripheralControl->pxTxControl = NULL;
			}
			if (pxPeripheralControl->pxRxControl)
			{
				vPortFree(pxPeripheralControl->pxRxControl);
				pxPeripheralControl->pxRxControl = NULL;
			}
		}
	}

	return xret;
}

/*
 * perform write function
 */
size_t FreeRTOS_SSP_write(Peripheral_Descriptor_t const pxPeripheral,
		const void *pvBuffer, const size_t xBytes)
{
	size_t ret = 0U;
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t * const)pxPeripheral;
	SPI_TypeDef * const spibase =
			(SPI_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	const int8_t pnum = diGET_PERIPHERAL_NUMBER(pxPeripheralControl);

	/* record transfer control structure in case interrupt needs to take over */
	pxTxTransferControlStructs[pnum] =
			diGET_TX_TRANSFER_STRUCT(pxPeripheralControl);

	switch(diGET_TX_TRANSFER_TYPE(pxPeripheralControl))
	{
	case ioctlUSE_POLLED_TX:
	{
		uint8_t *buff = (uint8_t *)pvBuffer;

		/* when polling we dont need the interrupts */
		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, DISABLE);
		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_TXE, DISABLE);

		for(uint32_t i = 0 ; i < xBytes ; ++i)
		{
			/* perform the writes */
			SPI_SendData(spibase, buff[i]);
			while(SPI_I2S_GetFlagStatus(spibase, SPI_I2S_FLAG_TXE) == RESET);
		}

		/* clear the rx */
		SPI_ReceiveData(spibase);

		/* we dont need to track */
		pxTxTransferControlStructs[pnum] = NULL;
	}
	break;

	case ioctlUSE_ZERO_COPY_TX:
	{
		/*
		 * ensure we are able to perform this transfer
		 * if you're stuck here its because your data resides in CCM.  The DMA
		 * cannot access CCM, move your data
		 */
#ifdef CCMDATARAM_BASE
		configASSERT((uint32_t)pvBuffer > (CCMDATARAM_BASE + (64 * 1024)));
#endif

		/* retrieve dma descriptors */
		DMA_Descriptions_t dmadesc;
		DMA_Descriptions_t *pdma = (DMA_Descriptions_t *)xDMASPIDescriptors;
		prv_getdmadesc(pxPeripheral, &pdma, &dmadesc);

		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_TXE, DISABLE);
		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, DISABLE);

		/* The implementation of the zero copy write uses a semaphore
		to indicate whether a write is complete (and so the buffer
		being written free again) or not.  The semantics of using a
		zero copy write dictate that a zero copy write can only be
		attempted by a task, once the semaphore has been successfully
		obtained by that task.  This ensure that only one task can
		perform a zero copy write at any one time.  Ensure the semaphore
		is not currently available, if this function has been called
		without it being obtained first then it is an error. */
		configASSERT(xIOUtilsGetZeroCopyWriteMutex(pxPeripheralControl,
				ioctlOBTAIN_WRITE_MUTEX, 0U) == 0);
		ret = xBytes;
		ioutilsINITIATE_ZERO_COPY_TX(pxPeripheralControl,
				SPI_I2S_DMACmd(spibase, SPI_I2S_DMAReq_Tx, DISABLE),
				SPI_I2S_DMACmd(spibase, SPI_I2S_DMAReq_Tx, ENABLE),
				prv_initdmatx(&dmadesc, (uint8_t **)&pvBuffer, xBytes),
				pvBuffer,
				ret);

		/*
		 * The above define releases the mutex since we copy all the fifo
		 * bits, we need to re-acquire that mutex since we are not ready
		 * to give it up
		 */
		Zero_Copy_Tx_State_t *pzxero = (Zero_Copy_Tx_State_t *)
				pxPeripheralControl->pxTxControl->pvTransferState;
		xSemaphoreTake(pzxero->xWriteAccessMutex, 0);
		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_TXE, ENABLE);
	}
	break;

	case ioctlUSE_CHARACTER_QUEUE_TX:
	{
		/* The queue allows multiple tasks to attempt to write bytes,
		but ensures only the highest priority of these tasks will
		actually succeed.  If two tasks of equal priority attempt to
		write simultaneously, then the application must ensure mutual
		exclusion, as time slicing could result in the strings being
		sent to the queue becoming interleaved. */
		ret = xIOUtilsSendCharsToTxQueue(
				pxPeripheralControl,
				(uint8_t *)pvBuffer,
				xBytes);

		ioutilsFILL_FIFO_FROM_TX_QUEUE(
				pxPeripheralControl,
				SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, DISABLE),
				SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, ENABLE),
				1,
				((spibase->SR & SPI_I2S_FLAG_TXE)),
				(spibase->DR = (uint16_t)ucChar)
				);
	}
	break;

	default:
	{
		pxTxTransferControlStructs[pnum] = NULL;
		configASSERT(ret);

		/* get rid of warnings */
		(void)pvBuffer;
		(void)xBytes;
		(void)spibase;
	}
	}

	return ret;
}

/*
 * perform a read
 */
size_t FreeRTOS_SSP_read(Peripheral_Descriptor_t const pxPeripheral,
						 void * const pvBuffer, const size_t xBytes)
{
	size_t ret = 0U;
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t * const)pxPeripheral;
	SPI_TypeDef * const spibase =
			(SPI_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	const int8_t pnum = diGET_PERIPHERAL_NUMBER(pxPeripheralControl);

	/* Sanity check the array index. */
	configASSERT(pnum < (int8_t)(sizeof(xIRQ) / sizeof(IRQn_Type)));

	pxRxTransferControlStructs[pnum] =
			diGET_RX_TRANSFER_STRUCT(pxPeripheralControl);

	switch(diGET_RX_TRANSFER_TYPE(pxPeripheralControl))
	{
	case ioctlUSE_POLLED_RX:
	{
		uint8_t *buff = (uint8_t *)pvBuffer;

		/* clear rx buffer */
		SPI_ReceiveData(spibase);

		for(uint32_t i = 0 ; i < xBytes ; ++i)
		{
			/* perform the writes */
			SPI_SendData(spibase, 0xFF);
			while(SPI_I2S_GetFlagStatus(spibase, SPI_I2S_FLAG_TXE) == RESET);
			buff[i] = SPI_ReceiveData(spibase);
		}

		/* we dont need to track */
		pxRxTransferControlStructs[pnum] = NULL;
		ret = xBytes;
	}
	break;

	case ioctlUSE_CIRCULAR_BUFFER_RX:
	{
		/* There is nothing to prevent multiple tasks attempting to
		read the circular buffer at any one time.  The implementation
		of the circular buffer uses a semaphore to indicate when new
		data is available, and the semaphore will ensure that only the
		highest priority task that is attempting a read will actually
		receive bytes. */

//#ifdef CCMDATARAM_BASE
//		/*
//		 * We have CCM, we need to be careful when we initialize the DMA
//		 */
//		Circular_Buffer_Rx_State_t *pxbuffstate =
//				((Circular_Buffer_Rx_State_t *)
//						pxPeripheralControl->pxRxControl->pvTransferState);
//		uint8_t *savebuff = pxbuffstate->pucBufferStart;
//		if ((uint32_t)pvBuffer > (CCMDATARAM_BASE + (64 * 1024)))
//		{
//			pxbuffstate->pucBufferStart =
//					(uint8_t *)malloc(pxbuffstate->usCharCount);
//		}
//#endif

		/* Ensure the last Tx has completed. */
		while(SPI_I2S_GetFlagStatus(spibase, SPI_I2S_FLAG_BSY) == SET);

		/* clear the rx buffer */
		SPI_ReceiveData(spibase);
		vIOUtilsClearRxCircularBuffer(pxPeripheralControl);

		/* receive data into the buffer */
		ulReceiveActive[pnum] = pdTRUE;
		pxRxCircularBuffer[pnum].tsize = xBytes;

		/* set us up for unidirectional mode rx only */
		SPI_Cmd(spibase, DISABLE);
		xSSPConfigurations[pnum].SPI_Direction = SPI_Direction_1Line_Rx;
		SPI_Init(spibase, &xSSPConfigurations[pnum]);
		SPI_Cmd(spibase, ENABLE);

		/*
		 * receive data from the ring buffer
		 */
		ioutilsRECEIVE_CHARS_FROM_CIRCULAR_BUFFER
		(
			pxPeripheralControl,
			SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, DISABLE),
			SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, ENABLE),
			(uint8_t *)pvBuffer,
			xBytes,
			ret
		);

		/* disable reception */
		SPI_Cmd(spibase, DISABLE);

		/* change back to bi-directional mode */
		xSSPConfigurations[pnum].SPI_Direction =
				SPI_Direction_2Lines_FullDuplex;
		SPI_Init(spibase, &xSSPConfigurations[pnum]);
		SPI_Cmd(spibase, ENABLE);

		/* disable save on reception */
		ulReceiveActive[pnum] = pdFALSE;

//#ifdef CCMDATARAM_BASE
//		/* Restore settings */
//		free(pxbuffstate->pucBufferStart);
//		pxbuffstate->pucBufferStart = savebuff;
//#endif
	}
	break;

	case ioctlUSE_CHARACTER_QUEUE_RX:
	{
		/* The queue allows multiple tasks to attempt to read bytes,
		but ensures only the highest priority of these tasks will
		actually receive bytes.  If two tasks of equal priority attempt
		to read simultaneously, then the application must ensure mutual
		exclusion, as time slicing could result in the string being
		received being partially received by each task. */

		/*
		 * before we start rxing, wait until all transmissions have been
		 * performed
		 */
		xIOUtilsWaitTxQueueEmpty(
				pxPeripheralControl,
				boardDEFAULT_READ_MUTEX_TIMEOUT);

		/* clear our rx buffer */
		xIOUtilsClearRxCharQueue(pxPeripheralControl);

		/* activate recieve store mode */
		ulReceiveActive[pnum] = pdTRUE;

		/* force a write of garbage data */
		FreeRTOS_SSP_write(pxPeripheralControl, NULL, xBytes);

		/* enable rx interrupts */
		SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, ENABLE);

		/*
		 * at this point we should be rcving data via interrupt this data
		 * should be going into the character queue we will remove it from
		 * the queue later
		 */

		/* read the data from the queue */
		ret = xIOUtilsReceiveCharsFromRxQueue(pxPeripheralControl,
				pvBuffer, xBytes);

		/* rx data shall not be saved */
		ulReceiveActive[pnum] = pdFALSE;
	}
	break;

	default:
	{
		configASSERT(ret);

		(void)pvBuffer;
		(void)xBytes;
	}
	}

	return ret;
}

portBASE_TYPE FreeRTOS_SSP_ioctl(Peripheral_Descriptor_t const pxPeripheral,
								 uint32_t ulRequest, void *pvValue)
{
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t * const)pxPeripheral;
	uint32_t ulValue = (uint32_t)pvValue;
	uint32_t ulRet = pdFALSE;
	const uint8_t pnum = diGET_PERIPHERAL_NUMBER(
			(Peripheral_Control_t * const)pxPeripheral);
	SPI_TypeDef * spibase = (SPI_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(
			(Peripheral_Control_t * const)pxPeripheral);
	BaseType_t changed = pdFALSE;

	taskENTER_CRITICAL();
	{
		switch(ulRequest)
		{
		case ioctlUSE_INTERRUPTS:
		{
			/* Sanity check the array index. */
			configASSERT(pnum < (int8_t)(sizeof(xIRQ) / sizeof(IRQn_Type)));

			/* Enable the Rx interrupts only. */
			SPI_I2S_ITConfig(spibase, SPI_I2S_IT_TXE, DISABLE);
			SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, ENABLE);

			/* Enable the interrupt and set its priority to the minimum
			interrupt priority.  A separate command can be issued to raise
			the priority if desired. */
			NVIC_InitTypeDef nvic;
			nvic.NVIC_IRQChannel = xIRQ[pnum];
			if (pvValue == (void *)pdTRUE)
				nvic.NVIC_IRQChannelCmd = ENABLE;
			else
				nvic.NVIC_IRQChannelCmd = DISABLE;
			nvic.NVIC_IRQChannelPreemptionPriority =
					configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
			nvic.NVIC_IRQChannelSubPriority = 0;
			NVIC_Init(&nvic);

			/* If the Rx is configured to use interrupts, remember the
			transfer control structure that should be used.  A reference
			to the Tx transfer control structure is taken when a write()
			operation is actually performed. */
			pxRxTransferControlStructs[pnum] =
					pxPeripheralControl->pxRxControl;

			ulRet = pdTRUE;
		}
		break;

		case ioctlSET_INTERRUPT_PRIORITY:
		{
			/* The ISR uses ISR safe FreeRTOS API functions, so the priority
			being set must be lower than (ie numerically larger than)
			configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
			configASSERT(ulValue <
					configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
			NVIC_SetPriority(xIRQ[pnum], ulValue);
			ulRet = pdTRUE;
		}
		break;

		case ioctlSET_SPEED:
		{
			/*
			 * we dont supporting setting a speed in the normal sense, the
			 * stm32f4 does it based on a divisor to the ahb2 clock
			 */
			configASSERT(pdFALSE);
		}
		break;

		case ioctlSET_SPI_DATA_BITS:
		{
			xSSPConfigurations[pnum].SPI_DataSize = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_CLOCK_PHASE:
		{
			xSSPConfigurations[pnum].SPI_CPHA = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_CLOCK_POLARITY:
		{
			xSSPConfigurations[pnum].SPI_CPOL = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_MODE:
		{
			xSSPConfigurations[pnum].SPI_Mode = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_NSS:
		{
			xSSPConfigurations[pnum].SPI_NSS = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_BAUDRATEPRESCALER:
		{
			xSSPConfigurations[pnum].SPI_BaudRatePrescaler = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_CRCPOLY:
		{
			xSSPConfigurations[pnum].SPI_BaudRatePrescaler = ulValue;
			changed = pdTRUE;
		}
		break;

		case ioctlSET_SPI_DIRECTION:
		{
			xSSPConfigurations[pnum].SPI_Direction = ulValue;
			changed = pdTRUE;
		}
		break;
		}

		/* check if changes need to be made to the spi port settings */
		if (changed == pdTRUE)
		{
			SPI_Cmd(spibase, DISABLE);
			SPI_Init(spibase, &xSSPConfigurations[pnum]);
			SPI_Cmd(spibase, ENABLE);
		}

	}
	taskEXIT_CRITICAL();

	return ulRet;
}

/*
 * initialize dma transfer
 */
static size_t prv_initdmatx(DMA_Descriptions_t const * const desc,
		   	   	   	   	  	uint8_t **ppucBuffer, const size_t xTotalBytes)
{
	SPI_TypeDef * const baseaddr = desc->baseaddress;
	uint32_t const dmachan = desc->txdmachannel;
	DMA_Stream_TypeDef const * const dmaystreamx =
			(DMA_Stream_TypeDef*)desc->txdmastream;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/*
	 * pause the stream before making changes to it
	 */
	SPI_I2S_DMACmd((SPI_TypeDef *)baseaddr, SPI_DMAReq_Tx, DISABLE);
	DMA_Cmd((DMA_Stream_TypeDef *)dmaystreamx, DISABLE);
	while(DMA_GetCmdStatus((DMA_Stream_TypeDef *)dmaystreamx) == ENABLE);

	/* clear dma status flags */
	prv_cleardmastatusflags((DMA_Stream_TypeDef *)dmaystreamx);

	DMA_InitTypeDef dma;
	DMA_StructInit(&dma);
	dma.DMA_Channel = dmachan;
	dma.DMA_PeripheralBaseAddr = (uint32_t)(&(baseaddr->DR));
	dma.DMA_Memory0BaseAddr = (uint32_t)(*ppucBuffer);
	dma.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	dma.DMA_BufferSize = xTotalBytes;
	dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc = DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	dma.DMA_Mode = DMA_Mode_Normal;
	dma.DMA_Priority = DMA_Priority_Low;
	dma.DMA_FIFOMode = DMA_FIFOMode_Enable;
	dma.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	dma.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
	dma.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init((DMA_Stream_TypeDef *)dmaystreamx, &dma);
	DMA_Cmd((DMA_Stream_TypeDef *)dmaystreamx, ENABLE);

	return xTotalBytes;
}

/*
 * spi interrupt handler, needs to be called by the actual isr
 */
void SPI_IRQHandler(SPI_TypeDef* spibase,
		const unsigned portBASE_TYPE spinum)
{
	portBASE_TYPE taskwoken = pdFALSE;
	Transfer_Control_t *pxtxtransfer;
	Transfer_Control_t *pxrxtransfer;
	uint16_t statusreg;
	volatile uint32_t junk;
	volatile uint32_t rcvd = 0;

	/*
	 * read the status flags, i've opted to check the status flags instead
	 * of using the interrupt flags since it doesnt look like the STM32 has
	 * a dedicated interrupt status register.
	 *
	 * The driver appears to do some bit shift and comparisons to figure out
	 * if the interrupt is enabled and if the status is true or false
	 *
	 * This is an interrupt and we can't have that kind of latency
	 */
	statusreg = spibase->SR;

	/* lets do all of our rx checks first */
	if (statusreg & SPI_FLAG_OVR)
	{
		/* rx overrun */
		/* clear flag */
		junk = spibase->DR;
		junk = spibase->SR;
	}
	if (statusreg & SPI_FLAG_RXNE)
	{
		/* rx not empty.. or as i like to call it dataavailable */
		pxrxtransfer = pxRxTransferControlStructs[spinum];

		/* make sure we actually have an object to work on */
		configASSERT(pxrxtransfer);

		if (pxrxtransfer)
		{
			/* do we need to save this data ? */
			if (ulReceiveActive[spinum] == pdFALSE)
			{
				junk = spibase->DR;
				++rcvd;
			}
			else
			{
				/* we need to save it */
				switch(diGET_TRANSFER_TYPE_FROM_CONTROL_STRUCT(pxrxtransfer))
				{
				case ioctlUSE_CIRCULAR_BUFFER_RX:
				{
					rcvd = 0;

					/* This call will empty the FIFO, and give the New
					Data semaphore so a task blocked on an SSP read
					will unblock.  Note that this does not mean that
					more data will not arrive after this interrupt,
					even if there is no more data to send. */
					ioutilsRX_CHARS_INTO_CIRCULAR_BUFFER_FROM_ISR(
							pxrxtransfer,
							(spibase->SR & SPI_FLAG_RXNE),
							(spibase->DR),
							rcvd,
							taskwoken);

					if (--pxRxCircularBuffer[spinum].tsize <= 1)
					{
						spibase->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);
					}
				}
				break;

				case ioctlUSE_CHARACTER_QUEUE_RX:
				{
					ioutilsRX_CHARS_INTO_QUEUE_FROM_ISR(
							pxrxtransfer,
							(spibase->SR & SPI_FLAG_RXNE),
							(spibase->DR),
							rcvd,
							taskwoken);
				}
				break;

				default:
				{
					/* we dont support your rx transfer choice */
					configASSERT(pdFALSE);
				}
				}
			}
		}

		if (statusreg & SPI_FLAG_TXE)
		{
			pxtxtransfer = pxTxTransferControlStructs[spinum];

			if (pxtxtransfer)
			{
				switch(diGET_TRANSFER_TYPE_FROM_CONTROL_STRUCT(pxtxtransfer))
				{
				case ioctlUSE_ZERO_COPY_TX:
				{
					/*
					 * we used a dma transfer to handle this, we are just
					 * notifying the user that we are done
					 *
					 */
					Zero_Copy_Tx_State_t *pxzcstate =
							(Zero_Copy_Tx_State_t *)
							(pxtxtransfer->pvTransferState);
					xSemaphoreGiveFromISR(pxzcstate->xWriteAccessMutex,
							&taskwoken);

					/*
					 * we disabled the interrupt for data reception when we
					 * started the write, we need to clear flags and re-enable
					 * here
					 */
					junk = spibase->DR;
					junk = spibase->SR;
					SPI_I2S_ITConfig(spibase, SPI_I2S_IT_RXNE, ENABLE);
					SPI_I2S_ITConfig(spibase, SPI_I2S_IT_TXE, DISABLE);
				}
				break;

				case ioctlUSE_CHARACTER_QUEUE_TX:
				{
					ioutilsTX_CHARS_FROM_QUEUE_FROM_ISR(pxtxtransfer,
							((rcvd--) > 0),
							spibase->DR = ucChar,
							taskwoken);
				}
				break;

				default:
				{
					/* we dont support your tx transfer choice */
					configASSERT(pdFALSE);
				}
				}
			}
		}
	}

	/* remove warnings */
	(void)junk;

	/* perform task switch if necessary */
	portEND_SWITCHING_ISR(taskwoken);
}

