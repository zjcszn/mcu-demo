#ifndef __TASK_KEYSCAN_H
#define __TASK_KEYSCAN_H

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

typedef enum
{
	CurrentMode_IDLE,
	CurrentMode_RUN,
	
}CurrentMode_t;

void osKeyCreate(void);
void setCurrentMode(int mode);
int getCurrentMode(void);

#endif

