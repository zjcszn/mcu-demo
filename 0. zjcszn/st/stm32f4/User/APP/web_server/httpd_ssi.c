/**
 * @file
 * HTTPD simple SSI example
 *
 * This file demonstrates how to add support for SSI.
 * It does this in a very simple way by providing the three tags 'HelloWorld'
 * 'counter', and 'MultiPart'.
 *
 * This file also demonstrates how to integrate CGI with SSI.
 */

/*
* Copyright (c) 2017 Simon Goldschmidt
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
* SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
* OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
*
* This file is part of the lwIP TCP/IP stack.
*
* Author: Simon Goldschmidt <goldsimon@gmx.de>
*
*/

#include "arch/cc.h"
#include "bsp_rtc.h"
#include "ethernetif.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "httpd_ssi.h"
#include "httpd_userfs.h"
#include "lwip/apps/httpd.h"

#include "lwip/def.h"
#include "lwip/mem.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/** define LWIP_HTTPD_EXAMPLE_SSI_SIMPLE to 1 to enable this ssi example*/
#ifndef LWIP_HTTPD_SSI_SIMPLE
#define LWIP_HTTPD_SSI_SIMPLE 1
#endif

/** define LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION to 1 to show how to
 * integrate CGI into SSI (LWIP_HTTPD_CGI_SSI) */
#ifndef LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION
#define LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION 0
#endif

//#if !LWIP_HTTPD_FILE_STATE
//#error LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION needs LWIP_HTTPD_FILE_STATE
//#endif
//#if !LWIP_HTTPD_CGI_SSI
//#error LWIP_HTTPD_EXAMPLE_SSI_SIMPLE_CGI_INTEGRATION needs LWIP_HTTPD_CGI_SSI
//#endif

u16_t ssi_conn_handler(char *pcInsert, int iInsertLen) {
    u16_t printed;
    if (1) {
        printed = snprintf(pcInsert, iInsertLen, "%s", "<span style=\"color: red;\">[not connect]</span>");
    } else {
        printed = snprintf(pcInsert, iInsertLen, "%s", "<span style=\"color: green;\">[connected]</span>");
    }
    return printed;
}

u16_t ssi_rtc_handler(char *pcInsert, int iInsertLen) {
    struct tm timestruct;
    time_t timestamp = rtc_time_get() + 28800; /* UTC+8 */
    localtime_r((time_t *)&timestamp, &timestruct);
    return strftime(pcInsert, iInsertLen, "%Y/%m/%d %H:%M:%S", &timestruct);
}

u16_t ssi_ip_handler(char *pcInsert, int iInsertLen) {
    u16_t printed;
    if (netif_default != NULL) {
        u32_t ip = netif_default->ip_addr.addr;
        printed = snprintf(pcInsert, iInsertLen, "%u.%u.%u.%u", (u8_t)ip, (u8_t)(ip >> 8),
                           (u8_t)(ip >> 16), (u8_t)(ip >> 24));
    } else {
        printed = snprintf(pcInsert, iInsertLen, "Can not read IP");
    }
    return printed;
}

u16_t ssi_mac_handler(char *pcInsert, int iInsertLen) {
    u16_t printed;
    if (netif_default != NULL) {
        printed = snprintf(pcInsert, iInsertLen, "%02X : %02X : %02X : %02X : %02X : %02X", netif_default->hwaddr[0],
                           netif_default->hwaddr[1], netif_default->hwaddr[2],
                           netif_default->hwaddr[3], netif_default->hwaddr[4], netif_default->hwaddr[5]);
    } else {
        printed = snprintf(pcInsert, iInsertLen, "Error MAC");
    }
    return printed;
}

u16_t ssi_linkspd_handler(char *pcInsert, int iInsertLen) {
    u16_t printed;
    printed = snprintf(pcInsert, iInsertLen, "%s", ethernetif_link_speed());
    return printed;
}

u16_t httpd_ssi_handler(const char *ssi_tag_name, char *pcInsert, int iInsertLen) {
    pcInsert[0] = '\0';
    if ((!strcmp(ssi_tag_name, "conn"))) {
        ssi_conn_handler(pcInsert, iInsertLen);
    } else if (!strcmp(ssi_tag_name, "rtc")) {
        ssi_rtc_handler(pcInsert, iInsertLen);
    } else if (!strcmp(ssi_tag_name, "ip")) {
        ssi_ip_handler(pcInsert, iInsertLen);
    } else if (!strcmp(ssi_tag_name, "mac")) {
        ssi_mac_handler(pcInsert, iInsertLen);
    } else if (!strcmp(ssi_tag_name, "linkspd")) {
        ssi_linkspd_handler(pcInsert, iInsertLen);
    }
    return strlen(pcInsert);
}

void
ssi_ex_init(void) {
    http_set_ssi_handler(httpd_ssi_handler, NULL, 0);
}

