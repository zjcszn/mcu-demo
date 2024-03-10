//
// Created by zjcszn on 2023/6/3.
//

#ifndef USB_DEMO_BSP_RNG_H
#define USB_DEMO_BSP_RNG_H

#include <stdint.h>

void bsp_rng_init(void);
uint32_t bsp_rng_read(void);

#endif //USB_DEMO_BSP_RNG_H
