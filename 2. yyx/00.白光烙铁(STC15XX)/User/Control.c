/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			Control.c
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

unsigned int IronTemp_Now = 0;//��ǰ�¶�
unsigned int IronTemp_AM = 375;//Ŀ���¶�
unsigned char SetTempFlag = 0;//�趨�¶ȱ�־λ����λ���ú��ɶ�ʱ�����㣬�ɴ�����ʾ�趨�¶�

//NTC�¶ȱ�����Ϊ-10��~50���Ӧ��ADCֵ
 unsigned int code NTCTAB[61] =
{
	324, 318, 312, 306, 300, 294, 288, 282, 276, 269, 263, 257, 251, 245, 238, 232, 226, 220, 214, 209, 
	203, 197, 192, 186, 181, 175, 170, 165, 160, 155, 150, 146, 141, 137, 132, 128, 124, 120, 116, 112, 
	108, 105, 101,  98,  95,  92,  89,  86,  83,  80,  77,  75,  72,  70,  67,  65,  63,  61,  59,  57,
	55 
};


//BEAT����ֵ�˲�����ȥ�����ֵ��Сֵ��ƽ��
static u16 mid_filter(unsigned char channel)
{
	u16 var_buf[NUM] = 0 ;            						//��������
	u16	sum = 0,var_max = 0, var_min = 1023; 			//��ֵ��ֵװ����Сֵװ��ֵ ���ֵװ��
	u8 temp;
	for(temp = 0; temp < NUM; temp++)
	{
	 var_buf[temp] = Get_ADC10bitResult(channel); //���������ʱ
	 if(var_buf[temp] > var_max)   							  //���
		 {
			 var_max = var_buf[temp];
		 }
	 else if(var_buf[temp] < var_min)  					  //��С
		 {
			 var_min = var_buf[temp];
		 }
		sum +=var_buf[temp];  
	}
	sum -= var_max;
	sum -= var_min;																//ȥ����ֵ
	return(sum >> DIV);  													//���ƴ������
}
//�޷��˲������¶����޶����ȷ�Χ�ڲ���ʱ��returnֵ������һ�ε�ֵ����
unsigned int lim_filter(unsigned int data_new,unsigned int lim)
{
	static unsigned int data_old;
	int deta;
	deta = (int)(data_new - data_old);
	if(deta > lim || deta <-lim)
	{
		data_old = data_new;
		return data_new;
	}
	else
	{
		data_old = data_new;
		return data_old;
	}
}

//ADC����ѹ
float GetVoltage(void)
{
	return ((float)Get_ADC10bitResult(3) * (float)FAC_DATA_Table.Adjust_s.Voltage) / 6801.0f;
}

//��������£����ַ����
float GetRoomTemp(void)
{
	u16 temp=0;
	unsigned char det = 60,head = 0,tail = 60,mid = 0;
	
	temp = Get_ADC10bitResult(7);

	//С��-10��ֱ�ӵ���
	if(temp >= NTCTAB[head])
	{
		return -10.0f;
	}
	//����50��ֱ�ӵ���
	else if(temp <= NTCTAB[tail])
	{
		return 50.0f;
	}
	//���򣬶��ַ����
	else
	{
		//ͷ-β ��= 1 ��ѭ��
		while(det > 1)
		{
			//��ֵ
			mid = (head + tail)/2;
			//���ǡ��Ϊ��ֵ ֱ��return
			if(temp == (NTCTAB[mid]))
			{
				return (float)(mid - 10);
			}
			else if(temp > (NTCTAB[mid]))
			{
				tail = mid;
			}
			else
			{
				head = mid;
			}
			det = tail - head;
		}
		return ((float)head-10.0f+1.0f/(float)(NTCTAB[head]-NTCTAB[tail]));
	}
}

//�������¶ȣ����
unsigned int GetIronTemp(unsigned char num)
{
	u16 temp=0;
	unsigned char det = 13,head = 0,tail = 13,mid = 0,i=0;
	
	//���������������24V��ƽ�������е������ĺ����پ����򲻴�����
	Iron_Off();
	
	//��ʱ���ȿն����α������
	//Delay_us(20);
	Get_ADC10bitResult(5); 
	//Get_ADC10bitResult(5);
	temp = lim_filter(mid_filter(5),20);
	//temp = mid_filter(5);
	
	//С��50�淵��0
	if(temp <= FAC_DATA_Table.Heater_Tip[num-1][head])
	{
		return 0;
	}
	//����700��ֱ�ӵ���
	else if(temp < 900 && temp>=FAC_DATA_Table.Heater_Tip[num-1][tail])
	{
		return 700;
	}
	else if(temp > 900)
	{
		return 800;
	}
	//���򣬶��ַ����
	else
	{
		//ͷ-β ��= 1 ��ѭ��
		while(det > 1)
		{
			//��ֵ
			mid = (head + tail) / 2;
			//���ǡ��Ϊ��ֵ ֱ��return
			if(temp == FAC_DATA_Table.Heater_Tip[num-1][mid])
			{
				return mid * 50 + 50;
			}
			else if(temp < (FAC_DATA_Table.Heater_Tip[num-1][mid]))
			{
				tail = mid;
			}
			else
			{
				head = mid;
			}
			det = tail - head;
		}
		//���Բ�ֵ����
		return 50 * (head + 1) + (unsigned int)((float)(temp - FAC_DATA_Table.Heater_Tip[num-1][head]) * 50.0f / (float)(FAC_DATA_Table.Heater_Tip[num-1][tail] - FAC_DATA_Table.Heater_Tip[num-1][head]));
	}
}
//PD���������¶ȣ�P�����ֶ�
void IronTempControl(unsigned int temp)
{
	static float fDelta[2]; //0�ǵ�ǰ���,1���ϴ���
	float fP=0,fD=0;
	float Control_P = 1.0f,Control_D = 0.01f;
	float ControlOut = 0;
	//�����¶�
	if(FtyModeFlag)
	{
		IronTemp_Now = GetIronTemp(FAC_DATA_Table.Adjust_s.Iron);
	}
	else
	{
		IronTemp_Now = GetIronTemp(MENU_DATA_Table.Other_s.Heater_Tip);
	}
	
  //������
  fDelta[0] = (float)temp - (float)IronTemp_Now; 
  
	//P�����ֶ�
	if(fDelta[0] > 40)
	{
		Control_P = 6.5f;
	}
	else if(fDelta[0] > 30)
	{
		Control_P = 5.5f;
	}
	else if(fDelta[0] > 20)
	{
		Control_P = 4.0f;
	}
	else if(fDelta[0] > 10)
	{
		Control_P = 2.5f;
	}
	else if(fDelta[0] > 5)
	{
		Control_P = 1.0f;
	}
  //PD����
  fP = Control_P * fDelta[0];
  fD = Control_D * (fDelta[0] - fDelta[1]);
	
  ControlOut = fP + fD;
  
	//�趨�¶ȴ���50��С������¶Ȳſ�����
	if(temp > 50 && temp <= MENU_DATA_Table.Temp_s.Max_Temp && ((unsigned char)GetVoltage()>=FAC_DATA_Table.Other_s.Lim_Voltage))
	{
		//����255
		if(ControlOut > 255.0f)
		{
			Iron_Run();
			ControlOut = 255.0f;
			UpdatePwm((unsigned char)ControlOut);
		}
		//����
		else if(ControlOut > 0.001f)
		{
			Iron_Run();
			UpdatePwm((unsigned char)ControlOut);
		}
		//����
		else
		{
			ControlOut = 0.0f;
			Iron_Off();
		}
	}
	else
	{
		ControlOut = 0.0f;
		Iron_Off();
	}

	//����
  fDelta[1] = fDelta[0];
	
}

//��ͨģʽ�µļ�ؽ���
void Normal_Monitor(void)
{
	int deta = 0,tmp;
	static unsigned int IronTemp_Display = 0,sum = 0,counter = 0;
	//����ʾ���¶��ٴ��˲�������ʾֵ�ȶ�
	if(counter<50)
	{
		sum += lim_filter(IronTemp_Now,20);
		//sum += IronTemp_Now;
		counter ++;
	}
	else
	{
		IronTemp_Display = sum / (counter-2);
		counter = 0;
		sum = 0;
	}
	deta = (int)IronTemp_Display - (int)IronTemp_AM;
	if(deta<10 && deta>-10)
	{
		IronTemp_Display = IronTemp_AM;
	}
	
	
	if(counter == 1)
	{
		//��ʾ��ѹ
		sprintf((char *)str_buff , "%3.1fV" ,GetVoltage());
		OLED_P8x16Str(0,6,str_buff); 
		//����
		sprintf((char *)str_buff , "%3.0f`C" ,GetRoomTemp());
		OLED_P8x16Str(48,6,str_buff); 
		//����ͷ
		tmp = MENU_DATA_Table.Other_s.Heater_Tip;
		sprintf((char *)str_buff , "%1d" ,tmp);
		OLED_P8x16Str(112,6,str_buff); 
	}


	if(P01)
	{
		OLED_P8x16Str(96,6,"."); 
	}
	else
	{
		OLED_P8x16Str(96,6," "); 
	}
	//�����������¶�
	if(ec11_state.Coder == 1)
	{
		IronTemp_AM +=FAC_DATA_Table.Other_s.Temp_Step;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_Temp = IronTemp_AM;
	}
	else if(ec11_state.Coder == 2)
	{
		IronTemp_AM -=FAC_DATA_Table.Other_s.Temp_Step;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_Temp = IronTemp_AM;
	}
	//�����趨�¶ȷ�Χ
	if(IronTemp_AM > 700)
	{
		IronTemp_AM = 0;
	}
	else if(IronTemp_AM > MENU_DATA_Table.Temp_s.Max_Temp)
	{
		IronTemp_AM = MENU_DATA_Table.Temp_s.Max_Temp;
	}
	else if(IronTemp_AM < 0)
	{
		IronTemp_AM = 0;
	}
	//��ʾ�趨�¶�
	sprintf((char *)str_buff , "�趨:%3u`C      " ,IronTemp_AM);
	OLED_printf(0,0,str_buff,0); 
	
	
	//�趨�¶�ʱ������ʾ�趨���¶�
	if(SetTempFlag)
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_AM);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
	else
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_Display);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
}
unsigned int chn=0;
void Channel_Monitor(void)
{
	int deta = 0,tmp;
	static unsigned int IronTemp_Display = 0,sum = 0,counter = 0;
	//��ʾ�¶��˲�
	if(counter<50)
	{
		sum += lim_filter(IronTemp_Now,25);
		counter ++;
	}
	else
	{
		IronTemp_Display = sum / (counter - 2);
		counter = 0;
		sum = 0;
	}
	deta = (int)IronTemp_Display - (int)IronTemp_AM;
	if(deta<10 && deta>-10)
	{
		IronTemp_Display = IronTemp_AM;
	}
	
	if(counter == 1)
	{
		//��ʾ��ѹ
		sprintf((char *)str_buff , "%3.1fV" ,GetVoltage());
		OLED_P8x16Str(0,6,str_buff); 
		//����
		sprintf((char *)str_buff , "%3.0f`C" ,GetRoomTemp());
		OLED_P8x16Str(48,6,str_buff); 
		//����ͷ
		tmp = MENU_DATA_Table.Other_s.Heater_Tip;
		sprintf((char *)str_buff , "%1d" ,tmp);
		OLED_P8x16Str(112,6,str_buff); 
	}
	
	//��ʾ��������״̬
	if(IronUseFlag)
	{
		OLED_P8x16Str(96,6,"."); 
	}
	else
	{
		OLED_P8x16Str(96,6," "); 
	}
	
	//����������ͨ��
	if(ec11_state.Coder == 1)
	{
		chn ++;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_CH = chn;
	}
	else if(ec11_state.Coder == 2)
	{
		chn --;
		SetTempFlag = 1;
		ec11_state.Coder = 0;
		MENU_DATA_Table.Temp_s.Last_CH = chn;
	}
	//����ͨ����Χ
	if(chn > 100)
	{
		chn = 0;
	}
	if(chn > 5)
	{
		chn = 5;
	}
	else if(chn < 0)
	{
		chn = 0;
	}
	
	//�趨Ŀ���¶�
	switch (chn)
	{
		case 0:
		{
			chn = 0;
			IronTemp_AM = 0;
		}break;
		case 1:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch1;
		}break;
		case 2:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch2;
		}break;
		case 3:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch3;
		}break;
		case 4:
		{
			IronTemp_AM = FAC_DATA_Table.Channel_s.Ch4;
		}break;
		case 5:
		{
			IronTemp_AM = MENU_DATA_Table.Temp_s.Max_Temp;
		}break;
	}
	sprintf((char *)str_buff , "�趨:%3u`C" ,IronTemp_AM);
	OLED_printf(0,0,str_buff,0); 	
	sprintf((char *)str_buff , "ͨ��:%1u" ,chn);
	OLED_printf(78,0,str_buff,0); 

	//�趨�¶�ʱ������ʾ�趨���¶�	
	if(SetTempFlag)
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_AM);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
	else
	{
		if(IronTemp_Display<690)
		{
			sprintf((char *)str_buff , " %3u `C " ,IronTemp_Display);
			OLED_P16x32Temp(0,2,str_buff);	
		}
		else
		{
			OLED_P16x32Temp(0,2," --- `C ");	
		}
	}
}

unsigned int	LastVotage=0;
unsigned char FtyModeFlag = 0;
void Factory_Model(void)
{
	unsigned int temp[9]={0},str=0;
	unsigned char i;
	unsigned char code recoverycode[5] = {0xff,0xff,0xff,0xff,0xff};
	
	IronTemp_AM = 0;
	ShutdownCounterFlag = 1;
	FtyModeFlag = 1;
	
	
	//�ȼ�¼�����ֵ��������������ˣ����Ǿͼ���
	LastVotage = FAC_DATA_Table.Adjust_s.Voltage;


	ParaSet_Fac();


	ec11_state.Key = 0;
	ec11_state.Coder = 0;

	//����У׼
	if(FAC_DATA_Table.Adjust_s.Iron > 0)
	{
		OLED_printf(0,0,"����ͷ %d У׼",FAC_DATA_Table.Adjust_s.Iron);
		OLED_printf(0,1,"ÿ50���϶�У׼",0);
		OLED_printf(0,2,"����ͷһ��!",0);
		while(ec11_state.Key != 1);
		ec11_state.Key = 0;
		OLED_CLS();
		Iron_Run();
		for(i=0;i<=13;i++)
		{
			OLED_printf(0,0,"%d `Cʱ",50+i*50);
			OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron][i]);
			OLED_printf(0,3,"�̰�����������",0);
			
			if(i<=8)
			{
				IronTemp_AM = 50+i*50;
			}
			else
			{
				IronTemp_AM = 0;
			}
			
			while(ec11_state.Key != 1)
			{
				if(ec11_state.Coder == 1)
				{
					FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i] ++;
					OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i]);
					ec11_state.Coder = 0;
				}
				if(ec11_state.Coder == 2)
				{
					FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i] --;
					OLED_printf(0,1,"AD = :%d",FAC_DATA_Table.Heater_Tip[FAC_DATA_Table.Adjust_s.Iron-1][i]);
					ec11_state.Coder = 0;
				}
			}
			ec11_state.Key = 0;
			
		}

		OLED_CLS();
		OLED_printf(0,2,"(��У׼����ͷ%d)",FAC_DATA_Table.Adjust_s.Iron);
		FAC_DATA_Table.Adjust_s.Iron = 0;
		//�洢
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		

	}
	if(FAC_DATA_Table.Other_s.Recovery == 1)
	{
		OLED_printf(0,3,"(�ѻָ���������)",0);
		FAC_DATA_Table.Other_s.Recovery = 0;
		//�洢
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&recoverycode,sizeof(recoverycode));
	}
	
	
	OLED_printf(0,0,"�̰�����У׼",0);
	OLED_printf(0,1,"�����˳�����ģʽ",0);
	while(ec11_state.Key == 0);
	if(ec11_state.Key == 2)
	{
		ec11_state.Key = 0;
		MENU_DATA_Table.Other_s.Work_Mode = 0;
		//�洢
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
		ShutdownCounterFlag = 0;
		IAP_CONTR = 0x20;
	}
}
unsigned char First_Sleep = 1;
void Sys_Monitor()
{
	//������������߻����������ʾ�����Ϣ
	if((IronSleepFlag||IronScrSaverFlag)!=1)
	{
		switch (MENU_DATA_Table.Other_s.Work_Mode)
		{
			case 0:
			{
				Normal_Monitor();
			}break;
			case 1:
			{
				Channel_Monitor();
			}break;
			case 2:
			{
				
				Factory_Model();
				
			}break;
			default:break;
		}
	}
	//���������
	else if(IronSleepFlag)
	{
		OLED_P8x16Str(0,0,"                ");
		OLED_P8x16Str(0,2,"   Sleeping...  ");
		OLED_P8x16Str(0,4,"                ");
		OLED_P8x16Str(0,6,"                ");
	}
	//����
	else if(IronScrSaverFlag)
	{
		OLED_P8x16Str(0,0,"                ");
		OLED_P8x16Str(0,2,"                ");
		OLED_P8x16Str(0,4,"                ");
		OLED_P8x16Str(0,6,"                ");
	}
}
