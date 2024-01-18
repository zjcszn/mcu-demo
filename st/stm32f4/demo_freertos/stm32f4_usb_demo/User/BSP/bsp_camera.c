//
// Created by zjcszn on 2023/11/11.
//

#define LOG_LVL ELOG_LVL_INFO
#define LOG_TAG "CAMERA"

#include "bsp_camera.h"
#include "bsp_soft_i2c.h"
#include "stm32f4xx_hal_dcmi.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_ll_bus.h"

#include "ov5640.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nt35510.h"

static soft_i2c_dev_t ov5640_i2c;
static OV5640_Object_t ov5640_obj;
static OV5640_CAMERA_Drv_t *ov5640_driver;
static DCMI_HandleTypeDef dcmi_handle;

static int32_t prv_camera_sccb_init(void) {
    /* init sccb(i2c) interface */
    ov5640_i2c.dev_addr = CAMERA_I2C_ADDR;
    ov5640_i2c.scl.pin = CAMERA_I2C_SCL_PIN;
    ov5640_i2c.scl.port = CAMERA_I2C_SCL_PORT;
    ov5640_i2c.sda.pin = CAMERA_I2C_SDA_PIN;
    ov5640_i2c.sda.port = CAMERA_I2C_SDA_PORT;
    soft_i2c_init(&ov5640_i2c, 400);
    log_i("camera sccb init success");
    return 0;
}

static void prv_camera_gpio_init(void) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA | LL_AHB1_GRP1_PERIPH_GPIOB | LL_AHB1_GRP1_PERIPH_GPIOC |
            LL_AHB1_GRP1_PERIPH_GPIOE | LL_AHB1_GRP1_PERIPH_GPIOG);

    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_13;

    /* init href(hsync) / PCLK */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_4 | LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* init d5 &vsync */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    /* init d0-d4 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7 | LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_12;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    /* init d6-d7 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* init pwdn (active high with internal pull-down) & reset */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_15;
    LL_GPIO_Init(GPIOG, &GPIO_InitStruct);
}

static void prv_camera_dcmi_init(void) {

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_DCMI);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);

    /*** Configures the DCMI to interface with the camera module ***/
    /* DCMI configuration */
    dcmi_handle.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;       /* hardware sysnc mode */
    dcmi_handle.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;     /* sampled on pclk rising */
    dcmi_handle.Init.VSPolarity = DCMI_VSPOLARITY_HIGH;          /* set vsync polarity */
    dcmi_handle.Init.HSPolarity = DCMI_HSPOLARITY_HIGH;          /* set href polarity */
    dcmi_handle.Init.CaptureRate = DCMI_CR_ALL_FRAME;           /* set capture rate, such as all , 1/2, 1/4 */
    dcmi_handle.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;    /* set dcmi data width */
    dcmi_handle.Init.JPEGMode = DCMI_JPEG_DISABLE;
    dcmi_handle.Instance = DCMI;
    HAL_DCMI_Init(&dcmi_handle);

    /*** Configure the DMA ***/
    /* Set the parameters to be configured */
    static DMA_HandleTypeDef dcmi_dma = {0};
    dcmi_dma.Instance = DMA2_Stream1;
    dcmi_dma.Init.Channel = DMA_CHANNEL_1;
    dcmi_dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dcmi_dma.Init.PeriphInc = DMA_PINC_DISABLE;
    dcmi_dma.Init.MemInc = DMA_MINC_DISABLE;
    dcmi_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dcmi_dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dcmi_dma.Init.Mode = DMA_CIRCULAR;
    dcmi_dma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    dcmi_dma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    dcmi_dma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dcmi_dma.Init.MemBurst = DMA_MBURST_SINGLE;
    dcmi_dma.Init.PeriphBurst = DMA_PBURST_SINGLE;

    __HAL_LINKDMA(&dcmi_handle, DMA_Handle, dcmi_dma);

    /*** Configure the NVIC for DCMI and DMA ***/
    NVIC_SetPriority(DCMI_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), DCMI_NVIC_PRIOR, 0));
    NVIC_EnableIRQ(DCMI_IRQn);
    NVIC_SetPriority(DMA2_Stream1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), DCMI_NVIC_PRIOR, 0));
    NVIC_EnableIRQ(DMA2_Stream1_IRQn);

    /* Configure the DMA stream */
    HAL_DMA_Init(dcmi_handle.DMA_Handle);
}

static void prv_camera_hwreset(void) {
    /* assert PWRDN pin (active high) */
    LL_GPIO_SetOutputPin(CAMERA_PWRDN_PORT, CAMERA_PWRDN_PIN);
    /* de-assert RST pin (active low) */
    LL_GPIO_ResetOutputPin(CAMERA_RESET_PORT, CAMERA_RESET_PIN);

    vTaskDelay(pdMS_TO_TICKS(5));   /* delay from AVDD stable to sensor power up stable */

    /* de-assert PWRDN pin */
    LL_GPIO_ResetOutputPin(CAMERA_PWRDN_PORT, CAMERA_PWRDN_PIN);
    vTaskDelay(pdMS_TO_TICKS(2));   /* delay from sensor power up stable to RESETB pull up */

    /* assert RST pin */
    LL_GPIO_SetOutputPin(CAMERA_RESET_PORT, CAMERA_RESET_PIN);
    vTaskDelay(pdMS_TO_TICKS(25));  /* delay from RESETB pull high to SCCB initialization */
}

static int32_t prv_sccb_readreg(uint16_t addr, uint16_t reg, uint8_t *pdata, uint16_t len) {
    int32_t ret;
    ret = soft_i2c_master_mem_read(&ov5640_i2c, reg, 2, pdata, len);
    return ret ? CAMERA_ERROR : CAMERA_OK;
}

static int32_t prv_sccb_writereg(uint16_t addr, uint16_t reg, uint8_t *pdata, uint16_t len) {
    int32_t ret;
    ret = soft_i2c_master_mem_write(&ov5640_i2c, reg, 2, pdata, len);
    return ret ? CAMERA_ERROR : CAMERA_OK;
}

static int32_t prv_camera_sensor_init(void) {
    int32_t ret;
    OV5640_IO_t ov5640_io = {
            .Init = prv_camera_sccb_init,
            .Address = CAMERA_I2C_ADDR,
            .ReadReg = prv_sccb_readreg,
            .WriteReg = prv_sccb_writereg,
    };

    ret = OV5640_RegisterBusIO(&ov5640_obj, &ov5640_io);
    if (ret != OV5640_OK) {
        log_e("register bus io failed");
        return CAMERA_ERROR;
    }

    uint32_t id;
    /* reset sensor */
    prv_camera_hwreset();
    /* check sensor id */
    ov5640_driver = &OV5640_CAMERA_Driver;

    if (ov5640_driver->ReadID(&ov5640_obj, &id) == OV5640_OK && id == 0x5640) {
        ret = ov5640_driver->Init(&ov5640_obj, OV5640_R320x240, OV5640_RGB565);
        if (ret != OV5640_OK) return CAMERA_ERROR;
        vTaskDelay(1000);
    }
    return CAMERA_OK;
}

void bsp_camera_continuous_start(void) {
    NT35510_SetWindow(0, 0, 320, 240);
    NT35510->REG = 0x2C00;
    HAL_DCMI_Start_DMA(&dcmi_handle, DCMI_MODE_CONTINUOUS, (uint32_t)(&(NT35510->RAM)), 0X9600);
}

void bsp_camera_init(void) {
    prv_camera_gpio_init();
    prv_camera_dcmi_init();
    prv_camera_hwreset();
    if (prv_camera_sensor_init() == CAMERA_OK) {
        log_i("sensor init success");
    } else {
        log_e("sensor init failed");
    };
    vTaskDelay(1000);
    bsp_camera_continuous_start();
}

void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi) {

}

void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi) {

}