#设置CMake最低版本要求
cmake_minimum_required(VERSION 3.21)

#引用ARMClang_Cortex-M4.cmake
include(ARMClang_Cortex-M4.cmake)

#设置工程名称
CMSIS_SetProjectName(usb_demo)

#设置浮点运算单元 FPU
CMSIS_SetFPU(TRUE)

#设置链接时优化 LTO
CMSIS_SetLTO(FALSE)

#添加全局宏定义
CMSIS_SetPreDefine(
        USE_FULL_LL_DRIVER
        USE_HAL_DRIVER
        STM32F407xx
        CONFIG_USB_DWC2_PORT=FS_PORT
)

#设置链接脚本路径
CMSIS_SetSectionsScriptPath("${CMAKE_HOME_DIRECTORY}/usb_demo.sct")

#配置交叉编译环境
CMSIS_CrossCompilingConfiguration()

#添加第三方库 file.cmake
include(User/Library/utils/backoff_algorithm/backoffAlgorithmFilePaths.cmake)
include(User/Library/coremqtt/mqttAgentFilePaths.cmake)
include(User/Library/corehttp/httpFilePaths.cmake)

#设置中间件头文件路径
set(MIDDLEWARE_INCLUDE
        Middleware/freertos
        Middleware/freertos/include
        Middleware/freertos/portable/GCC/ARM_CM4F
        Middleware/lwip
        Middleware/lwip/ports/drv
        Middleware/lwip/ports/ethernetif
        Middleware/lwip/src/include
        Middleware/lwip/system
        Middleware/mbedtls/include
)

#设置用户头文件路径
set(USER_INCLUDE

        User/BSP
        User/APP
        User/APP/coremqtt_client
        User/APP/corehttp_client

        # Middleware include
        Middleware/fatfs

        # Library include
        ${MQTT_INCLUDE_PUBLIC_DIRS}
        ${MQTT_AGENT_INCLUDE_PUBLIC_DIRS}
        ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS}
        ${HTTP_INCLUDE_PUBLIC_DIRS}
        User/Library
        User/Library/sfud/inc
        User/Library/elog
        User/Library/shell
        User/Library/zlib
        User/Library/cjson
        User/Library/network_transport
        User/Library/network_transport/sockets_wrapper
        User/Library/network_transport/mbedtls_platform
        User/Library/utils
        User/Library/utils/logging
        User/Library/lwmem/src/include
        User/Library/stm32_usb_host/Core/inc
        User/Library/stm32_usb_host/Port
        User/Library/stm32_usb_host/Class/HID/inc
        User/Library/stm32_usb_host/Class/CDC/Inc
        User/Library/stm32_usb_host/Class/MSC/Inc
        User/Library/tinyusb
)

#添加cmsis头文件目录
include_directories(BEFORE
        Drivers/CMSIS/Include
        ${MIDDLEWARE_INCLUDE}
        Core/Inc
        Drivers/CMSIS/Device/ST/STM32F4xx/Include
        Drivers/STM32F4xx_HAL_Driver/Inc
        Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
        ${USER_INCLUDE}
)

#配置工程属性
project(${CMAKE_PROJECT_NAME} LANGUAGES C ASM)


#添加子目录
add_subdirectory(Middleware/mbedtls)
add_subdirectory(Middleware/freertos)
add_subdirectory(Middleware/lwip)
add_subdirectory(Middleware/lvgl)
add_subdirectory(Middleware/cherryusb)


#添加 hal 库源文件
file(GLOB_RECURSE HAL_SOURCES
        Drivers/STM32F4xx_HAL_Driver/Src/*.c
)

#生成静态库
add_library(hal_core STATIC ${HAL_SOURCES})

#添加可执行程序源文件
file(GLOB_RECURSE MAIN_SOURCES
        Core/Src/*.c
        MDK-ARM/*.s
        User/*.c
        Middleware/fatfs/*.c
)

#设置排除列表
file(GLOB_RECURSE EXCLUDE_SRCS
        User/Library/zlib/examples/*.c
        User/Library/zlib/contrib/*.c
        User/APP/custom_gui/*.c
        #     User/BSP/bsp_lvgl.c
        User/Library/stm32_usb_host/Class/CDC/*.c
        User/Library/stm32_usb_host/Class/MSC/*.c
)

#从源文件列表(SOURCES)中剔除干扰文件(EXCLUDE_SRCS)
list(REMOVE_ITEM MAIN_SOURCES ${EXCLUDE_SRCS})


#添加生成可执行文件
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE
        cherryusb
        lvgl_demos
        lvgl_examples
        lvgl
        lwipallapps
        mbedtls
        lwipcore
        hal_core
        freertos_kernel)


set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)
set(ELF_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.elf)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND cd /d "${PROJECT_BINARY_DIR}"
        COMMAND del /q /s "${PROJECT_NAME}.*" >nul
        COMMENT "Delete Old Output File..."
)

#生成编译后显示固件大小的powershell脚本
file(GENERATE OUTPUT "${PROJECT_BINARY_DIR}/showinfo.ps1" CONTENT "\
        $esc = ([char]27)
        $elf_size = \"{0,8:D}\"  -f (Get-Item \"${ELF_FILE}\").Length
        $elf_size_kb = \"{0,8:N2}\" -f ($elf_size / 1KB)
        $hex_size = \"{0,8:D}\"  -f (Get-Item \"${HEX_FILE}\").Length
        $hex_size_kb = \"{0,8:N2}\" -f ($hex_size / 1KB)
        $bin_size = \"{0,8:D}\"  -f (Get-Item \"${BIN_FILE}\").Length
        $bin_size_kb = \"{0,8:N2}\" -f ($bin_size / 1KB)
        Write-Output \"$esc[32mELF Size: $elf_size ($elf_size_kb KB)$esc[0m\"
        Write-Output \"$esc[32mHEX Size: $hex_size ($hex_size_kb KB)$esc[0m\"
        Write-Output \"$esc[32mBIN Size: $bin_size ($bin_size_kb KB)$esc[0m\""
)

set(FROM_ELF fromelf.exe)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${FROM_ELF} --i32combined --output="${HEX_FILE}" "${ELF_FILE}"
        COMMAND ${FROM_ELF} --bincombined --output="${BIN_FILE}" "${ELF_FILE}"
        COMMAND powershell.exe -File "${PROJECT_BINARY_DIR}/showinfo.ps1"
        COMMENT "Building Hex & Bin Firmware..."
)


