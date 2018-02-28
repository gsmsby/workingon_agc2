/******************************************************************************
 |
 |  	FILENAME:  resmod.h
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
#ifndef RESMOD_H_
#define RESMOD_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
#include <FreeRTOS.h>
#include <task.h>


/* Namespace declaration -----------------------------------------------------*/
namespace resmod {
/* Class definition ----------------------------------------------------------*/
class ResmodMainTask {
 public:
  ResmodMainTask();
  ~ResmodMainTask();

  /* unused */
  ResmodMainTask(const ResmodMainTask&) = delete;
  ResmodMainTask& operator=(const ResmodMainTask&) = delete;

 private:
  TaskHandle_t taskhandle_;

 private:
  // FreeRTOS
  static void ResmodTask(void *inst);
  void InitADC_DMAOCSPITx(SPI_TypeDef * spibase, DMA_Stream_TypeDef * dxsy,
                     uint32_t const channel);
};
} // namespace resmod
#endif /* RESMOD_H_ */
