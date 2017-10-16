/******************************************************************************
 |
 | 	FILENAME:  stm32f4timeroc.cc
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
#include <algorithm>

#include "stm32f4timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace stm32f4;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
TimerOutputCompare::TimerOutputCompare(TimerBase &tb,
                                       OutputCompareChannel ch)
:timebase_(tb), oclistparams_(nullptr), dmaysx_(nullptr) {
  switch(ch) {
    case OutputCompareChannel::kCH1: {
      fnptr_.TIM_Channel = TIM_Channel_1;
      fnptr_.TIM_OCInit = TIM_OC1Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare1;
      fnptr_.TIM_ForcedOCConfig = TIM_ForcedOC1Config;
      fnptr_.DMA_CCRx = TIM_DMABase_CCR1;
      fnptr_.DMA_CMD_CCx = TIM_DMA_CC1;
      break;
    }
    case OutputCompareChannel::kCH2: {
      fnptr_.TIM_Channel = TIM_Channel_2;
      fnptr_.TIM_OCInit = TIM_OC2Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare2;
      fnptr_.TIM_ForcedOCConfig = TIM_ForcedOC2Config;
      fnptr_.DMA_CCRx = TIM_DMABase_CCR2;
      fnptr_.DMA_CMD_CCx = TIM_DMA_CC2;
      break;
    }
    case OutputCompareChannel::kCH3: {
      fnptr_.TIM_Channel = TIM_Channel_3;
      fnptr_.TIM_OCInit = TIM_OC3Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare3;
      fnptr_.TIM_ForcedOCConfig = TIM_ForcedOC3Config;
      fnptr_.DMA_CCRx = TIM_DMABase_CCR3;
      fnptr_.DMA_CMD_CCx = TIM_DMA_CC3;
      break;
    }
    case OutputCompareChannel::kCH4: {
      fnptr_.TIM_Channel = TIM_Channel_4;
      fnptr_.TIM_OCInit = TIM_OC4Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare4;
      fnptr_.TIM_ForcedOCConfig = TIM_ForcedOC4Config;
      fnptr_.DMA_CCRx = TIM_DMABase_CCR4;
      fnptr_.DMA_CMD_CCx = TIM_DMA_CC4;
      break;
    }
  }

  const TimerBase::TimerBusDetails& busdetails = timebase_.BusDetails();
  tim_ = busdetails.timbase;
}

TimerOutputCompare::~TimerOutputCompare() {
}
/* Public methods ------------------------------------------------------------*/
void
TimerOutputCompare::Start() {
  if (oclistparams_) {
    fnptr_.TIM_SetCompare(tim_, oclistparams_[0]);
    dmaysx_->M0AR = (uint32_t)oclistparams_;
    dmaysx_->NDTR = elements_;
    DMA_Cmd(dmaysx_, ENABLE);
  }
  TIM_CCxCmd(tim_, fnptr_.TIM_Channel, TIM_CCx_Enable);
}

void
TimerOutputCompare::Stop() {
  TIM_CCxCmd(tim_, fnptr_.TIM_Channel, TIM_CCx_Disable);
  if (oclistparams_) {
    DMA_Cmd(dmaysx_, DISABLE);
  }
}

void TimerOutputCompare::EnableDMATrigger() {
  TIM_DMACmd(tim_, fnptr_.DMA_CMD_CCx, ENABLE);
}

void TimerOutputCompare::DisableDMATrigger() {
  TIM_DMACmd(tim_, fnptr_.DMA_CMD_CCx, DISABLE);
}

void
TimerOutputCompare::ForceOutputLow() {
  fnptr_.TIM_ForcedOCConfig(tim_, TIM_ForcedAction_InActive);
}

void
TimerOutputCompare::ForceOutputHigh() {
    fnptr_.TIM_ForcedOCConfig(tim_, TIM_ForcedAction_Active);
}

void
TimerOutputCompare::ConfigureCompare(const uint32_t compareval) {
  InitBaseOC(compareval);
}

void
TimerOutputCompare::ConfigureCompare(uint32_t const * const arrcomp,
                                     uint32_t const elem,
                                     DMA_Stream_TypeDef * dysx,
                                     uint32_t const channel) {
  if (oclistparams_) {
    assert_param(false);
  }
  elements_ = elem;
  dmaysx_ = dysx;
  oclistparams_ = new uint32_t[elements_];
  std::copy(arrcomp, arrcomp + elements_, oclistparams_);

  InitBaseOC(oclistparams_[0]);
  TIM_DMAConfig(tim_, fnptr_.DMA_CCRx,
                TIM_DMABurstLength_1Transfer);
  TIM_DMACmd(tim_, fnptr_.DMA_CMD_CCx, ENABLE);

  InitDMACCx(channel);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void TimerOutputCompare::InitDMACCx(const uint32_t channel) {
  // Set up the DMA transfer
  DMA_InitTypeDef dmainit;
  DMA_StructInit(&dmainit);
  dmainit.DMA_Channel = channel;
  dmainit.DMA_PeripheralBaseAddr = (uint32_t)(&tim_->DMAR);
  dmainit.DMA_Memory0BaseAddr = (uint32_t)(oclistparams_);
  dmainit.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  dmainit.DMA_BufferSize = elements_;
  dmainit.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmainit.DMA_MemoryInc = DMA_MemoryInc_Enable;
  dmainit.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  dmainit.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  dmainit.DMA_Mode = DMA_Mode_Circular;
  dmainit.DMA_Priority = DMA_Priority_Medium;
  dmainit.DMA_FIFOMode = DMA_FIFOMode_Disable;
  dmainit.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  dmainit.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(dmaysx_, &dmainit);
}

void TimerOutputCompare::InitBaseOC(const uint32_t compareval) {
  TIM_OCInitTypeDef timerocparams;
  TIM_OCStructInit(&timerocparams);
  timerocparams.TIM_OCMode = TIM_OCMode_Toggle;
  timerocparams.TIM_OutputState = TIM_OutputState_Enable;
  timerocparams.TIM_Pulse = compareval;
  fnptr_.TIM_OCInit(tim_, &timerocparams);
}
