
/************************************************************    
 FileName: task_keyscan.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: ����ɨ��    // ģ������         
 Version:         			        // �汾��Ϣ   
 Function List:                 // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "task_keyscan.h"
#include "bsp.h"
#include "task_audio.h"
#include "task_rfIT.h"
#include "task_sync.h"


int currentMode = 0;		//��ǰ״̬

//���õ�ǰ״̬
void setCurrentMode(int mode)
{
	currentMode = mode;
}


//��ȡ��ǰ״̬
int getCurrentMode(void)
{
	return currentMode;
}


//�Խ��շ���������
void task_key(void* arg)
{
	uint8_t botton;
	uint8_t botFlg = 0;
	
	led_init();			/* ��ʼ������LED */
	key_init();			

	vTaskDelay(10);	//ȥ���������ʼ��
	while(1)
	{
		/* ��ⰴ�� PTT ���ͺͽ��� */
		botton = read_button();
		if(botton != 0)
		{
			/* ���°��� */
			botFlg = 1;
		}
		else if(botFlg == 1)
		{
			botFlg = 2;
		}
		
		if(botFlg == 2)
		{
			botFlg = 0;
			
			/* ����״̬��⵽PTT ���� ��������*/
			if(getCurrentMode() == CurrentMode_IDLE)
			{
				nrf_gpio_pin_clear(LED_RX);		//����
				
				setCurrentMode(CurrentMode_RUN); 	//��������״̬
				
				osAudioOpen();							//���벥��״̬
				
				setSyncEvent(SYNC_EVENT_START);		//����ʱ��ͬ����������
			}
			/* �������ģʽ,�˳���������*/
			else if(getCurrentMode() == CurrentMode_RUN)
			{
				osAudioClose();								//�ر�¼������
				
				setSyncEvent(SYNC_EVENT_STOP);		//�ر�ʱ��ͬ��,������������
				
				setCurrentMode(CurrentMode_IDLE);	//�л�������ģʽ
				
				nrf_gpio_pin_set(LED_RX);		//�ص�
			}			
		}
		
		vTaskDelay(50);
	}
}


void osKeyCreate(void)
{
	if( xTaskCreate(task_key,"key",256,NULL,configMAX_PRIORITIES - 2,NULL) != pdPASS)
	{
		while(1);
	}	
}
