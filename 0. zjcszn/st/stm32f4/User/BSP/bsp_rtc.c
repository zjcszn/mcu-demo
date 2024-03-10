//
// Created by zjcszn on 2023/6/6.
//

#define LOG_TAG "RTC"
#define LOG_LVL ELOG_LVL_INFO

#include <time.h>

#include "bsp_rtc.h"
#include <stdint.h>
#include "elog.h"
#include "shell.h"

#include "stm32f407xx.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_exti.h"

time_t rtc_to_timestamp(void);

static volatile time_t systimestamp;
static volatile int rtc_ready;

void bsp_rtc_init(void) {
    /* Enable RTC clock */
    LL_RCC_EnableRTC();
    /* Enable PWR clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    /* Enable access to the backup domain & RTC rigister */
    LL_PWR_EnableBkUpAccess();
    
    /* Enable RTC IRQ */
    NVIC_SetPriority(RTC_WKUP_IRQn, 0);
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
    
    /* Enable EXTI interrupt */
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_22);
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_22);
    
    LL_RTC_InitTypeDef RTC_InitStruct = {0};
    RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_24HOUR;
    RTC_InitStruct.AsynchPrescaler = 127;
    RTC_InitStruct.SynchPrescaler = 255;
    LL_RTC_Init(RTC, &RTC_InitStruct);
    
    
    /* Disable RTC WP */
    LL_RTC_DisableWriteProtection(RTC);
    /* Disable WAKEUP Timer */
    LL_RTC_WAKEUP_Disable(RTC);
    while (!LL_RTC_IsActiveFlag_WUTW(RTC));
    LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
    LL_RTC_WAKEUP_SetAutoReload(RTC, 0);
    /* Enable Wakeup timer */
    LL_RTC_WAKEUP_Enable(RTC);
    /* Enable Wakeup interrupt */
    LL_RTC_ClearFlag_WUT(RTC);
    LL_RTC_EnableIT_WUT(RTC);
    
    LL_RTC_EnableWriteProtection(RTC);
}

void wait_for_rtc_ready(void) {
    while (!rtc_ready) {
        vTaskDelay(10);
    }
}

void set_rtc_ready(void) {
    rtc_ready = 1;
}

time_t rtc_to_timestamp(void) {
    struct tm stm;
    
    stm.tm_year = (int)LL_RTC_DATE_GetYear(RTC) + 100;    //RTC_Year rang 0-99,but tm_year since 1900
    stm.tm_mon = (int)LL_RTC_DATE_GetMonth(RTC) - 1;        //RTC_Month rang 1-12,but tm_mon rang 0-11
    stm.tm_mday = (int)LL_RTC_DATE_GetDay(RTC);            //RTC_Date rang 1-31 and tm_mday rang 1-31
    stm.tm_hour = (int)LL_RTC_TIME_GetHour(RTC);            //RTC_Hours rang 0-23 and tm_hour rang 0-23
    stm.tm_min = (int)LL_RTC_TIME_GetMinute(RTC);   //RTC_Minutes rang 0-59 and tm_min rang 0-59
    stm.tm_sec = (int)LL_RTC_TIME_GetSecond(RTC);
    log_i("rtc: %d-%d-%d  %d:%d:%d", stm.tm_year, stm.tm_mon, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
    
    return mktime(&stm);
}

void timestamp_to_rtc(uint32_t timestamp) {
    struct tm *stm = NULL;
    
    LL_RTC_DateTypeDef RTC_DateStruct;
    LL_RTC_TimeTypeDef RTC_TimeStruct;
    
    stm = localtime(&timestamp);
    RTC_DateStruct.Year = stm->tm_year - 100;
    RTC_DateStruct.Month = stm->tm_mon + 1;
    RTC_DateStruct.Day = stm->tm_mday;
    RTC_DateStruct.WeekDay = stm->tm_wday;
    
    RTC_TimeStruct.Hours = stm->tm_hour + 8;
    RTC_TimeStruct.Minutes = stm->tm_min;
    RTC_TimeStruct.Seconds = stm->tm_sec;
    
    LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_DateStruct);
    LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);
}

void rtc_time_set(time_t timestamp) {
    portENTER_CRITICAL();
    systimestamp = timestamp;
    portEXIT_CRITICAL();
    timestamp_to_rtc(timestamp);
}

time_t rtc_time_get(void) {
    return systimestamp;
}

void rtc_time_show(void) {
    char buf[32];
    struct tm timestruct;
    time_t timestamp = systimestamp + 28800; /* UTC+8 */
    localtime_r((time_t *)&timestamp, &timestruct);
    strftime(buf, sizeof(buf), "%Y.%m.%d %H:%M:%S", &timestruct);
    log_i("systime: %s", buf);
}

SHELL_EXPORT_CMD(SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC) | SHELL_CMD_DISABLE_RETURN, time, rtc_time_show, show system time);

void RTC_WKUP_IRQHandler(void) {
    if (LL_RTC_IsEnabledIT_WUT(RTC) && LL_RTC_IsActiveFlag_WUT(RTC)) {
        LL_RTC_ClearFlag_WUT(RTC);
        systimestamp++;
    }
    LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_22);
}
