/************************************************************    
 FileName: task_rfIT.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: �����շ�    // ģ������         
 Version:         			        // �汾��Ϣ   
 Function List:                 // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
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
	
	esb_init(20);	//��ʼ��Ƶ��2420
	
	csma_timeout = xTaskGetTickCount();
	while(1)
	{
		if(ReadRxQueue(&rx_t,50) == pdTRUE)
		{		
			//�Ƿ���������
			if((e2a_t->type & TYPE_MASK) == TYPE_VOICE)
			{
				//����Ƿ������������ڴ���.
				if((e2a_t->type & TYPE_STA_MASK) == TYPE_ON)
				{
					csma_state = CS_BUSY;
					xQueueOverwrite(csma_listen,&csma_state);
					
					setSyncEvent(SYNC_EVENT_SYNC);
					
					//�Ƿ�������������ģʽ 
					if(getCurrentMode() == CurrentMode_RUN)
					{
						/* �ж϶����Ƿ��� */
						if(uxQueueSpacesAvailable(*audioGetInQueue()) == 0)
						{
							/* ��������,����Ƿ�i2s������û���������� */
							uint8_t dLength,dSeqNumber,dData[PayLoadSize];
							ReadInQueue(&dLength,&dSeqNumber,dData);
						}
						
						/* ���յ��������� �洢������  */
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
			//���100ms��û���յ�������ô,��˵���Է����߶Ͽ�
			csma_state = CS_IDLE;
		}
		
	}	
}

/* �������ȼ���߱�֤���ݽ�������  */
void osRfTaskCreate(void)
{
	csma_listen =	xQueueCreate(1, sizeof(uint8_t)); 
	if( xTaskCreate(task_rf,"osRF",256,NULL,configMAX_PRIORITIES - 1,NULL) != pdPASS)
	{
		while(1);
	}
}
