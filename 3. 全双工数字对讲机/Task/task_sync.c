/************************************************************    
 FileName: task_sync.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 时间同步    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "task_sync.h"
#include "task_audio.h"
#include "task_keyscan.h"
#include "task_rfIT.h"
#include "esb.h"
#include "bsp.h"

static uint8_t sync_state;

static QueueHandle_t SyncEvent;
static TimerHandle_t SyncTimer;

static void sync_handler(TimerHandle_t xTimer)
{
	syncType event = SYNC_EVENT_TIME;
	xQueueSend(SyncEvent,&event,0);	
}

static void sync_timer_init(uint32_t timers)
{
	SyncTimer = xTimerCreate("syn",timers,
					pdTRUE,NULL,sync_handler);
	
	if (NULL == SyncTimer)
	{
		while(1);//ERROR_NO_MEM
	}	
}

static void sync_timer_start(void)
{
	if (pdPASS != xTimerStart(SyncTimer, 0))
	{
		while(1);
	}	
}

static void sync_timer_stop(void)
{
	if(pdPASS != xTimerDelete(SyncTimer, 0))
	{
		while(1);//ERROR_NO_MEM
	}	
}



void task_sync(void *arg)
{
	syncType event;
	uint8_t AudioOverHighWaterMark = 0;
	
	uint8_t syncHangFlg = 0;
	
	ESB_TO_AUDIO_t data;
	
	
	while(1)
	{
		if(xQueueReceive(SyncEvent, (void *)&event,1000) == pdTRUE)
		{		
			switch(event)
			{
				case SYNC_EVENT_START:
				{
					//初始化定时器,20ms帧
					sync_timer_init(20);
					
					//需要发送语音,判断当前无线是否有语音在发送 
					if(getCSMA_state() == CS_BUSY)
					{
						// 有语音在发送,那么等待时间同步
						syncHangFlg  = 1;
					}
					else 
					{
						//没有数据在发送,那么开启定时器
						sync_timer_start();
						syncHangFlg = 0;
					}
					data.type = TYPE_ON | TYPE_VOICE;
					AudioOverHighWaterMark = 0;
				}
				break;	//End of case SYNC_EVENT_START:
				
				case SYNC_EVENT_STOP:
				{
					//开启语音传输
					data.type = TYPE_OFF | TYPE_VOICE;

					//发送结束语音包
					esbSendData((uint8_t*)&data,sizeof(data.type));	
					
					//关闭同步定时器
					sync_timer_stop();
				}
				break;	//End of case SYNC_EVENT_STOP:				
				
				case SYNC_EVENT_SYNC:
				{
					//即使在下一个周期,没有收到
					if(syncHangFlg == 1)
					{
						vTaskDelay(3);
						sync_timer_start();
						syncHangFlg = 0;						
					}
				}
				break;  //End of case SYNC_EVENT_SYNC:		
				
				case SYNC_EVENT_TIME:
				{
					//定时器20ms定时完成
					if(AudioOverHighWaterMark == 0)
					{
						if(uxQueueMessagesWaiting(*audioGetOutQueue()) == 4) 
						{
							AudioOverHighWaterMark = 1;
						}
					}
					
					//如果达到缓冲定义,开始发送数据
					if(AudioOverHighWaterMark == 1)
					{
						if(ReadOutQueue(&data.audio.Length,&data.audio.SeqNumber,data.audio.Data) == pdPASS)
						{
							//使用无线发送出去,注意要内存对齐
							esbSendData((uint8_t*)&data,sizeof(data.audio) + sizeof(data.type));
						}						
					}
				}
				break;	//End of case SYNC_EVENT_TIME:	
				
				default:
				{
				
				}
				break;	//End of case default:
				
			}  //End of Switch(event):	
			
		}  //End of if(xQueueReceive(SyncEvent... == pdTRUE)
		
	}  //End of While
	
} 


void setSyncEvent(syncType Event)
{
	xQueueSendToFront(SyncEvent,&Event,0);
}	

void osSyncTaskCreate(void)
{
	SyncEvent =	xQueueCreate(4, sizeof(syncType)); 
	if( xTaskCreate(task_sync,"sync",512,NULL,configMAX_PRIORITIES - 1,NULL) != pdPASS)
	{
		while(1);
	}
}
