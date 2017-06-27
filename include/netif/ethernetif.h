/*
 * ethernetif.h
 *
 *  Created on: Oct 27, 2016
 *      Author: Roque
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef INCLUDE_NETIF_ETHERNETIF_H_
#define INCLUDE_NETIF_ETHERNETIF_H_

#define MAC_ADDR0	0x00
#define MAC_ADDR1	0x04
#define MAC_ADDR2	0xa3
#define MAC_ADDR3	0x8d
#define MAC_ADDR4	0xc6
#define MAC_ADDR5	0x79

#define PHYADDRESS	0x00

err_t ethernetif_init(struct netif *netif);

#endif /* INCLUDE_NETIF_ETHERNETIF_H_ */
