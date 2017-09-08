/******************************************************************************
 |
 |  	FILENAME:  stm32f4timer.h
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
#ifndef LOWLEVELUTIL_STM32F4TIMER_H_
#define LOWLEVELUTIL_STM32F4TIMER_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
/* Namespace declaration -----------------------------------------------------*/
namespace stm32f4 {

/* Class definition ----------------------------------------------------------*/
struct STM32F4TimerParams{
  TIM_TypeDef timbase;
};

class STM32F4TimerOutputGenerator {
  STM32F4TimerOutputGenerator(const STM32F4TimerParams &tparams);
  ~STM32F4TimerOutputGenerator();

  /* unused */
  STM32F4TimerOutputGenerator(const STM32F4TimerOutputGenerator&) = delete;
  STM32F4TimerOutputGenerator& operator=(const STM32F4TimerOutputGenerator&) =
      delete;

 public:
  void Start();
  void Stop();

  uint32_t PeripheralFrequency();
  void Configure();

 private:
  enum class BusName {
    APB1,
    APB2
  };

  struct TimerBusDetails {
    TIM_TypeDef timbase;
    BusName bus;
  };
  static const TimerBusDetails timerbusdetails_[14];

  STM32F4TimerParams timerparams_;
  uint32_t busdetailsindex_;
};
}
#endif /* LOWLEVELUTIL_STM32F4TIMER_H_ */
