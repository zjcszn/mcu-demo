#ifndef _ADPCM_H_
#define _ADPCM_H_

#include <stdint.h>
#include <stdio.h> /*DBG*/

#ifndef __STDC__
#define signed
#endif

#ifdef ADPCM_ENDIAN_SWAP
static __INLINE uint16_t swap_uint16( uint16_t val ) 
{
	return (val << 8) | (val >> 8 );
}

//! Byte swap short
static __INLINE int16_t swap_int16( int16_t val ) 
{
	return (val << 8) | ((val >> 8) & 0xFF);
}
#endif


typedef struct adpcm_state_t {
    short	valprev;					/* Previous output value */
    char	index;						/* Index into stepsize table */
}adpcm_state;

extern adpcm_state pcmDecoder;
extern adpcm_state pcmcoder;

void adpcm_coder(short *indata, char *outdata, int len, adpcm_state *SS);
void adpcm_decoder(char *indata, short *outdata, int len, adpcm_state *SS);


#endif

