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
#include "stm32f4timer.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace stm32f4;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
TimerOutputCompare::TimerOutputCompare(TimerBase &tb,
                                       OutputCompareChannel ch)
:timebase_(tb) {
  switch(ch) {
    case OutputCompareChannel::kCH1: {
      fnptr_.TIM_Channel = TIM_Channel_1;
      fnptr_.TIM_OCInit = TIM_OC1Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare1;
      break;
    }
    case OutputCompareChannel::kCH2: {
      fnptr_.TIM_Channel = TIM_Channel_2;
      fnptr_.TIM_OCInit = TIM_OC2Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare2;
      break;
    }
    case OutputCompareChannel::kCH3: {
      fnptr_.TIM_Channel = TIM_Channel_3;
      fnptr_.TIM_OCInit = TIM_OC3Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare3;
      break;
    }
    case OutputCompareChannel::kCH4: {
      fnptr_.TIM_Channel = TIM_Channel_4;
      fnptr_.TIM_OCInit = TIM_OC4Init;
      fnptr_.TIM_SetCompare = TIM_SetCompare4;
      break;
    }
  }
}

TimerOutputCompare::~TimerOutputCompare() {
}
/* Public methods ------------------------------------------------------------*/
void
TimerOutputCompare::Start() {
  const TimerBase::TimerBusDetails& busdetails = timebase_.BusDetails();
  TIM_CCxCmd(busdetails.timbase, fnptr_.TIM_Channel, TIM_CCx_Enable);
}

void
TimerOutputCompare::Stop() {
  const TimerBase::TimerBusDetails& busdetails = timebase_.BusDetails();
  TIM_CCxCmd(busdetails.timbase, fnptr_.TIM_Channel, TIM_CCx_Disable);
}

void
TimerOutputCompare::ConfigureCompare(const uint32_t compareval) {
  const TimerBase::TimerBusDetails& busdetails = timebase_.BusDetails();
  TIM_OCInitTypeDef timerocparams;
  TIM_OCStructInit(&timerocparams);
  timerocparams.TIM_OCMode = TIM_OCMode_Toggle;
  timerocparams.TIM_OutputState = TIM_OutputState_Enable;
  timerocparams.TIM_Pulse = compareval;
  fnptr_.TIM_OCInit(busdetails.timbase, &timerocparams);
}

/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
