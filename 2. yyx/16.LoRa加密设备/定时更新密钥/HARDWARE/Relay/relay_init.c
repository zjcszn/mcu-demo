#include "relay_init.h"

u16 RelayCounter = 0x0;


void RelayInit(void)
{	
	GPIO_InitTypeDef	GPIO_InitStructure;					//����I/O�ڽṹ��
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE); 	//����GPIOB������ʱ��
													   
	GPIO_InitStructure.GPIO_Pin 	= 	GPIO_Pin_13|
										GPIO_Pin_14;		//ѡ��Ҫ���Ƶ�GPIOB����
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_Out_PP;  	//��������ģʽΪͨ��������� 
	GPIO_InitStructure.GPIO_Speed 	= 	GPIO_Speed_50MHz; 	//������������Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//����ָ��������ʼ��GPIO	  
	
	//�ؼ̵���
	RelayP = 0;	//�̵���������
	RelayN = 0;	//�̵���������
}


/**********************************************
	* �������������м̵�����ʱ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
***********************************************/
void RunRelayCounter(void)
{
	if(RelayCounter<2000)
	{
		RelayCounter ++;
	}
}


