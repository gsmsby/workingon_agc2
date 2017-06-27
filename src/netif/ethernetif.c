/*
 * ethernetif.c
 *
 *  Created on: Oct 27, 2016
 *      Author: RoqueO
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include <stm32f4xx.h>
#include <lwip/opt.h>
#include <lwip/timeouts.h>
#include <lwip/def.h>
#include <lwip/pbuf.h>
#include <netif/etharp.h>
#include <lwip/ethip6.h>
#include <lwip/err.h>
#include <FreeRTOS.h>

#include <netif/ethernetif.h>
#include <os/ethernetif.h>

/* Private define ------------------------------------------------------------*/
#define netifMTU                            (1500)
#define netifINTERFACE_TASK_STACK_SIZE		(15 * 1024)
#define netifINTERFACE_TASK_PRIORITY		(configMAX_PRIORITIES - 1)
#define netifGUARD_BLOCK_TIME				(250)
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	((TickType_t)100)

/* Network interface name */
#define IFNAME0 'a'
#define IFNAME1 'd'

/* Private variables ---------------------------------------------------------*/
static struct netif *s_pxNetIf = NULL;
//SemaphoreHandle_t s_xSemaphore = NULL;
static TaskHandle_t tnotify = NULL;

/* Ethernet Rx & Tx DMA Descriptors */
extern ETH_DMADESCTypeDef  DMARxDscrTab[ETH_RXBUFNB], DMATxDscrTab[ETH_TXBUFNB];

/* Ethernet Receive buffers  */
extern uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE];

/* Ethernet Transmit buffers */
extern uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

/* Global pointers to track current transmit and receive descriptors */
extern ETH_DMADESCTypeDef  *DMATxDescToSet;
extern ETH_DMADESCTypeDef  *DMARxDescToGet;

/* Global pointer for last received frame infos */
extern ETH_DMA_Rx_Frame_infos *DMA_RX_FRAME_infos;

/* Global Ethernet handle*/
//ETH_InitTypeDef heth;

/* Private functions ---------------------------------------------------------*/
static void ethernetif_input( void * pvParameters );
static void arp_timer(void *arg);

/**
* In this function, the hardware should be initialized.
* Called from ethernetif_init().
*
* @param netif the already initialized lwip network interface structure
*        for this ethernetif
*/
static void low_level_init(struct netif *netif)
{
	uint32_t i;

	/* set netif MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set netif MAC hardware address */
	netif->hwaddr[0] =  MAC_ADDR0;
	netif->hwaddr[1] =  MAC_ADDR1;
	netif->hwaddr[2] =  MAC_ADDR2;
	netif->hwaddr[3] =  MAC_ADDR3;
	netif->hwaddr[4] =  MAC_ADDR4;
	netif->hwaddr[5] =  MAC_ADDR5;

	/* set netif maximum transfer unit */
	netif->mtu = 1500;

	/* Accept broadcast address and ARP traffic */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	s_pxNetIf = netif;

//	/* create binary semaphore used for informing ethernetif of frame reception */
//	if (s_xSemaphore == NULL)
//	{
//		s_xSemaphore = xSemaphoreCreateBinary();
//	}

	/* initialize MAC address in ethernet MAC */
	ETH_MACAddressConfig(ETH_MAC_Address0, netif->hwaddr);

	/* Initialize Tx Descriptors list: Chain Mode */
	ETH_DMATxDescChainInit(DMATxDscrTab, &Tx_Buff[0][0], ETH_TXBUFNB);
	/* Initialize Rx Descriptors list: Chain Mode  */
	ETH_DMARxDescChainInit(DMARxDscrTab, &Rx_Buff[0][0], ETH_RXBUFNB);

	/* Enable Ethernet Rx interrrupt */
	{
		for(i = 0 ; i < ETH_RXBUFNB ; ++i)
		{
			ETH_DMARxDescReceiveITConfig(&DMARxDscrTab[i], ENABLE);
		}
	}

	#ifdef CHECKSUM_BY_HARDWARE
	/* Enable the checksum insertion for the Tx frames */
	{
		for(i = 0 ; i < ETH_TXBUFNB ; ++i)
		{
			ETH_DMATxDescChecksumInsertionConfig(&DMATxDscrTab[i],
					ETH_DMATxDesc_ChecksumTCPUDPICMPFull);
		}
	}
	#endif

	/* create the task that handles the ETH_MAC */
	xTaskCreate(ethernetif_input, "eth_if",
				netifINTERFACE_TASK_STACK_SIZE / sizeof(portSTACK_TYPE), NULL,
				netifINTERFACE_TASK_PRIORITY , &tnotify);

	/* Enable MAC and DMA transmission and reception */
	ETH_Start();

	/* netif up */
	netif->flags |= NETIF_FLAG_UP;
}

/**
* This function should do the actual transmission of the packet. The packet is
* contained in the pbuf that is passed to the function. This pbuf
* might be chained.
*
* @param netif the lwip network interface structure for this ethernetif
* @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
* @return ERR_OK if the packet could be sent
*         an err_t value if the packet couldn't be sent
*
* @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
*       strange results. You might consider waiting for space in the DMA queue
*       to become availale since the stack doesn't retry to send a packet
*       dropped because of memory failure (except for the TCP timers).
*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	(void)netif;
	static SemaphoreHandle_t xTxSemaphore = NULL;
	struct pbuf *q;
	u8 *buffer ;
	__IO ETH_DMADESCTypeDef *DmaTxDesc;
	uint32_t framelength = 0;
	uint32_t bufferoffset = 0;
	uint32_t byteslefttocopy = 0;
	uint32_t payloadoffset = 0;

	if (xTxSemaphore == NULL)
	{
		vSemaphoreCreateBinary (xTxSemaphore);
	}

	if (xSemaphoreTake(xTxSemaphore, netifGUARD_BLOCK_TIME))
	{
		DmaTxDesc = DMATxDescToSet;
		buffer = (u8 *)(DmaTxDesc->Buffer1Addr);
		bufferoffset = 0;

		for(q = p; q != NULL; q = q->next)
		{
		if((DmaTxDesc->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
		{
			goto error;
		}

		/* Get bytes in current lwIP buffer  */
		byteslefttocopy = q->len;
		payloadoffset = 0;

		/* Check if the length of data to copy is bigger than Tx buffer size*/
		while((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
		{
			/* Copy data to Tx buffer*/
			memcpy( (u8_t*)((u8_t*)buffer + bufferoffset), (u8_t*)((u8_t*)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset) );

			/* Point to next descriptor */
			DmaTxDesc = (ETH_DMADESCTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

			/* Check if the buffer is available */
			if((DmaTxDesc->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
			{
				goto error;
			}

			buffer = (u8 *)(DmaTxDesc->Buffer1Addr);

			byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
			payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
			framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
			bufferoffset = 0;
		}

		/* Copy the remaining bytes */
		memcpy( (u8_t*)((u8_t*)buffer + bufferoffset), (u8_t*)((u8_t*)q->payload + payloadoffset), byteslefttocopy );
		bufferoffset = bufferoffset + byteslefttocopy;
		framelength = framelength + byteslefttocopy;
	}

	/* Prepare transmit descriptors to give to DMA*/
	ETH_Prepare_Transmit_Descriptors((u16)framelength);

	/* Give semaphore and exit */
error:
	xSemaphoreGive(xTxSemaphore);
	}

	if ((ETH->DMASR & ETH_DMASR_TUS) != (u32)RESET)
	{
	/* Clear TBUS ETHERNET DMA flag */
	ETH->DMASR = ETH_DMASR_TUS;
	/* Resume DMA transmission*/
	ETH->DMATPDR = 0;
	}

	return ERR_OK;
}

/**
* Should allocate a pbuf and transfer the bytes of the incoming
* packet from the interface into the pbuf.
*
* @param netif the lwip network interface structure for this ethernetif
* @return a pbuf filled with the received packet (including MAC header)
*         NULL on memory error
*/
static struct pbuf * low_level_input(struct netif *netif)
{
	(void)netif;
	struct pbuf *p= NULL, *q;
	u16_t len;
	FrameTypeDef frame;
	u8 *buffer;
	__IO ETH_DMADESCTypeDef *DMARxDesc;
	uint32_t bufferoffset = 0;
	uint32_t payloadoffset = 0;
	uint32_t byteslefttocopy = 0;
	uint32_t i=0;

	/* get received frame */
	frame = ETH_Get_Received_Frame_interrupt();

	/* Obtain the size of the packet and put it into the "len" variable. */
	len = (u16)frame.length;
	buffer = (u8 *)frame.buffer;

	if (len > 0)
	{
		/* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	}

	if (p != NULL)
	{
		DMARxDesc = frame.descriptor;
		bufferoffset = 0;
		for(q = p; q != NULL; q = q->next)
		{
			byteslefttocopy = q->len;
			payloadoffset = 0;

			/* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
			while( (byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE )
			{
				/* Copy data to pbuf*/
				memcpy( (u8_t*)((u8_t*)q->payload + payloadoffset), (u8_t*)((u8_t*)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

				/* Point to next descriptor */
				DMARxDesc = (ETH_DMADESCTypeDef *)(DMARxDesc->Buffer2NextDescAddr);
				buffer = (unsigned char *)(DMARxDesc->Buffer1Addr);

				byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
				payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
				bufferoffset = 0;
			}

			/* Copy remaining data in pbuf */
			memcpy( (u8_t*)((u8_t*)q->payload + payloadoffset), (u8_t*)((u8_t*)buffer + bufferoffset), byteslefttocopy);
			bufferoffset = bufferoffset + byteslefttocopy;
		}
	}

	/* Release descriptors to DMA */
	DMARxDesc = frame.descriptor;

	/* Set Own bit in Rx descriptors: gives the buffers back to DMA */
	for (i = 0 ; i < DMA_RX_FRAME_infos->Seg_Count ; i++)
	{
		DMARxDesc->Status = ETH_DMARxDesc_OWN;
		DMARxDesc = (ETH_DMADESCTypeDef *)(DMARxDesc->Buffer2NextDescAddr);
	}

	/* Clear Segment_Count */
	DMA_RX_FRAME_infos->Seg_Count = 0;

	/* When Rx Buffer unavailable flag is set: clear it and resume reception */
	if ((ETH->DMASR & ETH_DMASR_RBUS) != (u32)RESET)
	{
		/* Clear RBUS ETHERNET DMA flag */
		ETH->DMASR = ETH_DMASR_RBUS;
		/* Resume DMA reception */
		ETH->DMARPDR = 0;
	}
	return p;
}

/**
* This function is the ethernetif_input task, it is processed when a packet
* is ready to be read from the interface. It uses the function low_level_input()
* that should handle the actual reception of bytes from the network
* interface. Then the type of the received packet is determined and
* the appropriate input function is called.
*
* @param netif the lwip network interface structure for this ethernetif
*/
void ethernetif_input( void * pvParameters )
{
	(void)pvParameters;
	struct pbuf *p;

	for( ;; )
	{
		if (ulTaskNotifyTake(pdTRUE, emacBLOCK_TIME_WAITING_FOR_INPUT))
		{
TRY_GET_NEXT_FRAME:
			p = low_level_input( s_pxNetIf );
			if (p != NULL)
			{
				if (ERR_OK != s_pxNetIf->input( p, s_pxNetIf))
				{
					pbuf_free(p);
				}
				else
				{
					goto TRY_GET_NEXT_FRAME;
				}
			}
		}

		/* Check stack usage */
//		SEGGER_RTT_printf(0, "ethernetif_input stack left: %dB\n",
//				((int)uxTaskGetStackHighWaterMark(NULL) /
//						sizeof(portSTACK_TYPE)));
	}
}

/**
* Should be called at the beginning of the program to set up the
* network interface. It calls the function low_level_init() to do the
* actual setup of the hardware.
*
* This function should be passed as a parameter to netif_add().
*
* @param netif the lwip network interface structure for this ethernetif
* @return ERR_OK if the loopif is initialized
*         ERR_MEM if private data couldn't be allocated
*         any other err_t on error
*/
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "adara";
#endif /* LWIP_NETIF_HOSTNAME */
	netif->name[0] = IFNAME0;
	netif->name[1] = IFNAME1;

	netif->output = etharp_output;
	netif->linkoutput = low_level_output;

	/* initialize the hardware */
	low_level_init(netif);

	etharp_init();
	sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);

	return ERR_OK;
}

void ETHERNET_IRQHandler(void)
{
	BaseType_t contextswreq = pdFALSE;

	vTaskNotifyGiveFromISR(tnotify, &contextswreq);
	portYIELD_FROM_ISR(contextswreq);
}

//void ETHERNET_PHYHandler(void)
//{
//	/* Check whether the link interrupt has occurred or not */
//	if(((ETH_ReadPHYRegister((uint16_t)pcPHYAddress, PHY_MISR)) & PHY_LINK_STATUS) != 0)
//	{
//		if((ETH_ReadPHYRegister((uint16_t)pcPHYAddress, PHY_SR) & 1))
//		{
//			netif_set_link_up(&xnetif);
//		}
//		else
//		{
//			netif_set_link_down(&xnetif);
//		}
//	}
//}

static void arp_timer(void *arg)
{
	if((ETH_ReadPHYRegister((uint16_t)PHYADDRESS, PHY_BSR) & 4))
	{
		netif_set_link_up(s_pxNetIf);
	}
	else
	{
		netif_set_link_down(s_pxNetIf);
	}

	(void)arg;
	etharp_tmr();
	sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}
