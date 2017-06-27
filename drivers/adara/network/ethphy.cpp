/******************************************************************************
 |
 | 	FILENAME:  ethphy.cpp
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
/* Includes ------------------------------------------------------------------*/
#include "ethphy.hpp"

using namespace drivers::network;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Constructor(s) / Destructor -----------------------------------------------*/
/* Public methods ------------------------------------------------------------*/
void
phyinterface_basic::readsummaryctrl(regctrl& rctl) const
{
	uint16_t reg;
	basicphycb::retstat ret = phycb_.read(reg_basic::control, reg);

	if (ret == basicphycb::retstat::success)
	{
		rctl.softreset = static_cast<ctrl_softreset>(mapbittovalue(reg, 15));
		rctl.loopback = static_cast<ctrl_loopback>(mapbittovalue(reg, 14));
		rctl.speed = static_cast<ctrl_speed>(mapbittovalue(reg, 13));
		rctl.automdix = static_cast<ctrl_automdix>(mapbittovalue(reg, 12));
		rctl.power = static_cast<ctrl_power>(mapbittovalue(reg, 11));
		rctl.isolate = static_cast<ctrl_isolate>(mapbittovalue(reg, 10));
		rctl.restartmdix = static_cast<ctrl_restartmdix>(mapbittovalue(reg, 9));
		rctl.duplex = static_cast<ctrl_duplex>(mapbittovalue(reg, 8));
	}
}

void
phyinterface_basic::readsummarystatus(regstatus& rstat) const
{
	uint16_t reg;
	basicphycb::retstat ret = phycb_.read(reg_basic::status, reg);

	if (ret == basicphycb::retstat::success)
	{
		rstat.t4base100 = static_cast<stat_100baset4>(mapbittovalue(reg, 15));
		rstat.fullduplexbase100 = static_cast<stat_100basetxfullduplex>(
				mapbittovalue(reg, 14));
		rstat.halffuplexbase100 = static_cast<stat_100basetxhalfduplex>(
				mapbittovalue(reg, 13));
		rstat.fullduplexbase10 = static_cast<stat_10basetfullduplex>(
				mapbittovalue(reg, 12));
		rstat.halfduplexbase10 = static_cast<stat_10basethalfduplex>(
				mapbittovalue(reg, 11));
		rstat.fullduplext2base100 = static_cast<stat_100baset2fullduplex>(
				mapbittovalue(reg, 10));
		rstat.halfduplext2base100 = static_cast<stat_100baset2halfduplex>(
				mapbittovalue(reg, 9));
		rstat.extended = static_cast<stat_extended>(mapbittovalue(reg, 8));
		rstat.autonegotiate = static_cast<stat_autonegotiate>(
				mapbittovalue(reg, 5));
		rstat.remotefault = static_cast<stat_remotefault>(
				mapbittovalue(reg, 4));
		rstat.autonegotiateable = static_cast<stat_autonegotiateable>(
				mapbittovalue(reg, 3));
		rstat.linkstatus = static_cast<stat_linkstatus>(mapbittovalue(reg, 2));
		rstat.jabberdetect = static_cast<stat_jabberdetect>(
				mapbittovalue(reg, 1));
		rstat.extendedable = static_cast<stat_extendedable>(
				mapbittovalue(reg, 0));
	}
}

void
phyinterface_basic::writesummaryctrl(const regctrl& rctrl) const
{
	uint16_t reg = 0;

	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.softreset), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.loopback), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.speed), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.automdix), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.power), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.isolate), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.restartmdix), 15);
	reg |= mapvaluetobit(static_cast<uint16_t>(rctrl.duplex), 15);

	phycb_.write(reg_basic::control, reg);
}

/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
uint16_t
phyinterface_basic::mapbittovalue(const uint8_t val, const uint8_t bstart,
					   	   	   	  const uint8_t bend) const
{
	uint16_t mask = 0;
	const auto iter = bend - bstart + 1;

	for (auto count = 0 ; count < iter ; ++count)
	{
		mask |= 1 << (bstart + count);
	}

	return (val & mask) >> bstart;
}

uint16_t
phyinterface_basic::mapvaluetobit(const uint8_t val, const uint8_t bstart,
					   	   	   	  const uint8_t bend) const
{
	uint16_t mask = 0;
	const auto iter = bend - bstart + 1;

	for (auto count = 0 ; count < iter ; ++count)
	{
		mask |= 1 << count;
	}

	return (mask & val) << bstart;
}
