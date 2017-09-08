/******************************************************************************
 |
 | 	FILENAME:  ad5541.cc
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
 |	    DATE:		Sep 7, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "ad5541.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace drivers;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
AD5541::AD5541(AD5541LowLevelInterface &lowlevel)
:lowlevel_(lowlevel) {
}

AD5541::~AD5541() {
}
/* Public methods ------------------------------------------------------------*/
void AD5541::Start() {
  dacmode_ = DACMode::enabled;
}

void AD5541::Stop() {
  dacmode_ = DACMode::disabled;
}

void AD5541::Reset() {
  dacmode_ = DACMode::disabled;
}

void AD5541::SetMode(const AD5541IncrementMode mode) {
  incrementmode_ = mode;
}

void AD5541::SetDataBuffer(const uint16_t* const dat,
                                    const uint32_t size) {
}

void AD5541::SetProgressCallback(CallBackInterface* const callback,
                                 const std::vector<uint32_t>& timeoutlist) {
  if (dacmode_ == DACMode::disabled) {
    callback_ = callback;
    timeoutlist_ = timeoutlist;

    InitCallbackTracker();
  }
}

void AD5541::NotifyDataTimeout() {
  if (dacmode_ == DACMode::enabled) {
    lowlevel_.LowLevelWriteData(dataout_.dat[dataoutoffset_]);

    // Check if time to callback
    if (callbacktracker_[dataoutoffset_] == 1) {
      callback_->OnCallbackEvent();
    }

    // Dataout pointer check
    ++dataoutoffset_;
    if (dataoutoffset_ > dataout_.size) {
      dataoutoffset_ = 0;
    }
  }
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void AD5541::InitCallbackTracker() {
  if (callbacktracker_ != nullptr) {
    delete[] callbacktracker_;
  }

  callbacktracker_ = new uint8_t[dataout_.size];
  std::fill(callbacktracker_, callbacktracker_ + dataout_.size, 0);
  for (const uint32_t& elem : timeoutlist_) {
    callbacktracker_[elem] = 1;
  }
}
