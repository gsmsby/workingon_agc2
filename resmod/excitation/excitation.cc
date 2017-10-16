/******************************************************************************
 |
 | 	FILENAME:  excitation.cc
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
 |	    DATE:		Sep 18, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "excitation.h"

#include <algorithm>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const uint32_t Excitation::outputcompare_[] = {5, 0};
const uint16_t Excitation::sineout_[] = {32767,  34745,  36717,  38673,  40609,  42515,  44386,  46215,  47995,  49719,  51381,  52975,  54496,  55937,  57293,  58561,  59734,  60808,  61781,  62647,  63405,  64050,  64582,  64997,  65295,  65474,  65534,  65474,  65295,  64997,  64582,  64050,  63405,  62647,  61781,  60808,  59734,  58561,  57293,  55937,  54496,  52975,  51381,  49719,  47995,  46215,  44386,  42515,  40609,  38673,  36717,  34745,  32767,  30789,  28817,  26861,  24925,  23019,  21148,  19319,  17539,  15815,  14153,  12559,  11038,  9597,  8241,  6973,  5800,  4726,  3753,  2887,  2129,  1484,  952,  537,  239,  60,  0,  60,  239,  537,  952,  1484,  2129,  2887,  3753,  4726,  5800,  6973,  8241,  9597,  11038,  12559,  14153,  15815,  17539,  19319,  21148,  23019,  24925,  26861,  28817,  30789};
/* Constructor(s) / Destructor -----------------------------------------------*/
Excitation::Excitation(ExcitationCallback cb)
:dactimebase_(stm32f4::TimerBaseParams{TIM5}), dmaroll_(cb) {
  dactimebase_.Configure(100E3);
//
//  uint16_t * pdat = new uint16_t[101];
//  std::copy(sineout_, sineout_ + 101, pdat);

  // Time Channel 4 CS signal
  stm32f4::TimerOutputCompare dac_timocch4(dactimebase_,
                                           stm32f4::OutputCompareChannel::
                                           kCH4);
  dac_timocch4.ConfigureCompare(outputcompare_, 2, DMA1_Stream1,
                                DMA_Channel_6);

  // DMA with TIM Channel 1 trigger signal
  DMA_InitTypeDef dmaparams;
  DMA_StructInit(&dmaparams);
  dmaparams.DMA_Channel = DMA_Channel_6;
  dmaparams.DMA_PeripheralBaseAddr = (uint32_t)(&SPI3->DR);
  dmaparams.DMA_Memory0BaseAddr = (uint32_t)(sineout_);
  dmaparams.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  dmaparams.DMA_BufferSize = sizeof(sineout_) / sizeof(uint16_t);
  dmaparams.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaparams.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmaparams.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmaparams.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmaparams.DMA_Mode = DMA_Mode_Circular;
  dmaparams.DMA_Priority = DMA_Priority_VeryHigh;
  dmaparams.DMA_FIFOMode = DMA_FIFOMode_Enable;
  dmaparams.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  dmaparams.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  dmaparams.DMA_MemoryBurst = DMA_MemoryBurst_INC8;
  DMA_Init(DMA1_Stream2, &dmaparams);
//  DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA1_Stream2, ENABLE);

  stm32f4::TimerOutputCompare dac_timocch1(dactimebase_,
                                           stm32f4::OutputCompareChannel::
                                           kCH1);
  dac_timocch1.ConfigureCompare(6);
  dac_timocch1.EnableDMATrigger();

  // SPI 3 to output data
  SPI_InitTypeDef spiinit;
  SPI_StructInit(&spiinit);
  spiinit.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  spiinit.SPI_DataSize = SPI_DataSize_16b;
  spiinit.SPI_CPHA = 0;
  spiinit.SPI_CPOL = 0;
  spiinit.SPI_FirstBit = SPI_FirstBit_MSB;
  spiinit.SPI_Mode = SPI_Mode_Master;
  spiinit.SPI_NSS = SPI_NSS_Soft;
  spiinit.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  //SPI_DMACmd(spibus_, SPI_DMAReq_Rx, ENABLE);
  SPI_Init(SPI3, &spiinit);
  SPI_Cmd(SPI3, ENABLE);

  dac_timocch4.Start();
  dac_timocch1.Start();
}

Excitation::~Excitation() {
  Stop();
}
/* Public methods ------------------------------------------------------------*/
void
Excitation::Start() {
  dactimebase_.Start();
}

void
Excitation::Stop() {
  dactimebase_.Stop();
}

void
Excitation::DMAComplete() {
  if (dmaroll_)
    dmaroll_();

  DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, DISABLE);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
