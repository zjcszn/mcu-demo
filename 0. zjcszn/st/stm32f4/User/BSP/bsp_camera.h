//
// Created by zjcszn on 2023/11/11.
//

#ifndef _BSP_BSP_CAMERA_H_
#define _BSP_BSP_CAMERA_H_

#include "stm32f4xx_ll_gpio.h"

#define CAMERA_I2C_ADDR     0x3C
#define CAMERA_I2C_SCL_PIN  LL_GPIO_PIN_6
#define CAMERA_I2C_SCL_PORT GPIOD
#define CAMERA_I2C_SDA_PIN  LL_GPIO_PIN_7
#define CAMERA_I2C_SDA_PORT GPIOD

#define CAMERA_RESET_PIN    LL_GPIO_PIN_15
#define CAMERA_RESET_PORT   GPIOG
#define CAMERA_PWRDN_PIN    LL_GPIO_PIN_9
#define CAMERA_PWRDN_PORT   GPIOG

#define DCMI_NVIC_PRIOR     0xF

#define CAMERA_OK           ( 0)
#define CAMERA_ERROR        (-1)

void bsp_camera_init(void);

#endif //_BSP_BSP_CAMERA_H_
