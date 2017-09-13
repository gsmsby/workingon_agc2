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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
//extern "C" {
//void TIM2_IRQHandler();
//void TIM3_IRQHandler();
//} // extern "C"
//
//void TIM2_IRQHandler() {
//  if (TIM2->SR & TIM_IT_CC1) {
//    TIM2->SR = (uint16_t)~TIM_IT_CC1;
//    if (TIM2->CCR1 == 9)
//      TIM2->CCR1 = 4;
//    else
//      TIM2->CCR1 = 9;
//  }
//  if (TIM2->SR & TIM_IT_CC2) {
//    TIM2->SR = (uint16_t)~TIM_IT_CC2;
//    SPI_Cmd(SPI2, ENABLE);
////    switch(TIM2->CCR2) {
////      case 7: {
////        TIM2->CCR2 = 8;
////        break;
////      }
////      case 8: {
////        TIM2->CCR2 = 9;
////        break;
////      }
////      case 9: {
////        TIM2->CCR2 = 0;
////        break;
////      }
////      case 0: {
////        TIM2->CCR2 = 7;
////        break;
////      }
////    }
//  }
//}
//
//void TIM3_IRQHandler() {
//  TIM3->SR = (uint16_t)~TIM_IT_CC1;
//
//  if (TIM3->CCR1 == 9)
//    TIM3->CCR1 = 4;
//  else
//    TIM3->CCR1 = 9;
//}
