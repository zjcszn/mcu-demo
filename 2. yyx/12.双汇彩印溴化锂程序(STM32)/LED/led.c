#define _LED_C
#include "head.h"
//#include "uart.h"

u16 led_run_time;
u16 led_flag;


/*
��ʼ��������
�ú���������һ��
*/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    //���е�ָʾ��ռ��IO��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 	
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	led_flag = 0;
	led_run_time = 0;
	
}


/*
������к������ú�����Ҫ���ڱ�����
*/
void LED_Run(void)
{
	if(led_flag==1)
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		led_run_time++;
		if(led_run_time>=10)
		{
			led_flag=0;
			led_run_time=0;
		}         
	}
	else
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		led_run_time++;
		if(led_run_time>=40)
		{
			led_flag=1;
			led_run_time=0;
		}          
	}
}


