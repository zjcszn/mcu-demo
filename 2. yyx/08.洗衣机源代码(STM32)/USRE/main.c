#include"led.h"
#include"key.h"
#include"delay.h"
#include"LCD12864.H"
#include"PWM.H"
#include"TIMER2.H"
#include<stdint.h>
#include<string.h>
#include"shui.h"
#include"MOTOR.H"
#include"HONG.H"
#include"BEEP.H"
#define mode_piaoxi

u8 i;//��������
u8 MODE_WORK=0;
u8 TIM2_5ms_ticks;
u8 TIM2_1s_ticks;
u8 cuuent_mode;

struct 
{
	u8 shuiwei_youxiao:1;
	u8 shuiwei_wuxiao:1;
}SHUI_BIT;


#define LEV_youxiao SHUI_BIT.shuiwei_youxiao


u8 mode0 =0;  //�����ʱ����ģʽ��
u8 flag_MODESTOP=0;  //������ģʽλ [0:2] 6��ϴ��ģʽ [3:4]  8��״̬��Ϣ[5:8]
char *const Dis_str1[]={"��׼ϴ��","����ϴ��","����ϴ��","����ϴ��","��ëϴ��","����ϴ��"};
char *const Dis_str2[]={"��","��","��"};
char *const Dis_str_xidi="��ѡ��ϴ��ģʽ��";  //mode1 dis3
char *const Dis_str_dawe="��  ѡ��λ  ��";  //mode2 dis4
char *const Dis_str_jish="�����ڽ�ˮ....��";  //mode3 dis6
char *const Dis_str_runi="������ϴ��....��";  //mode4 dis7
char *const Dis_str_pais="��������ˮ....��";  //mode5 dis8
char *const Dis_str_tuos="��������ˮ....��";  //mode4 dis7
char *const Dis_str_piao="������Ưϴ....��";  //mode4 dis7
char *const Dis_str_susp="��ϴ����ͣ��..��";  //mode6 dis9
char *const Dis_str_daij="��  ������    ��";  //mode0 dis10
char *const Dis_str_star="��  �Ƿ�ʼ? ��"; 
char *const Dis_str_xyco="��  ϴ�����  ��"; 

struct  TIME_S
{
	u8 Time_jinshui_s;  //��ˮ�͵�λ
	u8 Time_jinshui_z;  //��ˮ�е�λ
	u8 Time_jinshui_g;  //��ˮ�ߵ�λ
	u8 Time_paishui_s;  //��ˮ�͵�λ
	u8 Time_paishui_z;  //��ˮ�е�λ
	u8 Time_paishui_g;  //��ˮ�ߵ�λ
};
const struct  TIME_S TIM_shui=
{
	.Time_jinshui_s=125-43,
	.Time_jinshui_z=150-36,
	.Time_jinshui_g=160,
	.Time_paishui_s=125-39,  
	.Time_paishui_z=150-34,
	.Time_paishui_g=160,
};

struct
{
	u8 po_tick1;
}Pointer_count;

struct
{
	u8 Nu_tick1;  //��ˮ
	u16 nu_tick2; //ϴ��
	u8 Nu_tick3;  //��ˮ
	u16 Nu_tick5;  //Ưϴ
	u16 Nu_tick7;   //��ˮ
	u8 Nu_tick8;   //���
	u16 Nu_tick9;
}Num_count;

char *Num_str=NULL;
	
struct
{
	u8 f_tick_1s:1;
	u8 count_enable:1;
	u8 cishu_piaoxi:1;  //=1ʱ����Ưϴ��0ʱΪһ��
	u8 tuoshui_Enable:1; //��ˮģʽʹ��
	u8 xidi_Enable:1;    //ϴ��ģʽʹ��
	u8 piaoxi_Enable:1;  //Ưϴʹ��
}SY_BOOL;


typedef void (*motor_run_sta)(void);

motor_run_sta EVENT_CB[4]={tingzhi,zhengzhuan,zhengzhuan2,fanzhuan};
motor_run_sta CUUENT_CB = tingzhi;
motor_run_sta CBB=NULL;

typedef struct
{
	u8 ticks;
	u8 cuu_sta;
	u8 state_stop_time[4];  //����
	u8 stop_time;
	u8 flag_stop;
	u8 motor_state[4];
}WORD_STATE;

WORD_STATE WORK_xidi={.cuu_sta=0,.motor_state={1,0,3,0},.stop_time=28,.state_stop_time={10,14,24,28}};
WORD_STATE WORK_pixi={.cuu_sta=0,.motor_state={1,0,3,0},.stop_time=56,.state_stop_time={20,28,48,56}};
WORD_STATE WORK_tuos={.cuu_sta=0,.motor_state={2},.stop_time=15,.state_stop_time={15,15,15,15}};

void Shui_jiance()
{
	switch(mode0)
	{
		case 4:
		if(((flag_MODESTOP&0x18)>>3)==0)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao))   //SHUI_A �ں� SHUI_B����ˮʱ
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==1)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)) //��A,B,Cˮλ����⵽ˮʱ��˵��ˮ��ע��
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				  Jinshui();
					LED_Jin=0;
					LED_Pai=1;
					LED_Tuo=1;
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==2)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)&&(SHUI_D==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
					LED_Jin=0;
					LED_Pai=1;
					LED_Tuo=1;
			}
		}
		break;
		case 6:
			if(SHUI_A!=LEV_youxiao)//1
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;				
			}
			else
			{
				Chushui();
				LED_Jin=1;
				LED_Pai=0;
				LED_Tuo=1;				
			}
			break;
		case 7:
		if(((flag_MODESTOP&0x18)>>3)==0)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)) 
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;				
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;				
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==1)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;				
			}
		}
		else
		if(((flag_MODESTOP&0x18)>>3)==2)
		{
			if((SHUI_A==LEV_youxiao)&&(SHUI_B==LEV_youxiao)&&(SHUI_C==LEV_youxiao)&&(SHUI_D==LEV_youxiao)) 
			{
					Tingshui();
					LED_Jin=1;
					LED_Pai=1;
					LED_Tuo=1;				
			}
			else
			{
				Jinshui();
				LED_Jin=0;
				LED_Pai=1;
				LED_Tuo=1;			
			}
		}
		break;	
		case 9:
			LED_Tuo=0;
			LED_Jin=1;
			LED_Pai=1;
			Chushui();
			break;	
		case 10:
			if(SHUI_A!=LEV_youxiao)
			{
				Tingshui();
				LED_Jin=1;
				LED_Pai=1;
				LED_Tuo=1;			
			}
			else
			{
				Chushui();
				LED_Jin=1;
				LED_Pai=0;
				LED_Tuo=1;				
			}
			break;	
		default:
			Tingshui();
			LED_Jin=1;
			LED_Pai=1;
			LED_Tuo=1;	
			break;
	}
}


void Inlet_Init()  //��ˮ
{
	switch((flag_MODESTOP&0x18)>>3)
	{
		case 0:Num_count.Nu_tick1=TIM_shui.Time_jinshui_s;break;
		case 1:Num_count.Nu_tick1=TIM_shui.Time_jinshui_z;break;
		case 2:Num_count.Nu_tick1=TIM_shui.Time_jinshui_g;break;
		default:break;
	}	
}

void drain_Init()  //��ˮ��س�ʼ��
{
	switch((flag_MODESTOP&0x18)>>3)
	{
		case 0:Num_count.Nu_tick3=TIM_shui.Time_paishui_s;break;
		case 1:Num_count.Nu_tick3=TIM_shui.Time_paishui_z;break;
		case 2:Num_count.Nu_tick3=TIM_shui.Time_paishui_g;break;
		default:break;
	}	
}

void Wash_Init() //ϴ����س�ʼ��
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=12*60; break;
		case 1:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=1.5*60; break;
		case 2:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=10*60; break;
		case 3:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=15*60; break;
		case 4:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=15*60; break;		
		case 5:SY_BOOL.xidi_Enable=1;Num_count.nu_tick2=8*60; break;
		default:break;
	}
	WORK_xidi.ticks=0;	//ˢ��ticks
	WORK_xidi.cuu_sta=0;
}
void Wash_Stop()
{
	SY_BOOL.xidi_Enable=0;
	WORK_xidi.ticks=0;       //ˢ��ticks
}
void Rinse_Init()
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 1:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 2:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 3:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 4:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		case 5:SY_BOOL.piaoxi_Enable=1;Num_count.Nu_tick5=56; break;
		default:break;
	}
	WORK_pixi.ticks=0;	//ˢ��ticks
	WORK_pixi.cuu_sta=0;
}

void Rinse_Init_2()
{
	switch((flag_MODESTOP&0x07))
	{
		case 0:SY_BOOL.cishu_piaoxi=1;break;
		case 1:SY_BOOL.cishu_piaoxi=0;break;
		case 2:SY_BOOL.cishu_piaoxi=1;break;
		case 3:SY_BOOL.cishu_piaoxi=1;break;	
		case 4:SY_BOOL.cishu_piaoxi=1;break;
		case 5:SY_BOOL.cishu_piaoxi=1;break;
		default:break;
	}	
}

void Rinse_Stop()
{
	SY_BOOL.piaoxi_Enable=0;
}
void Tuoshui_Init()  //��ˮ��س�ʼ��
{
	switch(flag_MODESTOP&0x07)
	{
		case 0:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=6*60; break;
		case 1:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=3*60; break;
		case 2:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=3*60; break;
		case 3:SY_BOOL.tuoshui_Enable=1; Num_count.Nu_tick7=7*60; break;
		case 4:SY_BOOL.tuoshui_Enable=0; Num_count.Nu_tick7=0*60; break;		
		case 5:SY_BOOL.tuoshui_Enable=0; Num_count.Nu_tick7=0*60; break;
		default:break;
	}
	WORK_tuos.ticks=0;	//ˢ��ticks
	WORK_tuos.cuu_sta=0;
}

void Tuoshui_Stop()
{
	SY_BOOL.tuoshui_Enable=0;  //��ֹϴ��
}

void display0()  // await 
{
	Lcd12864_WriAdd(0,1);
	Lcd12864_PriStr("STM32");
	Lcd12864_WriAdd(0,4);
	Lcd12864_PriStr("ϴ�»�");
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("                ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_daij);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            ��ʼ");
	Lcd12864_SetWhite(3,6,7);
}

void display1()  //wash mode seclection
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr(" [");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr("] ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_xidi);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("ģʽ  ��λ  ȷ��");
	Lcd12864_SetWhite(3,0,1);
	Lcd12864_SetWhite(3,6,7);
}

void display2()  //water level mode seclection
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr(" [");
	Lcd12864_WriAdd(1,7);
	Lcd12864_PriStr("] ");
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_dawe);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("ģʽ  ��λ  ȷ��");
	Lcd12864_SetWhite(3,6,7);
	Lcd12864_SetWhite(3,3,4);
}
 

void display3()  //isstart interface
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_WriAdd(1,0);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,5);
	Lcd12864_PriStr("  ");
	Lcd12864_WriAdd(1,7);
	Lcd12864_PriStr("  ");
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_star);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("ģʽ  ��λ  ��ʼ");
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_SetWhite(3,6,7);
}

void display4()  //��ˮ����
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_jish);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("                ");
}

void display5()  //runing interface
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_runi);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            ��ͣ");
	Lcd12864_SetWhite(3,6,7);
}

void display6()  //��ˮ����
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_pais);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("                ");
}

void display7()  //rinse 
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_piao);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            ��ͣ");
	Lcd12864_SetWhite(3,6,7);
}

void display8()  //tuoshui
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_tuos);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            ��ͣ");
	Lcd12864_SetWhite(3,6,7);
}

void display9()  //wash complish
{
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_xyco);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("            ȷ��");
	Lcd12864_SetWhite(3,6,7);
}

void display10()  //suspend
{
	Lcd12864_WriAdd(1,1);
	Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
	Lcd12864_WriAdd(1,6);
	Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);
	Lcd12864_WriAdd(2,0);
	Lcd12864_PriStr(Dis_str_susp);
	Lcd12864_ClrWhite(3,0,7);
	Lcd12864_WriAdd(3,0);
	Lcd12864_PriStr("        ���¿�ʼ");
	Lcd12864_SetWhite(3,4,7);
}

void display11()
{
	Lcd12864_WriAdd(2,0);	
	Lcd12864_PriStr("��  �쳣����  ��"); 
	Lcd12864_WriAdd(3,6);	
	Lcd12864_PriStr("��ʼ");
	Lcd12864_SetWhite(3,6,7);
}

void Calc_tick9()
{
	Num_count.Nu_tick9=0;
	switch((flag_MODESTOP&0x07))
	{
		case 0:Num_count.Nu_tick9+=(12*60+56*2+6*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	
		break;
		case 1:Num_count.Nu_tick9+=(1.5*60+56+3*60+5); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*2+TIM_shui.Time_paishui_s*1;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*2+TIM_shui.Time_paishui_z*1;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*2+TIM_shui.Time_paishui_g*1;break;
							default:break;
						}	break;
		case 2:Num_count.Nu_tick9+=(10*60+56*2+3*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	break;
		case 3:Num_count.Nu_tick9+=(15*60+56*2+7*60+8); 
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*2;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*2;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*2;break;
							default:break;
						}	break;					
		case 4:Num_count.Nu_tick9+=(15*60+56*2+8);
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*3;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*3;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*3;break;
							default:break;
						}	break;
		case 5:Num_count.Nu_tick9+=(8*60+56*2+8);
						switch((flag_MODESTOP&0x18)>>3)
						{
							case 0:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_s*3+TIM_shui.Time_paishui_s*3;break;
							case 1:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_z*3+TIM_shui.Time_paishui_z*3;break;
							case 2:Num_count.Nu_tick9+=TIM_shui.Time_jinshui_g*3+TIM_shui.Time_paishui_g*3;break;
							default:break;
						}	break;
		default:break;
	}
}

void key1_Hander(void)
{
	u8 temp=0;
	switch(mode0)
	{
		case 1:
			{
				temp=flag_MODESTOP&0x07;
				temp++;
				if(temp>5)
				{
					temp=0;
				}
				flag_MODESTOP=(flag_MODESTOP&0xf8)|(temp&0x07);
			}
			break; //ϴ��ģʽѡ�����
		default:break;
	}
}

void key2_Hander(void)
{
	u8 temp=0;
	switch(mode0)
	{
		case 1:break; //ϴ�ӽ���
		case 2:
		{
			temp=(flag_MODESTOP&0x18)>>3;
			temp++;
			if(temp>2)
				temp=0;
			flag_MODESTOP=(flag_MODESTOP&0xe7)|((temp<<3)&0x18);
		}break; //��λѡ�����
		case 5:Num_count.nu_tick2=Num_count.nu_tick2-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		case 8:Num_count.Nu_tick5=Num_count.Nu_tick5-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		case 9:Num_count.Nu_tick7=Num_count.Nu_tick7-40;Num_count.Nu_tick9=Num_count.Nu_tick9-40;break;
		default:break;
	}
}
void key3_Hander(void)
{
	switch(mode0)
	{
		case 0:mode0=1;display1();break; //��������
		case 1:mode0=2;display2();break; //ϴ�ӽ���
		case 2:mode0=3;display3();break; //ģʽѡ�����
		case 3:
			mode0=4;
			Rinse_Init_2();
			Inlet_Init();
			Calc_tick9();//�����ܼ�ʱ���̡�
			SY_BOOL.count_enable=1;
			display4();break; //��Ҫ�����ˮ����
		case 4:break;
		case 5: //ϴ�ӽ���->��ͣ����
			SY_BOOL.count_enable=0;
			mode0=12;
			display10();
			break;
		case 6:break; //��ˮ����
		case 7:break; //��ˮ����
		case 8:       //Ưϴ����->��ͣ����
			mode0=13;
			SY_BOOL.count_enable=0;
			display10();
			break; 
		case 9:break; //��ˮ����
		case 10:  		//��ˮ����->��ͣ����3
			mode0=14;
			display10();
			SY_BOOL.count_enable=0;
			break; 
		case 11:mode0=0;display0();break;  //��ɽ���->��������
		case 12:       //��ͣ����1->ϴ�ӽ���
			SY_BOOL.count_enable=1;
			mode0=5;
			display5();
			break;  
		case 13:       //��ͣ����2->Ưϴ����
			SY_BOOL.count_enable=1;
			mode0=8;
			display7();
			break;  
		case 14:       //��ͣ����3->��ˮ����
			SY_BOOL.count_enable=1;
			mode0=10;
			display8();
			break;  
		case 15:
			if(Gaizi()==0)  //���Ӻ���
			{
				mode0=cuuent_mode;
				SY_BOOL.count_enable=1;
				beep=1;
				switch(mode0)
				{
					case 5:display5();break;
					case 8:display7();break;
					case 9:display8();break;
					case 12:
					case 13:
					case 14:display10();break;
					default:break;
				}
			}break;
		default:break;
	}
}

void display()
{
	switch(mode0)
	{
		case 0: display0(); break;
		case 1: display1(); break;
		case 2: display2(); break;
		case 3: display3(); break;
		case 4: display4(); break;
		case 5: display5(); break;
		case 6: display6(); break;
		case 7: display4(); break;
		case 8: display7();break;
		case 9: display6();break;
		case 10:display8();break;
		case 11:display9();break;
		case 12:display10();break;
		case 13:display10();break;
		case 14:display10();break;
	}
}

void Motor_Run()
{
	if(CBB!=CUUENT_CB)
	{
		CBB=CUUENT_CB;
		if(*CBB==zhengzhuan)
			zhengzhuan();
		else if (*CBB==fanzhuan)
			fanzhuan();
		else if (*CBB==tingzhi)
			tingzhi();
		else if(*CBB==zhengzhuan2)
			zhengzhuan2();
	}
}

void Active_action()
{
	switch(mode0)
	{
		case 5:
			if(SY_BOOL.xidi_Enable==0)//�ж�ϴ��ʱ���Ƿ����,0=����
			{
				
			}
			else  //û����ִ��ϴ�Ӳ���
			{
				if((WORK_xidi.ticks)>=WORK_xidi.stop_time)  //��ǰģʽ����
				{
					WORK_xidi.flag_stop=1; 	//���ǰ״ֹ̬ͣ��־
					WORK_xidi.ticks=0;       //ˢ��ticks
					WORK_xidi.cuu_sta=0;     //����״̬��ʼ��Ϊ0
				}else if((WORK_xidi.ticks)>=WORK_xidi.state_stop_time[WORK_xidi.cuu_sta])   //��ǰ״̬����
				{
					WORK_xidi.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
					WORK_xidi.cuu_sta++;     //����״̬+1
					CUUENT_CB=EVENT_CB[WORK_xidi.motor_state[WORK_xidi.cuu_sta]];
				}else                /*û�дﵽ���ı�״̬��ʱ��*/
				{
					WORK_xidi.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
					 CUUENT_CB=EVENT_CB[WORK_xidi.motor_state[WORK_xidi.cuu_sta]];
				}
				WORK_xidi.ticks++;
			}break;
			case 8:  //Ưϴ  
					if(SY_BOOL.piaoxi_Enable==0) //�ж��Ƿ���Խ�����һ��״̬
					{

					}
					else
					{
						if(((WORK_pixi.ticks)++)>=WORK_pixi.stop_time)  //��ǰģʽ����
						{
							WORK_pixi.flag_stop=1; 	//���ǰ״ֹ̬ͣ��־
							WORK_pixi.ticks=0;       //ˢ��ticks
							WORK_pixi.cuu_sta=0;     //����״̬��ʼ��Ϊ0
						}else if((WORK_pixi.ticks)>=WORK_pixi.state_stop_time[WORK_pixi.cuu_sta])   //��ǰ״̬����
						{
							WORK_pixi.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
							WORK_pixi.cuu_sta++;     //����״̬+1
							CUUENT_CB=EVENT_CB[WORK_pixi.motor_state[WORK_pixi.cuu_sta]];
						}else                /*û�дﵽ���ı�״̬��ʱ��*/
						{
							WORK_pixi.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
							CUUENT_CB=EVENT_CB[WORK_pixi.motor_state[WORK_pixi.cuu_sta]];
						}
					}break;
			case 9:	 //��ˮ 
				if(SY_BOOL.tuoshui_Enable==0) //�ж��Ƿ���Խ�����һ������
				{

				}
				else
				{
					if((WORK_tuos.ticks)>=WORK_tuos.stop_time)  //��ǰģʽ����
					{
						WORK_tuos.flag_stop=1; 	//���ǰ״ֹ̬ͣ��־
						WORK_tuos.ticks=0;       //ˢ��ticks
						WORK_tuos.cuu_sta=0;     //����״̬��ʼ��Ϊ0
					}else if((WORK_pixi.ticks)>=WORK_pixi.state_stop_time[WORK_tuos.cuu_sta])   //��ǰ״̬����
					{
						WORK_tuos.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
						WORK_tuos.cuu_sta++;     //����״̬+1
					}else                /*û�дﵽ���ı�״̬��ʱ��*/
					{
						WORK_tuos.flag_stop=0; 	//���ǰ״ֹ̬ͣ��zhi
						CUUENT_CB=EVENT_CB[WORK_tuos.motor_state[WORK_tuos.cuu_sta]];
					}
					WORK_tuos.ticks++;
				}break;
			default:CUUENT_CB=EVENT_CB[0];break;
			}
		}

void key_scan(void)
{
	static u8 key_state=1;
	u8 keytemp=0;
	if(KEY1==0)
		keytemp|=0x01;
	else if(KEY2==0)
		keytemp|=0x02;
	else if (KEY3==0)
		keytemp|=0x04;
	switch(key_state)
	{
		case 1: if(keytemp!=0x00) key_state=2;break;
		case 2: if(keytemp==0x00) key_state=1;
						else 
						{
							switch(keytemp)
							{
								case 0x01:key1_Hander();break;
								case 0x02:key2_Hander();break;
								case 0x04:key3_Hander();break;
								default:break;
							}
							key_state=3;
						}break;
		case 3: if(keytemp==0x00) key_state=1;break;
	}
}

void pointer_action()
{
	SY_BOOL.f_tick_1s=0;
	switch(mode0)
	{
		case 0:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
				
		case 4:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 5:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
 				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 6:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 7:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 8:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 9:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		case 10:
				Lcd12864_WriAdd(2,5);
				Lcd12864_PriStr("    ");
				Lcd12864_WriAdd(2,5);
				for(i=0;i<Pointer_count.po_tick1;i++)
					Lcd12864_PriAsc('.');
				Pointer_count.po_tick1++;
				if(Pointer_count.po_tick1>=5)
					Pointer_count.po_tick1=1;
				break;
		default:break;
	}
}

char* Ass_itos(u16 num)  //����������ֱ���ַ���
{
	char *num_str=NULL;
	if(num>60)
	{
		num_str[0]=(num/60)/10+0x30;
		num_str[1]=(num/60)%10+0x30;
		num_str[2]='m';
		num_str[3]='\0';
	}
	else
	{
		num_str[0]=num/10+0x30;
		num_str[1]=num%10+0x30;
		num_str[2]='s';
		num_str[3]='\0';
	}
	return num_str;	
}

void display_action()
{
	char str[10];
	char str1[6];
	char str2[16];
	switch(mode0)
	{
		case 4:  //��ˮ
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick1>60)
			{
				str2[6]=(Num_count.Nu_tick1/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick1/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick1/10+0x30;
				str2[7]=Num_count.Nu_tick1%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 5:  //ϴ��
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.nu_tick2>60)
			{
				str[6]=(Num_count.nu_tick2/60)/10+0x30;
				str[7]=(Num_count.nu_tick2/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.nu_tick2/10+0x30;
				str[7]=Num_count.nu_tick2%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 6: //��ˮ
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick3>60)
			{
				str2[6]=(Num_count.Nu_tick3/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick3/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick3/10+0x30;
				str2[7]=Num_count.Nu_tick3%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 7:
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick1 >60)
			{
				str2[6]=(Num_count.Nu_tick1/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick1/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick1/10+0x30;
				str2[7]=Num_count.Nu_tick1%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			}  
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 8:
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.Nu_tick5>60)
			{
				str[6]=(Num_count.Nu_tick5/60)/10+0x30;
				str[7]=(Num_count.Nu_tick5/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.Nu_tick5/10+0x30;
				str[7]=Num_count.Nu_tick5%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 9:
			if(Num_count.Nu_tick9>60)
			{
				str[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str[2]='m';
				str[3]=' ';
			}
			else
			{
				str[0]=Num_count.Nu_tick9/10+0x30;
				str[1]=Num_count.Nu_tick9%10+0x30;
				str[2]='s';
				str[3]=' ';
			}
			str[4]=' ';
			str[5]=' ';
			if(Num_count.Nu_tick7>60)
			{
				str[6]=(Num_count.Nu_tick7/60)/10+0x30;
				str[7]=(Num_count.Nu_tick7/60)%10+0x30;
				str[8]='m';
				str[9]='\0';
			}
			else
			{
				str[6]=Num_count.Nu_tick7/10+0x30;
				str[7]=Num_count.Nu_tick7%10+0x30;
				str[8]='s';
				str[9]='\0';
			}
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str);
			break;
		case 10:
			if(Num_count.Nu_tick9>60)
			{
				str2[0]=(Num_count.Nu_tick9/60)/10+0x30;
				str2[1]=(Num_count.Nu_tick9/60)%10+0x30;
				str2[2]='m';
				str2[3]=' ';
			}
			else
			{
				str2[0]=Num_count.Nu_tick9/10+0x30;
				str2[1]=Num_count.Nu_tick9%10+0x30;
				str2[2]='s';
				str2[3]=' ';
			}
			str2[4]=' ';
			str2[5]=' ';
			if(Num_count.Nu_tick3>60)
			{
				str2[6]=(Num_count.Nu_tick3/60)/10+0x30;
				str2[7]=(Num_count.Nu_tick3/60)%10+0x30;
				str2[8]='m';
				str2[9]='\0';
			}
			else
			{
				str2[6]=Num_count.Nu_tick3/10+0x30;
				str2[7]=Num_count.Nu_tick3%10+0x30;
				str2[8]='s';
				str2[9]='\0';
			} 
			str2[10]=' ';
			str2[11]=' ';
			str2[12]=' ';
			str2[13]=' ';
			str2[14]=' ';
			str2[15]=' ';
			Lcd12864_WriAdd(3,0);
			Lcd12864_PriStr(str2);
			break;
		case 11:
			str1[0]=(Num_count.Nu_tick8%100)%10+0x30;
			str1[1]='s';
		  str1[2]=' ';
			str1[3]=' ';
			str1[4]=' ';
			str1[5]=' ';
			Lcd12864_WriAdd(3,3);
			Lcd12864_PriStr(str1);
			break;
		case 15:display11();break;
		default:break;
		 
	}		
}



void count_action()  //��ʾ����ʱ
{
	switch(mode0)
	{
		case 4: //��ˮˮ����ʱ
			Num_count.Nu_tick1--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick1>250)
			{
				Num_count.Nu_tick1=0;
				mode0=5;
				Wash_Init();
				display5();
			}
			break;	
		case 5:  //ϴ�Ӽ�ʱ
			Num_count.nu_tick2--;
			Num_count.Nu_tick9--;
			if(Num_count.nu_tick2>10000)
			{
				Wash_Stop();
				Num_count.nu_tick2=0;
				drain_Init();
				mode0=6;
				display6(); //������ˮ����
			}
			break;
		case 6: //��ˮ��ʱ
			Num_count.Nu_tick3--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick3>250)
			{
				Num_count.Nu_tick3=0;
				mode0=7;
				Inlet_Init();  //��ˮ������ʼ��
				display4();  //�����ˮ����
			}
			break;
		case 7: //��ˮ
			Num_count.Nu_tick1--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick1>250)
			{
				Num_count.Nu_tick1=0;
				mode0=8;
				Rinse_Init();
				display7();  //����Ưϴ����
			}
			break;
		case 8: //Ưϴ
			Num_count.Nu_tick5--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick5>10000)
			{
				
				Num_count.Nu_tick5=0;
				if(SY_BOOL.cishu_piaoxi)
				{		
						SY_BOOL.cishu_piaoxi=0;
						drain_Init();
						mode0=6;  //->��ˮ
						display6();
				}
				else
				{
					Rinse_Stop();
  				Tuoshui_Init();
					if(SY_BOOL.tuoshui_Enable)
					{
						mode0=9;
						display8();// ������ˮģʽ
					}
					else
					{
						mode0=10;
						drain_Init();
						display6(); //������ˮģʽ
					}
				}				
			}
			break;
		case 9: //��ˮ��ʱ
			Num_count.Nu_tick7--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick7>1000)
			{
				Tuoshui_Stop();
				Num_count.Nu_tick7=0;
				mode0=11;
				Num_count.Nu_tick8=5;
				display9();  //ϴ����ɽ���
			}
			break;
		case 10: //��ˮ��ʱ
			Num_count.Nu_tick3--;
			Num_count.Nu_tick9--;
			if(Num_count.Nu_tick3>250)
			{
				Lcd12864_SetWhite_Init();
				Num_count.Nu_tick3=0;
				mode0=11;
				Num_count.Nu_tick8=5;
				display9();  //->ϴ����ɽ���
			}
			break;
		case 11: //���
			Num_count.Nu_tick8--;
			if(Num_count.Nu_tick8>250)
			{
				Num_count.Nu_tick8=0;
				mode0=0;
				display0();  //->��������
			}
			Lcd12864_WriAdd(0,0);
			Lcd12864_PriStr("  STM32 ϴ�»�");
			break;
	}
}
 

void JINJI()   //��������й����и��Ӵ�
{
	switch(mode0)
	{
			case 5:
			case 8:
			case 9:
				if(Gaizi()==0)  //���Ӻ���
				{
					SY_BOOL.count_enable=1;break;
				}
				else
				{
					SY_BOOL.count_enable=0;
					beep=0;//�򿪷�����
					display11();
					cuuent_mode=mode0;
					mode0=15;
				}
				break;			
			default:SY_BOOL.count_enable=1;break;
	}
}

int main()
{
		SHUI_BIT.shuiwei_youxiao=1;
		SHUI_BIT.shuiwei_wuxiao=0;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
		Led_init();
		Shui_init();
		HONG_Init();
		SY_BOOL.count_enable=1;
		Pointer_count.po_tick1=1;
		Key_init(); 						//��ʼ������
		delay_init();  					//��ʱ������ʼ��
		TIM2_IN_Init(9999,359);  //��ʱ��2��ʼ��
		PWM_Init();             //PWM��ʼ��
		Lcd12864_Init();  			//12864��ʼ��
		Lcd12864_SetWhite_Init();
		Lcd12864_WriAdd(0,0);
		Lcd12864_PriStr("  STM32 ϴ�»�");
		FMQ_Init();
		display();
		CBB=tingzhi;
		Motor_Init();
    while(1)
    {
			if(mode0==1)
			{
				Lcd12864_WriAdd(1,1);
				Lcd12864_PriStr(Dis_str1[flag_MODESTOP&0x07]);
			}
			if(mode0==2)
			{
				Lcd12864_WriAdd(1,6);
				Lcd12864_PriStr(Dis_str2[(flag_MODESTOP&0x18)>>3]);		
			}
			if(SY_BOOL.f_tick_1s==1)
			{
				SY_BOOL.f_tick_1s=0;
				pointer_action();
				display_action();
			}
			Motor_Run();
			Shui_jiance();
			JINJI();
    } 
}
//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
		{
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־ 
			key_scan();
		}
		TIM2_5ms_ticks++;  //5MS����
		if(TIM2_5ms_ticks>=20)
		{
			TIM2_5ms_ticks=0;
			TIM2_1s_ticks++;
			if(SY_BOOL.count_enable)
			{
				count_action();
			}
			Active_action();
			SY_BOOL.f_tick_1s=1;
		}
}



