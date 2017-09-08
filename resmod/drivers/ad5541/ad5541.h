/******************************************************************************
 |
 |  	FILENAME:  ad5541.h
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
#ifndef DRIVERS_AD5541_AD5541_H_
#define DRIVERS_AD5541_AD5541_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <vector>

#include <resmod/drivers/driverscommon.h>
/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
/* Class definition ----------------------------------------------------------*/
class AD5541LowLevelInterface {
 public:
  virtual ~AD5541LowLevelInterface() = default;

  virtual void LowLevelWriteData(uint16_t dat) = 0;
  virtual void LowLevelToggleLDAC() = 0;
};

enum class AD5541IncrementMode
{
  kSingle,
  kAuto
};

class AD5541 {
 public:
  AD5541(AD5541LowLevelInterface &lowlevel);
  ~AD5541();

  // Unused
  AD5541(const AD5541&) = delete;
  AD5541& operator=(const AD5541&) = delete;

  // Main methods
  void Start();
  void Stop();
  void Reset();

  // Options
  void SetMode(AD5541IncrementMode const mode);
  void SetDataBuffer(uint16_t const * const dat, uint32_t const size);
  void SetProgressCallback(CallBackInterface * const callback,
                           const std::vector<uint32_t> & timeoutlist);

  // Callback
  void NotifyDataTimeout();

 private:
  enum class DACMode {
    enabled,
    disabled
  };

  struct DataOutDetails
  {
    uint16_t * dat;
    uint32_t size;
  };

 private:
  AD5541LowLevelInterface &lowlevel_;
  AD5541IncrementMode incrementmode_;
  DACMode dacmode_;

  DataOutDetails dataout_;
  uint32_t dataoutoffset_;

  uint8_t * callbacktracker_;
  CallBackInterface * callback_;
  std::vector<uint32_t> timeoutlist_;

  void InitCallbackTracker();
};
} // namespace drivers
#endif /* DRIVERS_AD5541_AD5541_H_ */
