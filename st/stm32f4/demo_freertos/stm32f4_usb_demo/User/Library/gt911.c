#include <stdint.h>
#define LOG_TAG "LCD"
#define LOG_LVL  ELOG_LVL_INFO

#include "gt911.h"
#include "elog.h"

#include "stm32f4xx_ll_bus.h"


/* Private macro -------------------------------------------------------------*/


#if       GOODIX_LOG_EN
#define   GOODIX_LOG(...)            log_i(__VA_ARGS__)
#define   GOODIX_HEXDUMP(n,l,b,s)    elog_hexdump(n,l,b,s)
#else
#define   GOODIX_LOG(...)            ((void)0U)
#define   GOODIX_HEXDUMP(n,l,b,s)    ((void)0U)
#endif


/* Private variables ---------------------------------------------------------*/

// goodix i2c handle
static soft_i2c_dev_t goodix_i2c_dev = {0};


// goodix i2c rx buffer
static uint8_t goodix_rx_buf[10] = {0};

// goodix i2c tx buffer
static uint8_t goodix_tx_buf[2] = {0};

/* I2C Operate port -----------------------------------------------------------*/

/**
 * @brief I2C init
 * 
 */
static void goodix_i2c_init(void) {
    // add user init code
    goodix_i2c_dev.dev_addr = GOODIX_ADDRESS;
    goodix_i2c_dev.scl.port = GOODIX_SCL_PORT;
    goodix_i2c_dev.scl.pin = GOODIX_SCL_PIN;
    goodix_i2c_dev.sda.port = GOODIX_SDA_PORT;
    goodix_i2c_dev.sda.pin = GOODIX_SDA_PIN;
    soft_i2c_init(&goodix_i2c_dev, GOODIX_I2C_FREQ);
}

///**
// * @brief Write data to goodix ic
// *
// * @param wr_data pointer of data to write
// * @param len length of write data
// * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
// */
//static int goodix_i2c_write(uint8_t *wr_data, int len) {
//    if (soft_i2c_master_write(&Goodix_I2C, wr_data, len)) {
//        return GOODIX_ERROR;
//    }
//    return GOODIX_OK;
//}
//
///**
// * @brief Read data from goodix ic
// *
// * @param rd_data pointer of read buffer
// * @param len length of read data
// * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
// */
//static int goodix_i2c_read(uint8_t *rd_data, int len) {
//    if (soft_i2c_master_read(&Goodix_I2C, rd_data, len)) {
//        return GOODIX_ERROR;
//    }
//    return GOODIX_OK;
//}

/**
 * @brief Write data to goodix register
 * 
 * @param reg_addr register address
 * @param wr_data  data to write
 * @param data_len length of read data
 * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
 */
static int goodix_i2c_mem_write(uint16_t reg_addr, uint8_t *wr_data, int data_len) {
    if (soft_i2c_master_mem_write(&goodix_i2c_dev, reg_addr, 2, wr_data, data_len)) {
        return GOODIX_ERROR;
    }
    return GOODIX_OK;
}

/**
 * @brief Read data from goodix register
 * 
 * @param reg_addr register address
 * @param rd_data  pointer of read buffer
 * @param data_len length of read data
 * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
 */
static int goodix_i2c_mem_read(uint16_t reg_addr, uint8_t *rd_data, int data_len) {
    if (soft_i2c_master_mem_read(&goodix_i2c_dev, reg_addr, 2, rd_data, data_len)) {
        return GOODIX_ERROR;
    }
    return GOODIX_OK;
}

/* IO Operate port -----------------------------------------------------------*/

/**
 * @brief Set GPIO Input mode
 * 
 * @param gpio GPIO port
 * @param pin GPIO Pin
 */
static void goodix_gpio_set_input(GPIO_TypeDef *gpio, uint32_t pin) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}

/**
 * @brief Set GPIO Output mode
 * 
 * @param gpio GPIO port
 * @param pin GPIO Pin
 */
static void goodix_gpio_set_output(GPIO_TypeDef *gpio, uint32_t pin) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(gpio, &GPIO_InitStruct);
}

/**
 * @brief Set GPIO output high level
 * 
 * @param gpio GPIO port
 * @param pin GPIO pin
 */
static inline void goodix_gpio_set_high(GPIO_TypeDef *gpio, uint32_t pin) {
    LL_GPIO_SetOutputPin(gpio, pin);
}

/**
 * @brief Set GPIO output low level
 * 
 * @param gpio GPIO port
 * @param pin GPIO pin
 */
static inline void goodix_gpio_set_low(GPIO_TypeDef *gpio, uint32_t pin) {
    LL_GPIO_ResetOutputPin(gpio, pin);
}

/**
 * @brief Set GPIO clock enable
 * 
 * @param gpio GPIO port
 */
static void goodix_gpio_clock_enable(GPIO_TypeDef *gpio_x) {
    if (gpio_x == GPIOA) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    else if (gpio_x == GPIOB) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    else if (gpio_x == GPIOC) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    else if (gpio_x == GPIOD) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    else if (gpio_x == GPIOE) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
    else if (gpio_x == GPIOF) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
    else if (gpio_x == GPIOG) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG);
    else if (gpio_x == GPIOH) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOH);
    else if (gpio_x == GPIOI) LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOI);
}


/* Private function declaration ----------------------------------------------*/

static void goodix_reset(void);

static void goodix_hardware_init(void);

static int goodix_software_init(goodix_config_t *config);

static int goodix_read_id(uint32_t *id);

static int goodix_write_cmd(uint8_t cmd);

static int goodix_read_stat(uint8_t *status);

static int goodix_set_stat(uint8_t status);


/* Private function definition -----------------------------------------------*/

#if GOODIX_CFG_OVERWRITE

// goodix config register value buffer
#if GOODIX_USER_CONFIG_EN
static uint8_t goodix_cfg[GOODIX_CFG_LENGTH] = GOODIX_CFG_GROUP;
#else
static uint8_t goodix_cfg[GOODIX_CFG_LENGTH] = {0};
#endif

/**
 * @brief Calculate checksum function
 *
 * @return uint8_t checksum
 */
static uint8_t goodix_calc_checksum(void) {
    uint8_t checksum = 0;
    for (int i = 0; i < GOODIX_CFG_LENGTH - 2; i++) {
        checksum += goodix_cfg[i];
    }
    checksum = ~checksum + 1;
    return checksum;
}

/**
 * @brief Write to goodix config registers
 *
 * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
 */
static int goodix_write_cfg(void) {
    goodix_cfg[GOODIX_CFG_LENGTH - 2] = goodix_calc_checksum();
    goodix_cfg[GOODIX_CFG_LENGTH - 1] = 0x01;
    if (goodix_i2c_mem_write(GOODIX_REG_CONFIG_DATA, goodix_cfg, GOODIX_CFG_LENGTH) != GOODIX_OK) {
        GOODIX_LOG("goodix write config failed");
        return GOODIX_ERROR;
    }
    GOODIX_LOG("goodix write config success");
    return GOODIX_OK;
}

#if !GOODIX_USER_CONFIG_EN
/**
 * @brief Read goodix config registers value
 *
 * @return int GOODIX_OK on read success | GOODIX_ERROR on read error
 */
static int goodix_read_cfg(void) {
    if (goodix_i2c_mem_read(GOODIX_REG_CONFIG_DATA, goodix_cfg, GOODIX_CFG_LENGTH) == GOODIX_OK) {
        if (goodix_calc_checksum() == goodix_cfg[GOODIX_CFG_LENGTH - 2]) {
            return GOODIX_OK;
        }
    }
    GOODIX_LOG("goodix read config failed");
    return GOODIX_ERROR;
}
#endif

#endif


/**
 * @brief Goodix chip init function
 * 
 * @param config pointer of goodix config value
 * @return int GOODIX_OK on init success | GOODIX_ERROR on init error
 */
int goodix_init(goodix_config_t *config) {
    goodix_hardware_init();
    return goodix_software_init(config);
}

/**
 * @brief Goodix read touch point function 
 * 
 * @param cordinate pointer of touch point buffer
 * @param number_of_cordinate number  of touch point
 * @return int GOODIX_OK on read success | GOODIX_ERROR on read error
 */
int goodix_read_touchpoint(goodix_point_t *cordinate, uint8_t *number_of_cordinate) {
    uint8_t status, tpnum;
    if (goodix_read_stat(&status) != GOODIX_OK) {
        return GOODIX_ERROR;
    }
    if (!(status & 0x80)) {
        return GOODIX_NORDY;
    }
    tpnum = status & 0x0F;
    if (tpnum) {
        for (int i = 0; i < tpnum; i++) {
            if (goodix_i2c_mem_read(GOODIX_POINT1_X_ADDR + (i << 3), goodix_rx_buf, 6) != GOODIX_OK) {
                return GOODIX_ERROR;
            }
            cordinate[i].x = (goodix_rx_buf[1] << 8) | goodix_rx_buf[0];
            cordinate[i].y = (goodix_rx_buf[3] << 8) | goodix_rx_buf[2];
            cordinate[i].size = (goodix_rx_buf[5] << 8) | goodix_rx_buf[4];
#if GOODIX_LOG_TOUCHPOINT
            GOODIX_LOG("TP: X->%-4u Y->%-4u Size->%-4u", cordinate[i].x, cordinate[i].y, cordinate[i].size);
#endif
        }
    }
    *number_of_cordinate = tpnum;
    goodix_set_stat(0);
    return GOODIX_OK;
}

/**
 * @brief Print goodix config register value
 * 
 * @return int GOODIX_OK on success | GOODIX_ERROR on error
 */
int goodix_print_cfg(void) {
    uint8_t rd_off = 0,btr;
    uint8_t checksum = 0;

    GOODIX_LOG("goodix config registers value:");
    for (int count = GOODIX_CFG_LENGTH; count > 0; count -= btr, rd_off += btr) {
        btr = count < 10 ? count : 10;
        if (goodix_i2c_mem_read(GOODIX_REG_CONFIG_DATA + rd_off, goodix_rx_buf, btr) != GOODIX_OK) {
            return GOODIX_ERROR;
        }
        for (int i = 0; i < btr; i++) {
            checksum += goodix_rx_buf[i];
        }

        GOODIX_HEXDUMP("", 10, goodix_rx_buf, btr);
    }
    GOODIX_LOG("goodix config checksum verify %s", checksum ? "failed" : "pass");
    return GOODIX_OK;
}

/**
 * @brief Goodix hardware init
 * 
 */
static void goodix_hardware_init(void) {
    goodix_gpio_clock_enable(GOODIX_INT_PORT);
    goodix_gpio_clock_enable(GOODIX_RST_PORT);
    goodix_gpio_set_input(GOODIX_INT_PORT, GOODIX_INT_PIN);
    goodix_gpio_set_output(GOODIX_RST_PORT, GOODIX_RST_PIN);
    goodix_i2c_init();
}

/**
 * @brief Goodix software init
 * 
 * @return int GOODIX_OK on init success | GOODIX_ERROR on init error
 */
static int goodix_software_init(goodix_config_t *config) {
    // Reset chip
    goodix_reset();

    // Reference capacitance calibration (Internal test)
    goodix_write_cmd(GOODIX_CMD_CALIBRATE);

    // Read product ID
    uint32_t goodix_id = 0;
    if (goodix_read_id(&goodix_id) != GOODIX_OK || goodix_id != GOODIX_PRODUCT_ID) {
        GOODIX_LOG("goodix id error: 0x%04X", goodix_id);
        return GOODIX_ERROR;
    }
    GOODIX_LOG("goodix model: GT911");

    // Soft reset chip
    goodix_write_cmd(GOODIX_CMD_SOFTRESET);

#if GOODIX_CFG_PRINT

    if (goodix_print_cfg() != GOODIX_OK) {
        return GOODIX_ERROR;
    }

#endif

#if GOODIX_CFG_OVERWRITE

#if GOODIX_USER_CFG_EN == 0
    // Read config register value
    if (goodix_read_cfg() != GOODIX_OK) {
      return GOODIX_ERROR;
    }
#endif

    // Write config register
    goodix_cfg[0]  = 0;
    goodix_cfg[1]  =  config->X_Resolution & 0x00FF;
    goodix_cfg[2]  = (config->X_Resolution >> 8) & 0x00FF;
    goodix_cfg[3]  =  config->Y_Resolution & 0x00FF;
    goodix_cfg[4]  = (config->Y_Resolution >> 8) & 0x00FF;
    goodix_cfg[5]  =  config->Num_Of_Touch ? config->Num_Of_Touch : 1;
    goodix_cfg[6]  = 0;
    goodix_cfg[6]  = ((config->ReverseX & 1) << 7) | ((config->ReverseY & 1) << 6) | ((config->SwithX2Y & 1) << 3);
    goodix_cfg[6] |= ((GOODIX_SOFT_NOISE_REDUCTION << 2) | GOODIX_INT_TRIGGER);
    if (goodix_write_cfg() != GOODIX_OK) {
      return GOODIX_ERROR;
    }
    GOODIX_LOG("goodix X resolution: %u", config->X_Resolution);
    GOODIX_LOG("goodix Y resolution: %u", config->Y_Resolution);
    GOODIX_LOG("goodix touchpoint num: %u", config->Num_Of_Touch ? config->Num_Of_Touch : 1);
#endif

    // Set read coordinates status
    goodix_write_cmd(GOODIX_CMD_READ);

    GOODIX_LOG("goodix initialize success");
    return GOODIX_OK;
}

/**
 * @brief Goodix reset chip function
 * 
 */
static void goodix_reset(void) {
    goodix_gpio_set_output(GOODIX_INT_PORT, GOODIX_INT_PIN);
    goodix_gpio_set_low(GOODIX_RST_PORT, GOODIX_RST_PIN);
    goodix_gpio_set_low(GOODIX_INT_PORT, GOODIX_INT_PIN);
    Goodix_Delay(1);
    goodix_gpio_set_high(GOODIX_RST_PORT, GOODIX_RST_PIN);
    Goodix_Delay(100);
    goodix_gpio_set_input(GOODIX_INT_PORT, GOODIX_INT_PIN);
}


/**
 * @brief Read goodix product id
 * 
 * @param id pointer of id buffer
 * @return int 
 */
static int goodix_read_id(uint32_t *id) {
    if (goodix_i2c_mem_read(GOODIX_REG_ID, goodix_rx_buf, 4) != GOODIX_OK) {
        return GOODIX_ERROR;
    }
    *id = goodix_rx_buf[0] << 24 | goodix_rx_buf[1] << 16 | goodix_rx_buf[2] << 8 | goodix_rx_buf[3];
    return GOODIX_OK;
}


/**
 * @brief Write command to goodix chip
 * 
 * @param cmd command
 * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
 */
static int goodix_write_cmd(uint8_t cmd) {
    goodix_tx_buf[0] = cmd;
    return goodix_i2c_mem_write(GOODIX_REG_COMMAND, goodix_tx_buf, 1);
}

/**
 * @brief Get touch buffer status value
 * 
 * @param status value of touch buffer status
 * @return int GOODIX_OK on read success | GOODIX_ERROR on read error
 */
static int goodix_read_stat(uint8_t *status) {
    return goodix_i2c_mem_read(GOODIX_READ_COORD_ADDR, status, 1);
}

/**
 * @brief Set touch buffer status value
 * 
 * @param status value to write
 * @return int GOODIX_OK on write success | GOODIX_ERROR on write error
 */
static int goodix_set_stat(uint8_t status) {
    goodix_tx_buf[0] = status;
    return goodix_i2c_mem_write(GOODIX_READ_COORD_ADDR, goodix_tx_buf, 1);
}
