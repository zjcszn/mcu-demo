//
// Created by zjcszn on 2023/6/3.
//

#include "bsp_rng.h"
#include "stm32f4xx_ll_rng.h"
#include "stm32f4xx_ll_bus.h"

void bsp_rng_init(void) {
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_RNG);
    LL_RNG_Enable(RNG);
}

uint32_t bsp_rng_read(void) {
    while (!LL_RNG_IsActiveFlag_DRDY(RNG));
    return LL_RNG_ReadRandData32(RNG);
}
