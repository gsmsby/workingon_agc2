/******************************************************************************
 |
 |  	FILENAME:  lan8742a.hpp
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
#ifndef LAN8742A_LAN8742A_HPP_
#define LAN8742A_LAN8742A_HPP_

/* Includes ------------------------------------------------------------------*/
#include <cstdint>

/* Namespace declaration -----------------------------------------------------*/
namespace drivers {
namespace network {

/* Class definition ----------------------------------------------------------*/
class lan8742a
{
public:
	typedef struct configopt
	{
		uint8_t phyaddress;

		configopt():phyaddress(0) {}
	}configopt;

	typedef struct
	{

	};
};
}
}
#endif /* LAN8742A_LAN8742A_HPP_ */
