/******************************************************************************
 |
 | 	FILENAME:  isl28634.cc
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
/* Includes ------------------------------------------------------------------*/
#include "isl28634.h"

/* Private typedef -----------------------------------------------------------*/
using namespace drivers;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
const ISL28634::GainSettings ISL28634::gainsettings_[9] = {
    {1, ISL28XXXIOState::kLow, ISL28XXXIOState::kLow},
    {2, ISL28XXXIOState::kLow, ISL28XXXIOState::kHighZ},
    {10, ISL28XXXIOState::kLow, ISL28XXXIOState::kHigh},
    {50, ISL28XXXIOState::kHighZ, ISL28XXXIOState::kLow},
    {100, ISL28XXXIOState::kHighZ, ISL28XXXIOState::kHighZ},
    {200, ISL28XXXIOState::kHighZ, ISL28XXXIOState::kHigh},
    {300, ISL28XXXIOState::kHigh, ISL28XXXIOState::kLow},
    {500, ISL28XXXIOState::kHigh, ISL28XXXIOState::kHighZ},
    {1000, ISL28XXXIOState::kHigh, ISL28XXXIOState::kHigh}
};
/* Constructor(s) / Destructor -----------------------------------------------*/
ISL28634::ISL28634(const ISL28XXXParams &params)
:ISL28XXX(params) {

}

/* Public methods ------------------------------------------------------------*/
int8_t
ISL28634::setgain(uint32_t gain) {
  bool match = false;

  for (const GainSettings &a : gainsettings_) {
    if (a.gain == gain) {
      match = true;
      writestate(ISL28XXXGainSelector::kg0, a.iostate_g0);
      writestate(ISL28XXXGainSelector::kg1, a.iostate_g1);
      break;
    }
  }

  return match ? 0 : 1;
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
