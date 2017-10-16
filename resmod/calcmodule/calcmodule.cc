/******************************************************************************
 |
 | 	FILENAME:  calcmodule.cc
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
/* Includes ------------------------------------------------------------------*/
#include "calcmodule.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
using namespace resmod;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
CalcModule::CalcModule(const uint32_t gain) {
  debugbuff_ = new int16_t[301];
  debugindex_ = SEGGER_RTT_AllocUpBuffer("JScope_U2", debugbuff_, 602, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

}

CalcModule::~CalcModule() {

}
/* Public methods ------------------------------------------------------------*/
ResistivityResults
CalcModule::PerformCalculation(VTable_t& vt,
                               ITable_t& it) {
  SEGGER_RTT_Write(debugindex_, it.begin(), 600);
}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
