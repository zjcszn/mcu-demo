#include "WatchDog.h"

u16 WatchDogTimer = 0x00;

/*********************************************
	* ����������ָʾ�Ƴ�ʼ������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��ʼ��ʱ���͵�ƽΪ����LED
**********************************************/
void WatchDogPinInit(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( WATCHDOG_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= WATCHDOG_PIN;			//LED0-->PB.5 �˿�����
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//IO���ٶ�Ϊ50MHz
	GPIO_Init(WATCHDOG_PORT, &GPIO_InitStructure);			//�����趨������ʼ��GPIOA_0
}


void RunWatchDogTimer(void)
{
	if(WatchDogTimer<2000)
	{
		WatchDogTimer ++;
	}
}


/*******************************************
	* ����������ι��������������Ƿ����
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��1���ڸı�1�ε�ƽ
********************************************/
void WatchDogHandle(void)
{
	if( WatchDogTimer>FEED_DOG_CYCLE )
	{
		WatchDogTimer = 0x00;
		WATCH_DOG = !WATCH_DOG;
	}
}





