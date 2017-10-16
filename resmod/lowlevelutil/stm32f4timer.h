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
class TimerBase;

struct TimerBaseParams{
  TIM_TypeDef *timbase;
};

enum class OutputCompareChannel {
  kCH1,
  kCH2,
  kCH3,
  kCH4
};

enum class OutputCompareMode {
  kOUTPUT,
  kTIMING
};

// Simple helper class for setting up output compares in output toggle mode
class TimerOutputCompare {
 public:
  TimerOutputCompare(TimerBase &tb, OutputCompareChannel ch);
 ~TimerOutputCompare();

  /* unused */
  TimerOutputCompare(const TimerOutputCompare&) = delete;
  TimerOutputCompare& operator=(const TimerOutputCompare&) = delete;

  void Start();
  void Stop();
  void EnableDMATrigger();
  void DisableDMATrigger();
  void ForceOutputLow();
  void ForceOutputHigh();

  void ConfigureCompare(uint32_t const compareval);
  void ConfigureCompare(uint32_t const * const arrcomp, uint32_t const elem,
                        DMA_Stream_TypeDef * dysx, uint32_t const channel);

 private:
  struct OCFunctions {
    uint16_t TIM_Channel;
    void (*TIM_OCInit)(TIM_TypeDef* TIMx, TIM_OCInitTypeDef* TIM_OCInitStruct);
    void (*TIM_SetCompare)(TIM_TypeDef* TIMx, uint32_t Compare);
    void (*TIM_ForcedOCConfig)(TIM_TypeDef* TIMx, uint16_t TIM_ForcedAction);
    uint16_t DMA_CCRx;
    uint16_t DMA_CMD_CCx;
  };

  TimerBase &timebase_;
  TIM_TypeDef * tim_;
  OCFunctions fnptr_;
  DMA_Stream_TypeDef * dmaysx_;
  uint32_t * oclistparams_;
  uint32_t elements_;

  void InitDMACCx(const uint32_t channel);
  void InitBaseOC(const uint32_t compareval);
};

// Simple helper class for setting up timer bases
class TimerBase {
 public:
  TimerBase(const TimerBaseParams &tparams);
  ~TimerBase();

  /* unused */
  TimerBase(const TimerBase&) = delete;
  TimerBase& operator=(const TimerBase&) = delete;

  void Start();
  void Stop();
  void Reset();

  uint32_t PeripheralFrequency();
  void Configure(uint32_t frequency);

 private:
  enum class BusName {
    APB1,
    APB2
  };

  struct TimerBusDetails {
    TIM_TypeDef *timbase;
    BusName bus;
    uint32_t periphcommand;
  };

  struct PeriphLibraryHooks {
    void (*RCC_APBPeriphClockCmd)(uint32_t RCC_APB1Periph,
        FunctionalState NewState);
  };

  static const TimerBusDetails timerbusdetails_[14];
  PeriphLibraryHooks libhooks_;
  TimerBaseParams timerparams_;
  uint32_t busdetailsindex_;

 private:
  const TimerBusDetails& BusDetails() const;

  friend class TimerOutputCompare;
};
}
#endif /* LOWLEVELUTIL_STM32F4TIMER_H_ */
