/************************************************************    
 FileName: task_sync.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: ʱ��ͬ��    // ģ������         
 Version:         			        // �汾��Ϣ   
 Function List:                 // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
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
					//��ʼ����ʱ��,20ms֡
					sync_timer_init(20);
					
					//��Ҫ��������,�жϵ�ǰ�����Ƿ��������ڷ��� 
					if(getCSMA_state() == CS_BUSY)
					{
						// �������ڷ���,��ô�ȴ�ʱ��ͬ��
						syncHangFlg  = 1;
					}
					else 
					{
						//û�������ڷ���,��ô������ʱ��
						sync_timer_start();
						syncHangFlg = 0;
					}
					data.type = TYPE_ON | TYPE_VOICE;
					AudioOverHighWaterMark = 0;
				}
				break;	//End of case SYNC_EVENT_START:
				
				case SYNC_EVENT_STOP:
				{
					//������������
					data.type = TYPE_OFF | TYPE_VOICE;

					//���ͽ���������
					esbSendData((uint8_t*)&data,sizeof(data.type));	
					
					//�ر�ͬ����ʱ��
					sync_timer_stop();
				}
				break;	//End of case SYNC_EVENT_STOP:				
				
				case SYNC_EVENT_SYNC:
				{
					//��ʹ����һ������,û���յ�
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
					//��ʱ��20ms��ʱ���
					if(AudioOverHighWaterMark == 0)
					{
						if(uxQueueMessagesWaiting(*audioGetOutQueue()) == 4) 
						{
							AudioOverHighWaterMark = 1;
						}
					}
					
					//����ﵽ���嶨��,��ʼ��������
					if(AudioOverHighWaterMark == 1)
					{
						if(ReadOutQueue(&data.audio.Length,&data.audio.SeqNumber,data.audio.Data) == pdPASS)
						{
							//ʹ�����߷��ͳ�ȥ,ע��Ҫ�ڴ����
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
