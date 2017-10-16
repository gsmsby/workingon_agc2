/******************************************************************************
 |
 |  	FILENAME:  excitation.h
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
 |	    DATE:		Sep 18, 2017
 |
 ******************************************************************************/
#ifndef EXCITATION_EXCITATION_H_
#define EXCITATION_EXCITATION_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

#include <resmod/lowlevelutil/stm32f4timer.h>
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {
/* Class definition ----------------------------------------------------------*/
class Excitation {
 public:
  typedef void (*ExcitationCallback)(void);

  Excitation(ExcitationCallback cb);
  ~Excitation();

  void Start();
  void Stop();

  // Needs to be called when DMA transfer is complete
  void DMAComplete();

 private:
  stm32f4::TimerBase dactimebase_;

  ExcitationCallback dmaroll_;
  static const uint32_t outputcompare_[2];
  static const uint16_t sineout_[104];
};
} // namespace resmod
#endif /* EXCITATION_EXCITATION_H_ */
