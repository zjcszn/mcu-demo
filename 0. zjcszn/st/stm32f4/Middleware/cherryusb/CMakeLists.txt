cmake_minimum_required(VERSION 3.21)

set(CHERRY_USB_SRC
    core/usbd_core.c
    port/dwc2/usb_dc_dwc2.c
    osal/usb_osal_freertos.c
    class/cdc/usbd_cdc.c
    class/msc/usbd_msc.c
    user/cdc_acm_msc.c
    )

set(CHERRY_USB_INC
    core
    class/cdc
    class/msc
    common
    osal
    user
    port/dwc2
    )

add_library(cherryusb STATIC ${CHERRY_USB_SRC})
target_include_directories(cherryusb PUBLIC ${CHERRY_USB_INC})


