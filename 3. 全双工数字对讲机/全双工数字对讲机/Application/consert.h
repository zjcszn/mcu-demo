#ifndef __CONSERT_H_
#define __CONSERT_H_

#include <stdint.h>
#include <string.h>


void MonoToStereo(int16_t* src_audio,int samples_per_channel,int16_t* dst_audio) ;
void StereoToMono(int16_t* src_audio,int samples_per_channel,int16_t* dst_audio) ;



#endif

