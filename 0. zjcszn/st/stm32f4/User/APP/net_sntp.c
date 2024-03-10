//
// Created by zjcszn on 2023/6/4.
//

#define LOG_TAG "NTP"
#define LOG_LVL ELOG_LVL_INFO

#include "bsp_rtc.h"
#include "shell.h"
#include "lwip/apps/sntp.h"

#include "FreeRTOS.h"

#if !SNTP_SERVER_DNS
static uint32_t sntp_list[] = {0x78197314,     /* ntp1.aliyun.com */
                               0xCB6B0658,     /* ntp2.aliyun.com */
                               0x6A37B8C7,     /* time1.cloud.tencent.com */
};
#endif

void sntp_start(void) {
#if !SNTP_SERVER_DNS
    ip_addr_t sntp_server;
    LWIP_ASSERT("number of ntp server is out of limits", LWIP_ARRAYSIZE(sntp_list) <= SNTP_MAX_SERVERS);
    for (int i = 0; i < LWIP_ARRAYSIZE(sntp_list); i++) {
        sntp_server.addr = PP_HTONL(sntp_list[i]);
        sntp_setserver(i, &sntp_server);
    }
#endif
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_init();
}

void sntp_set_system_time(uint32_t sec) {
    rtc_time_set((time_t)sec);
    rtc_time_show();
    set_rtc_ready();
}

static void sntp_sync(void) {
    sntp_stop();
    sntp_init();
}

SHELL_EXPORT_CMD(SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, sync, sntp_sync, sync system time);
