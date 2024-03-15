#include "dip_switch.h"


//��¼�豸����
u8 LoRaDeviceIs = 0;


/*********************************************
	* �������������뿪�س�ʼ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	�������룬���ж����޲��£�
**********************************************/
void DIP_SwitchInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA| 
							RCC_APB2Periph_GPIOB|
							RCC_APB2Periph_GPIOC, ENABLE );		//ʹ��ʱ��
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);			//PB3 PB4 ����GPIO
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	//ʹ��SW ����JTAG ��PB3��PB4��	
	
	//GPIOB-3/4/8/9
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3 |
									GPIO_Pin_4 |
									GPIO_Pin_8 | 
									GPIO_Pin_9; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;	//���ó���������
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��GPIO
	
	//GPIOA-11/12/15
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_11 |	//PA11/12/15
									GPIO_Pin_12 |
									GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;	//���ó���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);			//��ʼ��GPIO
	
	//GPIOC-13
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_13;	//PC13
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPD;	//���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//��ʼ��GPIO
	
	JudgeMasterOrSlave();
}


/*******************************************
	* �����������ж�������OR�ӻ�
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	
********************************************/
void JudgeMasterOrSlave(void)
{
	if( ID8 == 1 )
		LoRaDeviceIs = MASTER;
	else
		LoRaDeviceIs = SLAVE;
}

/******************************************
	* ��ȡ���ں�

*******************************************/
void ConfirmUartx(void)
{
	if( ID1 == 1 )
	{
		hostcom_struct.WhichUart = UART1;
	}
	else
		hostcom_struct.WhichUart = UART3;
}


/**********************************************
	* �������������ò�����
	* ��ڲ�������
	* ����ֵ��
	* ��ע��
***********************************************/
void SetUartBound(void)
{
	u8 uartx_bound = 0x00;
	
	uartx_bound = ID2 | ID3<<1;
	
	switch(uartx_bound)
	{
		case 0:
			hostcom_struct.Bound = 9600;
		break;
		
		case 1:
			hostcom_struct.Bound = 19200;
		break;
		
		case 2:
			hostcom_struct.Bound = 38400;
		break;
		
		case 3:
			hostcom_struct.Bound = 115200;
		break;
		
		default:
		break;
	}
}


/********************************************
	* ������������ȡ�豸��ַ
	* ��ڲ�����
	* ����ֵ��
	* ��ע��
*********************************************/
u8 ReadDeviceID(void)
{
	u8 device_id = 0x00;
	
	device_id |= ID4 | (ID5<<1) | (ID6<<2) | (ID7<<3);
    return device_id;
}



