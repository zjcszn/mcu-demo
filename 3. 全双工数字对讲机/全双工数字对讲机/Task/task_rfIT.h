#ifndef __TASK_RFIT_H
#define __TASK_RFIT_H

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
#include "queue.h"

#define TYPE_MASK     (0x0f)
#define TYPE_VOICE	  (1<<0)

#define TYPE_STA_MASK (0xF0)
#define TYPE_ON			  (1<<7)
#define TYPE_OFF		  (0<<7)

#define CS_BUSY				1
#define CS_IDLE				2

void osRfTaskCreate(void);

uint8_t getCSMA_state(void);

#endif

