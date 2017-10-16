/******************************************************************************
 |
 | 	FILENAME:  stm32f4gpio.cc
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
/* Includes ------------------------------------------------------------------*/
#include "stm32f4gpio.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace stm32f4;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
void
GPIOConfiguration::MakeOutput(GPIO_TypeDef * const gpio,
                              uint32_t const gpio_pin) {
  GPIO_InitTypeDef gpiotypedef = {
      gpio_pin, GPIO_Mode_OUT, GPIO_Speed_2MHz, GPIO_OType_PP, GPIO_PuPd_NOPULL
  };
  GPIO_Init(gpio, &gpiotypedef);
}

void
GPIOConfiguration::MakeInput(GPIO_TypeDef * const gpio,
                             uint32_t const gpio_pin,
                             GPIOPuPd_TypeDef const pull) {
  GPIO_InitTypeDef gpiotypedef = {
      gpio_pin, GPIO_Mode_IN, GPIO_Speed_2MHz, GPIO_OType_PP, pull
  };
  GPIO_Init(gpio, &gpiotypedef);
}

void
GPIOConfiguration::MakeAF(GPIO_TypeDef * const gpio, uint32_t const gpio_pin,
                          uint8_t const gpio_af,
                          GPIOPuPd_TypeDef const pull) {
  GPIO_InitTypeDef gpiotypedef = {
      gpio_pin, GPIO_Mode_AF, GPIO_Speed_2MHz, GPIO_OType_PP, pull
  };
  GPIO_Init(gpio, &gpiotypedef);

  std::vector<uint8_t> pinlist;
  GeneratePinVector(gpio_pin, &pinlist);
  for (const uint8_t &val : pinlist) {
    GPIO_PinAFConfig(gpio, val, gpio_af);
  }
}

void
GPIOConfiguration::MakeEXTI(GPIO_TypeDef* const gpio,
                            uint32_t const gpio_pin,
                            uint32_t const exti_line,
                            uint32_t const exti_portsource,
                            EXTITrigger_TypeDef const exti_trigger) {
  MakeInput(gpio, gpio_pin, GPIO_PuPd_DOWN);

  EXTI_InitTypeDef exti;
  exti.EXTI_Line = exti_line;
  exti.EXTI_LineCmd = ENABLE;
  exti.EXTI_Mode = EXTI_Mode_Interrupt;
  exti.EXTI_Trigger = exti_trigger;
  EXTI_Init(&exti);

  std::vector<uint8_t> pinlist;
  GeneratePinVector(gpio_pin, &pinlist);
  for (const uint8_t &val : pinlist) {
    SYSCFG_EXTILineConfig(exti_portsource, val);
  }
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
void
GPIOConfiguration::GeneratePinVector(const uint32_t gpio_pin,
                                     std::vector<uint8_t> * const pinlist) {
  for (uint32_t i = 0; i < sizeof(uint16_t) * 8; ++i) {
    if ((gpio_pin >> i) & 0x0001) {
      pinlist->push_back(i);
    }
  }
}
