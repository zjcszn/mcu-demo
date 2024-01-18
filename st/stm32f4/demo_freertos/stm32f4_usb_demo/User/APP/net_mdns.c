//
// Created by zjcszn on 2023/6/10.
//
#define   LOG_TAG "NET"
#define   LOG_LVL  ELOG_LVL_INFO

#include "elog.h"
#include "lwip/apps/mdns.h"

#if LWIP_MDNS_RESPONDER

#if MDNS_PROVIDE_SERVICE
static void srv_txt(struct mdns_service *service, void *txt_userdata) {
    err_t res;
    LWIP_UNUSED_ARG(txt_userdata);

    res = mdns_resp_add_service_txtitem(service, "path=/", 6);
    if (res != ERR_OK) {
        log_e("mdns add service txt failed");
    }
}
#endif

static void mdns_report(struct netif *netif, u8_t result, s8_t service) {
    log_i("mdns status[netif %d][service %d]: %d\n", netif->num, service, result);
}

void mdns_start(void) {
    mdns_resp_register_name_result_cb(mdns_report);
    mdns_resp_init();
    mdns_resp_add_netif(netif_default, "zjcszn");
#if MDNS_PROVIDE_SERVICE
    mdns_resp_add_service(netif_default, "web", "_http", DNSSD_PROTO_TCP, 80, 3600, srv_txt, NULL);
#endif
    mdns_resp_announce(netif_default);
    log_i("mdns start...");
}

#endif