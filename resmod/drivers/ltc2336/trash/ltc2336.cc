/******************************************************************************
 |
 | 	FILENAME:  ltc2336.cc
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
 |	    DATE:		Sep 6, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "ltc2336.h"
/* Private typedef -----------------------------------------------------------*/
using namespace resmod;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
LTC2336::LTC2336(const LTC2336Params& params)
:params_(params), busy_(false), transfersetpoint_(0), transfersremaining_(0) {
  // Set up timer that provides conv signals

  // Set up dma transfer
  DMA_StructInit(&dma_params_);
  dma_params_.DMA_PeripheralBaseAddr = (uint32_t)params_.spibase;
  dma_params_.DMA_DIR = DMA_DIR_PeripheralToMemory;
  dma_params_.DMA_BufferSize = 1;
  dma_params_.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dma_params_.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dma_params_.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  dma_params_.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  dma_params_.DMA_Mode = DMA_Mode_Normal;
  dma_params_.DMA_Priority = DMA_Priority_High;
  dma_params_.DMA_FIFOMode = DMA_FIFOMode_Disable;
}

LTC2336::~LTC2336() {

}

/* Public methods ------------------------------------------------------------*/
int8_t
LTC2336::Start() {
}

int8_t
LTC2336::Stop() {
}

int8_t
LTC2336::Reset() {
}

int8_t
LTC2336::SetMaximumSampleCount(uint32_t samplecount) {
  transfersetpoint_ = samplecount;
}

int8_t
LTC2336::SetDataDestination(VTable_t& vtable) {
  destination_arr_ = &vtable;
}

void
LTC2336::RegisterTransferCompleteEvent(CallBackInterface * callback) {
  transfercompletecallbacks_.push_back(callback);
}

void
LTC2336::BusyISR(LTC2336& instance) {

}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
