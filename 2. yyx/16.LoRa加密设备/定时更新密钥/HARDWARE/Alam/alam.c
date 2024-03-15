/*******************************************************************
	* �ļ�����������ť
	* ���ߣ�	ZWQ
	* ʱ�䣺	2019-3-13
	* ��ע��	��ⰴ������⵽�����������Ӧ�Ĵ�ʩ��
********************************************************************/
#include "alam.h"

u16 AlamKeyPressCounter = 0x00;


//����������ʼ��
void AlamKeyPinInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(ALAM_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = ALAM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ó���������
	GPIO_Init(ALAM_PORT, &GPIO_InitStructure);		//��ʼ��GPIO
}


void ScanAlamKey(void)
{
	if( ALAM_KEY == 0 )
	{
		if(AlamKeyPressCounter <1000)
		{
			AlamKeyPressCounter ++;
		}
		
		else
		{
			AlamKeyPressCounter = 0x00;
		}
	}
}



