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
const STM32F4TimerOutputGenerator::TimerBusDetails
STM32F4TimerOutputGenerator::timerbusdetails_[14] = {
    {TIM1, BusName::APB2},
    {TIM2, BusName::APB1},
    {TIM3, BusName::APB1},
    {TIM4, BusName::APB1},
    {TIM5, BusName::APB1},
    {TIM6, BusName::APB1},
    {TIM7, BusName::APB1},
    {TIM8, BusName::APB2},
    {TIM9, BusName::APB2},
    {TIM10, BusName::APB2},
    {TIM11, BusName::APB2},
    {TIM12, BusName::APB1},
    {TIM13, BusName::APB1},
    {TIM14, BusName::APB1}
};
/* Constructor(s) / Destructor -----------------------------------------------*/
STM32F4TimerOutputGenerator::STM32F4TimerOutputGenerator(
    const STM32F4TimerParams& tparams)
:timerparams_(tparams) {
  uint32_t index = 0;

  for (const STM32F4TimerOutputGenerator::TimerBusDetails & a :
      timerbusdetails_) {
    if (a.timbase == tparams.timbase) {
      busdetailsindex_ = index;
      break;
    }
    ++index;
  }

  switch(timerbusdetails_[busdetailsindex_]) {

  }
}

STM32F4TimerOutputGenerator::~STM32F4TimerOutputGenerator() {
}


/* Public methods ------------------------------------------------------------*/
void
STM32F4TimerOutputGenerator::Start() {
}

void
STM32F4TimerOutputGenerator::Stop() {
}

uint32_t
STM32F4TimerOutputGenerator::PeripheralFrequency() {
}

void
STM32F4TimerOutputGenerator::Configure() {
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
