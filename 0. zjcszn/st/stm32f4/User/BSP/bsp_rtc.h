//
// Created by zjcszn on 2023/6/6.
//

#ifndef USB_DEMO_BSP_RTC_H
#define USB_DEMO_BSP_RTC_H

#include "stdint.h"
#include "time.h"

void bsp_rtc_init(void);

void rtc_time_set(time_t time);

void rtc_time_show(void);

void wait_for_rtc_ready(void);

void set_rtc_ready(void);

uint32_t rtc_time_get(void);

#endif //USB_DEMO_BSP_RTC_H
