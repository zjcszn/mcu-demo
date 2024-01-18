//
// Created by zjcszn on 2023/5/14.
//

#include <stdint.h>
#include "ff.h"
#include "http_utils.h"
#include "lwip/apps/httpd.h"
#include "mthread.h"
#include "ext_heap.h"
#include "dev_uart.h"
#include "elog.h"
#include "lwmem/lwmem.h"
#include "bsp_led.h"
#include "bsp_ethif.h"
#include "FreeRTOS.h"
#include "net_iperf.h"
#include "sfud.h"
#include "sfud_cfg.h"
#include "sfud_def.h"
#include "task.h"
#include "tcpecho.h"
#include "udpecho.h"
#include "shell_port.h"
#include "bsp_rng.h"
#include "bsp_rtc.h"
#include "bsp_sdcard.h"
#include "dwt_delay.h"
#include "udpecho_raw.h"

/* http & mqtt include */
#include "coremqtt_demo.h"
#include "coremqtt_client.h"
#include "corehttp_client.h"

#include "usbh_usr.h"
#include "tusb_custom_hid.h"
#include "cherryusb_demo.h"

#include "bsp_button.h"
#include "bsp_adc.h"
#include "zlib.h"
#include "web_server/http_server.h"
#include "bsp_camera.h"
#include "nt35510.h"
#include "bsp_fsmc_lcd.h"
#include "bsp_lvgl.h"
#include "bsp_encoder_exti.h"
#include "bsp_encoder_tim.h"
#include "bsp_encoder_polling.h"
#include "bsp_touchkey.h"

StackType_t mthread_stk[1024 + 256];
StaticTask_t handler;

void system_init(void *args);

void mthread_start(void) {
    xTaskCreateStatic((TaskFunction_t)system_init,
                      "system init",
                      1024 + 256,
                      NULL,
                      configMAX_PRIORITIES - 1,
                      mthread_stk,
                      &handler);
    vTaskStartScheduler();
}

FATFS f_sd, f_flash;

void system_init(void *args) {
    FRESULT fret;

    bsp_led_init();
    bsp_rng_init();
    bsp_dwt_init();

    bsp_shell_init();
    bsp_elog_init();

    ext_heap_init();
    bsp_rtc_init();
    sfud_init();
    bsp_netif_init();
    bsp_button_init();
    bsp_adc_init();
    bsp_iperf_init();
    bsp_fsmc_lcd_init();
    NT35510_Init(NT35510_DISP_PORTRAIT);
    NT35510_Clear(LCD_COLOR_WHITE);

#if TOUCHKEY_DEMO
    bsp_touchkey_init();
#endif

#if ENCODER_DEMO
    bsp_encoder_polling_mode_init();
#endif

#if CAMERA_DEMO
    bsp_camera_init();
#endif
    
#if FATFS_DEMO
    fret = f_mount(&f_flash, "1:", 1);
    log_i("spi flash fatfs mount %s", fret == FR_OK ? "success" : "failed");
    fret = f_mount(&f_sd, "0:", 1);
    log_i("sdcard fatfs mount %s", fret == FR_OK ? "success" : "failed");
#endif

#if STUSB_USBH_DEMO
    stusb_init();
    stusb_demo();
#endif

#if TINYUSB_USBD_DEMO
    tinyusb_init();
#endif

#if CHERRYUSB_USBD_DEMO
    cdc_acm_msc_init();
#endif

#if HTTPD_DEMO
    bsp_httpd_init();
    tcpecho_init();
    udpecho_init();
    wait_for_rtc_ready();
#endif

#if HTTP_DOWNLOAD_DEMO
    httpclient_download("https://dldir1.qq.com/qqfile/qq/TIM3.4.8/TIM3.4.8.22086.exe", NULL);
#endif

#if MQTT_DEMO
    vStartMQTTAgent();
#endif

#if HTTP_GET_DEMO
    extern void bsp_weather_get(void *args);
    xTaskCreate(bsp_weather_get, "http get", 512, NULL, 0, NULL);
#endif
    vTaskDelete(NULL);
}

void bsp_weather_get(void *args) {
    lwmem_stats_t temp;
    lwmem_get_stats(&temp);
    uint8_t *buf = lwmem_malloc(2048);
    const uint8_t *resp;
    uint8_t *uncomp = lwmem_malloc(4096);
    uint32_t resp_len;
    uint32_t uncomplen = 4096;
    int ret;
    log_d("zlib max decompress heap usage start: %u", (temp.mem_size_bytes - temp.minimum_ever_mem_available_bytes));
    while (1) {
        uncomplen = 4096;
        if (httpclient_get(
                "https://geoapi.qweather.com/v2/city/lookup?location=shenzhen&key=698a4c1889574b93b07ef8dad98d4702",
                buf,
                2048,
                &resp,
                &resp_len) == HTTPCLIENT_OK) {
            ret = uncompress(uncomp, (uLongf *)&uncomplen, (const Bytef *)resp, resp_len);
            if (ret == Z_OK) {
                log_i("HTTP Response: %u bytes\r\n %s", uncomplen, uncomp);
            } else {
                log_e("HTTP Response uncompress failed, %d", ret);
            }
        } else {
            log_e("HTTP GET failed");
        }
        lwmem_get_stats(&temp);
        log_d("zlib max decompress heap usage end: %u", (temp.mem_size_bytes - temp.minimum_ever_mem_available_bytes));
        vTaskDelay(2000);
    }
}



