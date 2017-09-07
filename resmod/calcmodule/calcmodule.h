/******************************************************************************
 |
 |  	FILENAME:  calcmodule.h
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
 |	    DATE:		Jul 11, 2017
 |
 ******************************************************************************/
#ifndef RESMOD_CALCMODULE_CALCMODULE_H_
#define RESMOD_CALCMODULE_CALCMODULE_H_

/* Includes ------------------------------------------------------------------*/
#include <array>
#include <cstdint>

#include "resmod/resmodcommon.h"
/* Namespace declaration -----------------------------------------------------*/
namespace resmod {

// Returned by calculations performed by the calc module
struct ResistivityResults {
  float average_current;
  float average_voltage;
  float resistivity;

  ResistivityResults();
};

/* Class definition ----------------------------------------------------------*/
class CalcModule {
public:
	CalcModule(const uint32_t gain);
	~CalcModule();

	/* unused */
	CalcModule(const CalcModule&) = delete;
	CalcModule& operator=(const CalcModule&) = delete;

	ResistivityResults PerformCalculation(VTable_t &vt, ITable_t &it);

private:
	std::array<float, 256> n1_;
	std::array<float, 256> n2_;
	std::array<float, 256> n3_;
	std::array<float, 256> n4_;



};
}  // namespace resmod
#endif /* RESMOD_CALCMODULE_CALCMODULE_HPP_ */
