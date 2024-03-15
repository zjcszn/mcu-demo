/************************************************************    
 FileName: task_rfIT.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 无线收发    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "task_rfIT.h"
#include "task_audio.h"
#include "task_keyscan.h"
#include "task_sync.h"
#include "esb.h"


static QueueHandle_t csma_listen;

uint8_t getCSMA_state(void)
{
	uint8_t state;
	xQueueReceive(csma_listen, (void *)&state,0);
	return state;
}

void task_rf(void*arg)
{

	ESB_RX_t rx_t;
	ESB_TO_AUDIO_t *e2a_t = (ESB_TO_AUDIO_t *)&rx_t.up_data;

	uint8_t csma_state = CS_IDLE;
	uint32_t csma_timeout;
	
	esb_init(20);	//初始化频率2420
	
	csma_timeout = xTaskGetTickCount();
	while(1)
	{
		if(ReadRxQueue(&rx_t,50) == pdTRUE)
		{		
			//是否是语音包
			if((e2a_t->type & TYPE_MASK) == TYPE_VOICE)
			{
				//检测是否有语音数据在传输.
				if((e2a_t->type & TYPE_STA_MASK) == TYPE_ON)
				{
					csma_state = CS_BUSY;
					xQueueOverwrite(csma_listen,&csma_state);
					
					setSyncEvent(SYNC_EVENT_SYNC);
					
					//是否开启了语音运行模式 
					if(getCurrentMode() == CurrentMode_RUN)
					{
						/* 判断队列是否满 */
						if(uxQueueSpacesAvailable(*audioGetInQueue()) == 0)
						{
							/* 队列满了,检查是否i2s或任务没有正常工作 */
							uint8_t dLength,dSeqNumber,dData[PayLoadSize];
							ReadInQueue(&dLength,&dSeqNumber,dData);
						}
						
						/* 接收到语音数据 存储到队列  */
						WriteInQueue(e2a_t->audio.Length,e2a_t->audio.SeqNumber,e2a_t->audio.Data);
					}
				}
				else if((e2a_t->type & TYPE_STA_MASK) == TYPE_OFF)
				{
					csma_state = CS_IDLE;
					xQueueOverwrite(csma_listen,&csma_state);					
				}
				csma_timeout = xTaskGetTickCount();
			}
		}

		if((xTaskGetTickCount() >= csma_timeout + 100)&&(csma_state == CS_BUSY))
		{	
			//如果100ms内没有收到数据那么,就说明对方无线断开
			csma_state = CS_IDLE;
		}
		
	}	
}

/* 任务优先级最高保证数据接受完整  */
void osRfTaskCreate(void)
{
	csma_listen =	xQueueCreate(1, sizeof(uint8_t)); 
	if( xTaskCreate(task_rf,"osRF",256,NULL,configMAX_PRIORITIES - 1,NULL) != pdPASS)
	{
		while(1);
	}
}
