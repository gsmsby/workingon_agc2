/******************************************************************************
 |
 |  	FILENAME:  resmodcommon.h
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
#ifndef RESMODCOMMON_H_
#define RESMODCOMMON_H_

/* Includes ------------------------------------------------------------------*/
#include <array>
#include <cstdint>
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {
/* Class definition ----------------------------------------------------------*/
// The following types must be used to provide data to the calculation module
typedef std::array<int16_t, 312> VTable_t;
typedef std::array<int16_t, 312> ITable_t;

class CallBackInterface {
public:
  virtual void OnCallbackEvent() = 0;

  virtual ~CallBackInterface() = default;
};

} // namespace resmod
#endif /* RESMODCOMMON_H_ */
