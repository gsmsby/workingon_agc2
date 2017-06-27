/******************************************************************************
 |
 |  	FILENAME:  ksz8895.hpp
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
#ifndef DRIVERS_KSZ8895_KSZ8895_HPP_
#define DRIVERS_KSZ8895_KSZ8895_HPP_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
namespace network {

/* Class definition ----------------------------------------------------------*/
class ksz8895
{
public:
	typedef struct configopt
	{
		uint8_t phyaddress;

		configopt():phyaddress(0) {}
	}configopt;

	typedef struct
	{
		uint8_t softreset;
		uint8_t loopback;
		uint8_t force100;
		uint8_t anenable;
		uint8_t powerdown;
		uint8_t phyisolate;
		uint8_t restartan;
		uint8_t forcefullduplex;
		uint8_t collisiontest;
		uint8_t hp_mdix;
		uint8_t forcemdi;
		uint8_t disableautomdix;
		uint8_t disablefarendfault;
		uint8_t disabletransmit;
		uint8_t disableled;
	}reg_miicontrol;

	typedef struct
	{
		uint8_t t4capable;
		uint8_t fullcapable100;
		uint8_t halfcapable100;
		uint8_t fullcapable10;
		uint8_t halfcapable10;
		uint8_t preamblesupressed;
		uint8_t ancomplete;
		uint8_t farendfault;
		uint8_t ancapable;
		uint8_t linkstatus;
		uint8_t jabbertest;
		uint8_t extendedcapable;
	}reg_miistatus;

	typedef struct
	{
		uint32_t phyid;
	}reg_phyid;

	typedef struct
	{
		uint8_t nextpage;
		uint8_t remotefault;
		uint8_t pause;
		uint8_t adv100full;
		uint8_t adv100half;
		uint8_t adv10full;
		uint8_t adv10half;
		uint8_t selectorfield;
	}reg_advertisementability;

	typedef struct
	{
		uint8_t nextpage;
		uint8_t lpack;
		uint8_t remotefault;
		uint8_t pause;
		uint8_t adv100full;
		uint8_t adv100half;
		uint8_t adv10full;
		uint8_t adv10half;
	}reg_linkpartnerability;

	typedef struct
	{
		uint8_t vctenable;
		uint8_t vctresult;
		uint8_t vct10mshort;
		uint16_t vctfaultcount;
		float disttofault;
	}reg_linkmdctrlstat;

	typedef struct
	{
		uint8_t portoperationmode;
		uint8_t polrvs;
		uint8_t mdixstatus;
		uint8_t forcelink;
		uint8_t pwrsave;
		uint8_t remoteloopback;
	}reg_physpecialctrlstat;

private:
	enum class phymiimreg: uint16_t
	{
		miicontrol,
		miistatus,
		phyidhigh,
		phyidlow,
		advertisementability,
		linkpartnerability,
		linkmd_ctrl_status,
		physpecal_ctrl_status
	};

	uint8_t phyaddress_;
public:
	explicit ksz8895(const configopt& opt);
	~ksz8895();

	/* unused */
	ksz8895(const ksz8895&) = delete;
	ksz8895& operator=(const ksz8895&) = delete;

	int8_t readmiicontrol(reg_miicontrol& value) const;
	int8_t readmiistatus(reg_miistatus& regstat) const;
	int8_t readphyid(reg_phyid& phyid) const;
	int8_t readadvertisementability(reg_advertisementability& advid) const;
	int8_t readlinkpartnerability(reg_linkpartnerability& linkp) const;
	int8_t readlinkmd_ctrlstat(reg_linkmdctrlstat& linkmd) const;
	int8_t readphyspecial_ctrlstat(reg_physpecialctrlstat& physpec) const;

	int8_t writemiicontrol(const reg_miicontrol& value) const;
	int8_t writeadvertisementability(const reg_advertisementability& advid)
									 const;
	int8_t writelinkmd_ctrlstat(const reg_linkmdctrlstat& linkmd) const;
	int8_t writephyspecial_ctrlstat(const reg_physpecialctrlstat& physpec)
									const;

private:
	uint16_t readphyreg(const phymiimreg reg) const;
	void writephyreg(const phymiimreg reg, const uint16_t val) const;

	inline uint16_t mapbittovalue(const uint8_t val, const uint8_t bit) const
	{return mapbittovalue(val, bit, bit);}
	uint16_t mapbittovalue(const uint8_t val, const uint8_t bstart,
						   const uint8_t bend) const;
	inline uint16_t mapvaluetobit(const uint8_t val, const uint8_t bit) const
	{return mapvaluetobit(val, bit, bit);}
	uint16_t mapvaluetobit(const uint8_t val, const uint8_t bstart,
						   const uint8_t bend) const;
};
}
}

#endif /* DRIVERS_KSZ8895_KSZ8895_HPP_ */
