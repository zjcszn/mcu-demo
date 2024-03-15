#include "advance_timer.h"

extern void RunCalcOneHourCounter(void);


/************************************************
	* �����������߼���ʱ����ʼ��
	* ��ڲ�����WhichTimer 	-- ��ʱ��1/2/3
				period	   	-- �Զ���װֵ
				prescaler	-- Ԥ��Ƶϵ��
	* ����ֵ��
	* ��ע��	
*************************************************/
void AdvanceTimerInit(u8 WhichTimer, u16 period, u16 prescaler)
{
    TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
    NVIC_InitTypeDef 			NVIC_InitStructure;
    TIM_TypeDef					*TIMx;
	
    u8 NVIC_IRQChannel;
    u8 priority;
	
	//ʹ�ܶ�ʱ����ʱ�Ӽ�ѡ��ͨ��
    switch(WhichTimer)
    {
		//��ʱ��1
		case 1:
			TIMx = TIM1;
			NVIC_IRQChannel = TIM1_UP_IRQn;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
			priority = 2;
		break;
		
		//��ʱ��8
		case 8:
			TIMx = TIM8;
			NVIC_IRQChannel = TIM8_UP_IRQn;
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
			priority = 1;
		break;
		
		//�������
		default:
			return ;
		break;
    }
	
	//��ʱ����������
    TIM_TimeBaseStructure.TIM_Period 			= period - 1;	 		//�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler 		= prescaler - 1;		//ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ		
    TIM_TimeBaseStructure.TIM_ClockDivision 	= TIM_CKD_DIV1;			//ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode 		= TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x00;					//�����ظ��������Ϊ0(�߼���ʱ��������)
    TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	
	//���
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);		//������±�־
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE );	//������ʱ������ж�
	
	//��ʱ���жϲ�������
    NVIC_InitStructure.NVIC_IRQChannel 					 = NVIC_IRQChannel;	//ѡ���ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 		 = priority;		//�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd 				 = ENABLE;			//ʹ��ͨ��
    NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM1,ENABLE);
}


/********************************************
	* ����������ʹ�ܶ�ʱ��
	* ��ڲ�����which -- ��ʱ�����
				state -- ״̬
	* ����ֵ��	��
	* ��ע��	��
*********************************************/
void SetAdvanceTimerState(u8 which,FunctionalState state)
{
	switch(which)
	{
		case 1:
			TIM_ITConfig(TIM1,TIM_IT_Update,state);
			TIM_Cmd(TIM1,state);
		break;
		
		case 8:
			TIM_ITConfig(TIM8,TIM_IT_Update,state);
			TIM_Cmd(TIM8,state);
		break;
		
		default:
		break;
	}
}


//��ʱ��1�жϷ������
void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
		//�����жϱ�־λ
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update); 
		
		RunComAtModeTimer();		//��λ��ָ���ʱ
		RunComStatusChangeTimer();	//����ʱ����(����״̬�л�)
		RunDelayCounter();			//��ʱ����(ms)
//		RunWatchDogTimer();			//ι����ʱ��
		RunCalcOneHourCounter();
    }
	
}


//��ʱ��1�жϷ������
void TIM8_UP_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)
    {
		//�����жϱ�־λ
		TIM_ClearITPendingBit(TIM8, TIM_FLAG_Update); 
    }
	
}



