/************************************************************    
 FileName: main.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 主程序任务创建    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
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
	/* 创建无线任务 */
	osRfTaskCreate();
	
	/* 创建按键扫描任务 */
	osKeyCreate();
	
	/* 创建音频任务 */
	osAudioCreate();
	
	/* 创建同步任务 */
	osSyncTaskCreate();
	
	/* 删除创建入口任务*/
	vTaskDelete(NULL);
}



/**
  * @brief  函数创建任务
  * @retval None  
	* @power  5V 待机电流70mA
  */
int main(void)
{
	/* 时钟和系统初始化*/
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



