/******************************************************************************
 |
 |  	FILENAME:  isl28634.h
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
#ifndef DRIVERS_ISL28XXX_ISL28634_H_
#define DRIVERS_ISL28XXX_ISL28634_H_

/* Includes ------------------------------------------------------------------*/
#include "isl28xx.h"

#include <vector>
#include <cstdint>
/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
/* Class definition ----------------------------------------------------------*/
class ISL28634 : public ISL28XXX {
 public:
  ISL28634(const ISL28XXXParams &params);

  virtual int8_t setgain(uint32_t gain);

 private:
  struct GainSettings {
    uint32_t gain;
    ISL28XXXIOState iostate_g0;
    ISL28XXXIOState iostate_g1;
  };
  static const GainSettings gainsettings_[9];
};
} // namespace drivers
#endif /* DRIVERS_ISL28XXX_ISL28634_H_ */
