#define _MEASURE_C
#include "head.h"
//#include "uart.h"

//
//ad_measure[]�ı���Լ��
//0123��4��4-20MA��ͨ��ֵ������ֵ��4��


//u16 Userdefined_Period = 625;
u16 Userdefined_Period3 = 250;//1s��8��
void Updata_Lcd_YC(void);
void time_control(void)
{
	u8 i;
	if(measure_flag>=6)
	{
		ad_calucate();
		measure_flag=0;
		UpdataModbusYc();
		Updata_Lcd_YC();
	}
	else
	{ 
		ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[3]=ADC_GetConversionValue(ADC1);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[2]=ADC_GetConversionValue(ADC1);
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[1]=ADC_GetConversionValue(ADC1);
		// printf("i=%d\r\n",ad_buf[measure_flag].ad_group[2]);
		
		
		
		
		ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_28Cycles5);
		i=10;
		while(--i);
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);    
		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) != SET);
		ad_buf[measure_flag].ad_group[0]=ADC_GetConversionValue(ADC1);    
		measure_flag++;
	}
	
	
}


/*
AD���㺯��
*/


void ad_calucate(void)
{
	u32 temp;
	u8 i,j;
    
	//���4-20MA�ĵ�ǰֵ
	for(i=0;i<4;i++)
	{
		u32 temp_buf[6];
		u8 min_index,max_index;		
		for(max_index=0; max_index<6; max_index++)
		{
			temp_buf[max_index] = ad_buf[max_index].ad_group[i];
		}
		//������Сֵ
		min_index = 0;
		for (max_index=0; max_index<6; max_index++)
		{
            if (temp_buf[min_index] >temp_buf[max_index])
            {
				min_index = max_index;
            }
		}
		temp_buf[min_index] = 0;
		//�������ֵ
		min_index = 0;
		for (max_index=0; max_index<6; max_index++)
		{
            if (temp_buf[min_index] < temp_buf[max_index])
            {
				min_index = max_index;
            }
		}
		temp_buf[min_index] = 0;
		
		
		//��ƽ��,ADת��������ԭʼֵ��
		temp = 0;
		for (j=0; j<6; j++)
		{
			temp += temp_buf[j];
		}
		
		temp = temp/4;	
        
		
		//��ʼ����ʵ��4-20MA����ֵ
		ad_measure[i].ad_samp=temp*336000/(4096*148);//��λΪma��100��,���ѹΪ3.36
		
        
		if(ad_measure[i].ad_samp >= 400)
		{
			ad_measure[i].ad_samp = ad_measure[i].ad_samp-400;
			
		}
		else
		{
			ad_measure[i].ad_samp = 0;
		}
		
	}
	
    //test.�˴��Ժ�Ҫ�������ݵ��Ա�־��������е��Ա�־ʱ����װ����λ���´������ݣ��ﵽģ����Ե�Ŀ�ġ�
	//for debug
	//     ad_measure[0].ad_samp = 150;	
	//     ad_measure[1].ad_samp = 1000;
	//     ad_measure[2].ad_samp = 800;
    //ad_measure[3].ad_samp = 1600;
    
	//��4-20MA����ϵ��ת����ת����ʽ�ǣ�4-20MA=>0-??,�����0-2000����ϵ����2000/��2000-400��=
	ad_measure[0].ad_value = ad_measure[0].ad_samp * AD1_xishu;
	ad_measure[1].ad_value = ad_measure[1].ad_samp * AD2_xishu; 
	ad_measure[2].ad_value = ad_measure[2].ad_samp * AD3_xishu;
	ad_measure[3].ad_value = ad_measure[3].ad_samp * AD4_xishu;
    //����Ӧ����С1600��������ʱ��Ϊ��ʾʱ����һλС���㣬��������С10��
    ad_measure[0].ad_value /= 160;
    ad_measure[1].ad_value /= 160;
    ad_measure[2].ad_value /= 160;
    ad_measure[3].ad_value /= 160;
	
	//------------------------------------------------------------------------------------------
	
	////hyman2011-4����������
	// ������
	// 230��������һ��ˮ��5S�ۼƵ������������1���Ӽ����ˮ������λL/�֣��������� 1000 ����(X/230 * 12 * 1000) => X*52)
	if(fulx_measure_flag==1)
	{
		fulx_measure_flag=0;
		yc_buf[ZJ_AI_LL] = pulse_minus*52;	        
		
	}
	//
	// ���¼���ÿ�������źţ���Ҫ�Ӷ���ҩ������д����ֵ���ݿͻ��ṩ�����⹤����Ҫ���� CHANGE
	//yc_buf[26] = 172;//360*0.48;
	//yc_buf[27] = 91;//190*0.48;
    float temp2 = yc_buf[26];
	ll_single_jiayao3_value = temp2/(230 * 1000);
	temp2 = yc_buf[27];
	ll_single_jiayao4_value = temp2/(230 * 1000);
	    
}

void Updata_Lcd_YC(void)
{
// "1#���¶�",
	// "1#��ֵ",
	// "1#���¶�",
	// "1#��ֵ",   
    
	// "2#���¶�",
	// "2#��ֵ",
	// "2#���¶�",
	// "2#��ֵ",       
	
    // "2#���¶�",
	// "2#��ֵ", 
    // "3#��ֵ",
    // "����",
    
	// "3#��ֵ",
    // "3#��ֵ",

	lcd_disp_measure[0] = yc_buf[ZJ_AI_MST_1_WD];
	lcd_disp_measure[1] = yc_buf[ZJ_AI_MST_1_VAL];
	lcd_disp_measure[2] = yc_buf[ZJ_AI_SLV_1_WD];
	lcd_disp_measure[3] = yc_buf[ZJ_AI_SLV_1_VAL];

	lcd_disp_measure[4] = yc_buf[ZJ_AI_MST_2_WD];
	lcd_disp_measure[5] = yc_buf[ZJ_AI_MST_2_VAL];
	lcd_disp_measure[6] = yc_buf[ZJ_AI_SLV_2_WD];
	lcd_disp_measure[7] = yc_buf[ZJ_AI_SLV_2_VAL];
	
	lcd_disp_measure[8] = yc_buf[ZJ_AI_SLV2_2_WD];
	lcd_disp_measure[9] = yc_buf[ZJ_AI_SLV2_2_VAL];
	lcd_disp_measure[10] = ad_measure[1].ad_value / 10;
	lcd_disp_measure[11] = yc_buf[ZJ_AI_LL];

	lcd_disp_measure[8] = ad_measure[3].ad_value / 10;
    lcd_disp_measure[9] = ad_measure[2].ad_value / 10;	
}

void Adc_Init(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_InitTypeDef   ADC_InitStructure;
    
	TIM_Configuration();
	//	DMA_InitTypeDef   DMA_InitStructure;
	/* Enable GPIOA clock                                                       */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	/* Enable ADC1 clock                                                        */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/* ADC1 Configuration (ADC1CLK = 18 MHz) -----------------------------------*/
	ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;//ADC_Mode_RegSimult;//������ͬ������ģʽ// ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode       = ENABLE;//ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//��4��ͨ��ɨ����ɺ�ֹͣת��������Ӧ�������ٴ�����						  
	ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel       = 1;//����ͨ��ת����������
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_28Cycles5);//���������е�һ��ת��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 3, ADC_SampleTime_28Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 4, ADC_SampleTime_28Cycles5);
	
	ADC_Cmd(ADC1, ENABLE);// 
	//ADC_ResetCalibration(ADC1);
	//while(ADC_GetResetCalibrationStatus(ADC1));           
	ADC_StartCalibration(ADC1);//xiaozhun 
	//while(ADC_GetCalibrationStatus(ADC1));                      
	//ADC_SoftwareStartConvCmd(ADC1,ENABLE);/* Start ADC1 Software Conversion     */ 
    
    
    //hyman2011-04
    //�Ѽ����ú������Ƶ����������0����б�Ҫ�ĳ�ʼ����
    jlb1_need_times = 0;  //�����ö�������
    jlb1_all_times = 0;  //�����ö�������
    jlb2_need_times = 0;  //�����ö�������
    jlb2_all_times = 0;  //�����ö�������
	jlb7_need_times = 0;  //�����ö�������
    jlb7_all_times = 0;  //�����ö�������
    
    jlb3_need_times = 0;  //�����ö�������
    jlb3_all_times = 0;  //�����ö�������
    jlb4_need_times = 0;  //�����ö�������
    jlb4_all_times = 0;  //�����ö�������
	jlb5_need_times = 0;  //�����ö�������
    jlb5_all_times = 0;  //�����ö�������
    liuliangjiMaster_all_times = 0;  //���������ܽ��մ���
    liuliangjiMaster_all_water = 0;  //�����еĲ�ˮ��


	ll_single_jiayao3_value = 0;
    ll_single_jiayao4_value = 0;

	F_Count=0;
	F_Count1=0;
    
}
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 0xFFFF; //�Զ�װ�ؼĴ�����ֵ         
	TIM_TimeBaseStructure.TIM_Prescaler = 0x8C9F;  //Ԥ��Ƶ��     
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; //   
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* Output Compare Active Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;          
	//  TIM_OCInitStructure.TIM_Channel = TIM_Channel_1;          
	TIM_OCInitStructure.TIM_Pulse = PERIOD;  
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);//��ֹԤװ�ع���
	
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	
	/* TIM IT enable */
	TIM_ITConfig(TIM2, TIM_IT_CC1 , ENABLE); 
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);  
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 65535;	  
	TIM_TimeBaseStructure.TIM_Prescaler = 0x8c9f;//�û��Զ���
	//TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//TIM_PrescalerConfig(TIM3, 35, TIM_PSCReloadMode_Immediate); 
	
	/* Output Compare Timing Mode configuration: Channel1 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing; //
	//����Ƚ϶�ʱģʽ
	TIM_OCInitStructure.TIM_Pulse = Userdefined_Period3; //
	//�û����嶨ʱ������
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable); //�ر�Ԥ����Ĵ���
	TIM_ITConfig(TIM3, TIM_IT_CC1, ENABLE); // �򿪲���Ƚ��ж�
	
	TIM_Cmd(TIM3, ENABLE);//ʹ��	
}





// �����������壬���¼�ҩ



                                                                     

