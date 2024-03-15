#ifndef __audioFuction_H_
#define __audioFuction_H_

#include <stdint.h>
#include <string.h>

#include "i2s.h"
#include "wm8978.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "consert.h"
#include "arm_math.h"


extern const unsigned char wavdata[];


void awModeSelecet(uint8_t mode);
void playAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size,uint8_t volue);
void recoderAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size);
void fullduplexAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size,uint8_t volue);
void stopAudio(void);
void setPlayVolue(uint8_t volue);

void dsp_fiiter_process(int16_t *in,int16_t *out);
void dsp_fitter_init(void);


#endif

