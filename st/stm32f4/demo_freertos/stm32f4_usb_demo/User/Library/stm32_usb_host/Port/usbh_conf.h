/**
  ******************************************************************************
  * @file    usbh_conf_template.h
  * @author  MCD Application Team
  * @brief   Header file for usbh_conf_template.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF_H
#define __USBH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"  /* replace 'stm32xxx' with your HAL driver header filename, ex: stm32f4xx.h */
#include "stm32f4xx_hal_hcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elog.h"
/** @addtogroup STM32_USB_HOST_LIBRARY
  * @{
  */

/** @defgroup USBH_CONF
  * @brief usb host low level driver configuration file
  * @{
  */

/** @defgroup USBH_CONF_Exported_Defines
  * @{
  */

#define USBH_MAX_NUM_ENDPOINTS                2U
#define USBH_MAX_NUM_INTERFACES               2U
#define USBH_MAX_NUM_CONFIGURATION            1U
#define USBH_KEEP_CFG_DESCRIPTOR              1U
#define USBH_MAX_NUM_SUPPORTED_CLASS          1U
#define USBH_MAX_SIZE_CONFIGURATION           256U
#define USBH_MAX_DATA_BUFFER                  512U
#define USBH_DEBUG_LEVEL                      3U
#define USBH_USE_OS                           1U

/****************************************/
/* #define for FS and HS identification */
#define HOST_HS        0
#define HOST_FS        1

#if (USBH_USE_OS == 1)
#include "FreeRTOS.h"
#include "queue.h"

#define USBH_PROCESS_PRIO          30
#define USBH_PROCESS_STACK_SIZE    (1024U)
#endif /* (USBH_USE_OS == 1) */

/** @defgroup USBH_Exported_Macros
  * @{
  */

/* Memory management macros */
/** Alias for memory allocation. */
#if (USBH_USE_OS == 1)
#define USBH_malloc         pvPortMalloc
#else
#define USBH_malloc         malloc
#endif

/** Alias for memory release. */
#if (USBH_USE_OS == 1)
#define USBH_free           vPortFree
#else
#define USBH_free           free
#endif

/** Alias for memory set. */
#define USBH_memset         memset

/** Alias for memory copy. */
#define USBH_memcpy         memcpy

/* DEBUG macros */
#if (USBH_DEBUG_LEVEL > 0U)
#define USBH_UsrLog(...)   do { \
                                 log_i(__VA_ARGS__); \
                              } while (0)
#else
#define USBH_UsrLog(...)    ((void)0U)
#endif

#if (USBH_DEBUG_LEVEL > 1U)

#define USBH_ErrLog(...) do { \
                               log_e(__VA_ARGS__); \
                            } while (0)
#else
#define USBH_ErrLog(...) ((void)0U)
#endif

#if (USBH_DEBUG_LEVEL > 2U)
#define USBH_DbgLog(...)   do { \
                                 log_d(__VA_ARGS__); \
                              } while (0)
#else
#define USBH_DbgLog(...) ((void)0U)
#endif

/**
  * @}
  */

#define USBH_POST_EVENT_MESSAGE(event, msg)                         \
do {                                                                \
    if (xPortIsInsideInterrupt() == pdTRUE) {                       \
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;              \
        xQueueSendFromISR(event, msg, &xHigherPriorityTaskWoken);   \
        if (xHigherPriorityTaskWoken == pdTRUE) {                   \
            taskYIELD();                                            \
        }                                                           \
    } else {                                                        \
        xQueueSend(event, msg, 0);                                  \
    }                                                               \
} while (0U)
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_Types
  * @{
  */
/**
  * @}
  */


/** @defgroup USBH_CONF_Exported_Macros
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_Variables
  * @{
  */
/**
  * @}
  */

/** @defgroup USBH_CONF_Exported_FunctionsPrototype
  * @{
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBH_CONF_TEMPLATE_H */


/**
  * @}
  */

/**
  * @}
  */
