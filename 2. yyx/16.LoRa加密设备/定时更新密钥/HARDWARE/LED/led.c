#include "led.h"


/*********************************************
	* ����������ָʾ�Ƴ�ʼ������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��ʼ��ʱ���͵�ƽΪ����LED
**********************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_1;			//LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB.5
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_2;			//LED0-->PB.5 �˿�����
	GPIO_Init(GPIOA, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB.5
	
	//��ָʾ��
	RUN_LED_ON();	
	NET_LED_ON();
}


static u16 LED_Counter = 0x00;
//����LED������
void RunLEDCounter(void)
{
	if(LED_Counter<LED_MAX_VALUE)
	{
		LED_Counter ++;
	}
}


/**************************************************
	* ����������LED���������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	ͨ�����ĺ�LED_FLASH_FRE��
				�ı�LED����˸Ƶ�ʣ�Ĭ��Ϊ500ms
***************************************************/
void IndicatorLightMainHandle(void)
{
	if( LED_Counter<LED_FLASH_FRE )
		return;
	
	LED_Counter = 0x00;		//������������
	RUN_LED = !RUN_LED;		//��תָʾ��״̬
}



