/******************************************************************************
 |
 | 	FILENAME:  dma1.cc
 |
 |	Copyright 2017 Adara Systems Ltd. as an unpublished work.
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
 |	    DATE:		Sep 12, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

#include <segger/SEGGER_SYSVIEW.h>
#include <resmod/acquisition/ltc2336spidmatrigger.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern resmod::LTC2336SPIDMATrigger *g_adcitrigger;

extern "C" {
void DMA1_Stream3_IRQHandler();
} // extern "C"

void DMA1_Stream3_IRQHandler() {
  DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
  g_adcitrigger->DMAComplete();
}
/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
