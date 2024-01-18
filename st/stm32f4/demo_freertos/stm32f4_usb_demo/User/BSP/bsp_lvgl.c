//
// Created by zjcszn on 2023/7/6.
//

#include "bsp_lvgl.h"

#include "FreeRTOS.h"
#include "elog.h"
#include "nt35510.h"
#include "task.h"

#include "lvgl.h"
#include "bsp_fsmc_lcd.h"
#include <examples/porting/lv_port_disp.h>
#include <demos/benchmark/lv_demo_benchmark.h>
#include <music/lv_demo_music.h>
#include <porting/lv_port_indev.h>

void lvgl_print(const char *str) {
    elog_raw(str);
}

static void lvglTaskHandler(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xPeriod = pdMS_TO_TICKS(10);
    (void)(pvParameters); // Suppress "unused parameter" warning

    lv_init();

    // Initialize the display buffer.
    lv_port_disp_init();
    lv_port_indev_init();

    lv_demo_music();
    while (1) {
        lv_timer_handler();
        vTaskDelayUntil(&xLastWakeTime, xPeriod);
    }
}

void bsp_lvgl_init(void) {
    xTaskCreate(lvglTaskHandler, "lvgl", 1024, NULL, 5, NULL);
}