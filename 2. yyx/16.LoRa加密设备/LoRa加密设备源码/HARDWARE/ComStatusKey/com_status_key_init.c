#include "com_status_key_init.h"


extern _ComWorkMode		com_work_mode;
extern _AT_TimerStruct	at_timer_struct;

static u8 ComStatusKeyPressFlag = DISABLE;
u16 ComStatusChangeTimer = 0x00;
u8	ComStatusChange = KEY_NONE;	


//����״̬�л�����(����ģʽ�л�)
void ComStatusChangeKeyInit(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOC, ENABLE );	//ʹ��ʱ��
				
	//����״̬�л�����
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;		//ѡ��PIN4����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);			//��ʼ��GPIOC4
}


//����ɨ����
void ScanComStatusKey(void)
{
	if(COM_STATUS_KEY == 0)
	{
		ComStatusKeyPressFlag = ENABLE;
	}
	else
	{
		ComStatusKeyPressFlag = DISABLE;
	}
}


/**************************************************
	* �������������д���״̬�л���ʱ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	
***************************************************/
void RunComStatusChangeTimer(void)
{
	if(ComStatusKeyPressFlag == ENABLE)
	{
		ComStatusChangeTimer ++;
	}
	else
	{
		if( ComStatusChangeTimer >1500 )
		{
			ComStatusChange = KEY_AT;
			ComStatusChangeTimer = 0x00;
			
		}
		else if(ComStatusChangeTimer < 1000)
		{
			ComStatusChange = KEY_NONE;
			ComStatusChangeTimer = 0x00;
		}
	}
}	


/************************************************
	* �����������ж��Ƿ���䴮�ڹ���״̬
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
*************************************************/
void IsComChangeToAT_WorkMode(void)
{
	if(ComStatusChange == KEY_AT)
	{
		com_work_mode = ComMode_AT;
		at_timer_struct.SetAtTimer_5s = 0x00;
	}
}





