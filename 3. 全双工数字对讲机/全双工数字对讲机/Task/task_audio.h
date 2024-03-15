#ifndef __TASK_AUDIO_H
#define __TASK_AUDIO_H

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

#define AudioEvent_RX 1
#define AudioEvent_TX 2

#define PayLoadSize 20

void osAudioClose(void);
void osAudioOpen(void);

void osAudioCreate(void);
void ResetOutQueue(void);

int	ReadOutQueue(uint8_t *Length,uint8_t *SeqNumber,uint8_t *data);
int WriteOutQueue(uint8_t Length,uint8_t SeqNumber,uint8_t *data);
QueueHandle_t *audioGetOutQueue(void);

void ResetInQueue(void);
int	ReadInQueue(uint8_t *Length,uint8_t *SeqNumber,uint8_t *data);
int WriteInQueue(uint8_t Length,uint8_t SeqNumber,uint8_t *data);
QueueHandle_t *audioGetInQueue(void);

#endif

