/******************************************************************************
 |
 |  	FILENAME:  isl28xx.h
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
#ifndef DRIVERS_ISL28XXX_ISL28XX_H_
#define DRIVERS_ISL28XXX_ISL28XX_H_

/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>
/* Namespace declaration -----------------------------------------------------*/
namespace drivers {

struct ISL28XXXParams
{
  GPIO_InitTypeDef g0;
  GPIO_InitTypeDef g0z;
  GPIO_InitTypeDef g1;
  GPIO_InitTypeDef g1z;
};

enum class ISL28XXXGainSelector
{
  kg0,
  kg1
};

enum class ISL28XXXIOState
{
  kHigh,
  kLow,
  kHighZ
};

/* Class definition ----------------------------------------------------------*/
class ISL28XXX {
 public:
  ISL28XXX(const ISL28XXXParams &params);
  virtual ~ISL28XXX();

  /* unused */
  ISL28XXX(const ISL28XXX&) = delete;
  ISL28XXX& operator=(const ISL28XXX&) = delete;

  virtual int8_t setgain(uint32_t gain) = 0;

 protected:
  void writestate(ISL28XXXGainSelector s, ISL28XXXIOState ios);

 private:
  ISL28XXXParams params_;
};
}  // namespace drivers

#endif /* DRIVERS_ISL28XXX_ISL28XX_H_ */
