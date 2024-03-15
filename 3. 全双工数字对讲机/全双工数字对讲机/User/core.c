/************************************************************    
 FileName: core.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description:  硬件时钟初始化   // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "core.h"
#include "nrf_clock.h"


volatile uint32_t CPU_RunTime;

volatile uint32_t uwTick;


void SystemClock_Config()
{
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 7, 0U));
  SysTick_Config(SystemCoreClock / (1000U / 1U));
}

void SystickInit( void )
{
	NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;		
	NRF_CLOCK->TASKS_HFCLKSTART = 1;			
	while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
	SystemClock_Config();
}

__weak uint32_t HAL_GetTick(void)
{
  return uwTick;
}

#define HAL_MAX_DELAY      0xFFFFFFFFU
__weak void HAL_Delay(uint32_t Delay)
{
  uint32_t tickstart = HAL_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(1U);
  }

  while ((HAL_GetTick() - tickstart) < wait)
  {
  }
}

extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
  uint32_t ulReturn;
  /* 进入临界段，临界段可以嵌套 */
  ulReturn = taskENTER_CRITICAL_FROM_ISR();
	
  uwTick += 1U; 
	
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
  xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  }
#endif  /* INCLUDE_xTaskGetSchedulerState */
  
  /* 退出临界段 */
  taskEXIT_CRITICAL_FROM_ISR( ulReturn );	
	
}
