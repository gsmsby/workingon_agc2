/******************************************************************************
 |
 |  	FILENAME:  stm32f4gpio.h
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
#ifndef LOWLEVELUTIL_STM32F4GPIO_H_
#define LOWLEVELUTIL_STM32F4GPIO_H_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>
#include <vector>

#include <stm32f4xx.h>
/* Namespace declaration -----------------------------------------------------*/
namespace stm32f4 {
/* Class definition ----------------------------------------------------------*/
class GPIOConfiguration {
 public:
  static void MakeOutput(GPIO_TypeDef * const gpio, uint32_t const gpiopin);
  static void MakeInput(GPIO_TypeDef * const gpio, uint32_t const gpiopin,
                        GPIOPuPd_TypeDef const pull = GPIO_PuPd_NOPULL);
  static void MakeAF(GPIO_TypeDef * const gpio, uint32_t const gpio_pin,
                     uint8_t const gpio_af,
                     GPIOPuPd_TypeDef const pull = GPIO_PuPd_NOPULL);
  static void MakeEXTI(GPIO_TypeDef * const gpio, uint32_t const gpio_pin,
                       uint32_t const exti_line,
                       uint32_t const exti_portsource,
                       EXTITrigger_TypeDef const exti_trgger);

 private:
  static void GeneratePinVector(const uint32_t gpio_pin,
                                std::vector<uint8_t> * const pinlist);
};

} // namespace stm32f4
#endif /* LOWLEVELUTIL_STM32F4GPIO_H_ */
