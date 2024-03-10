//
// Created by zjcszn on 2023/6/4.
//

#ifndef USB_DEMO_NET_SNTP_H
#define USB_DEMO_NET_SNTP_H

#include "stdint.h"

void sntp_start(void);

void sntp_set_system_time(uint32_t sec);

#endif //USB_DEMO_NET_SNTP_H
