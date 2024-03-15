#include "timer.h"

//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!

//u8 BeefWorkWay=0;   //0-fail,1-pass,other-no work
void TIMx_Int_Init(TIM_TypeDef* TIMx,u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
    if(TIMx == TIM1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
    if(TIMx == TIM2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
    if(TIMx == TIM3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
    if(TIMx == TIM4)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
    TIM_DeInit(TIMx);
	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ // MHzʱ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    
	//�ж����ȼ�NVIC����
    if(TIMx == TIM1)
        NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3�ж�
    if(TIMx == TIM2)
        NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
    if(TIMx == TIM3)
        NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
    if(TIMx == TIM4)
        NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
    //���TIM_IT_Update�Ĵ����ܷ�ֹ�жϳ�ʼ��ʱ�����������ж�
    TIM_Cmd(TIMx, ENABLE);
    TIM_ClearITPendingBit(TIMx, TIM_IT_Update);//��������ж�����λ
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);//ʹ�ܶ�ʱ�� �����ж�
    TIM_Cmd(TIMx, DISABLE);  //ʹ��TIMx    
}


void TIM4_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM ��ʼ��
    TIM_DeInit(TIM4);
	TIM_TimeBaseStructure.TIM_Period = arr-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ // MHzʱ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

    TIM_Cmd(TIM4, ENABLE);
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//��������ж�����λ
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);//ʹ�ܶ�ʱ�� �����ж�
    TIM_Cmd(TIM4, DISABLE);  //ʹ��TIMx  			 
}

void TIM1_UP_IRQHandler(void)   
{                      
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//���ָ����TIM�жϷ������:TIM �ж�Դ   
    {  
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ   
        printf("this TIM1\r\n");
    }          
}

u16 TIM2_Time=0;
void TIM2_IRQHandler(void)   
{                      
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)//���ָ����TIM�жϷ������:TIM �ж�Դ   
    {  
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//���TIMx���жϴ�����λ:TIM �ж�Դ   
        if(USART1_RX_STA&0x8000)
        {         
            
            if(Str_Pos((char*)USART1_RX_BUF,"CMD ERROR") )
            {
                USART1_RX_STA = 0;
                Cmd_SendErrorFlag = 0;
                return;
            }
			if(Str_Pos((char*)USART1_RX_BUF,"CMD OK") )
            {
                USART1_RX_STA = 0;
                Cmd_SendErrorFlag = 0;
                return;
            }
                        
        }
    }          
}

//��ʱ��3�жϷ������
u16 TIM3_Time = 0;
u16 TIM3_SignalTime = 0;
u16 TIM3_TempTime = 0;
u16 TIM3_TempTime2 = 0;

u16 TIM3_Times1=0;
u16 TIM3_Times2=0;
u16 TIM3_Times3=0;
u16 TIM3_Times4=0;
void TIM3_IRQHandler(void)   //TIM3�ж�
{
	//u8 i=0;
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);
        
        if(Timer3_EnableFlag&0x01)  TIM3_Times1++;
            else TIM3_Times1=0;
            
        if(Timer3_EnableFlag&0x02)  TIM3_Times2++;
            else TIM3_Times2=0;
            
        if(Timer3_EnableFlag&0x04)  TIM3_Times3++;
            else TIM3_Times3=0;
        
        if(Timer3_EnableFlag&0x08)  TIM3_Times4++;
            else TIM3_Times4=0;
      
	}
		
}
u8 TIM4_Temp=0;
u16 TIM4_Time=0;
void TIM4_IRQHandler(void)
{ 	
	//u16 i=0;
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIM4�����жϱ�־
        TIM_ClearFlag(TIM3, TIM_FLAG_Update);
        TIM4_Time++;
        if(TIM4_Temp)
        {
            if(0 == USART1_RecviceFlag)
            {
                if(TIM4_Time>5) //10*10ms ��û�н��յ����ݾͱ궨�˴ν������
                {
                    TIM_Cmd(TIM4, DISABLE);  //�ر�TIM4 
                    //for(i=USART1_RX_STA;i<USART_REC_LEN;i++) 
                    //{
                    //    USART1_RX_BUF[i] = 0;
                    //}
                    USART1_RX_STA |= 0x8000;	//��ǽ������ 
                    //USART1_RecviceFlag = 0;
                    TIM4_Temp = 0; 
                   
                }
            }
            else
            {
                USART1_RecviceFlag = 0;
                //TIM4_Time = 0;
              
            }
        }
        if(USART1_RecviceFlag && (!TIM4_Temp)) 
        {
            USART1_RecviceFlag = 0;
            TIM4_Temp = 1;
    
        } 
	}	    
}

static uint16_t TIM2_CR=0;
static uint16_t TIM3_CR=0;
static u8 TIM_Flag=0;
void TIM_TemporaryClose(u8 NewState)//0-�ָ���ʱ��ԭ����״̬,0x11-�ر�ָ����ʱ��
{
    if(((NewState&0x01) || (NewState&0x10) ) && (0 == TIM_Flag) ) //��Ҫ�رն�ʱ��֮ǰ�ȱ���ԭ����״̬
    {
        TIM2_CR = TIM2->CR1;
        TIM3_CR = TIM3->CR1;
    }
    
    if(0 == NewState)
    {
        if(0 == TIM_Flag)   return; //û��disable �Ͳ�ִ��
        TIM2->CR1 |= TIM2_CR;
        TIM3->CR1 |= TIM3_CR;
        TIM_Flag = 0;
    }
    else
    {
        TIM_Flag = 1;
        if(NewState&0x01)
            TIM_Cmd(TIM2,DISABLE);//
        if(NewState&0x10)
            TIM_Cmd(TIM3,DISABLE);//
    }
}











