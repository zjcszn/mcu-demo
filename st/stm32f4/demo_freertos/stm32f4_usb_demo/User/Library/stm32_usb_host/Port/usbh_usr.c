/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file            : usb_host.c
  * @version         : v1.0_Cube
  * @brief           : This file implements the USB Host
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#define LOG_TAG "USBH"
#define LOG_LVL ELOG_LVL_INFO

#include "main.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <usbh_usr.h>
#include <usbh_core.h>
#include <usbh_hid.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USB Host core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
ApplicationTypeDef Appli_state = APPLICATION_IDLE;
SemaphoreHandle_t hid_sync;
/*
 * -- Insert your variables declaration here --
 */
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*
 * user callback declaration
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);

/*
 * -- Insert your external function declaration here --
 */
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * Init USB host library, add supported class and start the library
  * @retval None
  */
void stusb_init(void) {
    /* USER CODE BEGIN USB_HOST_Init_PreTreatment */
    hid_sync = xSemaphoreCreateBinary();
    /* USER CODE END USB_HOST_Init_PreTreatment */
    
    /* Init host Library, add supported class and start the library. */
    if (USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS) != USBH_OK) {
        Error_Handler();
    }
    if (USBH_RegisterClass(&hUsbHostFS, USBH_HID_CLASS) != USBH_OK) {
        Error_Handler();
    }
    if (USBH_Start(&hUsbHostFS) != USBH_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN USB_HOST_Init_PostTreatment */
    
    /* USER CODE END USB_HOST_Init_PostTreatment */
}

/*
 * user callback definition
 */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id) {
    /* USER CODE BEGIN CALL_BACK_1 */
    switch (id) {
        case HOST_USER_SELECT_CONFIGURATION:
            break;
        
        case HOST_USER_DISCONNECTION:
            Appli_state = APPLICATION_DISCONNECT;
            break;
        
        case HOST_USER_CLASS_ACTIVE:
            Appli_state = APPLICATION_READY;
            break;
        
        case HOST_USER_CONNECTION:
            Appli_state = APPLICATION_START;
            break;
        
        default:
            break;
    }
    /* USER CODE END CALL_BACK_1 */
}

void USBH_HID_EventCallback(USBH_HandleTypeDef *phost) {
    switch (phost->pActiveClass->ClassCode) {
        case USB_HID_CLASS:
            xSemaphoreGiveFromISR(hid_sync, NULL);
            break;
        
        default:
            break;
    }
}

void USBH_User_GetMouseInfo(void *args) {
    HID_MOUSE_Info_TypeDef *mouse_info = NULL;
    while (1) {
        xSemaphoreTake(hid_sync, portMAX_DELAY);
        mouse_info = USBH_HID_GetMouseInfo(&hUsbHostFS);
        if (mouse_info != NULL) {
            if (mouse_info->x || mouse_info->y) {
                log_i("mouse: x-> %d y-> %d ", (int)mouse_info->x, (int)mouse_info->y);
            }
            if (mouse_info->buttons[0]) {
                log_i("mouse: left button click");
            }
            if (mouse_info->buttons[1]) {
                log_i("mouse: right button click");
            }
            if (mouse_info->buttons[2]) {
                log_i("mouse: mid button click");
            }
            if (mouse_info->wheel) {
                log_i("mouse: wheel %s", mouse_info->wheel > 0 ? "increase" : "decrease");
            }
        }
    }
}

void stusb_demo(void) {
    xTaskCreate(USBH_User_GetMouseInfo, "hid host demo", 256, NULL, 10, NULL);
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/