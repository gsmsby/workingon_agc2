/******************************************************************************
 |
 | 	FILENAME:  tim.cc
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

#include <segger/SEGGER_SYSVIEW.h>

#include <resmod/lowlevelutil/stm32f4timer.h>
#include <resmod/acquisition/ltc2336spidmatrigger.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern resmod::LTC2336SPIDMATrigger * g_adcitrigger;
extern resmod::LTC2336SPIDMATrigger * g_adcvtrigger;
extern stm32f4::TimerBase * g_adcv_timbase;  // Timer base clock
extern stm32f4::TimerBase * g_adci_timbase;  // Timer base clock
/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
extern "C" {
void TIM5_IRQHandler();
//void TIM2_IRQHandler();
//void TIM3_IRQHandler();
} // extern "C"
//
void TIM5_IRQHandler() {
  TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
  TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);

  g_adcvtrigger->Trigger();
  g_adcitrigger->Trigger();
}

