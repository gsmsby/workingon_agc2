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

#include "arm_math.h"
#include "tables.h"
#include <array>




/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TEST_LENGTH_SAMPLES  MAX_RECORD_NO
#define BLOCK_SIZE           MAX_RECORD_NO

using namespace resmod;
/* Private macro -------------------------------------------------------------*/
extern float resist_can;
/* Private variables ---------------------------------------------------------*/
//extern const float CalcModule::Ref_sine_ ;
int temp3;

 //float CalcModule::ref_sine_ [] = {0,0};

/* Constructor(s) / Destructor -----------------------------------------------*/
CalcModule::CalcModule(const uint32_t gain) {

  //debugbuff_ = new int16_t[313];
  //debugindex_ = SEGGER_RTT_AllocUpBuffer("JScope_U2", debugbuff_, 626, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
//  constexpr uint32_t size = sizeof(float[400]);
  debugbuff_ = new float[400];
  debugindex_ = SEGGER_RTT_AllocUpBuffer("JScope_i2i2", debugbuff_, sizeof(float[400]), SEGGER_RTT_MODE_NO_BLOCK_SKIP);



   offs_rtt_ = new float ;
   offs_rtt_id_ = SEGGER_RTT_AllocUpBuffer("0",offs_rtt_ , 1, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
}

CalcModule::~CalcModule() {

}
/* Public methods ------------------------------------------------------------*/
ResistivityResults
CalcModule::PerformCalculation(VTable_t& vt,
                               ITable_t& it) {



	float temp_v [MAX_RECORD_NO];
	float temp_i [MAX_RECORD_NO];
	float mul_v_sine [MAX_RECORD_NO];
	float mul_v_cosine [MAX_RECORD_NO];
	float mul_i_sine [MAX_RECORD_NO];
	float mul_i_cosine [MAX_RECORD_NO];
	float dc_v_mul_sine [MAX_RECORD_NO];
	float dc_v_mul_cosine [MAX_RECORD_NO];
	float dc_i_mul_sine [MAX_RECORD_NO];
	float dc_i_mul_cosine [MAX_RECORD_NO];
	float sqrt_v_sine_cosine[MAX_RECORD_NO];
	float sqrt_i_sine_cosine[MAX_RECORD_NO];
	float resist_value[MAX_RECORD_NO];
	float avg_resist;
	float calibration_constant = 1;


	float dc_offset;
	uint16_t j;

	float32_t* inputf32_lpf;
	float32_t* outputf32_lpf;


	arm_fir_instance_f32 s;
	float32_t firstatef32[BLOCK_SIZE + NUM_TAPS - 1];
	uint32_t blocksize = BLOCK_SIZE;
	uint32_t numblocks = TEST_LENGTH_SAMPLES/BLOCK_SIZE;



	for (j=0; j < MAX_RECORD_NO; j++) {
		//converting to +/- 1 amplitude, assuming 16 bit ADC unsigned
		uint16_t t[2];
		t[0] = vt[j];
		t[1] = it[j];
		temp_v[j] = 0.00003051757*vt[j];
		temp_i[j] = 0.00003051757*it[j];
		SEGGER_RTT_Write(debugindex_, t, sizeof(t));
	}

//	for (j=0; j < MAX_RECORD_NO; j++)
//		//assuming 16 bit ADC unsigned
//		temp_i[j] = 0.00003051757*it[j];


	//DC offset removal
	dc_offset = 0;
 	for (j=0; j < MAX_RECORD_NO; j++)
		dc_offset += temp_v[j];
	dc_offset = dc_offset / MAX_RECORD_NO;

	//SEGGER_RTT_printf(offs_rtt_id_,dc_offset );

	for (j=0; j < MAX_RECORD_NO; j++)
		temp_v[j] -= dc_offset;


	dc_offset = 0;
	for (j=0; j < MAX_RECORD_NO; j++)
		dc_offset += temp_i[j];
	dc_offset = dc_offset / MAX_RECORD_NO;

	for (j=0; j < MAX_RECORD_NO; j++)
		temp_i[j] -= dc_offset;

	//PSD multiplication

	for (j=0; j < MAX_RECORD_NO; j++)
		mul_v_sine[j] = temp_v[j] * CalcModule::ref_sine_[j];

	for (j=0; j < MAX_RECORD_NO; j++)
		mul_v_cosine[j] = temp_v[j] * CalcModule::ref_cosine_[j];

	for (j=0; j < MAX_RECORD_NO; j++)
		mul_i_sine[j] = temp_i[j] * CalcModule::ref_sine_[j];

	for (j=0; j < MAX_RECORD_NO; j++)
		mul_i_cosine[j] = temp_i[j] * CalcModule::ref_cosine_[j];


	//filtering voltage
	inputf32_lpf = &mul_v_sine[0];
	outputf32_lpf = &dc_v_mul_sine[0];
	arm_fir_init_f32(&s, NUM_TAPS, (float32_t *)&fircoeffs32[0], &firstatef32[0], blocksize);
	for(j=0; j < numblocks; j++)
		arm_fir_f32(&s, inputf32_lpf + (j * blocksize), outputf32_lpf + (j * blocksize), blocksize);


	inputf32_lpf = &mul_v_cosine[0];
	outputf32_lpf = &dc_v_mul_cosine[0];
	arm_fir_init_f32(&s, NUM_TAPS, (float32_t *)&fircoeffs32[0], &firstatef32[0], blocksize);
	for(j=0; j < numblocks; j++)
		arm_fir_f32(&s, inputf32_lpf + (j * blocksize), outputf32_lpf + (j * blocksize), blocksize);


	//filtering current
	inputf32_lpf = &mul_i_sine[0];
	outputf32_lpf = &dc_i_mul_sine[0];
	arm_fir_init_f32(&s, NUM_TAPS, (float32_t *)&fircoeffs32[0], &firstatef32[0], blocksize);
	for(j=0; j < numblocks; j++)
		arm_fir_f32(&s, inputf32_lpf + (j * blocksize), outputf32_lpf + (j * blocksize), blocksize);

	inputf32_lpf = &mul_i_cosine[0];
	outputf32_lpf = &dc_i_mul_cosine[0];
	arm_fir_init_f32(&s, NUM_TAPS, (float32_t *)&fircoeffs32[0], &firstatef32[0], blocksize);
	for(j=0; j < numblocks; j++)
		arm_fir_f32(&s, inputf32_lpf + (j * blocksize), outputf32_lpf + (j * blocksize), blocksize);


	for (j=0; j < MAX_RECORD_NO; j++)
		sqrt_v_sine_cosine[j] = sqrt((dc_v_mul_sine[j] * dc_v_mul_sine[j]) + (dc_v_mul_cosine[j] * dc_v_mul_cosine[j]));
	for (j=0; j < MAX_RECORD_NO; j++)
		sqrt_i_sine_cosine[j] = sqrt((dc_i_mul_sine[j] * dc_i_mul_sine[j]) + (dc_i_mul_cosine[j] * dc_i_mul_cosine[j]));


	for (j=0; j < MAX_RECORD_NO; j++)
		resist_value[j] = ((calibration_constant*sqrt_v_sine_cosine[j])/sqrt_i_sine_cosine[j]);

	avg_resist=0;
	for (j=120; j < MAX_RECORD_NO; j++)
		avg_resist += resist_value[j] ;

	avg_resist = avg_resist/(MAX_RECORD_NO-120+1);

	resist_can = avg_resist * 200;



	temp3 ++;
	if (temp3 == 10)
	{

		temp3 = 0;
	}


	for (auto i = 0 ; i < 100 ; ++i) {
		uint32_t t = 0;
		SEGGER_RTT_Write(debugindex_, &t, sizeof(t));
	}




	//convert to +/- 1 amplitude



  //SEGGER_RTT_Write(debugindex_, it.begin(), 624);



}
/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
