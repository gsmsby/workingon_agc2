/******************************************************************************
 |
 | 	FILENAME:  FreeRTOS_stm32f4_uart.c
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
/* FreeRTOS includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

/* IO library includes */
#include <freertos-plus-io/include/FreeRTOS_IO.h>
#include <freertos-plus-io/include/FreeRTOS_uart.h>
#include <freertos-plus-io/include/IOUtils_Common.h>
#include "stm32f4_utils.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DATRES boardNUM_UARTS + 1
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Stores the transfer control structures that are currently in use by the
supported UART ports. */
static Transfer_Control_t *pxTxTransferControlStructs[DATRES] = {NULL};
static Transfer_Control_t *pxRxTransferControlStructs[DATRES] = {NULL};

static const DMA_Descriptions_t xDMAUARTDescriptions[] =
		stm32f4DMA_UART_ASSIGNMENTS;

/*
 * Stores the IRQ numbers of the supported UART ports.
 *
 * NOTE: They must be in correct order... for example USART1 should be at index
 * 1... It has to do with the way the FreeRTOS IO driver finds offsets.  It uses
 * the name to pull out an index
 */
static const IRQn_Type xIRQ[] =
{0, USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn,
		UART7_IRQn, UART8_IRQn};

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
static inline size_t prvFillFifoFromBuffer(DMA_Descriptions_t const * const uartbase,
										   uint8_t **ppucBuffer,
										   const size_t xTotalBytes);

/*
 * sets the device up for use
 */
portBASE_TYPE FreeRTOS_UART_open(Peripheral_Control_t * const pxPeripheralControl)
{
	USART_InitTypeDef usartconf;
	portBASE_TYPE xReturn;
	USART_TypeDef* const uartbase =
			(USART_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	const uint8_t cPeripheralNumber =
			diGET_PERIPHERAL_NUMBER(pxPeripheralControl);

	/* set up the free rtos callbacks */
	pxPeripheralControl->read = FreeRTOS_UART_read;
	pxPeripheralControl->write = FreeRTOS_UART_write;
	pxPeripheralControl->ioctl = FreeRTOS_UART_ioctl;

	/* set up the io pins */
	taskENTER_CRITICAL();
	{
		boardCONFIGURE_UART_PINS(cPeripheralNumber);

		/* uart config */
		USART_DeInit(uartbase);
		USART_StructInit(&usartconf);
		usartconf.USART_BaudRate = boardDEFAULT_UART_BAUD;
		usartconf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		usartconf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		usartconf.USART_Parity = USART_Parity_No;
		usartconf.USART_StopBits = USART_StopBits_1;
		usartconf.USART_WordLength = USART_WordLength_8b;
		USART_Init(uartbase, &usartconf);

		USART_Cmd(uartbase, ENABLE);
	}
	taskEXIT_CRITICAL();

	xReturn = pdPASS;

	return xReturn;
}

/*
 * performs write function
 */
size_t FreeRTOS_UART_write( Peripheral_Descriptor_t const pxPeripheral, const void *pvBuffer, const size_t xBytes )
{
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t * const) pxPeripheral;
	USART_TypeDef* const uartbase =
			(USART_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	size_t xReturn = 0U;
	int8_t cPeripheralNumber;

	if( diGET_TX_TRANSFER_STRUCT( pxPeripheralControl ) == NULL )
	{
		#if ioconfigUSE_UART_POLLED_TX == 1
		{
			/* send out the data, we will block as this is happening */
			uint8_t *dat = (uint8_t *)pvBuffer;

			for(uint32_t i = 0 ; i < xBytes ; ++i)
			{
				while(USART_GetFlagStatus(uartbase, USART_FLAG_TC) == RESET);
				USART_SendData(uartbase, (uint16_t)(dat[i]));
			}
			while(USART_GetFlagStatus(uartbase, USART_FLAG_TC) == RESET);
			xReturn = xBytes;
		}
		#endif /* ioconfigUSE_UART_POLLED_TX */
	}
	else
	{
		cPeripheralNumber = diGET_PERIPHERAL_NUMBER(pxPeripheralControl);
		pxTxTransferControlStructs[cPeripheralNumber] =
				diGET_TX_TRANSFER_STRUCT(pxPeripheralControl);

		switch(diGET_TX_TRANSFER_TYPE(pxPeripheralControl))
		{
		case ioctlUSE_ZERO_COPY_TX:
		{
#if ioconfigUSE_UART_ZERO_COPY_TX == 1
			/*
			 * ensure we are able to perform this transfer
			 * if you're stuck here its because your data resides in CCM.  The DMA
			 * cannot access CCM, move your data
			 */
#ifdef CCMDATARAM_BASE
			configASSERT((uint32_t)pvBuffer > (CCMDATARAM_BASE + (64 * 1024)));
#endif
			configASSERT(xIOUtilsGetZeroCopyWriteMutex(pxPeripheralControl, ioctlOBTAIN_WRITE_MUTEX, 0U) == 0);
			xReturn = xBytes;

			/* retrieve dma descriptors */
			DMA_Descriptions_t dmadesc;
			DMA_Descriptions_t *pdma = (DMA_Descriptions_t *)xDMAUARTDescriptions;
			prv_getdmadesc(pxPeripheral, &pdma, &dmadesc);

			USART_ITConfig(uartbase, USART_IT_TC, DISABLE);
			USART_ClearFlag(uartbase, USART_FLAG_TC);
			USART_ClearITPendingBit(uartbase, USART_IT_TC);
			ioutilsINITIATE_ZERO_COPY_TX
			(
				pxPeripheralControl,
				USART_DMACmd(uartbase, USART_DMAReq_Tx, DISABLE),	/* Disable peripheral function. */
				USART_DMACmd(uartbase, USART_DMAReq_Tx, ENABLE), 	/* Enable peripheral function. */
				prvFillFifoFromBuffer(&dmadesc, (uint8_t **)&(pvBuffer),
									  xBytes), /* Write to peripheral function. */
				pvBuffer, 						/* Data source. */
				xReturn							/* Number of bytes to be written. This will get set to zero if the write mutex is not held. */
			);

			/*
			 * The above define releases the mutex since we copy all the fifo
			 * bits, we need to re-acquire that mutex since we are not ready
			 * to give it up
			 */
			Zero_Copy_Tx_State_t *pzxero = (Zero_Copy_Tx_State_t *)
					pxPeripheralControl->pxTxControl->pvTransferState;
			xSemaphoreTake(pzxero->xWriteAccessMutex, 0);
			USART_ITConfig(uartbase, USART_IT_TC, ENABLE);
#endif
		}
		break;

		case ioctlUSE_CHARACTER_QUEUE_TX:
		{
#if ioconfigUSE_UART_TX_CHAR_QUEUE == 1
			USART_ITConfig(uartbase, USART_IT_TC, ENABLE);
			ioutilsBLOCKING_SEND_CHARS_TO_TX_QUEUE
			(
				pxPeripheralControl,
				((uartbase->SR & USART_FLAG_TXE)),							/* Peripheral not busy condition. */
				uartbase->DR = (ucChar & (uint16_t)0x01FF),					/* Peripheral write function. */
				((uint8_t *) pvBuffer),										/* Data source. */
				xBytes, 													/* Number of bytes to be written. */
				xReturn
			);
#endif
		}
		break;

		default:
		{
			/* Other methods can be implemented here.  For now set the
			stored transfer structure back to NULL as nothing is being
			sent. */
			configASSERT(xReturn);
			pxTxTransferControlStructs[cPeripheralNumber] = NULL;

			/* Prevent compiler warnings when the configuration is set such
			that the following parameters are not used. */
			(void)pvBuffer;
			(void)xBytes;
			(void)uartbase;
			break;
		}
		}
	}

	return xReturn;
}

/*
 * performs read function
 */
size_t FreeRTOS_UART_read( Peripheral_Descriptor_t const pxPeripheral, void * const pvBuffer, const size_t xBytes )
{
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t * const) pxPeripheral;
	USART_TypeDef* const uartbase =
			(USART_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	size_t xReturn = 0U;

	if( diGET_RX_TRANSFER_STRUCT( pxPeripheralControl ) == NULL )
	{
		#if ioconfigUSE_UART_POLLED_RX == 1
		{
			/* rcv the data, we will block as this is happening */
			uint8_t *dat = (uint8_t *)pvBuffer;

			for(uint32_t i = 0 ; i < xBytes ; ++i)
			{
				while(USART_GetFlagStatus(uartbase, USART_FLAG_RXNE) == RESET);
				dat[i] = (uint8_t)USART_ReceiveData(uartbase);
			}
			xReturn = xBytes;
		}
		#endif /* ioconfigUSE_UART_POLLED_RX */
	}
	else
	{
		switch(diGET_RX_TRANSFER_TYPE(pxPeripheralControl))
		{
		case ioctlUSE_CIRCULAR_BUFFER_RX:
		{
#if ioconfigUSE_UART_CIRCULAR_BUFFER_RX == 1
			/* There is nothing to prevent multiple tasks attempting to
			read the circular buffer at any one time.  The implementation
			of the circular buffer uses a semaphore to indicate when new
			data is available, and the semaphore will ensure that only the
			highest priority task that is attempting a read will actually
			receive bytes. */
			ioutilsRECEIVE_CHARS_FROM_CIRCULAR_BUFFER
				(
					pxPeripheralControl,
					USART_ITConfig(uartbase, USART_IT_RXNE, DISABLE),
					USART_ITConfig(uartbase, USART_IT_RXNE, ENABLE),
					((uint8_t *)pvBuffer),							/* Data destination. */
					xBytes,												/* Bytes to read. */
					xReturn												/* Number of bytes read. */
				);
#endif
		}
		break;

		case ioctlUSE_CHARACTER_QUEUE_RX:
		{
#if ioconfigUSE_UART_RX_CHAR_QUEUE == 1
			/* The queue allows multiple tasks to attempt to read
			bytes, but ensures only the highest priority of these
			tasks will actually receive bytes.  If two tasks of equal
			priority attempt to read simultaneously, then the
			application must ensure mutual exclusion, as time slicing
			could result in the string being received being partially
			received by each task. */
			xReturn = xIOUtilsReceiveCharsFromRxQueue(pxPeripheralControl,
						(uint8_t *)pvBuffer, xBytes);
#endif
		}
		break;

		default:
		{
			/* Other methods can be implemented here. */
			configASSERT(xReturn);

			/* Prevent compiler warnings when the configuration is set such
			that the following parameters are not used. */
			(void)pvBuffer;
			(void)xBytes;
			(void)uartbase;
			break;
		}
		}
	}

	return xReturn;
}

/*
 * perform mode settings on device
 */
portBASE_TYPE FreeRTOS_UART_ioctl(Peripheral_Descriptor_t pxPeripheral,
								  uint32_t ulRequest, void *pvValue)
{
	Peripheral_Control_t * const pxPeripheralControl =
			(Peripheral_Control_t* const)pxPeripheral;
	uint32_t ulValue = (uint32_t)pvValue;
	const int8_t cPeripheralNumber =
			diGET_PERIPHERAL_NUMBER(((Peripheral_Control_t * const)pxPeripheral));
	USART_TypeDef* const uartbase =
			(USART_TypeDef *)diGET_PERIPHERAL_BASE_ADDRESS(pxPeripheralControl);
	portBASE_TYPE xReturn = pdPASS;

	taskENTER_CRITICAL();
	{
		switch(ulRequest)
		{
		case ioctlUSE_INTERRUPTS:
		{
			NVIC_InitTypeDef nvic;
			nvic.NVIC_IRQChannel = xIRQ[cPeripheralNumber];
			nvic.NVIC_IRQChannelPreemptionPriority =
					configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
			nvic.NVIC_IRQChannelSubPriority = 0;

			if (ulValue == pdFALSE)
			{
				nvic.NVIC_IRQChannelCmd = DISABLE;
			}
			else
			{
				/* enable interrupt at min priority */
				nvic.NVIC_IRQChannelCmd = ENABLE;
				nvic.NVIC_IRQChannelPreemptionPriority =
						configLIBRARY_LOWEST_INTERRUPT_PRIORITY;
				nvic.NVIC_IRQChannelSubPriority = 0;

				/* If the Rx is configured to use interrupts, remember the
				transfer control structure that should be used.  A reference
				to the Tx transfer control structure is taken when a write()
				operation is actually performed. */
				pxRxTransferControlStructs[cPeripheralNumber] =
						pxPeripheralControl->pxRxControl;
			}
			NVIC_Init(&nvic);
		}
		break;

		case ioctlSET_SPEED:
		{
			USART_InitTypeDef usart;
			USART_StructInit(&usart);
			usart.USART_BaudRate = ulValue;
			USART_Init(uartbase, &usart);
		}
		break;

		case ioctlSET_INTERRUPT_PRIORITY:
		{
			configASSERT(ulValue >= configLIBRARY_LOWEST_INTERRUPT_PRIORITY);
			NVIC_InitTypeDef nvic;
			nvic.NVIC_IRQChannel = xIRQ[cPeripheralNumber];
			nvic.NVIC_IRQChannelCmd = ENABLE;
			nvic.NVIC_IRQChannelPreemptionPriority = ulValue;
			nvic.NVIC_IRQChannelSubPriority = 0;
			NVIC_Init(&nvic);
		}
		break;

		default:
		{
			xReturn = pdFAIL;
		}
		}
	}
	taskEXIT_CRITICAL();

	return xReturn;
}

/*
 * fills the fifo dma buffer
 */
static inline size_t prvFillFifoFromBuffer(DMA_Descriptions_t const * const desc,
										   uint8_t **ppucBuffer,
										   const size_t xTotalBytes)
{
	USART_TypeDef * const baseaddr = desc->baseaddress;
	uint32_t const dmachan = desc->txdmachannel;
	DMA_Stream_TypeDef const * const dmaystreamx =
			(DMA_Stream_TypeDef*)desc->txdmastream;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

	/*
	 * pause the stream before making changes to it
	 */
	USART_DMACmd((USART_TypeDef *)baseaddr, USART_DMAReq_Tx, DISABLE);
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
	baseaddr->SR &= ~USART_FLAG_TC;
	DMA_Cmd((DMA_Stream_TypeDef *)dmaystreamx, ENABLE);

	return xTotalBytes;
}

/*
 * uart irq handler, needs to be called by the actual uart code
 */
void UART_IRQHandler(USART_TypeDef *usartbase,
		const unsigned portBASE_TYPE uartnum)
{
	uint32_t ulrcv = 0;
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
	Transfer_Control_t *pxTransferStruct;

	if (USART_GetITStatus(usartbase, USART_IT_RXNE))
	{
		pxTransferStruct = pxRxTransferControlStructs[uartnum];
		if (pxTransferStruct)
		{
			switch(diGET_TRANSFER_TYPE_FROM_CONTROL_STRUCT(pxTransferStruct))
			{
			case ioctlUSE_CIRCULAR_BUFFER_RX:
			{
#if ioconfigUSE_UART_CIRCULAR_BUFFER_RX == 1
				ioutilsRX_CHARS_INTO_CIRCULAR_BUFFER_FROM_ISR(
						pxTransferStruct,
						(usartbase->SR & USART_SR_RXNE),
						(usartbase->DR),
						ulrcv,
						xHigherPriorityTaskWoken
					);
#endif
			}
			break;
			case ioctlUSE_CHARACTER_QUEUE_RX:
			{
#if ioconfigUSE_UART_RX_CHAR_QUEUE == 1
				ioutilsRX_CHARS_INTO_QUEUE_FROM_ISR(
						pxTransferStruct,
						(usartbase->SR & USART_SR_RXNE),
						(usartbase->DR),
						ulrcv,
						xHigherPriorityTaskWoken
					);
#endif
			}
			break;
			default:
			{
				/* umm, you have a transfer control type we dont support */
				configASSERT(xHigherPriorityTaskWoken);
			}
			}

			USART_ClearITPendingBit(usartbase, USART_IT_RXNE);
		}
	}
	if (USART_GetITStatus(usartbase, USART_IT_TC))
	{
		pxTransferStruct = pxTxTransferControlStructs[uartnum];
		if (pxTransferStruct)
		{
			switch(diGET_TRANSFER_TYPE_FROM_CONTROL_STRUCT(pxTransferStruct))
			{
			case ioctlUSE_ZERO_COPY_TX:
			{
				/*
				 * release the mutex, this notifies the caller that the buffer
				 * is ready to be freed
				 */
				Zero_Copy_Tx_State_t *pxzcstate =
						(Zero_Copy_Tx_State_t *)
						pxTransferStruct->pvTransferState;
				xSemaphoreGiveFromISR(pxzcstate->xWriteAccessMutex,
						&xHigherPriorityTaskWoken);
			}
			break;
			case ioctlUSE_CHARACTER_QUEUE_TX:
			{
#if ioconfigUSE_UART_TX_CHAR_QUEUE == 1
				uint32_t c = 0;

				ioutilsTX_CHARS_FROM_QUEUE_FROM_ISR(
						pxTransferStruct,
						(c++ == 0),
						(usartbase->DR = ucChar),
						xHigherPriorityTaskWoken
					);
#endif
			}
			break;
			default:
			{
				/* umm, you have a transfer control type we dont support */
				configASSERT(xHigherPriorityTaskWoken);
			}
			}

			USART_ClearITPendingBit(usartbase, USART_IT_TC);
		}
	}

	/* the ulrcv param is not used */
	(void)ulrcv;

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
