
/************************************************************    
 FileName: task_keyscan.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 按键扫描    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "task_keyscan.h"
#include "bsp.h"
#include "task_audio.h"
#include "task_rfIT.h"
#include "task_sync.h"


int currentMode = 0;		//当前状态

//设置当前状态
void setCurrentMode(int mode)
{
	currentMode = mode;
}


//获取当前状态
int getCurrentMode(void)
{
	return currentMode;
}


//对讲收发触发任务
void task_key(void* arg)
{
	uint8_t botton;
	uint8_t botFlg = 0;
	
	led_init();			/* 初始化按键LED */
	key_init();			

	vTaskDelay(10);	//去其它任务初始化
	while(1)
	{
		/* 检测按键 PTT 发送和接收 */
		botton = read_button();
		if(botton != 0)
		{
			/* 按下按键 */
			botFlg = 1;
		}
		else if(botFlg == 1)
		{
			botFlg = 2;
		}
		
		if(botFlg == 2)
		{
			botFlg = 0;
			
			/* 空闲状态检测到PTT 按下 发送语音*/
			if(getCurrentMode() == CurrentMode_IDLE)
			{
				nrf_gpio_pin_clear(LED_RX);		//开灯
				
				setCurrentMode(CurrentMode_RUN); 	//发送语音状态
				
				osAudioOpen();							//进入播放状态
				
				setSyncEvent(SYNC_EVENT_START);		//开启时间同步发送数据
			}
			/* 检测语音模式,退出语音运行*/
			else if(getCurrentMode() == CurrentMode_RUN)
			{
				osAudioClose();								//关闭录音任务
				
				setSyncEvent(SYNC_EVENT_STOP);		//关闭时间同步,结束发送数据
				
				setCurrentMode(CurrentMode_IDLE);	//切换到空闲模式
				
				nrf_gpio_pin_set(LED_RX);		//关灯
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
