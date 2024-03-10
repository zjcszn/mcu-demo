/**
 * @file    DAP_config.h
 * @brief
 *
 * DAPLink Interface Firmware
 * Copyright (c) 2009-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__

#include "stm32h7xx_hal.h"
#include "stdint.h"
#include "cmsis_os2.h"
#include "IO_Config.h"
#include "bsp.h"


#define SPI_MODE_ENABLE        0                            /* 1 表示SPI模式， 0表示GPIO模式 */

//#include "uart.h"

//#include "debug_cm.h"
//#include "swd_host.h"
//**************************************************************************************************
/**
\defgroup DAP_Config_Debug_gr CMSIS-DAP Debug Unit Information
\ingroup DAP_ConfigIO_gr
@{
Provides definitions about:
 - Definition of Cortex-M processor parameters used in CMSIS-DAP Debug Unit.
 - Debug Unit communication packet size.
 - Debug Access Port communication mode (JTAG or SWD).
 - Optional information about a connected Target Device (for Evaluation Boards).
*/

/// Processor Clock of the Cortex-M MCU used in the Debug Unit.
/// This value is used to calculate the SWD/JTAG clock speed.
#define CPU_CLOCK               SystemCoreClock        ///< Specifies the CPU Clock in Hz

/// Number of processor cycles for I/O Port write operations.
/// This value is used to calculate the SWD/JTAG clock speed that is generated with I/O
/// Port write operations in the Debug Unit by a Cortex-M MCU. Most Cortex-M processors
/// requrie 2 processor cycles for a I/O Port Write operation.  If the Debug Unit uses
/// a Cortex-M0+ processor with high-speed peripheral I/O only 1 processor cycle might be
/// requrired.
#define IO_PORT_WRITE_CYCLES    2               ///< I/O Cycles: 2=default, 1=Cortex-M0+ fast I/0

/// Indicate that Serial Wire Debug (SWD) communication mode is available at the Debug Access Port.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define DAP_SWD                 1               ///< SWD Mode:  1 = available, 0 = not available

/// Indicate that JTAG communication mode is available at the Debug Port.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define DAP_JTAG                0               ///< JTAG Mode: 1 = available, 0 = not available.

/// Configure maximum number of JTAG devices on the scan chain connected to the Debug Access Port.
/// This setting impacts the RAM requirements of the Debug Unit. Valid range is 1 .. 255.
#define DAP_JTAG_DEV_CNT        0               ///< Maximum number of JTAG devices on scan chain

/// Default communication mode on the Debug Access Port.
/// Used for the command \ref DAP_Connect when Port Default mode is selected.
#define DAP_DEFAULT_PORT        1               ///< Default JTAG/SWJ Port Mode: 1 = SWD, 2 = JTAG.

/// Default communication speed on the Debug Access Port for SWD and JTAG mode.
/// Used to initialize the default SWD/JTAG clock frequency.
/// The command \ref DAP_SWJ_Clock can be used to overwrite this default setting.
#define DAP_DEFAULT_SWJ_CLOCK   5000000         ///< Default SWD/JTAG clock frequency in Hz.

/// Maximum Package Size for Command and Response data.
/// This configuration settings is used to optimized the communication performance with the
/// debugger and depends on the USB peripheral. Change setting to 1024 for High-Speed USB.
#define DAP_PACKET_SIZE        64              ///< USB: 64 = Full-Speed, 1024 = High-Speed.

/// Maximum Package Buffers for Command and Response data.
/// This configuration settings is used to optimized the communication performance with the
/// debugger and depends on the USB peripheral. For devices with limited RAM or USB buffer the
/// setting can be reduced (valid range is 1 .. 255). Change setting to 4 for High-Speed USB.
#define DAP_PACKET_COUNT       4              ///< Buffers: 64 = Full-Speed, 4 = High-Speed.

/// Indicate that UART Serial Wire Output (SWO) trace is available.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define SWO_UART                0               ///< SWO UART:  1 = available, 0 = not available

/// Maximum SWO UART Baudrate
#define SWO_UART_MAX_BAUDRATE   10000000U       ///< SWO UART Maximum Baudrate in Hz

/// Indicate that Manchester Serial Wire Output (SWO) trace is available.
/// This information is returned by the command \ref DAP_Info as part of <b>Capabilities</b>.
#define SWO_MANCHESTER          0               ///< SWO Manchester:  1 = available, 0 = not available

/// SWO Trace Buffer Size.
#define SWO_BUFFER_SIZE         4096U           ///< SWO Trace Buffer Size in bytes (must be 2^n)


/// Debug Unit is connected to fixed Target Device.
/// The Debug Unit may be part of an evaluation board and always connected to a fixed
/// known device.  In this case a Device Vendor and Device Name string is stored which
/// may be used by the debugger or IDE to configure device parameters.
#define TARGET_DEVICE_FIXED     0               ///< Target Device: 1 = known, 0 = unknown;

#if TARGET_DEVICE_FIXED
#define TARGET_DEVICE_VENDOR    ""              ///< String indicating the Silicon Vendor
#define TARGET_DEVICE_NAME      ""              ///< String indicating the Target Device
#endif

///@}


//static __inline void pin_out_init(GPIO_TypeDef* GPIOx, uint8_t pin_bit)
//{
//    if(pin_bit >= 8)
//    {
//        GPIOx->CRH &= ~(0x0000000F << ((pin_bit-8) << 2));
//        GPIOx->CRH |= ( ((uint32_t)(0x00|0x03) & 0x0F) << ((pin_bit-8) << 2) );
//    }
//    else
//    {
//        GPIOx->CRL &= ~(0x0000000F << ((pin_bit) << 2));
//        GPIOx->CRL |= ( ((uint32_t)(0x00|0x03) & 0x0F) << ((pin_bit) << 2) );
//    }
//}
static __inline void pin_out_init(GPIO_TypeDef* GPIOx, uint8_t pin_bit)
{
//#define  GPIO_MODE_INPUT                        ((uint32_t)0x00000000U)   /*!< Input Floating Mode                   */
//#define  GPIO_MODE_OUTPUT_PP                    ((uint32_t)0x00000001U)   /*!< Output Push Pull Mode                 */
//#define GPIO_MODE             ((uint32_t)0x00000003)

//#define GPIO_MODER_MODER0_Pos            (0U)        
//#define GPIO_MODER_MODER0_Msk            (0x3U << GPIO_MODER_MODER0_Pos)       /*!< 0x00000003 */
//#define GPIO_MODER_MODER0                GPIO_MODER_MODER0_Msk           


//#define GPIO_OTYPER_OT_0                 ((uint32_t)0x00000001)    
//#define GPIO_OUTPUT_TYPE      ((uint32_t)0x00000010)
    
      uint32_t temp;

      /*--------------------- GPIO Mode Configuration ------------------------*/

      /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
//      temp = GPIOx->MODER;
//      temp &= ~(GPIO_MODER_MODER0 << (pin_bit * 2));
//      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2));
//      GPIOx->MODER = temp;
      temp = GPIOx->MODER;
      temp &= ~(0x3U << (pin_bit * 2));
      temp |= ((0x00000001U & 0x00000003) << (pin_bit * 2));
      GPIOx->MODER = temp;        

      /* In case of Output or Alternate function mode selection */
//      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
//         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
//      {
        /* Check the Speed parameter */
//        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
//        /* Configure the IO Speed */
//        temp = GPIOx->OSPEEDR; 
//        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
//        temp |= (GPIO_Init->Speed << (position * 2));
//        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
//        temp = GPIOx->OTYPER;
//        temp &= ~(GPIO_OTYPER_OT_0 << pin_bit) ;
//        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << pin_bit);
//        GPIOx->OTYPER = temp;
        
        /* �Ż� */
        temp = GPIOx->OTYPER;
        temp &= ~(0x00000001 << pin_bit) ;
        //temp |= (((0x00000001U & 0x00000010) >> 4) << pin_bit);
        GPIOx->OTYPER = temp;        
//      }

      /* Activate the Pull-up or Pull down resistor for the current IO */
//      temp = GPIOx->PUPDR;
//      temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
//      temp |= ((GPIO_Init->Pull) << (position * 2));
//      GPIOx->PUPDR = temp;    
}


//static __inline void pin_out_od_init(GPIO_TypeDef* GPIOx, uint8_t pin_bit)
//{
//    if(pin_bit >= 8)
//    {
//        GPIOx->CRH &= ~(0x0000000F << ((pin_bit-8) << 2));
//        GPIOx->CRH |= ( ((uint32_t)(0x04|0x03) & 0x0F) << ((pin_bit-8) << 2) );
//    }
//    else
//    {
//        GPIOx->CRL &= ~(0x0000000F << ((pin_bit) << 2));
//        GPIOx->CRL |= ( ((uint32_t)(0x04|0x03) & 0x0F) << ((pin_bit) << 2) );
//    }
//}

//static __inline void pin_in_init(GPIO_TypeDef* GPIOx, uint8_t pin_bit, uint8_t mode)
//{
//    uint8_t config;
//    if(mode == 1)
//        config = 0x08; //Up
//    else if(mode == 2)
//        config = 0x08; //down
//    else
//        config = 0x00; //GPIO_Mode_AIN

//    if(pin_bit >= 8)
//    {
//        GPIOx->CRH &= ~(0x0000000F << ((pin_bit-8) << 2));
//        GPIOx->CRH |= ( ((uint32_t)(config) & 0x0F) << ((pin_bit-8) << 2) );
//        if(mode == 1)
//            GPIOx->BSRR = (((uint32_t)0x01) << pin_bit);
//        else if(mode == 2)
//            GPIOx->BRR = (((uint32_t)0x01) << pin_bit);
//    }
//    else
//    {
//        GPIOx->CRL &= ~(0x0000000F << ((pin_bit) << 2));
//        GPIOx->CRL |= ( ((uint32_t)(config) & 0x0F) << ((pin_bit) << 2) );
//        if(mode == 1)
//            GPIOx->BSRR = (((uint32_t)0x01) << pin_bit);
//        else if(mode == 2)
//            GPIOx->BRR = (((uint32_t)0x01) << pin_bit);
//    }
//}
static __inline void pin_in_init(GPIO_TypeDef* GPIOx, uint8_t pin_bit)
{
//#define  GPIO_MODE_INPUT                        ((uint32_t)0x00000000U)   /*!< Input Floating Mode                   */
//#define  GPIO_MODE_OUTPUT_PP                    ((uint32_t)0x00000001U)   /*!< Output Push Pull Mode                 */
//#define GPIO_MODE             ((uint32_t)0x00000003)

//#define GPIO_MODER_MODER0_Pos            (0U)        
//#define GPIO_MODER_MODER0_Msk            (0x3U << GPIO_MODER_MODER0_Pos)       /*!< 0x00000003 */
//#define GPIO_MODER_MODER0                GPIO_MODER_MODER0_Msk           


//#define GPIO_OTYPER_OT_0                 ((uint32_t)0x00000001)    
//#define GPIO_OUTPUT_TYPE      ((uint32_t)0x00000010)
    
      uint32_t temp;

      /*--------------------- GPIO Mode Configuration ------------------------*/

      /* Configure IO Direction mode (Input, Output, Alternate or Analog) */
//      temp = GPIOx->MODER;
//      temp &= ~(GPIO_MODER_MODER0 << (pin_bit * 2));
//      temp |= ((GPIO_Init->Mode & GPIO_MODE) << (position * 2));
//      GPIOx->MODER = temp;
      temp = GPIOx->MODER;
      temp &= ~(0x3U << (pin_bit * 2));
      //temp |= ((0x00000000U & 0x00000003) << (pin_bit * 2));
      GPIOx->MODER = temp;        

      /* In case of Output or Alternate function mode selection */
//      if((GPIO_Init->Mode == GPIO_MODE_OUTPUT_PP) || (GPIO_Init->Mode == GPIO_MODE_AF_PP) ||
//         (GPIO_Init->Mode == GPIO_MODE_OUTPUT_OD) || (GPIO_Init->Mode == GPIO_MODE_AF_OD))
//      {
        /* Check the Speed parameter */
//        assert_param(IS_GPIO_SPEED(GPIO_Init->Speed));
//        /* Configure the IO Speed */
//        temp = GPIOx->OSPEEDR; 
//        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2));
//        temp |= (GPIO_Init->Speed << (position * 2));
//        GPIOx->OSPEEDR = temp;

        /* Configure the IO Output Type */
//        temp = GPIOx->OTYPER;
//        temp &= ~(GPIO_OTYPER_OT_0 << pin_bit) ;
//        temp |= (((GPIO_Init->Mode & GPIO_OUTPUT_TYPE) >> 4) << pin_bit);
//        GPIOx->OTYPER = temp;
        
        /* �Ż� */
//        temp = GPIOx->OTYPER;
//        temp &= ~(0x00000001 << pin_bit) ;
//        //temp |= (((0x00000000U & 0x00000010) >> 4) << pin_bit);
//        GPIOx->OTYPER = temp;        
//      }

      /* Activate the Pull-up or Pull down resistor for the current IO */
//      temp = GPIOx->PUPDR;
//      temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2));
//      temp |= ((GPIO_Init->Pull) << (position * 2));
//      GPIOx->PUPDR = temp;    

 }

//**************************************************************************************************
/**
\defgroup DAP_Config_PortIO_gr CMSIS-DAP Hardware I/O Pin Access
\ingroup DAP_ConfigIO_gr
@{

Standard I/O Pins of the CMSIS-DAP Hardware Debug Port support standard JTAG mode
and Serial Wire Debug (SWD) mode. In SWD mode only 2 pins are required to implement the debug
interface of a device. The following I/O Pins are provided:

JTAG I/O Pin                 | SWD I/O Pin          | CMSIS-DAP Hardware pin mode
---------------------------- | -------------------- | ---------------------------------------------
TCK: Test Clock              | SWCLK: Clock         | Output Push/Pull
TMS: Test Mode Select        | SWDIO: Data I/O      | Output Push/Pull; Input (for receiving data)
TDI: Test Data Input         |                      | Output Push/Pull
TDO: Test Data Output        |                      | Input
nTRST: Test Reset (optional) |                      | Output Open Drain with pull-up resistor
nRESET: Device Reset         | nRESET: Device Reset | Output Open Drain with pull-up resistor


DAP Hardware I/O Pin Access Functions
-------------------------------------
The various I/O Pins are accessed by functions that implement the Read, Write, Set, or Clear to
these I/O Pins.

For the SWDIO I/O Pin there are additional functions that are called in SWD I/O mode only.
This functions are provided to achieve faster I/O that is possible with some advanced GPIO
peripherals that can independently write/read a single I/O pin without affecting any other pins
of the same I/O port. The following SWDIO I/O Pin functions are provided:
 - \ref PIN_SWDIO_OUT_ENABLE to enable the output mode from the DAP hardware.
 - \ref PIN_SWDIO_OUT_DISABLE to enable the input mode to the DAP hardware.
 - \ref PIN_SWDIO_IN to read from the SWDIO I/O pin with utmost possible speed.
 - \ref PIN_SWDIO_OUT to write to the SWDIO I/O pin with utmost possible speed.
*/


// Configure DAP I/O pins ------------------------------

/** Setup JTAG I/O pins: TCK, TMS, TDI, TDO, nTRST, and nRESET.
Configures the DAP Hardware I/O pins for JTAG mode:
 - TCK, TMS, TDI, nTRST, nRESET to output mode and set to high level.
 - TDO to input mode.
*/
static __inline void PORT_JTAG_SETUP(void)
{
#if (DAP_JTAG != 0)

#endif
}

/** Setup SWD I/O pins: SWCLK, SWDIO, and nRESET.
Configures the DAP Hardware I/O pins for Serial Wire Debug (SWD) mode:
 - SWCLK, SWDIO, nRESET to output mode and set to default high level.
 - TDI, TMS, nTRST to HighZ mode (pins are unused in SWD mode).
*/
static __inline void PORT_SWD_SETUP(void)
{
    GPIO_InitTypeDef gpio_init = {0};

	/* ��1������GPIOʱ�� */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ���ÿ�©��� */
	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;	// GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */
	
    /* SWCLK */
	gpio_init.Pin = GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOD, &gpio_init);	

    /* SWDIO */	
	gpio_init.Pin = GPIO_PIN_4;	
	HAL_GPIO_Init(GPIOD, &gpio_init);

    /* RESET */	   
    gpio_init.Pull = GPIO_PULLUP;			
    gpio_init.Mode = GPIO_MODE_OUTPUT_OD;    
  	gpio_init.Pin = GPIO_PIN_4;	
	HAL_GPIO_Init(GPIOE, &gpio_init); 

    GPIOD->BSRR = GPIO_PIN_3;
    GPIOD->BSRR = GPIO_PIN_4;
    GPIOE->BSRR = GPIO_PIN_4;
}

/** Disable JTAG/SWD I/O Pins.
Disables the DAP Hardware I/O pins which configures:
 - TCK/SWCLK, TMS/SWDIO, TDI, TDO, nTRST, nRESET to High-Z mode.
*/
static __inline void PORT_OFF(void)
{
    GPIO_InitTypeDef gpio_init = {0};

	/* ��1������GPIOʱ�� */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	gpio_init.Mode = GPIO_MODE_INPUT;	    /* ����Ϊ���� */
	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;	// GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */
	
    /* SWCLK */
	gpio_init.Pin = GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOD, &gpio_init);	

    /* SWDIO */	
	gpio_init.Pin = GPIO_PIN_4;	
	HAL_GPIO_Init(GPIOD, &gpio_init);
    
    /* RESET */	
	gpio_init.Pin = GPIO_PIN_4;	
	HAL_GPIO_Init(GPIOE, &gpio_init);
}

// SWCLK/TCK I/O pin -------------------------------------

/** SWCLK/TCK I/O pin: Get Input.
\return Current status of the SWCLK/TCK DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_SWCLK_TCK_IN(void)
{
    return ((SWCLK_TCK_PIN_PORT->ODR & SWCLK_TCK_PIN) ? 1 : 0);
}

/** SWCLK/TCK I/O pin: Set Output to High.
Set the SWCLK/TCK DAP hardware I/O pin to high level.
*/
static __forceinline void PIN_SWCLK_TCK_SET(void)
{
    BSP_SET_GPIO_1(SWCLK_TCK_PIN_PORT, SWCLK_TCK_PIN);
}

/** SWCLK/TCK I/O pin: Set Output to Low.
Set the SWCLK/TCK DAP hardware I/O pin to low level.
*/
static __forceinline void PIN_SWCLK_TCK_CLR(void)
{
    BSP_SET_GPIO_0(SWCLK_TCK_PIN_PORT, SWCLK_TCK_PIN);
}

// SWDIO/TMS Pin I/O --------------------------------------

/** SWDIO/TMS I/O pin: Get Input.
\return Current status of the SWDIO/TMS DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_SWDIO_TMS_IN(void)
{
    return ((SWDIO_IN_PIN_PORT->IDR & SWDIO_IN_PIN) ? 1 : 0);
}

/** SWDIO/TMS I/O pin: Set Output to High.
Set the SWDIO/TMS DAP hardware I/O pin to high level.
*/
static __forceinline void PIN_SWDIO_TMS_SET(void)
{
    BSP_SET_GPIO_1(SWDIO_OUT_PIN_PORT, SWDIO_OUT_PIN);
}

/** SWDIO/TMS I/O pin: Set Output to Low.
Set the SWDIO/TMS DAP hardware I/O pin to low level.
*/
static __forceinline void PIN_SWDIO_TMS_CLR(void)
{
    BSP_SET_GPIO_0(SWDIO_OUT_PIN_PORT, SWDIO_OUT_PIN);
}

/** SWDIO I/O pin: Get Input (used in SWD mode only).
\return Current status of the SWDIO DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_SWDIO_IN(void)
{
    return ((SWDIO_IN_PIN_PORT->IDR & SWDIO_IN_PIN) ? 1 : 0);
}

/** SWDIO I/O pin: Set Output (used in SWD mode only).
\param bit Output value for the SWDIO DAP hardware I/O pin.
*/

static __forceinline void PIN_SWDIO_OUT(uint32_t bit)
{
    if (bit & 1)
    {
        BSP_SET_GPIO_1(SWDIO_OUT_PIN_PORT, SWDIO_OUT_PIN);
    }
    else
    {
        BSP_SET_GPIO_0(SWDIO_OUT_PIN_PORT, SWDIO_OUT_PIN);
    }
}

/** SWDIO I/O pin: Switch to Output mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to output mode. This function is
called prior \ref PIN_SWDIO_OUT function calls.
*/
static __forceinline void PIN_SWDIO_OUT_ENABLE(void)
{
//    GPIO_InitTypeDef gpio_init = {0};

    BSP_SET_GPIO_1(GPIOG, GPIO_PIN_9);
    
//	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;	/* ���ÿ�©��� */
//	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
//	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;	// GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */

//	gpio_init.Pin = GPIO_PIN_4;	
//	HAL_GPIO_Init(GPIOD, &gpio_init);
    
    uint32_t temp;

    temp = GPIOD->MODER;
    temp &= 0xFFFFFCFF;
    temp |= 0x00000D00;
    GPIOD->MODER = temp;  

    BSP_SET_GPIO_0(GPIOD, GPIO_PIN_4); 
}

/** SWDIO I/O pin: Switch to Input mode (used in SWD mode only).
Configure the SWDIO DAP hardware I/O pin to input mode. This function is
called prior \ref PIN_SWDIO_IN function calls.
*/
//static __forceinline void PIN_SWDIO_OUT_DISABLE(void)
static  void PIN_SWDIO_OUT_DISABLE(void)
{
//   GPIO_InitTypeDef gpio_init = {0};

    BSP_SET_GPIO_0(GPIOG, GPIO_PIN_9);
    
//	gpio_init.Mode = GPIO_MODE_INPUT;	    /* ���ÿ�©��� */
//	gpio_init.Pull = GPIO_NOPULL;			/* ���������費ʹ�� */
//	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;	// GPIO_SPEED_FREQ_HIGH;  /* GPIO�ٶȵȼ� */

//	gpio_init.Pin = GPIO_PIN_4;	
//	HAL_GPIO_Init(GPIOD, &gpio_init);

    uint32_t temp;

    temp = GPIOD->MODER;
    temp &= 0xFFFFFCFF;
    GPIOD->MODER = temp;  

    BSP_SET_GPIO_0(GPIOD, GPIO_PIN_4);
}


// TDI Pin I/O ---------------------------------------------

/** TDI I/O pin: Get Input.
\return Current status of the TDI DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_TDI_IN(void)
{
    return (0);   // Not available
}

/** TDI I/O pin: Set Output.
\param bit Output value for the TDI DAP hardware I/O pin.
*/
static __forceinline void PIN_TDI_OUT(uint32_t bit)
{
    ;             // Not available
}


// TDO Pin I/O ---------------------------------------------

/** TDO I/O pin: Get Input.
\return Current status of the TDO DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_TDO_IN(void)
{
    return (0);   // Not available
}


// nTRST Pin I/O -------------------------------------------

/** nTRST I/O pin: Get Input.
\return Current status of the nTRST DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_nTRST_IN(void)
{
    return (0);   // Not available
}

/** nTRST I/O pin: Set Output.
\param bit JTAG TRST Test Reset pin status:
           - 0: issue a JTAG TRST Test Reset.
           - 1: release JTAG TRST Test Reset.
*/
static __forceinline void PIN_nTRST_OUT(uint32_t bit)
{
    ;             // Not available
}

// nRESET Pin I/O------------------------------------------

/** nRESET I/O pin: Get Input.
\return Current status of the nRESET DAP hardware I/O pin.
*/
static __forceinline uint32_t PIN_nRESET_IN(void)
{
    return ((nRESET_PIN_PORT->IDR >> nRESET_PIN_Bit) & 1);
}

/** nRESET I/O pin: Set Output.
\param bit target device hardware reset pin status:
           - 0: issue a device hardware reset.
           - 1: release device hardware reset.
*/
// TODO - sw specific implementation should be created

static __forceinline void     PIN_nRESET_OUT(uint32_t bit)
{
#if 0
    if (bit & 1)
        BSP_SET_GPIO_1(nRESET_PIN_PORT, nRESET_PIN);
    else
        BSP_SET_GPIO_0(nRESET_PIN_PORT, nRESET_PIN);
#else
    if (bit & 1)
    {
        //GPIOE->BSRR = GPIO_PIN_4;
        BSP_SET_GPIO_0(GPIOG, GPIO_PIN_3);      /* PG3 = 0   NRESET = 1 */        
        
    }
    else
    {
        //GPIOE->BSRR = ((uint32_t)GPIO_PIN_4 << 16U);  
        BSP_SET_GPIO_1(GPIOG, GPIO_PIN_3);      /* PG3 = 1   NRESET = 0 */
        
//        /* 硬件复位时，执行软件复位 */
//        {
//            uint32_t val;

//            swd_read_word(NVIC_AIRCR, &val);

//            swd_write_word(NVIC_AIRCR, VECTKEY | (val & SCB_AIRCR_PRIGROUP_Msk) | SYSRESETREQ);
//            
//            /* 不做复位鸣叫�? 体验不好 */
//            //BEEP_Start(5, 1, 1); /* 鸣叫50ms，停10ms�?1�?*/
//        }
    }
    
#endif
}

//**************************************************************************************************
/**
\defgroup DAP_Config_LEDs_gr CMSIS-DAP Hardware Status LEDs
\ingroup DAP_ConfigIO_gr
@{

CMSIS-DAP Hardware may provide LEDs that indicate the status of the CMSIS-DAP Debug Unit.

It is recommended to provide the following LEDs for status indication:
 - Connect LED: is active when the DAP hardware is connected to a debugger.
 - Running LED: is active when the debugger has put the target device into running state.
*/

/** Debug Unit: Set status of Connected LED.
\param bit status of the Connect LED.
           - 1: Connect LED ON: debugger is connected to CMSIS-DAP Debug Unit.
           - 0: Connect LED OFF: debugger is not connected to CMSIS-DAP Debug Unit.
*/
static __inline void LED_CONNECTED_OUT(uint32_t bit)
{
    if (bit & 1)
        BSP_SET_GPIO_1(CONNECTED_LED_PORT, CONNECTED_LED_PIN);  // LED on
    else
        BSP_SET_GPIO_0(CONNECTED_LED_PORT, CONNECTED_LED_PIN);  // LED off
}

/** Debug Unit: Set status Target Running LED.
\param bit status of the Target Running LED.
           - 1: Target Running LED ON: program execution in target started.
           - 0: Target Running LED OFF: program execution in target stopped.
*/
static __inline void LED_RUNNING_OUT(uint32_t bit)
{
    ;             // Not available
}

///@}


//**************************************************************************************************
/**
\defgroup DAP_Config_Initialization_gr CMSIS-DAP Initialization
\ingroup DAP_ConfigIO_gr
@{

CMSIS-DAP Hardware I/O and LED Pins are initialized with the function \ref DAP_SETUP.
*/

/** Setup of the Debug Unit I/O pins and LEDs (called when Debug Unit is initialized).
This function performs the initialization of the CMSIS-DAP Hardware I/O Pins and the
Status LEDs. In detail the operation of Hardware I/O and LED pins are enabled and set:
 - I/O clock system enabled.
 - all I/O pins: input buffer enabled, output pins are set to HighZ mode.
 - for nTRST, nRESET a weak pull-up (if available) is enabled.
 - LED output pins are enabled and LEDs are turned off.
*/
extern void bsp_InitSPI2_Fast(void);
__inline void DAP_SETUP(void)
{
    GPIO_InitTypeDef gpio_init = {0};

	/* 第1步：打开GPIO时钟 */
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

    BSP_SET_GPIO_1(GPIOD, GPIO_PIN_3);
    BSP_SET_GPIO_1(GPIOD, GPIO_PIN_4);
    BSP_SET_GPIO_1(GPIOE, GPIO_PIN_4); /* 设置为1 */
        
    EIO_D4_Config(ES_GPIO_IN);         /* RESET - 输入。 用PG3控制 */
    EIO_D6_Config(ES_GPIO_OUT);        /* SWCLK - 输出 */
    EIO_D8_Config(ES_GPIO_OUT);        /* SWDIO - 输出 */
    EIO_D2_Config(ES_GPIO_OUT);        /* 测试引脚 - 输出 */
    
    /* RESET 用PG3推挽 硬件三极管开漏反向 */	 
    BSP_SET_GPIO_0(GPIOG, GPIO_PIN_3);
    
	gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;	// GPIO_SPEED_FREQ_HIGH;  /* GPIO速度等级 */   
    gpio_init.Pull = GPIO_PULLUP;			
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;    
  	gpio_init.Pin = GPIO_PIN_3;	
	HAL_GPIO_Init(GPIOG, &gpio_init);   
    
    #if SPI_MODE_ENABLE == 1
        bsp_InitSPI2_Fast();
    #endif
}

/** Reset Target Device with custom specific I/O pin or command sequence.
This function allows the optional implementation of a device specific reset sequence.
It is called when the command \ref DAP_ResetTarget and is for example required
when a device needs a time-critical unlock sequence that enables the debug port.
\return 0 = no device specific reset sequence is implemented.\n
        1 = a device specific reset sequence is implemented.
*/
static __inline uint32_t RESET_TARGET(void)
{
    return (0);              // change to '1' when a device reset sequence is implemented
}

///@}


#endif /* __DAP_CONFIG_H__ */
