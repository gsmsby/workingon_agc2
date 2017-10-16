/******************************************************************************
 |
 |  	FILENAME:  calctask.h
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
#ifndef CALCMODULE_CALCTASK_H_
#define CALCMODULE_CALCTASK_H_

/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {
/* Class definition ----------------------------------------------------------*/
class CalculationTask {
 public:
  CalculationTask();
  ~CalculationTask();

  /* unused */
  CalculationTask(const CalculationTask&) = delete;
  CalculationTask& operator=(const CalculationTask&) = delete;

  void ADCVComplete(int16_t const * const pv);
  void ADCIComplete(int16_t const * const pi);

 private:
  TaskHandle_t taskhandle_;
  SemaphoreHandle_t mutv_;
  SemaphoreHandle_t muti_;
  int16_t const * pvarr_;
  int16_t const * piarr_;

 private:
  // FreeRTOS
  static void CalcTask(void *inst);
};
} // namespace resmod
#endif /* CALCMODULE_CALCTASK_H_ */
