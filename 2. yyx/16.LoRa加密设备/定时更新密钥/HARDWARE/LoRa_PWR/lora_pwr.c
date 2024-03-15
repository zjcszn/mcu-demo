#include "lora_pwr.h"


//��ʼ��LORA��Դʹ�����ţ����򿪵�Դ
void LoRaPWR_Init(void)
{	
	GPIO_InitTypeDef 	GPIO_InitStructure;					//����ṹ��
	
	RCC_APB2PeriphClockCmd( LORA_PWR_RCC, ENABLE); 			//����GPIOC������ʱ��
															   
	GPIO_InitStructure.GPIO_Pin 	= LORA_PWR_PIN;			//ѡ��Ҫ���Ƶ�GPIOB����
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;  	//��������ģʽΪͨ���������
	GPIO_InitStructure.GPIO_Speed  	= GPIO_Speed_50MHz; 	//������������Ϊ50MHz 
	GPIO_Init(LORA_PWR_PORT, &GPIO_InitStructure);			//���ÿ⺯������ʼ��GPIO

	GPIO_SetBits(LORA_PWR_PORT,LORA_PWR_PIN);
}