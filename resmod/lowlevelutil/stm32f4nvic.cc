/******************************************************************************
 |
 | 	FILENAME:  stm32f4nvic.cc
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
#include "stm32f4nvic.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace stm32f4;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
void
NVICConfiguration::Enable(uint8_t nvic_irq, uint8_t prio) {
  NVIC_InitTypeDef nvic;
  nvic.NVIC_IRQChannel = nvic_irq;
  nvic.NVIC_IRQChannelCmd = ENABLE;
  nvic.NVIC_IRQChannelPreemptionPriority = prio;
  nvic.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&nvic);
}

void
NVICConfiguration::Disable(uint8_t nvic_irq) {
  NVIC_InitTypeDef nvic;
  nvic.NVIC_IRQChannel = nvic_irq;
  nvic.NVIC_IRQChannelCmd = DISABLE;
  nvic.NVIC_IRQChannelPreemptionPriority = 0;
  nvic.NVIC_IRQChannelSubPriority = 0;
  NVIC_Init(&nvic);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
