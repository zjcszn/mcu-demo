//
// Created by zjcszn on 2023/8/16.
//

#include "net_iperf.h"
#include "lwip/apps/lwiperf.h"

void bsp_iperf_init(void) {
    lwiperf_start_tcp_server_default(NULL, NULL);
}