/******************************************************************************
 |
 | 	FILENAME:  stm32f4timer.cc
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
 |	    DATE:		Sep 8, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace stm32f4;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const TimerBase::TimerBusDetails
TimerBase::timerbusdetails_[14] = {
    {TIM1, BusName::APB2, RCC_APB2Periph_TIM1},
    {TIM2, BusName::APB1, RCC_APB1Periph_TIM2},
    {TIM3, BusName::APB1, RCC_APB1Periph_TIM3},
    {TIM4, BusName::APB1, RCC_APB1Periph_TIM4},
    {TIM5, BusName::APB1, RCC_APB1Periph_TIM5},
    {TIM6, BusName::APB1, RCC_APB1Periph_TIM6},
    {TIM7, BusName::APB1, RCC_APB1Periph_TIM7},
    {TIM8, BusName::APB2, RCC_APB2Periph_TIM8},
    {TIM9, BusName::APB2, RCC_APB2Periph_TIM9},
    {TIM10, BusName::APB2, RCC_APB2Periph_TIM10},
    {TIM11, BusName::APB2, RCC_APB2Periph_TIM11},
    {TIM12, BusName::APB1, RCC_APB1Periph_TIM12},
    {TIM13, BusName::APB1, RCC_APB1Periph_TIM13},
    {TIM14, BusName::APB1, RCC_APB1Periph_TIM14}
};
/* Constructor(s) / Destructor -----------------------------------------------*/
TimerBase::TimerBase(
    const TimerBaseParams& tparams) : timerparams_(tparams) {
  uint32_t index = 0;

  for (const TimerBase::TimerBusDetails & a :
      timerbusdetails_) {
    if (a.timbase == tparams.timbase) {
      busdetailsindex_ = index;
      break;
    }
    ++index;
  }

  // Load function pointers
  const TimerBusDetails& busdetails = BusDetails();
  switch(busdetails.bus) {
    case BusName::APB1: {
      libhooks_.RCC_APBPeriphClockCmd = RCC_APB1PeriphClockCmd;
      break;
    }

    case BusName::APB2: {
      libhooks_.RCC_APBPeriphClockCmd = RCC_APB2PeriphClockCmd;
      break;
    }
  }

  libhooks_.RCC_APBPeriphClockCmd(busdetails.periphcommand, ENABLE);
}

TimerBase::~TimerBase() {
  const TimerBusDetails& busdetails = BusDetails();

  Stop();
  libhooks_.RCC_APBPeriphClockCmd(busdetails.periphcommand, DISABLE);
}


/* Public methods ------------------------------------------------------------*/
void
TimerBase::Start() {
  const TimerBusDetails& busdetails = BusDetails();
  TIM_Cmd(busdetails.timbase, ENABLE);
}

void
TimerBase::Stop() {
  const TimerBusDetails& busdetails = BusDetails();
  TIM_Cmd(busdetails.timbase, DISABLE);
}

uint32_t
TimerBase::PeripheralFrequency() {
  uint32_t pfreq;
  RCC_ClocksTypeDef clocks;
  const TimerBusDetails& busdetails = BusDetails();

  RCC_GetClocksFreq(&clocks);
  switch(busdetails.bus) {
    case BusName::APB1: {
      pfreq = clocks.PCLK1_Frequency;
      break;
    }

    case BusName::APB2: {
      pfreq = clocks.PCLK2_Frequency;
      break;
    }

    default: {
      pfreq = 0;
    }
  }

  return pfreq * 2; // timer clocks operate at twice the APB freq
}

void
TimerBase::Configure(uint32_t freq) {
  const TimerBusDetails& busdetails = BusDetails();
  TIM_TimeBaseInitTypeDef timerparams;
  uint32_t apbfreq = PeripheralFrequency();

  TIM_TimeBaseStructInit(&timerparams);
  timerparams.TIM_ClockDivision = TIM_CKD_DIV1;
  timerparams.TIM_Prescaler = (apbfreq / 1E6) - 1;
  timerparams.TIM_Period = (1.0 / freq / 1.0E-6) - 1;
  timerparams.TIM_RepetitionCounter = 0;
  timerparams.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(busdetails.timbase, &timerparams);

//  TIM_OCInitTypeDef timerocparams;
//  TIM_OCStructInit(&timerocparams);
//  timerocparams.TIM_OCMode = TIM_OCMode_Toggle;
//  timerocparams.TIM_OutputState = TIM_OutputState_Enable;
//  TIM_OC1Init(busdetails.timbase, &timerocparams);

  TIM_InternalClockConfig(busdetails.timbase);
}

/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
const TimerBase::TimerBusDetails&
TimerBase::BusDetails() const {
  return timerbusdetails_[busdetailsindex_];
}
