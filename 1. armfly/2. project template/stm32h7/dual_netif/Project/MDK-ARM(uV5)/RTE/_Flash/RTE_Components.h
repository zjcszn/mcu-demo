
/*
 * Auto generated Run-Time-Environment Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'project' 
 * Target:  'Flash' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "stm32h7xx.h"

/*  ARM::CMSIS:RTOS2:Keil RTX5:Source:5.5.2 */
#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
        #define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
        #define RTE_CMSIS_RTOS2_RTX5_SOURCE     /* CMSIS-RTOS2 Keil RTX5 Source */
/*  Keil.ARM Compiler::Compiler:Event Recorder:DAP:1.4.0 */
#define RTE_Compiler_EventRecorder
          #define RTE_Compiler_EventRecorder_DAP
/*  Keil.ARM Compiler::Compiler:I/O:STDOUT:EVR:1.2.0 */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
          #define RTE_Compiler_IO_STDOUT_EVR      /* Compiler I/O: STDOUT EVR */
/*  Keil.MDK-Pro::Network:CORE:IPv4 Release:7.15.0 */
#define RTE_Network_Core                /* Network Core */
          #define RTE_Network_IPv4                /* Network IPv4 Stack */
          #define RTE_Network_Release             /* Network Release Version */
/*  Keil.MDK-Pro::Network:Interface:ETH:7.15.0 */
#define RTE_Network_Interface_ETH_0     /* Network Interface ETH 0 */
#define RTE_Network_Interface_ETH_1     /* Network Interface ETH 1 */

/*  Keil.MDK-Pro::Network:Socket:TCP:7.15.0 */
#define RTE_Network_Socket_TCP          /* Network Socket TCP */
/*  Keil.MDK-Pro::Network:Socket:UDP:7.15.0 */
#define RTE_Network_Socket_UDP          /* Network Socket UDP */
/*  Keil::Device:STM32Cube Framework:STM32CubeMX:1.1.0 */
#define RTE_DEVICE_FRAMEWORK_CUBE_MX


#endif /* RTE_COMPONENTS_H */
