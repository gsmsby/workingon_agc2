/******************************************************************************
 |
 |  	FILENAME:  ltc2336.h
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
 |	    DATE:		Sep 6, 2017
 |
 ******************************************************************************/
#ifndef DRIVERS_LTC2336_LTC2336_H_
#define DRIVERS_LTC2336_LTC2336_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <vector>

#include <stm32f4xx.h>

#include "resmod/resmodcommon.h"
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {

struct LTC2336Params
{
  SPI_TypeDef *spibase;
  SPI_InitTypeDef spi_channel;

  TIM_TypeDef timbase;
  TIM_TimeBaseInitTypeDef tim_channel;

  GPIO_InitTypeDef busy;
  GPIO_InitTypeDef cnv;
};
/* Class definition ----------------------------------------------------------*/
// Sets up DMA acquisition from an LTC2336 ADC.  The following resources will
// need to be provided:
// 1 clock module to generate a 100 kHz conv signal
// 1 trigger module to cause the DMA module to retrieve a single sample
class LTC2336 {
 public:
  LTC2336(const LTC2336Params &params);
  ~LTC2336();

  /* unused */
  LTC2336(const LTC2336&) = delete;
  LTC2336& operator=(const LTC2336&) = delete;

  int8_t Start();
  int8_t Stop();
  int8_t Reset();

  int8_t SetMaximumSampleCount(uint32_t samplecount);
  int8_t SetDataDestination(VTable_t &vtable);
  inline uint32_t RemainingTransfers() const {return transfersremaining_;}

  void RegisterTransferCompleteEvent(CallBackInterface * callback);

  static void BusyISR(LTC2336 &instance);

 private:
  LTC2336Params params_;
  bool busy_;

  uint32_t transfersetpoint_;
  uint32_t transfersremaining_;
  VTable_t * destination_arr_;

  DMA_InitTypeDef dma_params_;

  std::vector<CallBackInterface *> transfercompletecallbacks_;
};
} // namespace resmod
#endif /* DRIVERS_LTC2336_LTC2336_H_ */
