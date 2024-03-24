#ifndef __SPEEXC_H_
#define __SPEEXC_H_

#include "core.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <speex/speex.h>
#include "arch.h"

void SpeexEncode_Init(void);
int SpeexEncodeData(uint16_t *EnCode,uint8_t *OutBytes);
void SpeexEncodeFree(void);

void SpeexDecode_Init(void);
int SpeexDecodeData(uint8_t *DeCode,uint16_t *OutBytes,uint8_t size,uint8_t isLoss);
void SpeexDecodeFree(void);

void SpeexDenoiseInit(void);
void SpeexDenoiseprocess(spx_int16_t *in);
void SpeexDenoiseDestrory(void);

void SpeexEchoInit(void);
void SpeexEchoprocess(short *ref_buf,short *echo_buf,short *e_buf);
void SpeexEchoDestroy(void);





#endif

