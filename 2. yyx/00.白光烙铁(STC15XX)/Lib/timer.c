
/*************	����˵��	**************

���ļ�ΪSTC15xxxϵ�еĶ�ʱ����ʼ�����жϳ���,�û�����������ļ����޸��Լ���Ҫ���жϳ���.


******************************************/



#include	"timer.h"
int temp = 0;

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt TIMER0_VECTOR
{
	static unsigned int BeepCounter = 0;
	
	//��ת���º�BeepCounterFlag����λ����ʼ����
	if(BeepCounterFlag)
	{
		BeepCounter++;
	}

	//����С���趨ֵʱ�򿪣�����ʱ�ر�,Beep_Volume��ֵ������ռ�ձ�
	if((BeepCounter > 0) && (BeepCounter <= MENU_DATA_Table.Other_s.Beep_Volume*150))
	{
		BEEP_ON;
	}
	else
	{
		BEEP_OFF;
	}
	//����
	if(BeepCounter == 1500)
	{
		BeepCounter = 0;
		BeepCounterFlag = 0;
	}
}
unsigned char IronUseFlag = 0;
unsigned char CoderUseFlag = 0;
unsigned char IronSleepFlag = 0;
unsigned char IronScrSaverFlag = 0;
unsigned char IronShutdownFlag = 0;

/********************* Timer1�жϺ���************************/
void timer1_int (void) interrupt TIMER1_VECTOR
{
	static unsigned int Timer0_Counter=0,IronUseCounter=0;
	static unsigned char Sec_Conter=0,Min_Conter=0;
	static unsigned char P01_Old = 0;
	//��ʱ��0�ֶμ������Լ�
	Timer0_Counter++;
	
	//1ms������
	EC11_Capture();//��׽EC11��Ϣ	

	//20ms������
	if(Timer0_Counter%10 == 0)
	{
		if(P01 != P01_Old)
		{
			IronUseCounter++;
		}
		
		if(IronUseFlag || CoderUseFlag)//��������������ˣ���Ҫ���������ǲ�������ʲô��״̬��
		{
			if(IronSleepFlag)//�������л���
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronSleepFlag = 0;
			}
			else if(IronScrSaverFlag)//�ӹ����л���
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronScrSaverFlag = 0;
				IronSleepFlag = 0;
			}
			else if(IronShutdownFlag)//���绽��
			{
				Sec_Conter = 0;
				Min_Conter = 0;
				ec11_state.Key = 0;
				ec11_state.Coder = 0;
				IronShutdownFlag = 0;
			}
		}
	}
	//100ms������
	if(Timer0_Counter%100 == 0)
	{

		//�¶ȿ���

		if(IronSleepFlag || IronScrSaverFlag)
		{
			IronTempControl(MENU_DATA_Table.Temp_s.Slp_Temp);
		}
		else
		{
			IronTempControl(IronTemp_AM);
		}
		
		P01_Old = P01;
	}
	

	
	//1000ms������
	if(Timer0_Counter == 200)
	{
		//һ���ڿ�10�Σ������10����N�������ֱ�״̬�����仯����ô��Ϊ��ȷʵ��ʹ��
		if(IronUseCounter>MENU_DATA_Table.Other_s.Shock_Sensor)
		{
			IronUseFlag = 1;
			IronUseCounter = 0;
		}
		else
		{
			IronUseFlag = 0;
			IronUseCounter = 0;
		}

		//���û�ã���ô��ʼ��ʱ��ֻҪ���˾�����
		if(IronUseFlag==0 && CoderUseFlag==0 && ShutdownCounterFlag==0)
		{
			Sec_Conter++;
		}
		else
		{
			Sec_Conter = 0;
			Min_Conter = 0;
			CoderUseFlag = 0;
		}
		Timer0_Counter = 0;
	}
	
	//ÿ����
	if(Sec_Conter == 250)//��ʱ��׼
	{
		Min_Conter++;
		Sec_Conter = 0;
		//���������ߡ�����������ػ�
		//���˯��ʱ�䲻Ϊ0�Ҳ������߻����״̬
		if((MENU_DATA_Table.Time_s.Slp_Time != 0) && (IronSleepFlag == 0) && (IronScrSaverFlag == 0))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.Slp_Time)
			{
				IronSleepFlag = 1;
				IronScrSaverFlag = 0;
				BEEP_ON;
				Delay_ms(150);
				BEEP_OFF;
				//���ʱ��Ϊ������׼��
				Sec_Conter = 0;
				Min_Conter = 0;
			}
		}
		//�Ѿ�����,����δ����,���ܹ���ʱ�䣬�������ʱ����0����ֱ�����ߵ�ʱ��Ͱ�������
		if((IronSleepFlag == 1) && (IronScrSaverFlag == 0))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.ScrSaver_Time)
			{
				IronSleepFlag = 0;
				IronScrSaverFlag = 1;
				BEEP_ON;
				Delay_ms(150);
				BEEP_OFF;
				//���ʱ��Ϊ��ػ���׼��
				Sec_Conter = 0;
				Min_Conter = 0;
			}
		}
		//�����ػ�ʱ�䲻Ϊ0���Ѿ�����
		else if((MENU_DATA_Table.Time_s.Shutdown_Time != 0) && (IronScrSaverFlag == 1))
		{
			if(Min_Conter == MENU_DATA_Table.Time_s.Shutdown_Time)
			{
				//���ʱ��Ϊ���绽����׼��
				Sec_Conter = 0;
				Min_Conter = 0;
				//�����ߡ�������־λ
				IronScrSaverFlag = 0;
				IronSleepFlag = 0;
				IronShutdownFlag = 1;
				//��������
				BEEP_ON;
				Delay_ms(250);
				Delay_ms(50);
				BEEP_OFF;
				//��ػ�
				P10 = 0;//ȷ�������ص�λ��
				PCON |= 0x02;
			}
		}

	}
}
/********************* Timer2�жϺ���************************/
void timer2_int (void) interrupt TIMER2_VECTOR
{
	static unsigned char SetTempCounter = 0;
	
	if(SetTempFlag == 1)
	{
		SetTempCounter++;
	}
	if(SetTempCounter == 60)
	{
		SetTempCounter = 0;
		SetTempFlag = 0;
		if((MENU_DATA_Table.Temp_s.Default_Temp == 0) || (MENU_DATA_Table.Temp_s.Default_CH == 0))
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	}
}


//========================================================================
// ����: u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
// ����: ��ʱ����ʼ������.
// ����: TIMx: �ṹ����,��ο�timer.h��Ķ���.
// ����: �ɹ�����0, �ղ�������1,���󷵻�2.
// �汾: V1.0, 2012-10-22
//========================================================================
u8	Timer_Inilize(u8 TIM, TIM_InitTypeDef *TIMx)
{
	if(TIM > Timer2)	return 1;	//�ղ���

	if(TIM == Timer0)
	{
		TR0 = 0;		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		ET0 = 1;	//�����ж�
		else									ET0 = 0;	//��ֹ�ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT0 = 1;	//�����ȼ��ж�
		else									PT0 = 0;	//�����ȼ��ж�
		if(TIMx->TIM_Mode >  TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x03) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ, 3: 16λ�Զ���װ, ���������ж�
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x80;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x80;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x04;	//����������Ƶ
		else										TMOD &= ~0x04;	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x01;	//���ʱ��
		else							INT_CLKO &= ~0x01;	//�����ʱ��
		
		TH0 = (u8)(TIMx->TIM_Value >> 8);
		TL0 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR0 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer1)
	{
		TR1 = 0;		//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)		ET1 = 1;	//�����ж�
		else									ET1 = 0;	//��ֹ�ж�
		if(TIMx->TIM_Polity == PolityHigh)		PT1 = 1;	//�����ȼ��ж�
		else									PT1 = 0;	//�����ȼ��ж�
		if(TIMx->TIM_Mode >= TIM_16BitAutoReloadNoMask)	return 2;	//����
		TMOD = (TMOD & ~0x30) | TIMx->TIM_Mode;	//����ģʽ,0: 16λ�Զ���װ, 1: 16λ��ʱ/����, 2: 8λ�Զ���װ
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~0x40;	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  0x40;	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	TMOD |=  0x40;	//����������Ƶ
		else										TMOD &= ~0x40;	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x02;	//���ʱ��
		else							INT_CLKO &= ~0x02;	//�����ʱ��
		
		TH1 = (u8)(TIMx->TIM_Value >> 8);
		TL1 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	TR1 = 1;	//��ʼ����
		return	0;		//�ɹ�
	}

	if(TIM == Timer2)		//Timer2,�̶�Ϊ16λ�Զ���װ, �ж������ȼ�
	{
		AUXR &= ~(1<<4);	//ֹͣ����
		if(TIMx->TIM_Interrupt == ENABLE)			IE2  |=  (1<<2);	//�����ж�
		else										IE2  &= ~(1<<2);	//��ֹ�ж�
		if(TIMx->TIM_ClkSource >  TIM_CLOCK_Ext)	return 2;
		if(TIMx->TIM_ClkSource == TIM_CLOCK_12T)	AUXR &= ~(1<<2);	//12T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_1T)		AUXR |=  (1<<2);	//1T
		if(TIMx->TIM_ClkSource == TIM_CLOCK_Ext)	AUXR |=  (1<<3);	//����������Ƶ
		else										AUXR &= ~(1<<3);	//��ʱ
		if(TIMx->TIM_ClkOut == ENABLE)	INT_CLKO |=  0x04;	//���ʱ��
		else							INT_CLKO &= ~0x04;	//�����ʱ��

		TH2 = (u8)(TIMx->TIM_Value >> 8);
		TL2 = (u8)TIMx->TIM_Value;
		if(TIMx->TIM_Run == ENABLE)	AUXR |=  (1<<4);	//��ʼ����
		return	0;		//�ɹ�
	}
	return 2;	//����
}
