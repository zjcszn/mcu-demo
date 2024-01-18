//
// Created by zjcszn on 2023/8/5.
//

#include "bsp_button.h"
#include "lw_button.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_bus.h"

static void button_gpio_init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);

    LL_GPIO_InitTypeDef GPIO_Initstruct = {0};
    /* init KEY2 KEY1 KEY0 */
    GPIO_Initstruct.Pin = LL_GPIO_PIN_2 | LL_GPIO_PIN_3 | LL_GPIO_PIN_4;
    GPIO_Initstruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_Initstruct.Pull = LL_GPIO_PULL_UP;
    GPIO_Initstruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOE, &GPIO_Initstruct);
    /* init WKUP */
    GPIO_Initstruct.Pin = LL_GPIO_PIN_0;
    GPIO_Initstruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(GPIOA, &GPIO_Initstruct);
}

static uint8_t button_gpio_read(hardkey_id_t hkey_id) {
    switch (hkey_id) {
        case KEY0:
            return LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_4);
        case KEY1:
            return LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_3);
        case KEY2:
            return LL_GPIO_IsInputPinSet(GPIOE, LL_GPIO_PIN_2);
        case WKUP:
            return LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0);
        default:
            return 0;
    }
}

void bsp_button_init(void) {
    button_gpio_init();
    lwkey_init(button_gpio_read);
}