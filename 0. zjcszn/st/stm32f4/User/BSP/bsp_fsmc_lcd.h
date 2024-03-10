#ifndef __BSP_FSMC_NT_35510__
#define __BSP_FSMC_NT_35510__

#include <stdint.h>

#define NT35510_BL_PIN        LL_GPIO_PIN_15
#define NT35510_BL_PORT       GPIOB

#define NT35510_CS_PIN        LL_GPIO_PIN_12
#define NT35510_CS_PORT       GPIOG

struct lcd_dmatx_msg {
  void *buf;
  uint32_t len;
};

typedef struct lcd_dmatx_msg lcd_dmatx_msg_t;

void bsp_fsmc_lcd_init(void);

void lcd_fsmc_dma_write(void *buf, uint32_t len);

#endif