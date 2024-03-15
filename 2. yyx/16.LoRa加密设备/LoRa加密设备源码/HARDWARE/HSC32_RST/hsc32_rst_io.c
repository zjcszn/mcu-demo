#include "hsc32_rst_io.h"

extern void TimerDelay_Ms(u16 counter);


//��ʼ��LORA��Դʹ�����ţ����򿪵�Դ
void HSC32RstInit(void)
{	
	GPIO_InitTypeDef 	GPIO_InitStructure;					//����ṹ��
	
	RCC_APB2PeriphClockCmd( HSC32IO_RCC, ENABLE); 			//����GPIOC������ʱ��
	
	GPIO_InitStructure.GPIO_Pin 	= HSC32IO_PIN;			//ѡ��Ҫ���Ƶ�GPIOB����
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;  	//��������ģʽΪͨ���������
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz; 	//������������Ϊ50MHz 
	GPIO_Init(HSC32IO_PORT, &GPIO_InitStructure);			//���ÿ⺯������ʼ��GPIO
	
	HSC_RST_PIN = 0;
	delay_ms(500);
	HSC_RST_PIN = 1;
}



//��λHSC32ģ��
void RestHSC32Module(void)
{
	HSC_RST_PIN = 0;
	TimerDelay_Ms(100);
	HSC_RST_PIN = 1;
	TimerDelay_Ms(100);
}




