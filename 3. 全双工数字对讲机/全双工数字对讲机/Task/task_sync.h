#ifndef __TASK_SYNC_H_
#define __TASK_SYNC_H_

#include <stdint.h>
#include <stdlib.h>

#define SYNC_EVENT_START	1
#define SYNC_EVENT_STOP		2
#define SYNC_EVENT_TIME		3
#define SYNC_EVENT_SYNC   4


typedef uint8_t syncType;

void osSyncTaskCreate(void);

void setSyncEvent(syncType Event);

#endif
