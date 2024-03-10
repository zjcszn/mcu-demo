//
// Created by zjcszn on 2024/1/6.
//

#include "bsp_encoder_tim.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_gpio.h"
#include "FreeRTOS.h"
#include "task.h"

static void encoder_gpio_init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_5;    /* PB4:TIM3_CH1 PB5:TIM3_CH2 */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_2;

    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void encoder_tim_init(void) {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Autoreload = 0xFFFF;
    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(TIM3, &TIM_InitStruct);

    LL_TIM_ENCODER_InitTypeDef TIM_ENCODER_InitStruct = {0};
    TIM_ENCODER_InitStruct.EncoderMode = LL_TIM_ENCODERMODE_X2_TI1;
    TIM_ENCODER_InitStruct.IC2Prescaler = LL_TIM_ICPSC_DIV1;
    TIM_ENCODER_InitStruct.IC2Polarity = LL_TIM_IC_POLARITY_BOTHEDGE;
    TIM_ENCODER_InitStruct.IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    TIM_ENCODER_InitStruct.IC2Filter = LL_TIM_IC_FILTER_FDIV1;
    TIM_ENCODER_InitStruct.IC1Prescaler = LL_TIM_ICPSC_DIV1;
    TIM_ENCODER_InitStruct.IC1Polarity = LL_TIM_IC_POLARITY_BOTHEDGE;
    TIM_ENCODER_InitStruct.IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI;
    TIM_ENCODER_InitStruct.IC1Filter = LL_TIM_IC_FILTER_FDIV1;

    LL_TIM_ENCODER_Init(TIM3, &TIM_ENCODER_InitStruct);
    LL_TIM_EnableCounter(TIM3);
}

static void encoder_tim_task(void *arg) {
    (void)arg;

    while (1) {
        log_i("count: %i", (int16_t)LL_TIM_GetCounter(TIM3));
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void bsp_encoder_tim_mode_init(void) {
    encoder_gpio_init();
    encoder_tim_init();
    xTaskCreate(encoder_tim_task, "encoder task", 128, NULL, 24, NULL);
}
