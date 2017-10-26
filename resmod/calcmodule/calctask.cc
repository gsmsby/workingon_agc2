/******************************************************************************
 |
 | 	FILENAME:  calctask.cc
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
 |	    DATE:		Sep 14, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/

#include "calctask.h"

#include <stm32f4xx.h>

#include "calcmodule.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
CalculationTask::CalculationTask():pvarr_(nullptr), piarr_(nullptr) {
  mutv_ = xSemaphoreCreateBinary();
  muti_ = xSemaphoreCreateBinary();

  xTaskCreate(CalcTask, "calc_main", (32 * 1024) / sizeof(portSTACK_TYPE),
              this, 2, &taskhandle_);
}

CalculationTask::~CalculationTask() {

}
/* Public methods ------------------------------------------------------------*/
void CalculationTask::ADCVComplete(int16_t const * const pv) {
  BaseType_t higherprio;

  pvarr_ = pv;

  xSemaphoreGiveFromISR(mutv_, &higherprio);
  portYIELD_FROM_ISR(higherprio);
}

void CalculationTask::ADCIComplete(int16_t const * const pi) {
  BaseType_t higherprio;

  piarr_ = pi;

  xSemaphoreGiveFromISR(muti_, &higherprio);
  portYIELD_FROM_ISR(higherprio);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void CalculationTask::CalcTask(void* inst) {
  CalculationTask &instance = *(CalculationTask *)inst;
  CalcModule calcmod(0);
  VTable_t waveform_v;
  ITable_t waveform_i;

  for ( ; ; ) {
    xSemaphoreTake(instance.mutv_, ~0);
    xSemaphoreTake(instance.muti_, ~0);

    std::copy(instance.pvarr_, instance.pvarr_ + 312, waveform_v.begin());
    std::copy(instance.piarr_, instance.piarr_ + 312, waveform_i.begin());

    TickType_t curtime = xTaskGetTickCount();
    calcmod.PerformCalculation(waveform_v, waveform_i);

    //remove this later !
    vTaskDelayUntil(&curtime, 10);

    DMA_ClearITPendingBit(DMA1_Stream2, DMA_IT_TCIF2);
    DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
  }
}
