#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/err.h"
#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

err_t ethernetif_init(struct netif *netif);      
void ETHERNET_IRQHandler(void);

#ifdef __cplusplus
}
#endif
#endif 
