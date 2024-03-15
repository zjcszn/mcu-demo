#include "ACPowerDetection.h" 
#include "delay.h" 
#include "RS485.h"
#include "CRC.h"

uint8_t u8_ACPowerStatus;
__IO uint16_t u16_ACPowerDetectionTimerMs;
//IO��ʼ��
void ACPowerDetection_GPIO_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��

  //GPIOA4��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
//  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	u8_ACPowerStatus = 1;
}
void ACPowerDetection_Process(void)
{
	if(u16_ACPowerDetectionTimerMs > 10000)		//ÿ10����һ��
	{
		u16_ACPowerDetectionTimerMs = 0;
		
		u8_AlarmStatus &= ~0x80;
		  
		if(ACPowerIN() == 0)			//  ����������  �����ͨ����������
		{
			delay_ms(10);
			if(ACPowerIN() == 0)
			{
//				u8_ACPowerStatus = 1;
				u8_AlarmStatus |= 0x80;  //���辯��
			}
		}
	}
}
void ACPowerDetection_Timer_Ms(void)
{
	u16_ACPowerDetectionTimerMs++;
}





