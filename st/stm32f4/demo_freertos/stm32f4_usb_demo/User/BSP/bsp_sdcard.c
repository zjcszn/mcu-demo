/**
  ******************************************************************************
  * @file    stm32469i_discovery_sd.c
  * @author  MCD Application Team
  * @brief   This file includes the uSD card driver mounted on STM32469I-Discovery
  *          board.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* File Info : -----------------------------------------------------------------
                                   User NOTES
1. How To use this driver:
--------------------------
   - This driver is used to drive the micro SD external card mounted on STM32469I-Discovery
     board.
   - This driver does not need a specific component driver for the micro SD device
     to be included with.

2. Driver description:
---------------------
  + Initialization steps:
     o Initialize the micro SD card using the BSP_SD_Init() function. This
       function includes the MSP layer hardware resources initialization and the
       SDIO interface configuration to interface with the external micro SD. It
       also includes the micro SD initialization sequence.
     o To check the SD card presence you can use the function BSP_SD_IsDetected() which
       returns the detection status
     o If SD presence detection interrupt mode is desired, you must configure the
       SD detection interrupt mode by calling the function BSP_SD_ITConfig(). The interrupt
       is generated as an external interrupt whenever the micro SD card is
       plugged/unplugged in/from the board.
     o The function BSP_SD_GetCardInfo() is used to get the micro SD card information
       which is stored in the structure "HAL_SD_CardInfoTypeDef".

     + Micro SD card operations
        o The micro SD card can be accessed with read/write block(s) operations once
          it is ready for access. The access can be performed whether using the polling
          mode by calling the functions BSP_SD_ReadBlocks()/BSP_SD_WriteBlocks(), or by DMA
          transfer using the functions BSP_SD_ReadBlocks_DMA()/BSP_SD_WriteBlocks_DMA()
        o The DMA transfer complete is used with interrupt mode. Once the SD transfer
          is complete, the SD interrupt is handled using the function BSP_SD_IRQHandler(),
          the DMA Tx/Rx transfer complete are handled using the functions
          BSP_SD_DMA_Tx_IRQHandler()/BSP_SD_DMA_Rx_IRQHandler(). The corresponding user callbacks
          are implemented by the user at application level.
        o The SD erase block(s) is performed using the function BSP_SD_Erase() with specifying
          the number of blocks to erase.
        o The SD runtime status is returned when calling the function BSP_SD_GetCardState().

------------------------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "bsp_sdcard.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "stm32f4xx_hal_sd.h"
#include <string.h>
/* Warnning *******************************************************************
 * 1. sdio dma memory data alignment should be set by byte alignment
 *
*******************************************************************************/


/**
* @brief SD Card handle
*/
SD_HandleTypeDef uSdHandle;

static SemaphoreHandle_t sd_dmarx_notice;
static SemaphoreHandle_t sd_dmatx_notice;

/**
  * @brief  Initializes the SD card device.
  * @retval SD status
  */
uint8_t BSP_SD_Init(void) {
    uint8_t sd_state = MSD_OK;
    /* PLLSAI is dedicated to LCD periph. Do not use it to get 48MHz*/
    if (sd_dmarx_notice == NULL) {
        sd_dmarx_notice = xSemaphoreCreateBinary();
    }
    if (sd_dmatx_notice == NULL) {
        sd_dmatx_notice = xSemaphoreCreateBinary();
    }
    
    if (!sd_dmarx_notice || !sd_dmatx_notice) {
        sd_state = MSD_ERROR;
        goto _exit;
    }
    
    memset(&uSdHandle, 0X00, sizeof(SD_HandleTypeDef));
    
    /* uSD device interface configuration */
    uSdHandle.Instance = SDIO;
    /* If Use DMA to transfer data, should be disable HardwareFlowControl */
    uSdHandle.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
    uSdHandle.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
    uSdHandle.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
    uSdHandle.Init.BusWide = SDIO_BUS_WIDE_1B;
    uSdHandle.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
    uSdHandle.Init.ClockDiv = SDIO_TRANSFER_CLK_DIV + 1;

#if BSP_SD_USE_DETECT
    
    /* Msp SD Detect pin initialization */
    BSP_SD_Detect_MspInit(&uSdHandle, NULL);
    if(BSP_SD_IsDetected() != SD_PRESENT)   /* Check if SD card is present */
    {
      return MSD_ERROR_SD_NOT_PRESENT;
    }

#endif
    
    /* Msp SD initialization */
    BSP_SD_MspInit(&uSdHandle, NULL);
    
    /* HAL SD initialization */
    if (HAL_SD_Init(&uSdHandle) != HAL_OK) {
        sd_state = MSD_ERROR;
    }
    
    /* Configure SD Bus width */
    if (sd_state == MSD_OK) {
        /* Enable wide operation */
        if (HAL_SD_ConfigWideBusOperation(&uSdHandle, SDIO_BUS_WIDE_4B) != HAL_OK) {
            sd_state = MSD_ERROR;
        } else {
            sd_state = MSD_OK;
        }
    }

_exit:
    if (sd_state == MSD_OK) {
        return MSD_OK;
    } else {
        BSP_SD_DeInit();
        return MSD_ERROR;
    }
}

/**
  * @brief  DeInitializes the SD card device.
  * @retval SD status
  */
uint8_t BSP_SD_DeInit(void) {
    uint8_t sd_state = MSD_OK;
    
    uSdHandle.Instance = SDIO;
    /* HAL SD deinitialization */
    if (HAL_SD_DeInit(&uSdHandle) != HAL_OK) {
        sd_state = MSD_ERROR;
    }
    
    /* Msp SD deinitialization */
    uSdHandle.Instance = SDIO;
    BSP_SD_MspDeInit(&uSdHandle, NULL);
    
    if (sd_dmarx_notice)
        vSemaphoreDelete(sd_dmarx_notice);
    if (sd_dmatx_notice)
        vSemaphoreDelete(sd_dmatx_notice);
    sd_dmarx_notice = NULL;
    sd_dmatx_notice = NULL;
    
    return sd_state;
}

#if BSP_SD_USE_DETECT
/**
  * @brief  Configures Interrupt mode for SD detection pin.
  * @retval Returns 0
  */
uint8_t BSP_SD_ITConfig(void)
{
  GPIO_InitTypeDef gpio_init_structure;

  /* Configure Interrupt mode for SD detection pin */
  gpio_init_structure.Pin = SD_DETECT_PIN;
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FAST;
  gpio_init_structure.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpio_init_structure);

  /* Enable and set SD detect EXTI Interrupt to the lowest priority */
  HAL_NVIC_SetPriority((IRQn_Type)(SD_DETECT_EXTI_IRQn), 0x0F, 0x00);
  HAL_NVIC_EnableIRQ((IRQn_Type)(SD_DETECT_EXTI_IRQn));

  return MSD_OK;
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @retval Returns if SD is detected or not
 */
uint8_t BSP_SD_IsDetected(void)
{
  __IO uint8_t  status = SD_PRESENT;

  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) == GPIO_PIN_SET)
  {
    status = SD_NOT_PRESENT;
  }

    return status;
}
#endif

/**
  * @brief  Reads block(s) from a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @param  Timeout: Timeout for read operation
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout) {
    if (HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in polling mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @param  Timeout: Timeout for write operation
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout) {
    if (HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}

/**
  * @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  ReadAddr: Address from where data is to be read
  * @param  NumOfBlocks: Number of SD blocks to read
  * @retval SD status
  */
uint8_t BSP_SD_ReadBlocks_DMA(uint32_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks) {
    /* Read block(s) in DMA transfer mode */
    if (HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks) != HAL_OK) {
        return MSD_ERROR;
    }
    if (xSemaphoreTake(sd_dmarx_notice, pdMS_TO_TICKS(SD_DATATIMEOUT)) != pdTRUE) {
        return MSD_ERROR;
    }
    return MSD_OK;
}

/**
  * @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
  * @param  pData: Pointer to the buffer that will contain the data to transmit
  * @param  WriteAddr: Address from where data is to be written
  * @param  NumOfBlocks: Number of SD blocks to write
  * @retval SD status
  */
uint8_t BSP_SD_WriteBlocks_DMA(uint32_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks) {
    /* Write block(s) in DMA transfer mode */
    if (HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks) != HAL_OK) {
        return MSD_ERROR;
    }
    if (xSemaphoreTake(sd_dmatx_notice, pdMS_TO_TICKS(SD_DATATIMEOUT)) != pdTRUE) {
        return MSD_ERROR;
    }
    return MSD_OK;
}

/**
  * @brief  Erases the specified memory area of the given SD card.
  * @param  StartAddr: Start byte address
  * @param  EndAddr: End byte address
  * @retval SD status
  */
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr) {
    if (HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != HAL_OK) {
        return MSD_ERROR;
    } else {
        return MSD_OK;
    }
}


/**
  * @brief  Initializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_SD_MspInit(SD_HandleTypeDef *hsd, void *Params) {
    static DMA_HandleTypeDef dma_rx_handle;
    static DMA_HandleTypeDef dma_tx_handle;
    LL_GPIO_InitTypeDef gpio_init_structure;
    
    /* Enable SDIO clock */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SDIO);
    
    /* Enable DMA2 clocks */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
    
    /* Enable GPIOs clock */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD);
    
    /* Common GPIO configuration */
    gpio_init_structure.Mode = LL_GPIO_MODE_ALTERNATE;
    gpio_init_structure.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    gpio_init_structure.Pull = LL_GPIO_PULL_UP;
    gpio_init_structure.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = LL_GPIO_AF_12;
    
    /* GPIOC configuration */
    gpio_init_structure.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12;
    
    LL_GPIO_Init(GPIOC, &gpio_init_structure);
    
    /* GPIOD configuration */
    gpio_init_structure.Pin = LL_GPIO_PIN_2;
    LL_GPIO_Init(GPIOD, &gpio_init_structure);
    
    /* NVIC configuration for SDIO interrupts */
    NVIC_SetPriority(SDIO_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0X05, 0));
    NVIC_EnableIRQ(SDIO_IRQn);
    
    /* Configure DMA Rx parameters */
    dma_rx_handle.Init.Channel = SD_DMAx_Rx_CHANNEL;
    dma_rx_handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma_rx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_rx_handle.Init.MemInc = DMA_MINC_ENABLE;
    dma_rx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_rx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_rx_handle.Init.Mode = DMA_PFCTRL;
    dma_rx_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    dma_rx_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    dma_rx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_rx_handle.Init.MemBurst = DMA_MBURST_INC4;
    dma_rx_handle.Init.PeriphBurst = DMA_PBURST_INC4;
    
    dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
    
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, dma_rx_handle);
    
    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dma_rx_handle);
    
    /* Configure the DMA stream */
    HAL_DMA_Init(&dma_rx_handle);
    
    /* Configure DMA Tx parameters */
    dma_tx_handle.Init.Channel = SD_DMAx_Tx_CHANNEL;
    dma_tx_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
    dma_tx_handle.Init.PeriphInc = DMA_PINC_DISABLE;
    dma_tx_handle.Init.MemInc = DMA_MINC_ENABLE;
    dma_tx_handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dma_tx_handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    dma_tx_handle.Init.Mode = DMA_PFCTRL;
    dma_tx_handle.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    dma_tx_handle.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    dma_tx_handle.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    dma_tx_handle.Init.MemBurst = DMA_MBURST_INC4;
    dma_tx_handle.Init.PeriphBurst = DMA_PBURST_INC4;
    
    dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
    
    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, dma_tx_handle);
    
    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dma_tx_handle);
    
    /* Configure the DMA stream */
    HAL_DMA_Init(&dma_tx_handle);
    
    /* NVIC configuration for DMA transfer complete interrupt */
    NVIC_SetPriority(SD_DMAx_Rx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0X05, 0));
    NVIC_EnableIRQ(SD_DMAx_Rx_IRQn);
    
    /* NVIC configuration for DMA transfer complete interrupt */
    NVIC_SetPriority(SD_DMAx_Tx_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0X05, 0));
    NVIC_EnableIRQ(SD_DMAx_Tx_IRQn);
}

#if BSP_SD_USE_DETECT
/**
  * @brief  Initializes the SD Detect pin MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_SD_Detect_MspInit(SD_HandleTypeDef *hsd, void *Params)
{
  GPIO_InitTypeDef  gpio_init_structure;

  SD_DETECT_GPIO_CLK_ENABLE();

  /* GPIO configuration in input for uSD_Detect signal */
  gpio_init_structure.Pin       = SD_DETECT_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_INPUT;
  gpio_init_structure.Pull      = GPIO_PULLUP;
  gpio_init_structure.Speed     = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(SD_DETECT_GPIO_PORT, &gpio_init_structure);
}
#endif

/**
  * @brief  DeInitializes the SD MSP.
  * @param  hsd: SD handle
  * @param  Params : pointer on additional configuration parameters, can be NULL.
  */
__weak void BSP_SD_MspDeInit(SD_HandleTypeDef *hsd, void *Params) {
    static DMA_HandleTypeDef dma_rx_handle;
    static DMA_HandleTypeDef dma_tx_handle;
    
    
    /* Disable NVIC for DMA transfer complete interrupts */
    NVIC_DisableIRQ(SD_DMAx_Rx_IRQn);
    NVIC_DisableIRQ(SD_DMAx_Tx_IRQn);
    
    /* Deinitialize the stream for new transfer */
    dma_rx_handle.Instance = SD_DMAx_Rx_STREAM;
    HAL_DMA_DeInit(&dma_rx_handle);
    
    /* Deinitialize the stream for new transfer */
    dma_tx_handle.Instance = SD_DMAx_Tx_STREAM;
    HAL_DMA_DeInit(&dma_tx_handle);
    
    /* Disable NVIC for SDIO interrupts */
    NVIC_DisableIRQ(SDIO_IRQn);
    
    /* DeInit GPIO pins can be done in the application
       (by surcharging this __weak function) */
    
    /* Disable SDIO clock */
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_SDIO);
    
    /* GPOI pins clock and DMA cloks can be shut down in the applic
       by surcgarging this __weak function */
}

/**
  * @brief  Gets the current SD card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  SD_TRANSFER_OK: No data transfer is acting
  *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
  */
uint8_t BSP_SD_GetCardState(void) {
    return ((HAL_SD_GetCardState(&uSdHandle) == HAL_SD_CARD_TRANSFER) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY);
}

/**
  * @brief  Get SD information about specific SD card.
  * @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
  * @retval None
  */
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo) {
    /* Get SD card Information */
    HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}

/**
  * @brief SD Abort callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd) {
    BSP_SD_AbortCallback();
}

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd) {
    BSP_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd) {
    BSP_SD_ReadCpltCallback();
}

/**
  * @brief BSP SD Abort callbacks
  * @retval None
  */
void BSP_SD_AbortCallback(void) {

}

/**
  * @brief BSP Tx Transfer completed callbacks
  * @retval None
  */
void BSP_SD_WriteCpltCallback(void) {
    xSemaphoreGiveFromISR(sd_dmatx_notice, NULL);
}

/**
  * @brief BSP Rx Transfer completed callbacks
  * @retval None
  */
void BSP_SD_ReadCpltCallback(void) {
    xSemaphoreGiveFromISR(sd_dmarx_notice, NULL);
}

/**
  * @brief  This function handles DMA2 Stream 3 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SD_DMA_Rx_IRQHandler(void) {
    HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SD_DMA_Tx_IRQHandler(void) {
    HAL_DMA_IRQHandler(uSdHandle.hdmatx);
}

/**
  * @brief  This function handles SDIO interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SD_IRQHandler(void) {
    HAL_SD_IRQHandler(&uSdHandle);
}