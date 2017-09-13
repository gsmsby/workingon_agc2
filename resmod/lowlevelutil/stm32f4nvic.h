/******************************************************************************
 |
 |  	FILENAME:  stm32f4nvic.h
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
 |	    DATE:		Sep 11, 2017
 |
 ******************************************************************************/
#ifndef LOWLEVELUTIL_STM32F4NVIC_H_
#define LOWLEVELUTIL_STM32F4NVIC_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

#include <stm32f4xx.h>
/* Namespace declaration -----------------------------------------------------*/
namespace stm32f4 {
/* Class definition ----------------------------------------------------------*/
class NVICConfiguration {
 public:
  static void Enable(uint8_t nvic_irq, uint8_t prio);
  static void Disable(uint8_t nvic_irq);
};
} // namespace stm32f4
#endif /* LOWLEVELUTIL_STM32F4NVIC_H_ */
