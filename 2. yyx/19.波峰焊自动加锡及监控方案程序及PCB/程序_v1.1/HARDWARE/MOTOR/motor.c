#include "stm32f10x.h"
#include "motor.h"
#include "sys.h" 
#include "delay.h"
#include "key.h"

void motor_Init(void) //IO��ʼ��
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PA\PB�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12;						//PA.11.12 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						    //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);								   	    //�����趨������ʼ��GPIOA.11.12
	GPIO_SetBits(GPIOA,GPIO_Pin_11|GPIO_Pin_12);								//PA.11.12�����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;						//PB.0.1 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						    //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);								   	    //�����趨������ʼ��GPIOB.0
	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1);									//PB.0.1�����
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;						//PB.0.1 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 					//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;						    //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);								   	    //�����趨������ʼ��GPIOB.0
	GPIO_SetBits(GPIOC,GPIO_Pin_0);									//PB.0.1�����
}
//MP_delay:���ת��ʱ��  PN��PN=0����ת��  PN=1����ת��
u8 motor_R(u32 MP_delay,u8 PN)
{
	u32 mp_delay;
	ENA=1;
	DIR=PN;
	for(mp_delay=0;mp_delay<MP_delay;mp_delay++)
	{
		PUL=0;
		delay_us(100);
	//	delay_ms(1);
		PUL=1;
		delay_us(100);
	//	delay_ms(1);
		if(DIR==0)
		{
			if(IR_DOWN==0)
			{
				return 0;
			}
		}	
		if(DIR==1)
		{
			if(IR_UP==0)
			{
				return 0;
			}
		}	
	}	
}
u32 motor_P(u32 MP_delay)			//����ת��
{
	u32 mp_delay;
	ENA=1;
//	delay_us(10);
	DIR=1;
	
	for(mp_delay=0;mp_delay<MP_delay;mp_delay++)
	{
		PUL=1;
		MCU_TEST=1;
		delay_us(50);
		PUL=0;
		MCU_TEST=0;
		delay_us(50);
		if(IR_UP==0)
		{
			return 0;
		}
	}	
}

u32 motor_N(u32 MN_delay)			//����ת��
{
	u32 mn_delay;
	ENA=1;
//	delay_us(5);
	DIR=0;
	for(mn_delay=0;mn_delay<MN_delay;mn_delay++)
	{
		PUL=1;
		delay_us(20);
		PUL=0;
		delay_us(20);
		if(IR_DOWN==0)
		{
			return 0;
		}
	}	
}

void motor_S()			//ֹͣת��
{
	delay_ms(10);

	ENA = 0;	//ֹͣת��
}
