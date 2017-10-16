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
#include <resmod/excitation/excitation.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern resmod::LTC2336SPIDMATrigger *g_adcvtrigger;
extern resmod::LTC2336SPIDMATrigger *g_adcitrigger;
extern resmod::Excitation *g_excitation;

extern "C" {
void DMA1_Stream2_IRQHandler();
void DMA1_Stream3_IRQHandler();
void DMA2_Stream0_IRQHandler();
} // extern "C"

void DMA1_Stream2_IRQHandler() {
//  SEGGER_SYSVIEW_RecordEnterISR();
  DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
  g_excitation->DMAComplete();
//  SEGGER_SYSVIEW_RecordExitISR();
}

void DMA1_Stream3_IRQHandler() {
//  SEGGER_SYSVIEW_RecordEnterISR();
  DMA_ClearITPendingBit(DMA1_Stream3, DMA_IT_TCIF3);
  g_adcitrigger->DMAComplete();
//  SEGGER_SYSVIEW_RecordExitISR();
}

void DMA2_Stream0_IRQHandler() {
//  SEGGER_SYSVIEW_RecordEnterISR();
  DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
  g_adcvtrigger->DMAComplete();
//  SEGGER_SYSVIEW_RecordExitISR();
}
/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
