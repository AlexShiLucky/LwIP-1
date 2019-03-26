#ifndef LWIP_DEMO__H
#define LWIP_DEMO__H


#include "lwip/netif.h"         //����ӿڹ������ͽṹ����
#include "lwip/ip.h"            //IP����صĺ����ͻ�������
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/timers.h"

extern err_t ethernetif_init(struct netif * netif);
extern void process_mac(void);
extern struct netif enc28j60_netif;

void lwip_demo(void);

#endif

