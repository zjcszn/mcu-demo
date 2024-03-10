#include "bsp_led.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

static uint32_t blink_delay = 300;

static void led_task(void *args) {
    (void)args;
    for (int led = 1; true; led = !led) {
        LED_GPIO_PORT->BSRR = LED_GREEN_GPIO_PIN << (led << 4);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
    }
}

void bsp_led_init(void) {
    LED_CLOCK_ENABLE();
    LL_GPIO_SetOutputPin(LED_GPIO_PORT, LED_RED_GPIO_PIN);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LED_GREEN_GPIO_PIN | LED_RED_GPIO_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);
    xTaskCreate(led_task, "led task", LED_TASK_STK_SIZE, NULL, 1, NULL);
}

void set_led_freq(uint32_t freq) {
    blink_delay = freq;
}

void bsp_led_control(led_type_t led, uint32_t control) {
    uint32_t pin_mask;
    if (led == LED_RED) {
        pin_mask = LED_RED_GPIO_PIN;
    } else if (led == LED_GREEN) {
        pin_mask = LED_GREEN_GPIO_PIN;
    } else {
        return;
    }
    LED_GPIO_PORT->BSRR = pin_mask << (control ? 16 : 0);
}


