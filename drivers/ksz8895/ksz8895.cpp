/******************************************************************************
 |
 | 	FILENAME:  ksz8895.cpp
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
#include "ksz8895.hpp"

#include <stm32f4xx_eth.h>
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
using namespace drivers::network;

/* Constructor(s) / Destructor -----------------------------------------------*/
/*
 * perform initialization and brings up the interface
 */
ksz8895::ksz8895(const configopt& opt)
:phyaddress_(opt.phyaddress)
{

}

/*
 * brings down the interface and powers down the phy
 */
ksz8895::~ksz8895()
{

}

/* Public methods ------------------------------------------------------------*/
/*
 * read from mii control register
 */
int8_t
ksz8895::readmiicontrol(reg_miicontrol& value) const
{
	uint16_t reg = readphyreg(phymiimreg::miicontrol);

	value.softreset = mapbittovalue(reg, 15);
	value.loopback = mapbittovalue(reg, 14);
	value.force100 = mapbittovalue(reg, 13);
	value.anenable = mapbittovalue(reg, 12);
	value.powerdown = mapbittovalue(reg, 11);
	value.phyisolate = mapbittovalue(reg, 10);
	value.restartan = mapbittovalue(reg, 9);
	value.forcefullduplex = mapbittovalue(reg, 8);
	value.collisiontest = mapbittovalue(reg, 7);
	value.hp_mdix = mapbittovalue(reg, 5);
	value.forcemdi = mapbittovalue(reg, 4);
	value.disableautomdix = mapbittovalue(reg, 3);
	value.disablefarendfault = mapbittovalue(reg, 2);
	value.disabletransmit = mapbittovalue(reg, 1);
	value.disableled = mapbittovalue(reg, 0);

	return 0;
}

/*
 * read mii status
 */
int8_t
ksz8895::readmiistatus(reg_miistatus& regstat) const
{
	uint16_t reg = readphyreg(phymiimreg::miistatus);

	regstat.t4capable = mapbittovalue(reg, 15);
	regstat.fullcapable100 = mapbittovalue(reg, 14);
	regstat.halfcapable100 = mapbittovalue(reg, 13);
	regstat.fullcapable10 = mapbittovalue(reg, 12);
	regstat.halfcapable10 = mapbittovalue(reg, 11);
	regstat.preamblesupressed = mapbittovalue(reg, 6);
	regstat.ancomplete = mapbittovalue(reg, 5);
	regstat.farendfault = mapbittovalue(reg, 4);
	regstat.ancapable = mapbittovalue(reg, 3);
	regstat.linkstatus = mapbittovalue(reg, 2);
	regstat.jabbertest = mapbittovalue(reg, 1);
	regstat.extendedcapable = mapbittovalue(reg, 0);

	return 0;
}

int8_t
ksz8895::readphyid(reg_phyid& phyid) const
{
	phyid.phyid = (readphyreg(phymiimreg::phyidhigh) << 16) |
				   readphyreg(phymiimreg::phyidlow);

	return 0;
}

int8_t
ksz8895::readadvertisementability(reg_advertisementability& adv) const
{
	uint16_t reg = readphyreg(phymiimreg::advertisementability);

	adv.nextpage = mapbittovalue(reg, 15);
	adv.remotefault = mapbittovalue(reg, 13);
	adv.pause = mapbittovalue(reg, 10);
	adv.adv100full = mapbittovalue(reg, 8);
	adv.adv100half = mapbittovalue(reg, 7);
	adv.adv10full = mapbittovalue(reg, 6);
	adv.adv10half = mapbittovalue(reg, 5);
	adv.selectorfield = mapbittovalue(reg, 4, 0);

	return 0;
}

int8_t
ksz8895::readlinkpartnerability(reg_linkpartnerability& linkp) const
{
	uint16_t reg = readphyreg(phymiimreg::linkpartnerability);

	linkp.nextpage = mapbittovalue(reg, 15);
	linkp.lpack = mapbittovalue(reg, 14);
	linkp.remotefault = mapbittovalue(reg, 13);
	linkp.pause = mapbittovalue(reg, 10);
	linkp.adv100full = mapbittovalue(reg, 8);
	linkp.adv100half = mapbittovalue(reg, 7);
	linkp.adv10full = mapbittovalue(reg, 6);
	linkp.adv10half = mapbittovalue(reg, 5);

	return 0;
}

int8_t
ksz8895::readlinkmd_ctrlstat(reg_linkmdctrlstat& linkmd) const
{
	uint16_t reg = readphyreg(phymiimreg::linkmd_ctrl_status);

	linkmd.vctenable = mapbittovalue(reg, 15);
	linkmd.vctresult = mapbittovalue(reg, 14, 13);
	linkmd.vct10mshort = mapbittovalue(reg, 12);
	linkmd.vctfaultcount = mapbittovalue(reg, 8, 0);
	linkmd.disttofault = 0.4f * linkmd.vctfaultcount;

	return 0;
}

int8_t
ksz8895::readphyspecial_ctrlstat(reg_physpecialctrlstat& physpec) const
{
	uint16_t reg = readphyreg(phymiimreg::physpecal_ctrl_status);

	physpec.portoperationmode = mapbittovalue(reg, 10, 8);
	physpec.polrvs = mapbittovalue(reg, 5);
	physpec.mdixstatus = mapbittovalue(reg, 4);
	physpec.forcelink = mapbittovalue(reg, 3);
	physpec.pwrsave = mapbittovalue(reg, 2);
	physpec.remoteloopback = mapbittovalue(reg, 1);

	return 0;
}

/*
 * write to mii control register
 */
int8_t
ksz8895::writemiicontrol(const reg_miicontrol& value) const
{
	uint16_t reg = 0;

	reg |= mapvaluetobit(value.softreset, 15);
	reg |= mapvaluetobit(value.loopback, 14);
	reg |= mapvaluetobit(value.force100, 13);
	reg |= mapvaluetobit(value.anenable, 12);
	reg |= mapvaluetobit(value.powerdown, 11);
	reg |= mapvaluetobit(value.phyisolate, 10);
	reg |= mapvaluetobit(value.restartan, 9);
	reg |= mapvaluetobit(value.forcefullduplex, 8);
	reg |= mapvaluetobit(value.collisiontest, 7);
	reg |= mapvaluetobit(value.hp_mdix, 5);
	reg |= mapvaluetobit(value.forcemdi, 4);
	reg |= mapvaluetobit(value.disableautomdix, 3);
	reg |= mapvaluetobit(value.disablefarendfault, 2);
	reg |= mapvaluetobit(value.disabletransmit, 1);
	reg |= mapvaluetobit(value.disableled, 0);

	writephyreg(phymiimreg::miicontrol, reg);

	return 0;
}

/*
 * write allowable advertisement ability registers
 */
int8_t
ksz8895::writeadvertisementability(const reg_advertisementability& advid) const
{
	uint16_t reg = 0;

	reg |= mapvaluetobit(advid.pause, 10);
	reg |= mapvaluetobit(advid.adv100full, 8);
	reg |= mapvaluetobit(advid.adv100half, 7);
	reg |= mapvaluetobit(advid.adv10full, 6);
	reg |= mapvaluetobit(advid.adv10half, 5);

	writephyreg(phymiimreg::advertisementability, reg);

	return 0;
}

int8_t
ksz8895::writelinkmd_ctrlstat(const reg_linkmdctrlstat& linkmd) const
{
	uint16_t reg = 0;

	reg |= mapvaluetobit(linkmd.vctenable, 15);

	writephyreg(phymiimreg::linkmd_ctrl_status, reg);

	return 0;
}

int8_t
ksz8895::writephyspecial_ctrlstat(const reg_physpecialctrlstat& physpec) const
{
	uint16_t reg = 0;

	reg |= mapvaluetobit(physpec.forcelink, 3);
	reg |= mapvaluetobit(physpec.pwrsave, 2);
	reg |= mapvaluetobit(physpec.remoteloopback, 1);

	writephyreg(phymiimreg::physpecal_ctrl_status, reg);

	return 0;
}

/* Protected methods ---------------------------------------------------------*/
/* Private methods -----------------------------------------------------------*/
uint16_t
ksz8895::readphyreg(const phymiimreg reg) const
{
	return ETH_ReadPHYRegister(phyaddress_, static_cast<uint16_t>(reg));
}

uint16_t
ksz8895::mapbittovalue(const uint8_t val, const uint8_t bstart,
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
ksz8895::mapvaluetobit(const uint8_t val, const uint8_t bstart,
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
