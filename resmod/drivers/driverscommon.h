/******************************************************************************
 |
 |  	FILENAME:  driverscommon.h
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
#ifndef DRIVERS_DRIVERSCOMMON_H_
#define DRIVERS_DRIVERSCOMMON_H_

/* Includes ------------------------------------------------------------------*/

/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
/* Class definition ----------------------------------------------------------*/
class CallBackInterface {
public:
  virtual void OnCallbackEvent() = 0;

  virtual ~CallBackInterface() = default;
};
} // namespace drivers
#endif /* DRIVERS_DRIVERSCOMMON_H_ */
