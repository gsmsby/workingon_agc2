/******************************************************************************
 |
 |  	FILENAME:  spidmatrigger.h
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
 |	    DATE:		Sep 11, 2017
 |
 ******************************************************************************/
#ifndef ACQUISITION_LTC2336SPIDMATRIGGER_H_
#define ACQUISITION_LTC2336SPIDMATRIGGER_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

#include <stm32f4xx.h>
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {
/* Class definition ----------------------------------------------------------*/
typedef void (*SPIDMATriggerCallback)(int16_t const * const databuff,
    uint32_t const elements);
// Handles acquisition of data from the 18 bit ADC through the use of multiple
// DMA transfers.
class LTC2336SPIDMATrigger {
public:
  LTC2336SPIDMATrigger(SPIDMATriggerCallback callback,
                       DMA_Stream_TypeDef *dmay_streamx,
                       uint32_t const channel,
                       SPI_TypeDef *spibus,
                       uint32_t const samplesize);

  // Unused
  LTC2336SPIDMATrigger(const LTC2336SPIDMATrigger&) = delete;
  LTC2336SPIDMATrigger& operator=(const LTC2336SPIDMATrigger&) = delete;

  // Needs to be called when busy flag falls
  void Trigger();
  // Needs to be called when DMA transfer is complete
  void DMAComplete();
private:
  SPI_TypeDef *spibus_;
  DMA_InitTypeDef dmatypedef_;
  DMA_Stream_TypeDef *dmay_streamx_;
  uint32_t acquisitioncount_;
  int16_t *buff_;

  SPIDMATriggerCallback triggercallback_;
};
} // namespace resmod
#endif /* ACQUISITION_LTC2336SPIDMATRIGGER_H_ */
