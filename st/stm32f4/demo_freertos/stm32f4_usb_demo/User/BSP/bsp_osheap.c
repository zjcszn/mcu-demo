//
// Created by zjcszn on 2023/7/4.
//

#include "FreeRTOS.h"
#include "task.h"

uint8_t ucHeap[configTOTAL_HEAP_SIZE]  __attribute__((section(".ccm")));

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    __disable_irq();
    printf("ERROR: task[%s] stack overflow", pcTaskName);
    while (1);
}

void vApplicationMallocFailedHook(void) {
    printf("ERROR: malloc failed");
}
