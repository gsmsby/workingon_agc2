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
 |	    DATE:		Sep 7, 2017
 |
 ******************************************************************************/
#ifndef DRIVERS_LTC2336_LTC2336_H_
#define DRIVERS_LTC2336_LTC2336_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
/* Class definition ----------------------------------------------------------*/
class LTC2336LowLevelInterface {
 public:
  virtual ~LTC2336LowLevelInterface() = default;

  virtual void LowLevelStartConversion() = 0;
  virtual void LowLevelRead() = 0;
};
// LTC2336 is a relatively simple devices that can run at up to 250 kSPS.
// Example usage:
// LTC2336LowLevelInterface lowlevelinterface;
// LTC2336 ltc2336(lowlevelinterface);
// ltc2336.StartConversion();
// -- AFTER CONVERSION COMPLETES --
// ltc2336.ReadConvertedResult();
//
// It is required to notify this driver that the conversion is complete.  There
// is a busy flag that needs to be monitored by the low level driver.  On a high
// to low transition a conversion is complete.  The driver will need to call
// NotifyConversionComplete.
class LTC2336 {
 public:
  LTC2336(LTC2336LowLevelInterface lowlevel);
  ~LTC2336();

  // Unused
  LTC2336(const LTC2336&) = delete;
  LTC2336& operator=(const LTC2336&) = delete;

  // Main methods
  void StartConversion();
  uint32_t ReadConvertedResult();

  // Callback
  void NotifyConversionComplete();
};

} // namespace drivers
#endif /* DRIVERS_LTC2336_LTC2336_H_ */
