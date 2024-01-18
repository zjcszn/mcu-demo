//
// Created by zjcszn on 2023/8/6.
//

#define LOG_TAG "ADC"
#define LOG_LVL ELOG_LVL_INFO

#include "stm32f407xx.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_bus.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define ADC_BUFFER_SIZE    1

static QueueHandle_t adc_queue;

static void adc_gpio_init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;                             // 设置 GPIO 为模拟模式
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7;                                    // PF7
    LL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}

static void adc_tim_init(void) {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = ((SystemCoreClock / 2) / 1000000) - 1;       // 预分频系数：83 < TIMER 时钟：APB1 * 2 = 84Mhz >
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;                     // 计数模式： 递增
    TIM_InitStruct.Autoreload = 99999;                                      // 自动重加载值：99999
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(TIM2, &TIM_InitStruct);                                     // 频率：84Mhz / (83 + 1) / (99999 + 1) = 10 Hz
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_UPDATE);
    LL_TIM_EnableCounter(TIM2);
}

static void adc_periph_init(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3);                     // adc 时钟频率： 84Mhz

    LL_ADC_InitTypeDef ADC_InitStruct = {0};
    ADC_InitStruct.Resolution = LL_ADC_RESOLUTION_12B;                      // 12bit分辨率
    ADC_InitStruct.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;                 // 右对齐
    ADC_InitStruct.SequencersScanMode = LL_ADC_SEQ_SCAN_DISABLE;            // 禁止扫描模式
    LL_ADC_Init(ADC3, &ADC_InitStruct);

    LL_ADC_REG_InitTypeDef ADC_REG_InitStruct = {0};
    ADC_REG_InitStruct.TriggerSource = LL_ADC_REG_TRIG_EXT_TIM2_TRGO;       // 使能外部触发：TIM2 TRGO
    ADC_REG_InitStruct.DMATransfer = LL_ADC_REG_DMA_TRANSFER_NONE;          // 关闭DMA传输
    ADC_REG_InitStruct.ContinuousMode = LL_ADC_REG_CONV_SINGLE;             // 单次模式
    ADC_REG_InitStruct.SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE;
    ADC_REG_InitStruct.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    LL_ADC_REG_Init(ADC3, &ADC_REG_InitStruct);

    LL_ADC_CommonInitTypeDef ADC_CommonInitStruct = {0};
    ADC_CommonInitStruct.CommonClock = LL_ADC_CLOCK_SYNC_PCLK_DIV4;         // 四倍分频-> 84/4 = 21Mhz
    ADC_CommonInitStruct.Multimode = LL_ADC_MULTI_INDEPENDENT;              // 独立模式
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC3), &ADC_CommonInitStruct);

    NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0xF, 0));
    NVIC_EnableIRQ(ADC_IRQn);

    LL_ADC_REG_SetSequencerRanks(ADC3, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);
    LL_ADC_SetChannelSamplingTime(ADC3, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_3CYCLES);
    LL_ADC_REG_StartConversionExtTrig(ADC3, LL_ADC_REG_TRIG_EXT_RISING);    // 开启外部触发转换（上升沿触发）
    LL_ADC_EnableIT_EOCS(ADC3);                                             // 使能转换结束中断
    LL_ADC_Enable(ADC3);                                                    // 使能ADC3
}

static void adc_task(void *args) {
    uint16_t adc_value;
    adc_queue = xQueueCreate(ADC_BUFFER_SIZE, sizeof(uint16_t));
    configASSERT(adc_queue != NULL);

    /* init adc */
    adc_gpio_init();
    adc_tim_init();
    adc_periph_init();
    while (1) {
        if (xQueueReceive(adc_queue, &adc_value, portMAX_DELAY) == pdTRUE) {
            log_d("light sensor: %u\r\n", adc_value);
        }
    }
}

void bsp_adc_init(void) {
    xTaskCreate(adc_task, "adc task", 128, NULL, 15, NULL);
}

bool bsp_adc_read(uint16_t *read_value) {
    return (xQueueReceive(adc_queue, read_value, 0) == pdTRUE) ? true : false;
}

void ADC_IRQHandler(void) {
    /* end of conversation */
    if (LL_ADC_IsEnabledIT_EOCS(ADC3) && LL_ADC_IsActiveFlag_EOCS(ADC3)) {
        LL_ADC_ClearFlag_EOCS(ADC3);
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        uint16_t sample = LL_ADC_REG_ReadConversionData12(ADC3);
        xQueueOverwriteFromISR(adc_queue, &sample, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            taskYIELD();
        }
    }
    /*  */
    if (LL_ADC_IsActiveFlag_OVR(ADC3)) {
        LL_ADC_ClearFlag_OVR(ADC3);
    }
}
