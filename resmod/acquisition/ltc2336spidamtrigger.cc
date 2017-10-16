/******************************************************************************
 |
 | 	FILENAME:  spidamtrigger.cc
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
 |	    DATE:		Sep 11, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <algorithm>

#include <resmod/acquisition/ltc2336spidmatrigger.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
LTC2336SPIDMATrigger::LTC2336SPIDMATrigger(SPIDMATriggerCallback callback,
                                           DMA_Stream_TypeDef *dmay_streamx,
                                           uint32_t const channel,
                                           SPI_TypeDef *spibus,
                                           uint32_t const samplesize)
:spibus_(spibus), acquisitioncount_(samplesize),
 buff_(new int16_t[samplesize]), triggercallback_(callback),
 dmay_streamx_(dmay_streamx) {
  std::fill(buff_, buff_ + acquisitioncount_, 0);

  dmatypedef_.DMA_Channel = channel;
  dmatypedef_.DMA_PeripheralBaseAddr = (uint32_t)&spibus_->DR;
  dmatypedef_.DMA_Memory0BaseAddr = (uint32_t)buff_;
  dmatypedef_.DMA_DIR = DMA_DIR_PeripheralToMemory;
  dmatypedef_.DMA_BufferSize = acquisitioncount_;
  dmatypedef_.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmatypedef_.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmatypedef_.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmatypedef_.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmatypedef_.DMA_Mode = DMA_Mode_Normal;
  dmatypedef_.DMA_FIFOMode = DMA_FIFOMode_Enable;
  dmatypedef_.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  dmatypedef_.DMA_Priority = DMA_Priority_High;
  dmatypedef_.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
  dmatypedef_.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(dmay_streamx, &dmatypedef_);
  DMA_ITConfig(dmay_streamx, DMA_IT_TC, ENABLE);
  DMA_Cmd(dmay_streamx, ENABLE);

  SPI_InitTypeDef spiinit;
  SPI_StructInit(&spiinit);
  spiinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spiinit.SPI_DataSize = SPI_DataSize_16b;
  spiinit.SPI_CPHA = 0;
  spiinit.SPI_CPOL = 0;
  spiinit.SPI_FirstBit = SPI_FirstBit_MSB;
  spiinit.SPI_Mode = SPI_Mode_Master;
  spiinit.SPI_NSS = SPI_NSS_Soft;
  spiinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_DMACmd(spibus_, SPI_DMAReq_Rx, ENABLE);
  SPI_Init(spibus_, &spiinit);
}
/* Public methods ------------------------------------------------------------*/
void
LTC2336SPIDMATrigger::DMAComplete() {
  SPI_Cmd(spibus_, DISABLE);

  // Reset DMA settings
  dmay_streamx_->M0AR = (uint32_t)buff_;
  dmay_streamx_->NDTR = acquisitioncount_;

  if (triggercallback_) {
    triggercallback_(buff_, acquisitioncount_);
  }
}

void
LTC2336SPIDMATrigger::Trigger() {
  // Enable DMA
  dmay_streamx_->CR |= DMA_SxCR_EN;
  SPI_I2S_ReceiveData(spibus_);
  SPI_Cmd(spibus_, ENABLE);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
