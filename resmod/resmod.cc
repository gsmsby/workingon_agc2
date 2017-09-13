
/******************************************************************************
 |
 | 	FILENAME:  resmod.cc
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
#include "resmod.h"

#include <stm32f4xx.h>

#include <resmod/lowlevelutil/stm32f4gpio.h>
#include <resmod/lowlevelutil/stm32f4nvic.h>
#include <resmod/lowlevelutil/stm32f4timer.h>
#include <resmod/acquisition/ltc2336spidmatrigger.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
LTC2336SPIDMATrigger *g_adcitrigger;
/* Constructor(s) / Destructor -----------------------------------------------*/
ResmodMainTask::ResmodMainTask() {
  xTaskCreate(ResmodTask, "resmod_main", 2048 / sizeof(portSTACK_TYPE),
              this, 1, &taskhandle_);
}

ResmodMainTask::~ResmodMainTask() {
}

/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void ResmodMainTask::ResmodTask(void *inst) {
  //ResmodMainTask &instance = *((ResmodMainTask *)inst);

  // Configure gpio
  // Clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

  // Conv signals for ADC
  stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_8, GPIO_AF_TIM1);
  stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_0, GPIO_AF_TIM2);
  // Busy signals for ADC
  stm32f4::GPIOConfiguration::MakeEXTI(GPIOC, GPIO_Pin_0, EXTI_Line0,
                                       EXTI_PortSourceGPIOC,
                                       EXTI_Trigger_Falling);
  stm32f4::GPIOConfiguration::MakeEXTI(GPIOC, GPIO_Pin_1, EXTI_Line1,
                                       EXTI_PortSourceGPIOC,
                                       EXTI_Trigger_Falling);

  // SPI1
  stm32f4::GPIOConfiguration::MakeAF(GPIOA,
                                     GPIO_Pin_5 | // SPI1CLK
                                     GPIO_Pin_6,  // SPI1MISO
                                     GPIO_AF_SPI1);
  // SPI2
  stm32f4::GPIOConfiguration::MakeAF(GPIOB,
                                     GPIO_Pin_10, // SPI2CLK
                                     GPIO_AF_SPI2);
  stm32f4::GPIOConfiguration::MakeAF(GPIOC,
                                     GPIO_Pin_2,  // SPI2MISO
                                     GPIO_AF_SPI2);
  // SPI3
  stm32f4::GPIOConfiguration::MakeAF(GPIOC,
                                     GPIO_Pin_10 | // SPI3CLK
                                     GPIO_Pin_12,  // SPI3MOSI
                                     GPIO_AF_SPI3);

  // ADC Timers
  stm32f4::TimerBaseParams paramstim1;
  paramstim1.timbase = TIM1;
  stm32f4::TimerBase adcv_conv(paramstim1);
  adcv_conv.Configure(200E3);
  stm32f4::TimerBaseParams paramstim2;
  paramstim2.timbase = TIM2;
  stm32f4::TimerBase adci_conv(paramstim2);
  adci_conv.Configure(200E3);
  stm32f4::TimerOutputCompare tim3ch1(adcv_conv,
                                      stm32f4::OutputCompareChannel::kCH1);
  tim3ch1.ConfigureCompare(0);
  tim3ch1.Start();

  stm32f4::TimerOutputCompare tim2ch1(adci_conv,
                                      stm32f4::OutputCompareChannel::kCH1);
  tim2ch1.ConfigureCompare(0);
  tim2ch1.Start();

  // DAC Timer
  stm32f4::TimerBaseParams paramstim3;
  paramstim3.timbase = TIM3;
  stm32f4::TimerBase timer_dac(paramstim3);
  adcv_conv.Configure(100E3);

  LTC2336SPIDMATrigger adcitrigger(nullptr, DMA1_Stream3, DMA_Channel_0, SPI2,
                                   300);
  g_adcitrigger = &adcitrigger;

  stm32f4::NVICConfiguration::Enable(IRQn::EXTI0_IRQn, 3);
//  stm32f4::NVICConfiguration::Enable(IRQn::EXTI1_IRQn, 3);
  stm32f4::NVICConfiguration::Enable(IRQn::DMA1_Stream3_IRQn, 3);

  SPI_DMACmd(SPI2, SPI_DMAReq_Rx, ENABLE);

  // Start the ADCs
  adcv_conv.Start();
  adci_conv.Start();

  for( ; ; ) {
    vTaskDelay(1000);
  }
}
