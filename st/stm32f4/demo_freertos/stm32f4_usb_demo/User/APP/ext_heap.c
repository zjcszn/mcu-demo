//
// Created by zjcszn on 2023/7/4.
//

#define LOG_TAG "SRAM"
#define LOG_LVL ELOG_LVL_INFO

#include "bsp_fsmc_sram.h"
#include "lwmem/lwmem.h"
#include "lvgl.h"
#include "porting/lv_port_disp.h"

/* lvgl disp buffer */
lv_color_t buf_2_1[MY_DISP_HOR_RES * (MY_DISP_VER_RES / 10)] __attribute__((section(".sram")));
lv_color_t buf_2_2[MY_DISP_HOR_RES * (MY_DISP_VER_RES / 10)] __attribute__((section(".sram")));

/* lwmem heap */
#define LWMWM_HEAP_SIZE (256 * 1024)
static uint8_t lwmem_heap[LWMWM_HEAP_SIZE] __attribute__((section(".sram")));
static const lwmem_region_t heap_region[] = {
    {&(lwmem_heap[0]), LWMWM_HEAP_SIZE},
    {NULL, 0}
};

void ext_heap_init(void) {
    bsp_fsmc_sram_init();
    log_i("ext-sram: addr-> 0x%p, size-> %u KB", &(lwmem_heap[0]), LWMWM_HEAP_SIZE / 1024);
    if (lwmem_assignmem(heap_region)) {
        log_i("ext-sram heap initialize success.");
    } else {
        log_e("ext-sram heap initialize failed.");
    }
}


