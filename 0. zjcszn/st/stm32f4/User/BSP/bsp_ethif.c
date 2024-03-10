//
// Created by zjcszn on 2023/5/23.
//
#define LOG_TAG "NET"
#define LOG_LVL ELOG_LVL_INFO

#include "FreeRTOS.h"
#include "task.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/tcpip.h"
#include "lwip/dns.h"

#include "ethernetif.h"
#include "net_mdns.h"
#include "elog.h"

static struct netif netif;
static volatile bool link_ok, addr_ok;

static void netif_status_task(void *args) {
    (void)args;

    while (1) {
        if (ethernetif_link_update(&netif) == true) {
            if (netif.flags & NETIF_FLAG_LINK_UP) {
                link_ok = true;
                log_i("netif: %c%c%u -> link up ", netif.name[0], netif.name[1], netif.num);
                log_i("dhcp discover...");
            } else {
                link_ok = false;
                addr_ok = false;
                netif_set_addr(&netif, NULL, NULL, NULL);
                log_e("netif: %c%c%u -> link down ", netif.name[0], netif.name[1], netif.num);
            }
        }
        if (!addr_ok && link_ok) {
#if LWIP_DHCP
            if (dhcp_supplied_address(&netif)) {
                addr_ok = true;
                log_i("dhcp discover success");
            }
#endif
#if LWIP_AUTOIP
            if (autoip_supplied_address(&netif)) {
                addr_ok = true;
                log_e("dhcp discover failed, set netif by autoip...");
            }
#endif
#if LWIP_DHCP && !LWIP_AUTOIP
            struct dhcp *dhcp = netif_dhcp_data(&netif);
            if (dhcp->tries >= LWIP_DHCP_MAX_TRIES)
#endif

#if !LWIP_AUTOIP
            {
                log_e("dhcp discover failed, set netif by pre-set...");
                ip_addr_t ip_addr, gw_addr, netmask;
                IP4_ADDR(&ip_addr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                IP4_ADDR(&gw_addr, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                netif_set_addr(&netif, &ip_addr, &netmask, &gw_addr);
                addr_ok = true;
            }
#endif

            if (addr_ok) {
                ethernetif_info_show(&netif);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void bsp_netif_init(void) {
    /* init tcpip stack */
    tcpip_init(NULL, NULL);

    /* add netif to lwip netif_list */
    if (netif_add(&netif, NULL, NULL, NULL, NULL, &ethernetif_init, &tcpip_input) == NULL) {
        log_e("add netif failed");
    }

    /*  enable the netif and set as default */
    netif_set_default(&netif);
    netif_set_up(&netif);

#if LWIP_DNS
    ip_addr_t dns_server;
    IP4_ADDR(&dns_server, DNS1_ADDR0, DNS1_ADDR1, DNS1_ADDR2, DNS1_ADDR3);
    dns_setserver(0, &dns_server);
    dns_init();
#endif

#if LWIP_DHCP
    if (dhcp_start(&netif) != ERR_OK) {
        log_e("start dhcp discover failed");
    }
#endif

#if LWIP_SNTP
    sntp_start();
#endif

#if LWIP_MDNS_RESPONDER
    mdns_start();
#endif

    xTaskCreate(&netif_status_task, "eth link", 512, &netif, LWIP_APP_THREAD_PRIO, NULL);
    xTaskCreate(&ethernetif_rx_thread, "eth rx", 512, &netif, LWIP_ETH_THREAD_PRIO, NULL);
}

bool bsp_netif_is_linkup(void) {
    return addr_ok;
}

