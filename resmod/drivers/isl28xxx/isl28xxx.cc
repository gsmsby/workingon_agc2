/******************************************************************************
 |
 | 	FILENAME:  isl28xxx.cc
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
#include "isl28xx.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace drivers;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
ISL28XXX::ISL28XXX(const ISL28XXXParams& params) {
}

ISL28XXX::~ISL28XXX() {
}


/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
void
ISL28XXX::writestate(ISL28XXXGainSelector s, ISL28XXXIOState ios) {
  GPIO_InitTypeDef * g = (s == ISL28XXXGainSelector::kg0) ? &params_.g0 :
                                                            &params_.g1;
  GPIO_InitTypeDef * gz = (s == ISL28XXXGainSelector::kg0) ? &params_.g0z :
                                                             &params_.g1z;

  if (ios != ISL28XXXIOState::kHighZ) {
    // TODO(RO): lower gz
    switch(ios) {
      case ISL28XXXIOState::kHigh: // TODO(RO): raise g
        break;
      case ISL28XXXIOState::kLow: // TODO(RO): lower g
        break;
    }
  }
  else {
    // TODO(RO): raise gz
  }
}
/* Private methods -----------------------------------------------------------*/
