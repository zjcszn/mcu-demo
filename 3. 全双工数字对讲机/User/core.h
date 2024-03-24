#ifndef __CORE_H_
#define __CORE_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "nrf.h"
#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrf_systick.h"
#include "nrf_drv_systick.h"
#include "cmsis_armcc.h"


#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"




#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bits for pre-emption priority
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bits for pre-emption priority
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority
                                                                 1 bits for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority
                                                                 0 bits for subpriority */
															
															
/**
  * @brief  产生随机数
  * @retval RNG
  */
static __inline uint32_t RNG_Get_RandomRange(int min,int max)
{ 
   return rand()%(max-min+1) +min;
}


static __inline void MemSet(uint8_t *p,uint8_t val,uint32_t length)
{
	uint32_t i=0;
	for(i=0;i<length;i++)
	{
		p[i]=val;
	}
}

static __inline void MemCopy(uint8_t *dst,uint8_t *src,uint32_t length)
{
	uint32_t i=0;
	for(i=0;i<length;i++)
	{
		dst[i]=src[i];
	}
}

/* 计算校验值 */
static __inline uint8_t calculate_cksum(uint8_t *ptr,uint16_t len)
{
	uint8_t crc;
	uint8_t i;
	crc=0;
	while(len--)
	{
		crc^=*ptr++;
		for(i=0;i<8;i++)
		{
			if(crc&0x01)crc=(crc>>1)^0x8C;
			else crc >>= 1;
		}
	}
	return crc;
}


/* systick init */
uint32_t HAL_GetTick(void);
void SystickInit(void);


#endif

