/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			EC11.c
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"


EC11_State_s	ec11_state;
unsigned char str_buff[20];//����
unsigned char ShutdownCounterFlag;//������˵����߹���ģʽ����λ��1��ֹͣ���߼���������
unsigned char BeepCounterFlag = 0;//������������־λ

//���ô˺����򿪷����������򿪶�ʱ��0���ɶ�ʱ��0���������������ʱ���������������������ʱ������������Ӱ��
void Beep(unsigned int beep)
{
	if(beep>0 && beep<11)
	{
		BeepCounterFlag = 1;
	}
	else
	{
		BeepCounterFlag = 0;
	}
}

//��׽EC11���º���ת��Ϣ
void EC11_Capture(void)
{
	static unsigned char Aold=0,Bold=0;
	static unsigned char RotatingFlag=0;
	static unsigned int KeyDown_Counter=0,KeyDown_Counter_Old=0;

	//���ͬʱΪ�ߵ�ƽ��˵��ת��
	if(EC11_A && EC11_B)
	{
		RotatingFlag = 1;
	}
	//���ת��֮��
	if(RotatingFlag)
	{
		//ͬʱΪ�͵�ƽ���ж��ϴε�ֵ�����ɵó�ת�ķ���
		if(!EC11_A && !EC11_B)
		{
			if(Aold)
			{
				RotatingFlag = 0;
				CoderUseFlag = 1;
				if(!FAC_DATA_Table.Adjust_s.Encoder)
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 2;
				}
				else
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 1;
				}
			}
			if(Bold)
			{
				RotatingFlag = 0;
				CoderUseFlag = 1;
				if(!FAC_DATA_Table.Adjust_s.Encoder)
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 1;
				}
				else
				{
					Beep(MENU_DATA_Table.Other_s.Beep_Volume);
					ec11_state.Coder = 2;
				}
			}
		}
	}
	//�����ϴε�ֵ
	Aold = EC11_A;
	Bold = EC11_B;
	
	//��¼��������ʱ��
	if(!EC11_KEY)
	{
		_nop_();
		if(!EC11_KEY)
		{
			KeyDown_Counter++;
			if(KeyDown_Counter >= KEYHOLD_TIME)
			{
				BEEP_ON;
				Delay_ms(100);
				BEEP_OFF;
			}
		}
	}
	else
	{
		KeyDown_Counter_Old = KeyDown_Counter;
		KeyDown_Counter = 0;
	}

	//���ݰ���ʱ���жϳ����̰�
	if((KeyDown_Counter_Old < KEYHOLD_TIME) && (KeyDown_Counter_Old > KEYDOWN_TIME))
	{
		Beep(MENU_DATA_Table.Other_s.Beep_Volume);
		ec11_state.Key = 1;
		CoderUseFlag = 1;
	}
	else if(KeyDown_Counter_Old >= KEYHOLD_TIME)
	{
		//Beep(MENU_DATA_Table.Other_s.Beep_Volume);
		ec11_state.Key = 2;
		CoderUseFlag = 1;
		KeyDown_Counter_Old = KEYHOLD_TIME;
	}

}


static char Page_num = 0,Option_num = 80 ,option_var;//ҳ�ţ�ѡ���
/* �˵����� */
MENU_MSG_s MENU_MSG = 
{   
		{//ҳ������
			{"�¶�����"},
			{"ʱ������"},
			{"��������"},
		},
		{//TEMP����ѡ��   
			{
				{"����¶�:%d"},   
				{"�����¶�:%d"},
				{"Ĭ���¶�:%d"},
				{"Ĭ��ͨ��:%d"},
			},
			{//TIME����ѡ��
				{"����ʱ��:%d"}, 
				{"����ʱ��:%d"},
				{"����ʱ��:%d"},//����ʱ��
				{0},
			},
			{//OTHER����ѡ��
				{"����ģʽ:%u"},
				{"����ͷѡ��:%u"},
				{"��������:%u"},
				{"����������:%u"},
			},
		}  
};
MENU_DATA_s MENU_DATA_Table = 
{ 
	//Max_Temp   	Slp_Temp   Default_Temp	Default_CH
	{ 410 ,     	0,       		0,					0				},//�¶Ȳ���ֵ
	//Slp_Time  	Scr_Time  	Shd_Time 
	{ 1 ,    			1,					1										},//ʱ�����ֵ
	//Wrk_Mode  	Heater_Tip	Shk_Sens    Bep_Volm
	{ 0,        	1,       		0,					0				},//��������ֵ
};
FAC_DATA_s					FAC_DATA_Table = 
{
	//����ģʽ
	{    
		{//ҳ������
			{"����У׼"},
			{"ͨ������"},
			{"��������"}
		},
		{//TEMP����ѡ��   
			{
				{"����ͷУ׼:%d"},   
				{"��ѹУ׼:%d"},
				{"������У׼:%d"},
				{"�ָ�����:%d"},
			},
			{
				{"ͨ��1:%d"},   
				{"ͨ��2:%d"},
				{"ͨ��3:%d"},
				{"ͨ��4:%d"},
			},
			{
				{"�¶Ȳ���:%d"},   
				{"������ѹ:%d"},
				{"�ָ�����:%d"},
				{0},
			},
		},  
	},
		//����Ϊ����ģʽ����
	//Iron				Voltage   	Encoder			
	{ 0,					244,     		0,									},//У׼����ֵ
	//Ch1					Ch2   			Ch3					Ch4
	{ 200,				300,     		370,     		400			},//ͨ���¶��趨
	//Temp_Step		Lim_Voltage	Recovery
	{ 10,					20,				0,     								},//����
	//����ͷ����
	{
		{
			120,	173,	209,	245,	287,	329,	381,	432,	480,	565,	625,	685,	740,	800,
		},
		{
			118,	148,	184,	213,	264,	303,	350,	401,	480,	565,	625,	685,	740,	800,
		},
		{
			118,	148,	184,	213,	264,	303,	350,	401,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
		{
			120,	148,	186,	214,	255,	304,	340,	390,	480,	565,	625,	685,	740,	800,
		},
	},
};

/* OLED��ӡ */
void OLED_printf(unsigned char x,unsigned char y,char *str,float str_num)
{
	int num_temp = (int)(str_num );
	sprintf((char *)str_buff , str ,num_temp);//��ֵ̬
	OLED_Print(x,2*y,str_buff);     
}
unsigned char Select_flag = 0;
/* ��� */
void Display_cursor(void)
{
	OLED_CLS();
	if(Option_num != 80)//ҳ����
	{
		if(!Select_flag)
		{
			OLED_P8x16Str(104,2*Option_num,"< ");  
		}
		else
		{
			OLED_P8x16Str(104,2*Option_num,"<="); 
		}			
	}
	else								//ѡ����
	{
		OLED_P8x16Str(112,2*Page_num+2,"< ");     
	}
}
/* ��ʾ */
void DisPlay(unsigned char num)
{
	unsigned char j;
	float var_temp;
	
	//��ʾһ���˵�
	if(Option_num == 80)
	{
		OLED_P14x16Str(50,0,"�˵�");
		for(j = 0;j < num;j++)//����
		{
			OLED_printf(0,j+1,MENU_MSG.OPTINO_NAME[j],0);//��ʾֵ
		}
	}
	//��ʾ�����˵�
	else
	{

		switch(Page_num)
		{
			case 0:
			{              
				var_temp = MENU_DATA_Table.Temp_s.Max_Temp;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//��ʾֵ
				var_temp = MENU_DATA_Table.Temp_s.Slp_Temp;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//��ʾֵ    
				var_temp = MENU_DATA_Table.Temp_s.Default_Temp;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//��ʾֵ
				var_temp = MENU_DATA_Table.Temp_s.Default_CH;
				OLED_printf(0,3,MENU_MSG.ADS_str[Page_num][3],var_temp);//��ʾֵ  				
			}break;
			case 1:
			{
				var_temp = MENU_DATA_Table.Time_s.Slp_Time;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//��ʾֵ 
				var_temp = MENU_DATA_Table.Time_s.ScrSaver_Time;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//��ʾֵ 
				var_temp = MENU_DATA_Table.Time_s.Shutdown_Time;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//��ʾֵ  				
			}break;
			case 2:
			{
				var_temp = MENU_DATA_Table.Other_s.Work_Mode;
				OLED_printf(0,0,MENU_MSG.ADS_str[Page_num][0],var_temp);//��ʾֵ 
				var_temp = MENU_DATA_Table.Other_s.Heater_Tip;
				OLED_printf(0,1,MENU_MSG.ADS_str[Page_num][1],var_temp);//��ʾֵ 
				var_temp = MENU_DATA_Table.Other_s.Shock_Sensor;
				OLED_printf(0,2,MENU_MSG.ADS_str[Page_num][2],var_temp);//��ʾֵ 
				var_temp = MENU_DATA_Table.Other_s.Beep_Volume;
				OLED_printf(0,3,MENU_MSG.ADS_str[Page_num][3],var_temp);//��ʾֵ 
				switch (MENU_DATA_Table.Other_s.Work_Mode)
				{
					case 0 :
					{
						OLED_P14x16Str(64,0,"����");  
					}break;
					case 1 :
					{
						OLED_P14x16Str(64,0,"����");  
					}break;
					case 2 :
					{
						OLED_P14x16Str(64,0,"����");  
					}break;
				}
			}break;
			default:;
		}     
	}   
}


/* ����ֵ����ֵ*/
unsigned char set_var()
{ 
	unsigned char var_flag = 1;
	//΢����
	float div_num = 1.0f; //

  /**************************/
  while(var_flag)//��� ����
  {
		if((ec11_state.Key+ec11_state.Coder) > 0)//����������ж���
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 0;
				ec11_state.Key = 0;
				return(0);
			}
			else if(ec11_state.Key == 2)
			{
				Select_flag = 0;				
				ec11_state.Key = 0;				
				var_flag = 0; 
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)//˳ʱ��
			{
				ec11_state.Coder = 0;
				switch(Page_num)//ҳ��
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Temp_s.Max_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Max_Temp >= 700)
									MENU_DATA_Table.Temp_s.Max_Temp = 700;
							}break;
							case 1:
							{
								MENU_DATA_Table.Temp_s.Slp_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Slp_Temp >= MENU_DATA_Table.Temp_s.Max_Temp)
									MENU_DATA_Table.Temp_s.Slp_Temp = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 2:
							{
								MENU_DATA_Table.Temp_s.Default_Temp += FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Default_Temp >= MENU_DATA_Table.Temp_s.Max_Temp)
									MENU_DATA_Table.Temp_s.Default_Temp = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 3:
							{
								MENU_DATA_Table.Temp_s.Default_CH += div_num;
								if(MENU_DATA_Table.Temp_s.Default_CH >= 5)
									MENU_DATA_Table.Temp_s.Default_CH = 5;
							}break;
						default:break;
						}
					}break;
					case 1: 
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Time_s.Slp_Time += div_num;
							} break;
							case 1:
							{
								MENU_DATA_Table.Time_s.ScrSaver_Time += div_num;
							}break;
							case 2:
							{
								MENU_DATA_Table.Time_s.Shutdown_Time += div_num;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Other_s.Work_Mode += 2;
								if(MENU_DATA_Table.Other_s.Work_Mode>2)
								{
									MENU_DATA_Table.Other_s.Work_Mode = 0;
								}
							}break;
							case 1:
							{
								MENU_DATA_Table.Other_s.Heater_Tip += div_num;
								if(MENU_DATA_Table.Other_s.Heater_Tip>8)
								{
									MENU_DATA_Table.Other_s.Heater_Tip = 1;
								}
							}break;
							case 2:
							{
								MENU_DATA_Table.Other_s.Shock_Sensor += div_num;
								if(MENU_DATA_Table.Other_s.Shock_Sensor>10)
								{
									MENU_DATA_Table.Other_s.Shock_Sensor = 0;
								}
							}break;
							case 3:
							{
								MENU_DATA_Table.Other_s.Beep_Volume += div_num;
								if(MENU_DATA_Table.Other_s.Beep_Volume>10)
								{
									MENU_DATA_Table.Other_s.Beep_Volume = 0;
									BEEP_OFF;
								}
							}break;
						default :break;
						}
					}break;
				default :break;
				}//ҳ�ż��    
			} //˳ʱ��
			else if(ec11_state.Coder == 2)//��ʱ��
			{
				ec11_state.Coder = 0;
				switch(Page_num)//ҳ��
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Temp_s.Max_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Max_Temp > 1000)
									MENU_DATA_Table.Temp_s.Max_Temp = 0;
							}break;
							case 1:
							{
								MENU_DATA_Table.Temp_s.Slp_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Slp_Temp > 1000)
									MENU_DATA_Table.Temp_s.Slp_Temp = 0;
							}break;
							case 2:
							{
								MENU_DATA_Table.Temp_s.Default_Temp -= FAC_DATA_Table.Other_s.Temp_Step;
								if(MENU_DATA_Table.Temp_s.Default_Temp > 1000)
									MENU_DATA_Table.Temp_s.Default_Temp = 0;
							}break;
							case 3:
							{
								MENU_DATA_Table.Temp_s.Default_CH -= div_num;
								if(MENU_DATA_Table.Temp_s.Default_CH > 100)
									MENU_DATA_Table.Temp_s.Default_CH = 0;
							}break;
						default:break;
						}
					}break;
					case 1: 
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Time_s.Slp_Time -= div_num;
								if(MENU_DATA_Table.Time_s.Slp_Time > 100)
									MENU_DATA_Table.Time_s.Slp_Time = 0;
							} break;
							case 1:
							{
								MENU_DATA_Table.Time_s.ScrSaver_Time -= div_num;
								if(MENU_DATA_Table.Time_s.ScrSaver_Time > 100)
									MENU_DATA_Table.Time_s.ScrSaver_Time = 0;
							}break;
							case 2:
							{
								MENU_DATA_Table.Time_s.Shutdown_Time -= div_num;
								if(MENU_DATA_Table.Time_s.Shutdown_Time > 100)
									MENU_DATA_Table.Time_s.Shutdown_Time = 0;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								MENU_DATA_Table.Other_s.Work_Mode -= 2;
								if(MENU_DATA_Table.Other_s.Work_Mode>100)
								{
									MENU_DATA_Table.Other_s.Work_Mode = 2;
								}
							}break;
							case 1:
							{
								MENU_DATA_Table.Other_s.Heater_Tip -= div_num;
								if(MENU_DATA_Table.Other_s.Heater_Tip==0)
								{
									MENU_DATA_Table.Other_s.Heater_Tip = 8;
								}
							}break;
							case 2:
							{
								MENU_DATA_Table.Other_s.Shock_Sensor -= div_num;
								if(MENU_DATA_Table.Other_s.Shock_Sensor>100)
								{
									MENU_DATA_Table.Other_s.Shock_Sensor = 10;
								}
							}break;
							case 3:
							{
								MENU_DATA_Table.Other_s.Beep_Volume -= div_num;
								if(MENU_DATA_Table.Other_s.Beep_Volume>100)
								{
									MENU_DATA_Table.Other_s.Beep_Volume = 10;
								}
							}break;
						default :;
						}
					}break;
				default :break;
				}//ҳ�ż��    
			} //��ʱ��
			else
			{
				return(0);
			}
			
			/* ��ʾ���� */
			Display_cursor();
			DisPlay(option_var);
			/* ��ʾ���� */
			
		}//����������ж���
  /**************************/
		return(1);
	}
	return(0);
}

/* ѡ���ѯ */
unsigned char option_check_data()
{
	unsigned char Option_flag = 1;  //Option_flag ѡ���־
	unsigned char first_enter = 1;		//�״ν����־
	switch(Page_num)       //option_var  ѡ��Ԫ�ظ���
	{
		case 0:		//TEMP Ԫ�ظ���
			option_var = 3;
		break;
		case 1:		//TIME Ԫ�ظ���
			option_var = 2;
		break;
		case 2: 	//OTHER Ԫ�ظ��� 
			option_var = 3;
		break;
		default:break;
	}
	Option_num = 0;
	while(Option_flag)//����ѭ���ȴ� ������Ϣ
	{
		if(first_enter)//��һ�ν���ʱֱ����ʾ���棬�Ժ�����в����Ÿ��½���
		{
			Display_cursor();
			DisPlay(Option_num);
			first_enter = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//�������ж���
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 1;
				Display_cursor();
				DisPlay(Option_num);
				ec11_state.Key = 0;
				while(set_var());
				while((ec11_state.Key+ec11_state.Coder) > 0)//����ȷ���˳�
				{
					if(ec11_state.Key == 2)
					{
						return(0);
					}
				}	
			}
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Option_flag = 0;//ѡ���־���� �˳�
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Option_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Option_num;
			}
			else
			{
			}

			/* ѭ��ѡ��� */
			if(Option_num  > option_var)//ѡ��ѭ��
				Option_num = 0;//ѡ���ѭ��
			if(Option_num < 0)//ѡ��ѭ��
				Option_num = option_var;
			 /* ѡ���ѭ�� */
					/* ��ʾ���� */
			Display_cursor();
			DisPlay(Option_num);
		}//�������ж���
		
	}//����ѭ���ȴ� ������Ϣ
  return(1);
}
	unsigned char Page_flag = 1;//ҳ���־
/* ҳ���ѯ */
unsigned char Menu_check_data() 
{              

	unsigned char first_enter = 1;//�״ν���
	
	while(Page_flag)//�ȴ� ɨ��ҳ��
	{
		if(first_enter)
		{
			Display_cursor();
			DisPlay(3);//ҳ�� 
			first_enter = 0;
			ec11_state.Key = 0;
			ec11_state.Coder = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//�Ƿ����ֵ Page
		{
			/* ȷ��ҳ�� */
			if(ec11_state.Key == 1)
			{    /*ˢ��*/
				ec11_state.Key = 0;
				while(option_check_data());//����ѡ���ѯ
			}                         
			/* �˳�ҳ�� */   
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Page_flag = 0;
				return(0);	
			} 
			/* ҳ�żӼ� */
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Page_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Page_num;
			}
			else
			{}
				
			if(Page_num > 2) //ҳ���ѭ�� 
				Page_num = 0;
			if(Page_num < 0)
				Page_num = 2; 

				Display_cursor();
				DisPlay(3);//ҳ�� 			
		}//if() �Ƿ����ֵ Page
	}//while()
	return(1);
}

unsigned int ParaSet()
{
	unsigned char KEY_flag = 1;
	//�������������״̬�µİ���
	if(ec11_state.Key == 1)
	{
		ec11_state.Key = 0;
		if(MENU_DATA_Table.Other_s.Work_Mode == 1)
			MENU_DATA_Table.Other_s.Work_Mode = 0;
		else if(MENU_DATA_Table.Other_s.Work_Mode == 0)
			MENU_DATA_Table.Other_s.Work_Mode = 1;
		//�洢
		EEPROM_SectorErase(0x0);
		EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
	}
	if(ec11_state.Key == 2)
	{
		ShutdownCounterFlag = 1;
		OLED_CLS();//����
		Page_flag = 1;
		while(KEY_flag)
		{
			if(Menu_check_data());
			while(Page_flag == 1);
			OLED_CLS();//����
			//�洢
			EEPROM_SectorErase(0x0);
			EEPROM_write_n(0x0,(unsigned char *)&MENU_DATA_Table,sizeof(MENU_DATA_Table));
			
			KEY_flag = 0;//�˳�ѭ��
		}
		if(MENU_DATA_Table.Other_s.Work_Mode == 2)
		{
			//��ػ�
			IAP_CONTR = 0x20;
		}
	}
	ShutdownCounterFlag = 0;
	return 0;
}


//����Ϊ����ģʽ�˵�
/* ��ʾ */
void DisPlay_Fac(unsigned char num)
{
	unsigned char j;
	float var_temp;
	
	//��ʾһ���˵�
	if(Option_num == 80)
	{
		OLED_P14x16Str(32,0,"����ģʽ");     
		for(j = 0;j < num;j++)//����
		{
			OLED_printf(0,j+1,FAC_DATA_Table.FAC_MSG_s.OPTINO_NAME[j],0);//��ʾֵ
		}
	}
	//��ʾ�����˵�
	else
	{

		switch(Page_num)
		{
			case 0:
			{              
				var_temp = FAC_DATA_Table.Adjust_s.Iron;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//��ʾֵ
				var_temp = FAC_DATA_Table.Adjust_s.Voltage;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//��ʾֵ
				var_temp = FAC_DATA_Table.Adjust_s.Encoder;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//��ʾֵ  				
			}break;
			case 1:
			{              
				var_temp = FAC_DATA_Table.Channel_s.Ch1;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//��ʾֵ
				var_temp = FAC_DATA_Table.Channel_s.Ch2;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//��ʾֵ
				var_temp = FAC_DATA_Table.Channel_s.Ch3;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//��ʾֵ    
				var_temp = FAC_DATA_Table.Channel_s.Ch4;
				OLED_printf(0,3,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][3],var_temp);//��ʾֵ  				
			}break;
			case 2:
			{              
				var_temp = FAC_DATA_Table.Other_s.Temp_Step;
				OLED_printf(0,0,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][0],var_temp);//��ʾֵ 
				var_temp = FAC_DATA_Table.Other_s.Lim_Voltage;
				OLED_printf(0,1,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][1],var_temp);//��ʾֵ 					
				var_temp = FAC_DATA_Table.Other_s.Recovery;
				OLED_printf(0,2,FAC_DATA_Table.FAC_MSG_s.ADS_str[Page_num][2],var_temp);//��ʾֵ 					
			}break;
			default:break;
		}     
	}   
}


/* ����ֵ����ֵ*/
unsigned char set_var_fac()
{ 
	unsigned char var_flag = 1;
	//΢����
	float div_num = 1.0f; //

  /**************************/
  while(var_flag)//��� ����
  {
		if((ec11_state.Key+ec11_state.Coder) > 0)//����������ж���
		{
			if(ec11_state.Key == 1)
			{
				Select_flag = 0;
				ec11_state.Key = 0;
				return(0);
			}
			else if(ec11_state.Key == 2)
			{  
				Select_flag = 0;
				ec11_state.Key = 0;				
				var_flag = 0; 
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)//˳ʱ��
			{
				ec11_state.Coder = 0;
				switch(Page_num)//ҳ��
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Adjust_s.Iron += div_num;
								if(FAC_DATA_Table.Adjust_s.Iron >= 8)
									FAC_DATA_Table.Adjust_s.Iron = 8;
							}break;
							case 1:
							{
								FAC_DATA_Table.Adjust_s.Voltage += div_num;
							}break;
							case 2:
							{
								FAC_DATA_Table.Adjust_s.Encoder += div_num;
								if(FAC_DATA_Table.Adjust_s.Encoder >= 1)
									FAC_DATA_Table.Adjust_s.Encoder = 1;
							}break;
						default:break;
						}
					}break;
					case 1:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Channel_s.Ch1 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch1 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch1 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 1:
							{
								FAC_DATA_Table.Channel_s.Ch2 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch2 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch2 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 2:
							{
								FAC_DATA_Table.Channel_s.Ch3 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch3 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch3 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
							case 3:
							{
								FAC_DATA_Table.Channel_s.Ch4 += FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch4 >= MENU_DATA_Table.Temp_s.Max_Temp)
									FAC_DATA_Table.Channel_s.Ch4 = MENU_DATA_Table.Temp_s.Max_Temp;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Other_s.Temp_Step += div_num;
								if(FAC_DATA_Table.Other_s.Temp_Step > 50)
									FAC_DATA_Table.Other_s.Temp_Step = 50;
							}break;
							case 1:
							{
								FAC_DATA_Table.Other_s.Lim_Voltage += div_num;
								if(FAC_DATA_Table.Other_s.Lim_Voltage > 30)
									FAC_DATA_Table.Other_s.Lim_Voltage = 30;
							}break;
							case 2:
							{
								FAC_DATA_Table.Other_s.Recovery += div_num;
								if(FAC_DATA_Table.Other_s.Recovery >= 1)
									FAC_DATA_Table.Other_s.Recovery = 1;
							}break;
						default:break;
						}
					}break;
				default :break;
				}//ҳ�ż��    
			} //˳ʱ��
			else if(ec11_state.Coder == 2)//��ʱ��
			{
				ec11_state.Coder = 0;
				switch(Page_num)//ҳ��
				{
					case 0:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Adjust_s.Iron -= div_num;
								if(FAC_DATA_Table.Adjust_s.Iron > 100)
									FAC_DATA_Table.Adjust_s.Iron = 0;
							}break;
							case 1:
							{
								FAC_DATA_Table.Adjust_s.Voltage -= div_num;
								if(FAC_DATA_Table.Adjust_s.Voltage >= 1000)
									FAC_DATA_Table.Adjust_s.Voltage = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Adjust_s.Encoder -= div_num;
								if(FAC_DATA_Table.Adjust_s.Encoder >= 100)
									FAC_DATA_Table.Adjust_s.Encoder = 0;
							}break;
						default:break;
						}
					}break;
					case 1:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Channel_s.Ch1 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch1 >= 1000)
									FAC_DATA_Table.Channel_s.Ch1 = 0;
							}break;
							case 1:
							{
								FAC_DATA_Table.Channel_s.Ch2 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch2 >= 1000)
									FAC_DATA_Table.Channel_s.Ch2 = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Channel_s.Ch3 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch3 >= 1000)
									FAC_DATA_Table.Channel_s.Ch3 = 0;
							}break;
							case 3:
							{
								FAC_DATA_Table.Channel_s.Ch4 -= FAC_DATA_Table.Other_s.Temp_Step;
								if(FAC_DATA_Table.Channel_s.Ch4 >= 1000)
									FAC_DATA_Table.Channel_s.Ch4 = 0;
							}break;
						default:break;
						}
					}break;
					case 2:
					{
						switch(Option_num)
						{
							case 0:
							{
								FAC_DATA_Table.Other_s.Temp_Step -= div_num;
								if(FAC_DATA_Table.Other_s.Temp_Step == 0)
									FAC_DATA_Table.Other_s.Temp_Step = 1;
							}break;
							case 1:
							{
								FAC_DATA_Table.Other_s.Lim_Voltage -= div_num;
								if(FAC_DATA_Table.Other_s.Lim_Voltage >= 100)
									FAC_DATA_Table.Other_s.Lim_Voltage = 0;
							}break;
							case 2:
							{
								FAC_DATA_Table.Other_s.Recovery -= div_num;
								if(FAC_DATA_Table.Other_s.Recovery >= 100)
									FAC_DATA_Table.Other_s.Recovery = 0;
							}break;
						default:break;
						}
					}break;
				default :break;
				}//ҳ�ż��    
			} //��ʱ��
			else
			{
				return(0);
			}
			
			/* ��ʾ���� */
			Display_cursor();
			DisPlay_Fac(option_var);
			/* ��ʾ���� */
			
		}//����������ж���
  /**************************/
		return(1);
	}
	return(0);
}

/* ѡ���ѯ */
unsigned char option_check_data_fac()
{
	unsigned char Option_flag = 1;  //Option_flag ѡ���־
	unsigned char first_enter = 1;		//�״ν����־
	switch(Page_num)       //option_var  ѡ��Ԫ�ظ���
	{
		case 0:		//���� Ԫ�ظ���
			option_var = 2;
		break;
		case 1:		//ͨ�� Ԫ�ظ���
			option_var = 3;
		break;
		case 2:		//ͨ�� Ԫ�ظ���
			option_var = 2;
		break;
		default:break;
	}
	Option_num = 0;
	while(Option_flag)//����ѭ���ȴ� ������Ϣ
	{
		if(first_enter)//��һ�ν���ʱֱ����ʾ���棬�Ժ�����в����Ÿ��½���
		{
			Display_cursor();
			DisPlay_Fac(Option_num);
			first_enter = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//�������ж���
		{
			if(ec11_state.Key == 1)
			{
				ec11_state.Key = 0;
				Select_flag = 1;
				Display_cursor();
				DisPlay_Fac(option_var);
				while(set_var_fac());
				while((ec11_state.Key+ec11_state.Coder) > 0)//����ȷ���˳�
				{
					if(ec11_state.Key == 2)
					{
						return(0);
					}
				}	
			}
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Option_flag = 0;//ѡ���־���� �˳�
				Option_num = 80;
				return(0);
			}
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Option_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Option_num;
			}
			else
			{
			}

			/* ѭ��ѡ��� */
			if(Option_num  > option_var)//ѡ��ѭ��
				Option_num = 0;//ѡ���ѭ��
			if(Option_num < 0)//ѡ��ѭ��
				Option_num = option_var;
			 /* ѡ���ѭ�� */
					/* ��ʾ���� */
			Display_cursor();
			DisPlay_Fac(Option_num);
		}//�������ж���
		
	}//����ѭ���ȴ� ������Ϣ
  return(1);
}
/* ҳ���ѯ */
unsigned char Fac_check_data() 
{              

	unsigned char first_enter = 1;//�״ν���
	
	while(Page_flag)//�ȴ� ɨ��ҳ��
	{
		if(first_enter)
		{
			Display_cursor();
			DisPlay_Fac(3);//ҳ�� 
			first_enter = 0;
			ec11_state.Key = 0;
			ec11_state.Coder = 0;
		}
		if((ec11_state.Key+ec11_state.Coder) > 0)//�Ƿ����ֵ Page
		{
			/* ȷ��ҳ�� */
			if(ec11_state.Key == 1)
			{    /*ˢ��*/
				ec11_state.Key = 0;
				while(option_check_data_fac());//����ѡ���ѯ
			}                         
			/* �˳�ҳ�� */   
			else if(ec11_state.Key == 2)
			{
				ec11_state.Key = 0;
				Page_flag = 0;
				return(0);	
			} 
			/* ҳ�żӼ� */
			else if(ec11_state.Coder == 1)
			{
				ec11_state.Coder = 0;
				++Page_num;
			}
			else if(ec11_state.Coder == 2)
			{
				ec11_state.Coder = 0;
				--Page_num;
			}
			else
			{}
				
			if(Page_num > 2) //ҳ���ѭ�� 
				Page_num = 0;
			if(Page_num < 0)
				Page_num = 2; 

				Display_cursor();
				DisPlay_Fac(3);//ҳ�� 			
		}//if() �Ƿ����ֵ Page
	}//while()
	return(1);
}

unsigned int ParaSet_Fac()
{
	unsigned char KEY_flag = 1;
	
	OLED_CLS();//����
	Page_flag = 1;
	while(KEY_flag)
	{
		if(Fac_check_data());
		while(Page_flag == 1);
		OLED_CLS();//����
		
		//�洢
		EEPROM_SectorErase(0x200);
		EEPROM_write_n(0x200,(unsigned char *)&FAC_DATA_Table,sizeof(FAC_DATA_Table));	
		
		KEY_flag = 0;//�˳�ѭ��
	}

	return 0;
}