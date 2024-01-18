#include "bsp_fsmc_lcd.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_dma.h"
#include "nt35510.h"

#include <porting/lv_port_disp.h>

/* dma complete semaphore */
static SemaphoreHandle_t lcd_dmatx_sem;
/* dma transfer config message */
static QueueSetHandle_t lcd_dmatx_msg;

static void fsmc_lcd_gpio_init(void) {
    // 使能GPIO时钟
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;               // GPIO 输出类型：推挽输出
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;                             // GPIO 上下拉：无
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;               // GPIO 速率：VERY HIGH

    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;                         // GPIO 模式：输出模式
    GPIO_InitStruct.Pin = NT35510_BL_PIN;
    LL_GPIO_Init(NT35510_BL_PORT, &GPIO_InitStruct);                    // 初始化 LCD_BL

    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;                      // GPIO 模式：复用模式
    GPIO_InitStruct.Alternate = LL_GPIO_AF_12;                          // GPIO 复用选择：FSMC

    // 初始化 PD0  | PD1  | PD4  | PD5  | PD8  | PD9  | PD10 | PD14 | PD15
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0 | LL_GPIO_PIN_1 | LL_GPIO_PIN_4 | LL_GPIO_PIN_5 | LL_GPIO_PIN_8 |
            LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    // 初始化 PE7  | PE8  | PE9  | PE10 | PE11 | PE12 | PE13 | PE14 | PE15
    GPIO_InitStruct.Pin = LL_GPIO_PIN_7 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 |
            LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // 初始化 PF12
    GPIO_InitStruct.Pin = LL_GPIO_PIN_12;
    LL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    // 初始化 PG12
    GPIO_InitStruct.Pin = NT35510_CS_PIN;
    LL_GPIO_Init(NT35510_CS_PORT, &GPIO_InitStruct);
}

static void fsmc_lcd_periph_init(void) {
    SRAM_HandleTypeDef fsmc_lcd = {0};
    fsmc_lcd.Instance = FSMC_NORSRAM_DEVICE;
    fsmc_lcd.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

    FSMC_NORSRAM_TimingTypeDef RdTiming = {0};
    FSMC_NORSRAM_TimingTypeDef WrTiming = {0};

    fsmc_lcd.Init.NSBank = FSMC_NORSRAM_BANK4;                                // 设置要控制的Bank 区域：BANK4
    fsmc_lcd.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;             // 设置地址总线与数据总线是否复用：否
    fsmc_lcd.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;                         // 设置存储器的类型：SRAM
    fsmc_lcd.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;            // 设置存储器的数据宽度：16bit
    fsmc_lcd.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;           // 设置是否支持突发访问模式：否（仅支持同步存储器）
    fsmc_lcd.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;         // 设置等待信号的极性：低电平
    fsmc_lcd.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;                          // 设置是否支持对齐的突发模式：否
    fsmc_lcd.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;              // 配置等待信号在等待前有效还是等待期间有效：等待前
    fsmc_lcd.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;               // 设置是否写使能：是
    fsmc_lcd.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;                      // 设置是否使能等待状态插入：否
    fsmc_lcd.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;                   // 设置是否使能扩展模式：是
    fsmc_lcd.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;          // 设置是否使能等待信号：否
    fsmc_lcd.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;                      // 设置是否使能写突发操作：否
    fsmc_lcd.Init.PageSize = FSMC_PAGE_SIZE_NONE;                             // 指定页的大小：无

    RdTiming.AddressSetupTime = 2;                                            // 地址建立时间：0-15
    RdTiming.DataSetupTime = 5;                                               // 数据建立时间：1-255
    RdTiming.BusTurnAroundDuration = 0;                                       // 总线转换周期：0-15
    RdTiming.AccessMode = FSMC_ACCESS_MODE_A;                                 // 设置访问模式：Mode A

    WrTiming.AddressSetupTime = 0;                                            // 地址建立时间：0-15
    WrTiming.DataSetupTime = 1;                                               // 数据建立时间：1-255
    WrTiming.BusTurnAroundDuration = 0;                                       // 总线转换周期：0-15
    WrTiming.AccessMode = FSMC_ACCESS_MODE_A;                                 // 设置访问模式：Mode A

    __HAL_RCC_FSMC_CLK_ENABLE();

    extern void Error_Handler(void);
    if (HAL_SRAM_Init(&fsmc_lcd, &RdTiming, &WrTiming) != HAL_OK) {
        Error_Handler();
    }
}

static void lcd_dmatx_handler(void *src, uint32_t length) {
    uint16_t *ptr = src;

    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_1);
    for (uint32_t wrlen = 0; length != 0; length -= wrlen, ptr += wrlen) {
        wrlen = length <= UINT16_MAX ? length : UINT16_MAX;
        while (LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_1));
        LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_1, wrlen);
        LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_1, (uint32_t)ptr);
        LL_DMA_ClearFlag_TC1(DMA2);
        LL_DMA_ClearFlag_TE1(DMA2);
        LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_1);
        LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_1);
        LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_1);
        xSemaphoreTake(lcd_dmatx_sem, portMAX_DELAY);
    }
}

static void lcd_dmatx_thread(void *arg) {
    static lcd_dmatx_msg_t msg_recv;

    while (1) {
        if (xQueueReceive(lcd_dmatx_msg, &msg_recv, portMAX_DELAY) == pdTRUE) {
            lcd_dmatx_handler(msg_recv.buf, msg_recv.len);
            disp_flush_release();
        }
    }
}

static void fsmc_lcd_dma_init(void) {
    LL_DMA_InitTypeDef dmacfg = {0};
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    LL_DMA_DeInit(DMA2, LL_DMA_STREAM_1);
    dmacfg.Mode = LL_DMA_MODE_NORMAL;
    dmacfg.Channel = LL_DMA_CHANNEL_3;
    dmacfg.Direction = LL_DMA_DIRECTION_MEMORY_TO_MEMORY;
    dmacfg.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
    dmacfg.Priority = LL_DMA_PRIORITY_LOW;
    dmacfg.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_NOINCREMENT;
    dmacfg.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_INCREMENT;
    dmacfg.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dmacfg.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dmacfg.MemoryOrM2MDstAddress = (uint32_t)(&(NT35510->RAM));
    LL_DMA_Init(DMA2, LL_DMA_STREAM_1, &dmacfg);

    NVIC_SetPriority(DMA2_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0xF, 0));
    NVIC_EnableIRQ(DMA2_Stream1_IRQn);

    lcd_dmatx_sem = xSemaphoreCreateBinary();
    lcd_dmatx_msg = xQueueCreate(1, sizeof(lcd_dmatx_msg_t));
    xTaskCreate(lcd_dmatx_thread, "lcd dma task", 256, NULL, configMAX_PRIORITIES - 2, NULL);
}

void bsp_fsmc_lcd_init(void) {
    fsmc_lcd_gpio_init();
    fsmc_lcd_periph_init();
    fsmc_lcd_dma_init();
}

void lcd_fsmc_dma_write(void *buf, uint32_t len) {
    static lcd_dmatx_msg_t msg_send;
    msg_send.buf = buf;
    msg_send.len = len;
    xQueueSend(lcd_dmatx_msg, &msg_send, portMAX_DELAY);
}

void DMA2_Stream1_IRQHandler(void) {
    if (LL_DMA_IsEnabledIT_TC(DMA2, LL_DMA_STREAM_1) && LL_DMA_IsActiveFlag_TC1(DMA2)) {
        LL_DMA_ClearFlag_TC1(DMA2);
        LL_DMA_DisableIT_TC(DMA2, LL_DMA_STREAM_1);
    }
    if (LL_DMA_IsEnabledIT_TE(DMA2, LL_DMA_STREAM_1) && LL_DMA_IsActiveFlag_TE1(DMA2)) {
        LL_DMA_ClearFlag_TE1(DMA2);
        LL_DMA_DisableIT_TE(DMA2, LL_DMA_STREAM_1);
        assert(0);
    }
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(lcd_dmatx_sem, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        taskYIELD();
    }
}