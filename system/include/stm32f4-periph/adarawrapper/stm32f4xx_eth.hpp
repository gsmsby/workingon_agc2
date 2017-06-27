/******************************************************************************
 |
 |  	FILENAME:  stm32f4xx_eth.hpp
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
 |	    DATE:		Nov 1, 2016
 |
 ******************************************************************************/
#ifndef INCLUDE_STM32F4_PERIPH_ADARAWRAPPER_STM32F4XX_ETH_HPP_
#define INCLUDE_STM32F4_PERIPH_ADARAWRAPPER_STM32F4XX_ETH_HPP_

/* Includes ------------------------------------------------------------------*/
#include <FreeRTOS.h>
#include <semphr.h>

/* Namespace declaration -----------------------------------------------------*/
namespace stm32f4xx {
#include "../stm32f4xx_eth.h"

/* Class definition ----------------------------------------------------------*/
class eth_mac
{
public:

private:
	SemaphoreHandle_t mutex_;
	uint32_t sesspend_;

public:
	eth_mac()
	:sesspend_(0)
	{
		mutex_ = xSemaphoreCreateBinary();
	}

	~eth_mac()
	{
		/* wait until no one is waiting for the semaphore */
		while (sesspend_)
			vTaskDelay(500u / portTICK_PERIOD_MS);

		vSemaphoreDelete(mutex_);
	}

	class sess
	{
	private:
		eth_mac& eth_;

	public:
		explicit sess(const eth_mac& eth)
		:eth_(eth)
		{
			++eth_.sesspend_;
			xSemaphoreTake(eth_.mutex_, portMAX_DELAY);
		}
		sess(const eth_mac& eth, const uint32_t to)
		:eth_(eth)
		{
			++eth_.sesspend_;
			xSemaphoreTake(eth_.mutex_, to);
		}
		~sess()
		{
			--eth_.sesspend_;
			xSemaphoreGive(eth_.mutex_);
		}

		void deinit() const
		{ETH_DeInit();}
		uint32_t init(ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress)
		const
		{return ETH_Init(ETH_InitStruct, PHYAddress);}
		void structinit(ETH_InitTypeDef* ETH_InitStruct) const
		{ETH_StructInit(ETH_InitStruct);}
		void softwarereset() const
		{ETH_SoftwareReset();}
		FlagStatus getsoftwareresetstatus() const
		{return ETH_GetSoftwareResetStatus();}
		void start() const
		{ETH_Start();}
		void stop() const
		{ETH_Stop();}
		uint32_t getrxpktsize(ETH_DMADESCTypeDef *DMARxDesc) const
		{return ETH_GetRxPktSize(DMARxDesc);}

		void transmissioncmd(FunctionalState NewState) const
		{ETH_MACTransmissionCmd(NewState);}
		void receptioncmd(FunctionalState NewState) const
		{ETH_MACReceptionCmd(NewState);}
		FlagStatus getflowcontrolbuststatus() const
		{return ETH_GetFlowControlBusyStatus();}
		void initiatepausecontrolframe() const
		{ETH_InitiatePauseControlFrame();}
	};

	friend class sess;

};

class eth_phy
{
public:

private:

public:

private:

};
}
#endif /* INCLUDE_STM32F4_PERIPH_ADARAWRAPPER_STM32F4XX_ETH_HPP_ */
