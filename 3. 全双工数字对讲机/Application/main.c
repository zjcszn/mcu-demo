/************************************************************    
 FileName: main.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: ���������񴴽�    // ģ������         
 Version:         			        // �汾��Ϣ   
 Function List:                 // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "core.h"
#include "task_audio.h"
#include "task_keyscan.h"
#include "task_rfIT.h"
#include "task_sync.h"

void fw_main(void *arg)
{
	/* ������������ */
	osRfTaskCreate();
	
	/* ��������ɨ������ */
	osKeyCreate();
	
	/* ������Ƶ���� */
	osAudioCreate();
	
	/* ����ͬ������ */
	osSyncTaskCreate();
	
	/* ɾ�������������*/
	vTaskDelete(NULL);
}



/**
  * @brief  ������������
  * @retval None  
	* @power  5V ��������70mA
  */
int main(void)
{
	/* ʱ�Ӻ�ϵͳ��ʼ��*/
	SystickInit();		
	
	/* Create App Task */
	xTaskCreate(fw_main,"main",256,NULL,configMAX_PRIORITIES - 1,NULL);	
	
	/* Activate deep sleep mode  */
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	
	/* Start FreeRTOS scheduler. */
	vTaskStartScheduler();

	for(;;);
	/* Never go there */
}



