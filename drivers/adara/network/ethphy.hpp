/******************************************************************************
 |
 |  	FILENAME:  ethphy.hpp
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
#ifndef ETHPHY_HPP_
#define ETHPHY_HPP_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
namespace network {

/* Class definition ----------------------------------------------------------*/
class phyinterface_basic
{
public:
	enum class reg_basic
	{
		control,
		status
	};

	enum class ctrl_softreset
	{
		normal,
		reset
	};

	enum class ctrl_loopback
	{
		normal,
		loopback
	};

	enum class ctrl_speed
	{
		mbps10,
		mbps100
	};

	enum class ctrl_automdix
	{
		disable,
		enable
	};

	enum class ctrl_power
	{
		normal,
		powerdown
	};

	enum class ctrl_isolate
	{
		normal,
		isolatephy
	};

	enum class ctrl_restartmdix
	{
		normal,
		restart
	};

	enum class ctrl_duplex
	{
		half,
		full
	};

	enum class stat_100baset4
	{
		t4ability_none,
		t4ability_able
	};

	enum class stat_100basetxfullduplex
	{
		fullduplex100_none,
		fullduplex100_able
	};

	enum class stat_100basetxhalfduplex
	{
		halfduplex100_none,
		halfduplex100_able
	};

	enum class stat_10basetfullduplex
	{
		fullduplex10_none,
		fullduplex10_able
	};

	enum class stat_10basethalfduplex
	{
		halfduplex10_none,
		halfduplex10_able
	};

	enum class stat_100baset2fullduplex
	{
		fullduplext2100_none,
		fullduplext2100_able
	};

	enum class stat_100baset2halfduplex
	{
		halfduplext2100_none,
		halfduplext2100_able
	};

	enum class stat_extended
	{
		extended_none,
		extended_available
	};

	enum class stat_autonegotiate
	{
		autonegotiate_notdone,
		autonegotiate_done
	};

	enum class stat_remotefault
	{
		faultdetected,
		faultnotdetected
	};

	enum class stat_autonegotiateable
	{
		autonegotiate_none,
		autonegotiate_able
	};

	enum class stat_linkstatus
	{
		linkup,
		linkdown
	};

	enum class stat_jabberdetect
	{
		nojabber,
		jabber
	};

	enum class stat_extendedable
	{
		notsupported,
		supported
	};

	typedef struct
	{
		enum class retstat
		{
			success,
			failure
		};

		typedef retstat (*readphyreg)(const reg_basic regb, uint16_t &val);
		typedef retstat (*writephyreg)(const reg_basic regb,
									   const uint16_t val);

		readphyreg read;
		writephyreg write;
	}basicphycb;

	typedef struct
	{
		ctrl_softreset softreset;
		ctrl_loopback loopback;
		ctrl_speed speed;
		ctrl_automdix automdix;
		ctrl_power power;
		ctrl_isolate isolate;
		ctrl_restartmdix restartmdix;
		ctrl_duplex duplex;
	}regctrl;

	typedef struct
	{
		stat_100baset4 t4base100;
		stat_100basetxfullduplex fullduplexbase100;
		stat_100basetxhalfduplex halffuplexbase100;
		stat_10basetfullduplex fullduplexbase10;
		stat_10basethalfduplex halfduplexbase10;
		stat_100baset2fullduplex fullduplext2base100;
		stat_100baset2halfduplex halfduplext2base100;
		stat_extended extended;
		stat_autonegotiate autonegotiate;
		stat_remotefault remotefault;
		stat_autonegotiateable autonegotiateable;
		stat_linkstatus linkstatus;
		stat_jabberdetect jabberdetect;
		stat_extendedable extendedable;
	}regstatus;

private:
	basicphycb phycb_;

public:
	/*
	 * low level driver call to initialize generic driver
	 *
	 * this function must be called first before use
	 */
	void initbasicphy(const basicphycb &cb) {phycb_ = cb;}

	void readsummaryctrl(regctrl& rctl) const;
	void readsummarystatus(regstatus& rstat) const;

	void writesummaryctrl(const regctrl& rctrl) const;

	void performsoftreset() const;
	void loopback(const ctrl_loopback& lb) const;
	void speedsel(const ctrl_speed& cs) const;
	void automdix(const ctrl_automdix& mdix) const;
	void powermode(const ctrl_power& power) const;
	void isolate(const ctrl_isolate& isolate) const;
	void restartautoneg(const ctrl_restartmdix& restartmd) const;
	void duplex(const ctrl_duplex& duplex) const;

private:
	inline uint16_t mapbittovalue(const uint8_t val, const uint8_t bit) const
	{return mapbittovalue(val, bit, bit);}
	uint16_t mapbittovalue(const uint8_t val, const uint8_t bstart,
						   const uint8_t bend) const;
	inline uint16_t mapvaluetobit(const uint8_t val, const uint8_t bit) const
	{return mapvaluetobit(val, bit, bit);}
	uint16_t mapvaluetobit(const uint8_t val, const uint8_t bstart,
						   const uint8_t bend) const;
};

class phyinterface_extended
{
public:

};

}
}

#endif /* ETHPHY_HPP_ */
