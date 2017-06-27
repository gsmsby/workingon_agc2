/******************************************************************************
 |
 | 	FILENAME:  main.cpp
 |
 |	Copyright 2016 Adara Systems Ltd. as an unpublished work.
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
 |	    DATE:		Oct 31, 2016
 |
 ******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <string>

#include <FreeRTOS.h>
#include <task.h>
#include <segger/SEGGER_SYSVIEW.h>

#include <freertos-plus-io/include/FreeRTOS_IO.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern unsigned int _HeapCCM_Begin;
extern unsigned int _HeapCCM_Limit;

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void uarttesttask(void *pv)
{
	Peripheral_Descriptor_t pb;
	BaseType_t ret;
	std::string strhello = "hello world";
	TickType_t starttime, finishtime1, finishtime2;

//	pb = FreeRTOS_open((const int8_t *)"/USART2/", 0);
//	FreeRTOS_ioctl(pb, ioctlUSE_ZERO_COPY_TX, (void *)1);
//	FreeRTOS_ioctl(pb, ioctlSET_SPEED, (void *)9600);
//
//	ret = FreeRTOS_ioctl(pb, ioctlOBTAIN_WRITE_MUTEX, (void *)100);
//	starttime = (*(TickType_t *)(0xE0001004));
//	FreeRTOS_write(pb, strhello.c_str(), strhello.size());
//	ret = FreeRTOS_ioctl(pb, ioctlOBTAIN_WRITE_MUTEX, (void *)1000);
//	FreeRTOS_write(pb, strhello.c_str(), strhello.size());
//	ret = FreeRTOS_ioctl(pb, ioctlWAIT_PREVIOUS_WRITE_COMPLETE, (void *)1000);
//	finishtime = (*(TickType_t *)(0xE0001004));
//	vTaskDelay(100);
//	vTaskDelay(~0);

	pb = FreeRTOS_open((const int8_t *)"/SSP3/", 0);

	vTaskDelay(100);
	FreeRTOS_ioctl(pb, ioctlUSE_INTERRUPTS, (void *)pdTRUE);
	FreeRTOS_ioctl(pb, ioctlUSE_ZERO_COPY_TX, (void *)1);

	FreeRTOS_ioctl(pb, ioctlOBTAIN_WRITE_MUTEX, (void *)100);
	starttime = (*(TickType_t *)(0xE0001004));
	FreeRTOS_write(pb, strhello.c_str(), strhello.size());
	FreeRTOS_ioctl(pb, ioctlOBTAIN_WRITE_MUTEX, (void *)100);
	finishtime1 = (*(TickType_t *)(0xE0001004));
	FreeRTOS_write(pb, strhello.c_str(), strhello.size());
	FreeRTOS_ioctl(pb, ioctlWAIT_PREVIOUS_WRITE_COMPLETE, (void *)100);
	finishtime2 = (*(TickType_t *)(0xE0001004));

	vTaskDelay(10000);
}

int
main(int argc, char* argv[])
{
	/* initialize rtos */
	/* define the heap regions for the rtos */
	HeapRegion_t xhregions[] =
	{
		{NULL, 0},
		{NULL, 0}
	};
	xhregions[0].pucStartAddress = (uint8_t *)&_HeapCCM_Begin;
	xhregions[0].xSizeInBytes = (size_t)((size_t)&_HeapCCM_Limit -
								(size_t)&_HeapCCM_Begin);
	vPortDefineHeapRegions(xhregions);

	/* perform support lib initialization */
	/* segger debug utils init */
	SEGGER_SYSVIEW_Conf();

	xTaskCreate(uarttesttask, "uarttest", 4096 / sizeof(portSTACK_TYPE),
				nullptr, 2, nullptr);

	/* start the rtos */
	vTaskStartScheduler();

	/* should never return, do something that makes sense */
	while (1)
	{
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
