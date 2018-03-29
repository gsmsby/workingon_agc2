
/******************************************************************************
 |
 |  FILENAME:  resmod.cc
 |
 |  Copyright 2017 Adara Systems Ltd. as an unpublished work.
 |  All Rights Reserved.
 |
 |  The information contained herein is confidential property of Adara Systems
 |  Ltd. The use, copying, transfer or disclosure of such information is
 |  prohibited except by express written agreement with Adara Systems Ltd.
 |
 |    DESCRIPTION:
 |
 |    PUBLIC FUNCTIONS:
 |
 |
 |    NOTES:
 |
 |    AUTHOR(S):  Roque
 |      DATE:   Sep 8, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#define V_PGA_G4_Pin GPIO_Pin_13

#define V_PGA_G3_Pin GPIO_Pin_14

#define V_PGA_G2_Pin GPIO_Pin_15


#define I_PGA_G0_Pin GPIO_Pin_7

#define I_PGA_G1_Pin GPIO_Pin_4

#define I_PGA_G2_Pin GPIO_Pin_5

#define I_PGA_G3_Pin GPIO_Pin_0

#define I_PGA_G4_Pin GPIO_Pin_1

#define V_PGA_G0_Pin GPIO_Pin_6

#define V_PGA_G1_Pin GPIO_Pin_7


#include "resmod.h"

#include <stm32f4xx.h>
#include <segger/SEGGER_RTT.h>
#include <FreeRTOS.h>
#include <task.h>

#include <resmod/lowlevelutil/stm32f4gpio.h>
#include <resmod/lowlevelutil/stm32f4nvic.h>
#include <resmod/lowlevelutil/stm32f4timer.h>
#include <resmod/acquisition/ltc2336spidmatrigger.h>
#include <resmod/calcmodule/calctask.h>
#include <resmod/excitation/excitation.h>

#include <resmod/drivers/isl28xxx/isl28634.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;

using namespace drivers;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
CalculationTask * g_calctask;
resmod::Excitation *g_excitation;
LTC2336SPIDMATrigger * g_adcitrigger;
LTC2336SPIDMATrigger * g_adcvtrigger;
stm32f4::TimerBase * g_adcv_timbase;  // Timer base clock
stm32f4::TimerBase * g_adci_timbase;  // Timer base clock

extern uint8_t gain_value;
extern uint8_t gain_flag ;

//drivers::ISL28634  isl28634_i;



/* Constructor(s) / Destructor -----------------------------------------------*/
ResmodMainTask::ResmodMainTask() {
  xTaskCreate(ResmodTask, "resmod_main", 6144 / sizeof(portSTACK_TYPE),
              this, 1, &taskhandle_);
}

ResmodMainTask::~ResmodMainTask() {
}

/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void ADCICallback(int16_t const * const databuff, uint32_t const elements);
void ADCVCallback(int16_t const * const databuff, uint32_t const elements);
void ExcitationRoll();

void ResmodMainTask::ResmodTask(void *inst) {
  ResmodMainTask &instance = *((ResmodMainTask *)inst);

  TIM_DeInit(TIM1);
  TIM_DeInit(TIM2);
  TIM_DeInit(TIM3);
  TIM_DeInit(TIM5);

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
  stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_8, GPIO_AF_TIM1);	//TIM1 Voltage ADC
  stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_0, GPIO_AF_TIM2);	//TIm2 Current ADC

  // CS signal for DAC
  stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_3, GPIO_AF_TIM5);
  //stm32f4::GPIOConfiguration::MakeAF(GPIOA, GPIO_Pin_4, GPIO_AF_TIM5);

  // SPI1
  stm32f4::GPIOConfiguration::MakeAF(GPIOA,
                                     GPIO_Pin_5 | // SPI1CLK
                                     GPIO_Pin_6,  // SPI1MISO
                                     GPIO_AF_SPI1,
                                     GPIO_PuPd_UP);
  // SPI2
  stm32f4::GPIOConfiguration::MakeAF(GPIOB,
                                     GPIO_Pin_10, // SPI2CLK
                                     GPIO_AF_SPI2,
                                     GPIO_PuPd_UP);
  stm32f4::GPIOConfiguration::MakeAF(GPIOC,
                                     GPIO_Pin_2,  // SPI2MISO
                                     GPIO_AF_SPI2,
                                     GPIO_PuPd_UP);
  // SPI3
  stm32f4::GPIOConfiguration::MakeAF(GPIOC,
                                     GPIO_Pin_10 | // SPI3CLK
                                     GPIO_Pin_12,  // SPI3MOSI
                                     GPIO_AF_SPI3,
                                     GPIO_PuPd_UP);


  //PGA281 gain setting
    //pga current gain 64
  /*HAL_GPIO_WritePin(GPIOB, I_PGA_G3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, I_PGA_G2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, I_PGA_G1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, I_PGA_G0_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, I_PGA_G4_Pin, GPIO_PIN_RESET);*/


  stm32f4::GPIOConfiguration::MakeOutput(GPIOB, I_PGA_G4_Pin|I_PGA_G3_Pin);
  stm32f4::GPIOConfiguration::MakeOutput(GPIOC, I_PGA_G2_Pin|I_PGA_G1_Pin);
  stm32f4::GPIOConfiguration::MakeOutput(GPIOA, I_PGA_G0_Pin);

  //pga current gain 176
//  GPIO_SetBits(GPIOB, I_PGA_G4_Pin);
//  GPIO_SetBits(GPIOB, I_PGA_G3_Pin);
//  GPIO_SetBits (GPIOC,I_PGA_G1_Pin);
//
//  GPIO_ResetBits (GPIOC,I_PGA_G2_Pin);
//
//  GPIO_ResetBits (GPIOA,I_PGA_G0_Pin);

  gain_value = 1;
  gain_flag = 0;

  //gain 32

//  GPIO_SetBits(GPIOB, I_PGA_G3_Pin);
//
//  GPIO_ResetBits (GPIOB, I_PGA_G4_Pin);
//  GPIO_ResetBits (GPIOC,I_PGA_G2_Pin);
//  GPIO_ResetBits (GPIOC,I_PGA_G1_Pin);
//  GPIO_ResetBits (GPIOA,I_PGA_G0_Pin);


  //gain 44
//  GPIO_SetBits (GPIOB, I_PGA_G4_Pin);
//  GPIO_SetBits(GPIOB, I_PGA_G3_Pin);
//
//
//  GPIO_ResetBits (GPIOC,I_PGA_G2_Pin);
//  GPIO_ResetBits (GPIOC,I_PGA_G1_Pin);
//  GPIO_ResetBits (GPIOA,I_PGA_G0_Pin);

  //gain 16

//  GPIO_ResetBits(GPIOB, I_PGA_G4_Pin);
//  GPIO_ResetBits(GPIOB, I_PGA_G3_Pin);
//
//  GPIO_SetBits (GPIOC,I_PGA_G2_Pin);
//  GPIO_SetBits (GPIOC,I_PGA_G1_Pin);
//  GPIO_SetBits (GPIOA,I_PGA_G0_Pin);

  //gain 11 :

  GPIO_ResetBits(GPIOB, I_PGA_G3_Pin);
  GPIO_ResetBits (GPIOA,I_PGA_G0_Pin);

  GPIO_SetBits(GPIOB, I_PGA_G4_Pin);
  GPIO_SetBits (GPIOC,I_PGA_G2_Pin);
  GPIO_SetBits (GPIOC,I_PGA_G1_Pin);

//


  //gain 22 :


//  GPIO_ResetBits(GPIOB, I_PGA_G3_Pin);
//
//  GPIO_SetBits (GPIOB, I_PGA_G4_Pin);
//  GPIO_SetBits (GPIOC,I_PGA_G2_Pin);
//  GPIO_SetBits (GPIOC,I_PGA_G1_Pin);
//  GPIO_SetBits (GPIOA,I_PGA_G0_Pin);

  //pga voltage gain 32
  /*HAL_GPIO_WritePin(GPIOC, V_PGA_G3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, V_PGA_G2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, V_PGA_G1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, V_PGA_G0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOC, V_PGA_G4_Pin, GPIO_PIN_RESET);*/

  stm32f4::GPIOConfiguration::MakeOutput(GPIOB, V_PGA_G1_Pin|V_PGA_G0_Pin);
  stm32f4::GPIOConfiguration::MakeOutput(GPIOC, V_PGA_G3_Pin|V_PGA_G2_Pin|V_PGA_G4_Pin);


  //gain 16:
//  GPIO_ResetBits (GPIOC,V_PGA_G4_Pin);
//  GPIO_ResetBits (GPIOC,V_PGA_G3_Pin);
//
//
//GPIO_SetBits (GPIOC,V_PGA_G2_Pin);
//GPIO_SetBits (GPIOB,V_PGA_G1_Pin);
// GPIO_SetBits (GPIOB,V_PGA_G0_Pin);



  //gain 11:

//  GPIO_ResetBits (GPIOC,V_PGA_G3_Pin);
//  GPIO_ResetBits (GPIOB,V_PGA_G0_Pin);
//
//  GPIO_SetBits (GPIOC,V_PGA_G4_Pin);
//  GPIO_SetBits (GPIOC,V_PGA_G2_Pin);
//  GPIO_SetBits (GPIOB,V_PGA_G1_Pin);

//gain 8 :
  GPIO_ResetBits (GPIOC,V_PGA_G4_Pin);
  GPIO_ResetBits (GPIOC,V_PGA_G3_Pin);
  GPIO_ResetBits (GPIOB,V_PGA_G0_Pin);

  GPIO_SetBits (GPIOC,V_PGA_G2_Pin);
  GPIO_SetBits (GPIOB,V_PGA_G1_Pin);

  //isl28634_i.setgain(50);


  // ---DAC Configuration---
  Excitation excitation(ExcitationRoll);

  // ADC Timers
  stm32f4::TimerBaseParams paramstim1;
  paramstim1.timbase = TIM1;
  stm32f4::TimerBase adcv_timbase(paramstim1);
  adcv_timbase.Configure(100E3);

  stm32f4::TimerBaseParams paramstim2;
  paramstim2.timbase = TIM2;
  stm32f4::TimerBase adci_timbase(paramstim2);
  adci_timbase.Configure(100E3);

  // ---Channel 1 (Cnv signal)---
  // Sets up the output compare registers to perfom a DMA transfer into the
  // corresponding compare register.  This allows us to generate a 100 kHz
  // conv signal
  static const uint32_t ch1ccr1[] = {
      0,
      5
  };
  stm32f4::TimerOutputCompare adcv_timocch1(adcv_timbase,
                                      stm32f4::OutputCompareChannel::kCH1);
  adcv_timocch1.ConfigureCompare(ch1ccr1, 2, DMA2_Stream1, DMA_Channel_6);
  stm32f4::TimerOutputCompare adci_timocch1(adci_timbase,
                                      stm32f4::OutputCompareChannel::kCH1);
  adci_timocch1.ConfigureCompare(ch1ccr1, 2, DMA1_Stream5, DMA_Channel_3);
  // ---End Channel 1---

  // ---Channel 2---
  // Sets up output compare registers to perform a DMA transfer into the SPI
  // TX registers.  This ocurrs ~3 us after the rising edge of the Cnv signal.
  instance.InitADC_DMAOCSPITx(SPI1, DMA2_Stream2, DMA_Channel_6);			//TIM1_CH2
  instance.InitADC_DMAOCSPITx(SPI2, DMA1_Stream6, DMA_Channel_3);			//TIM2_CH2
  stm32f4::TimerOutputCompare adci_timocch2(adci_timbase,
                                      stm32f4::OutputCompareChannel::kCH2);
  adci_timocch2.ConfigureCompare(9);
  adci_timocch2.EnableDMATrigger();
  stm32f4::TimerOutputCompare adcv_timocch2(adcv_timbase,
                                      stm32f4::OutputCompareChannel::kCH2);
  adcv_timocch2.ConfigureCompare(9);
  adcv_timocch2.EnableDMATrigger();
  // ---End Channel 2---

  LTC2336SPIDMATrigger adcvtrigger(ADCVCallback, DMA2_Stream0, DMA_Channel_3, SPI1,
                                   312);
  LTC2336SPIDMATrigger adcitrigger(ADCICallback, DMA1_Stream3, DMA_Channel_0, SPI2,
                                   312);
  g_adcvtrigger = &adcvtrigger;
  g_adcitrigger = &adcitrigger;

  g_adcv_timbase = &adcv_timbase;
  g_adci_timbase = &adci_timbase;

  g_excitation = &excitation;

  stm32f4::NVICConfiguration::Enable(DMA2_Stream0_IRQn, 5); // SPI1 DMA RX
  stm32f4::NVICConfiguration::Enable(DMA1_Stream3_IRQn, 5); // SPI2 DMA RX
  stm32f4::NVICConfiguration::Enable(DMA1_Stream2_IRQn, 5); // SPI2 DMA RX  ??
  stm32f4::NVICConfiguration::Enable(TIM5_IRQn, 4); // SPI2 DMA RX  TIM5

  CalculationTask calctask;
  g_calctask = &calctask;

  adcv_timocch2.Start();
  adci_timocch2.Start(); // SPI2 Tx Generator
  adcv_timocch1.Start();
  adci_timocch1.Start(); // CNV signal generator

  stm32f4::TimerBaseParams paramstim3;
  paramstim3.timbase = TIM3;
  stm32f4::TimerBase masterswitch(paramstim3);
  masterswitch.Configure(100E3);
  TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Enable);

  TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
  TIM_SelectInputTrigger(TIM1, TIM_TS_ITR2);
  TIM_SelectInputTrigger(TIM2, TIM_TS_ITR2);
  TIM_SelectInputTrigger(TIM5, TIM_TS_ITR1);
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Trigger);
  TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Trigger);
  TIM_SelectSlaveMode(TIM5, TIM_SlaveMode_Trigger);

  masterswitch.Start();

  DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
  for( ; ; ) {
    TickType_t curtime = xTaskGetTickCount();

    vTaskDelayUntil(&curtime, 10000);
  }
}

void ResmodMainTask::InitADC_DMAOCSPITx(SPI_TypeDef * spibase, DMA_Stream_TypeDef * dxsy,
                                   uint32_t const channel) {
  static const uint16_t dummy = ~0;

  DMA_InitTypeDef dmaparams;
  DMA_StructInit(&dmaparams);
  dmaparams.DMA_Channel = channel;
  dmaparams.DMA_PeripheralBaseAddr = (uint32_t)(&spibase->DR);
  dmaparams.DMA_Memory0BaseAddr = (uint32_t)(&dummy);
  dmaparams.DMA_DIR = DMA_DIR_MemoryToPeripheral;
  dmaparams.DMA_BufferSize = 1;
  dmaparams.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  dmaparams.DMA_MemoryInc = DMA_MemoryInc_Disable;
  dmaparams.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  dmaparams.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  dmaparams.DMA_Mode = DMA_Mode_Circular;
  dmaparams.DMA_Priority = DMA_Priority_Medium;
  dmaparams.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  dmaparams.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(dxsy, &dmaparams);
  DMA_Cmd(dxsy, ENABLE);
}

void ADCICallback(int16_t const * const databuff, uint32_t const elements) {
  g_calctask->ADCIComplete(databuff);
}

void ADCVCallback(int16_t const * const databuff, uint32_t const elements) {
  g_calctask->ADCVComplete(databuff);
}

void ExcitationRoll() {
  TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
}
