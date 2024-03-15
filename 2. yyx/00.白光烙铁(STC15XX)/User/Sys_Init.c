/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			ϵͳ��ʼ��
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

/************************����ȫ�ֱ���*****************************/
void GPIO_Init(void)
{
	//�ṹ����
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	//����P01��sleep��,P03��beep��Ϊ����
	GPIO_InitStructure.Pin  = GPIO_Pin_1|GPIO_Pin_3;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P0,&GPIO_InitStructure);

	
	//����P20~P24��OLED����Ϊ����
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_InitStructure.Mode = GPIO_PullUp;
	GPIO_Inilize(GPIO_P2,&GPIO_InitStructure);
	
	//����P30~P32��EC11��������Ϊ��©
	GPIO_InitStructure.Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
	
	//����P13��P15��P17��ADC��Ϊ����
	GPIO_InitStructure.Pin  = GPIO_Pin_3|GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.Mode = GPIO_OUT_OD;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);
	
	//����P10��PWM��Ϊ����
	GPIO_InitStructure.Pin  = GPIO_Pin_0;
	GPIO_InitStructure.Mode = GPIO_OUT_PP;
	GPIO_Inilize(GPIO_P1,&GPIO_InitStructure);

}

void ADC_Init(void)
{
	//�ṹ����
	ADC_InitTypeDef		ADC_InitStructure;		
	
	ADC_InitStructure.ADC_Px        = ADC_P13|ADC_P15|ADC_P17;	
	ADC_InitStructure.ADC_Speed     = ADC_180T;				//ADC�ٶ�			ADC_90T,ADC_180T,ADC_360T,ADC_540T
	ADC_InitStructure.ADC_Power     = ENABLE;					//ADC����			ENABLE,DISABLE
	ADC_InitStructure.ADC_AdjResult = ADC_RES_H2L8;		//ADC�������,	ADC_RES_H2L8,ADC_RES_H8L2
	ADC_InitStructure.ADC_Polity    = PolityLow;			//���ȼ�����	PolityHigh,PolityLow
	ADC_InitStructure.ADC_Interrupt = DISABLE;				//�ж�����		ENABLE,DISABLE
	ADC_Inilize(&ADC_InitStructure);									//��ʼ��
	ADC_PowerControl(ENABLE);													//������ADC��Դ��������, ENABLE��DISABLE
}

void Iron_Init(void)
{
	//�ṹ����
	PCA_InitTypeDef		PCA_InitStructure;

	PCA_InitStructure.PCA_Clock    = PCA_Clock_Timer0_OF;		//PCA_Clock_1T, PCA_Clock_2T, PCA_Clock_4T, PCA_Clock_6T, PCA_Clock_8T, PCA_Clock_12T, PCA_Clock_Timer0_OF, PCA_Clock_ECI
	PCA_InitStructure.PCA_IoUse    = PCA_P12_P11_P10_P37;	//PCA_P12_P11_P10_P37, PCA_P34_P35_P36_P37, PCA_P24_P25_P26_P27
	PCA_InitStructure.PCA_Interrupt_Mode = DISABLE;		//ENABLE, DISABLE
	PCA_InitStructure.PCA_Polity   = PolityLow;			//���ȼ�����	PolityHigh,PolityLow
	PCA_Init(PCA_Counter,&PCA_InitStructure);

	PCA_InitStructure.PCA_Mode     = PCA_Mode_PWM;		//PCA_Mode_PWM, PCA_Mode_Capture, PCA_Mode_SoftTimer, PCA_Mode_HighPulseOutput
	PCA_InitStructure.PCA_PWM_Wide = PCA_PWM_8bit;		//PCA_PWM_8bit, PCA_PWM_7bit, PCA_PWM_6bit
	PCA_InitStructure.PCA_Value    = 128 << 8;			//����PWM,��8λΪPWMռ�ձ�
	PCA_Init(PCA1,&PCA_InitStructure);
	
	CR = 1;
}
void Iron_Run()
{
	AUXR1 = (AUXR1 & ~(3<<4)) | (0<<4);			//��ʵ�ǰ������ת���ˣ��Ҳ�֪����ô��
}


void Iron_Off(void)
{
	AUXR1 = (AUXR1 & ~(3<<4)) | (2<<4);			//��ʵ�ǰ������ת���ˣ��Ҳ�֪����ô��
	P10 = 0;
}
void EXTI_Init(void)
{
	EXTI_InitTypeDef	EXTI_InitStructure;					//�ṹ����
	
	//�������İ�������Ϊ�����ȼ��ж�
	EXTI_InitStructure.EXTI_Mode = EXT_MODE_Fall;
	EXTI_InitStructure.EXTI_Polity = PolityLow;
	EXTI_InitStructure.EXTI_Interrupt = ENABLE;
	Ext_Inilize(EXT_INT4,&EXTI_InitStructure);

}

void Timer_Init(void)
{
	//�ṹ����
	TIM_InitTypeDef		TIM_InitStructure;	
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0xFF9C;				//��ֵ,50us
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer0,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityHigh;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_1T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0xA240;				//��ֵ,1ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer1,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;			//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 0x3CB0;				//��ֵ,25ms
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ��ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//��ʼ��Timer0	  Timer0,Timer1,Timer2
}

void ParaInit(void)
{
	unsigned char temp[5],i;
	
	//�ȶ�EEPROMǰ��λ
	EEPROM_read_n(0x0,(unsigned char *)&temp,sizeof(temp));
	for(i=0;i<5;i++)
	{
		temp[0] &= temp[i];
	}
	
	//�������OXFF����ô����ˢEEPROM�������Ļ����ص�ʱ��ֻ��Ҫ��ѡ���ν�EEPROMȫ�����ΪFF�Ļ����Ϳ���ʵ������ˢEEPROM
	if(temp[0] == 0xff)
	{
		EEPROM_SectorErase(0x0);
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));
	}
	
	EEPROM_read_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	EEPROM_read_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));
}

void Sys_Init(void)

{
	//����˼�壬�Ͳ�һһд��
	EA = 0;
	GPIO_Init();
	BEEP_ON;
	OLED_Init();
	Draw_BMP(Logo);
	ADC_Init();
	Iron_Init();
	EXTI_Init();
	Timer_Init();
	ParaInit();
	//KalmanFilterInit();
	BEEP_OFF;
	Delay_ms(200);Delay_ms(200);
	Delay_ms(200);Delay_ms(200);
	Delay_ms(200);Delay_ms(200);
	OLED_CLS();
	//���Ĭ���¶Ȳ�Ϊ0����Ĭ���¶��趨Ϊ�����¶�
	if(MENU_DATA_Table.Temp_s.Default_Temp != 0)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Default_Temp;
	}
	if(MENU_DATA_Table.Temp_s.Default_CH != 0)
	{
		chn = MENU_DATA_Table.Temp_s.Default_CH;
	}
	//���Ĭ���¶�Ϊ0�����ϴ��¶��趨Ϊ�����¶�
	if(MENU_DATA_Table.Temp_s.Default_Temp == 0)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Last_Temp;
	}
	if(MENU_DATA_Table.Temp_s.Default_CH == 0)
	{
		chn = MENU_DATA_Table.Temp_s.Last_CH;
	}
	EA = 1;
}