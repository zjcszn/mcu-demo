#include "core.h"
#include "nrf_i2s.h"

#define I2S_LRCK_PIN		19
#define I2S_SCK_PIN			18
#define I2S_SDOUT_PIN		16
#define I2S_SDIN_PIN		17
#define I2S_MCK_PIN 		15


void I2S_Init(void);
void nrfI2S_Start_Rec(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size);
void nrfI2S_Start_play(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size);
void nrfI2S_Start_FullDuplex(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size);

void nrfI2S_Stop(void);


