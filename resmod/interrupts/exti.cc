/******************************************************************************
 |
 | 	FILENAME:  exti.cc
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
 |	    DATE:		Sep 12, 2017
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stm32f4xx.h>

#include <resmod/acquisition/ltc2336spidmatrigger.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern resmod::LTC2336SPIDMATrigger *g_adcitrigger;

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
extern "C" {
void EXTI0_IRQHandler();
} // extern "C"

void EXTI0_IRQHandler() {
  EXTI_ClearITPendingBit(EXTI_Line0);
  g_adcitrigger->Trigger();
}
