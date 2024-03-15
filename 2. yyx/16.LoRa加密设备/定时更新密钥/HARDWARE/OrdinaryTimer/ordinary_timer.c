#include "ordinary_timer.h"


/****************************************************
	* ������������ʱ��2��ʼ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
*****************************************************/ 
void OrdinaryTimerInit(u8 WhichTimer,u16 period, u16 prescaler)
{
	
	u8 priority;			//�����ȼ�
	u8 NVIC_IRQChannel;		//��ʱ��ͨ��
   
	//����ṹ��
    TIM_TimeBaseInitTypeDef		TIM_TimeBaseStructure;
    NVIC_InitTypeDef 			NVIC_InitStructure;
	TIM_TypeDef 				*TIMx;
	
	switch( WhichTimer )
	{
		case 0x02:
			//TIM3ʱ��ʹ�� 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
			TIMx = TIM2;
			NVIC_IRQChannel = TIM2_IRQn;
			priority = 1;
		break;
		
		case 0x03:
			//TIM3ʱ��ʹ�� 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
			TIMx = TIM3;
			NVIC_IRQChannel = TIM3_IRQn;
			priority = 2;
		break;
		
		case 0x04:
			//TIM3ʱ��ʹ�� 
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
			TIMx = TIM4;
			NVIC_IRQChannel = TIM4_IRQn;
			priority = 3;
		break;
		
		default:
			//... ...
		break;
	}
	
    //TIM3��ʼ������
    TIM_TimeBaseStructure.TIM_Period 		= period - 1;			//�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler 	= prescaler - 1;		//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     	//����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode 	= TIM_CounterMode_Up; 	//TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);             	//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    TIM_ClearFlag(TIMx, TIM_FLAG_Update);		//������±�־λ			
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);	//����TIM3�����ж�

    //TIM3�ж�����
    NVIC_InitStructure.NVIC_IRQChannel 					 = NVIC_IRQChannel;	//TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;   			//��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = priority;		//�����ȼ�1��
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                             			//����ָ���Ĳ�����ʼ������NVIC�Ĵ���

    //ʹ��TIM3
    TIM_Cmd(TIMx, ENABLE);
}


/***************************************
	* ������������ʱ��3�жϷ�����
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
****************************************/
void TIM3_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
		//����жϱ�־
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		
		RunLEDCounter();			//LED��ת������
		RunWaitIDCounter();			//�ȴ��Է�ID��Ӧ
		RunLoRaTransCounter();		//LoRa����ȴ�(���շ�)
		JudgeTheHostComWorkMode();	//����״̬ɨ��
	}
}


void TIM2_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
		//����жϱ�־
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		//... ...
	}
}


void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
		//����жϱ�־
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		//... ...
	}
}




