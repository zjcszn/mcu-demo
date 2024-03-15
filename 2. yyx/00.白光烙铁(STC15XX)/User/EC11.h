/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			EC11.h
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef _EC11_H_
#define	_EC11_H_


//�궨��
#define KEYDOWN_TIME	10u
#define	KEYHOLD_TIME	250u

/*****************************************************/
/******************** �����ṹ���� *******************/
/****************************************************/
extern unsigned char str_buff[20];//����
extern unsigned char BeepCounterFlag;

typedef struct
{
	unsigned char	Coder;				//��������ת��־λ��0Ϊ�޶�����1Ϊ˳ʱ�룬2Ϊ��ʱ��
	unsigned char	Key;					//�����̰���־λ,0Ϊû�а������£�1Ϊ�̰���2Ϊ����
}EC11_State_s;

//����ģʽ�µĲ˵��ṹ��
	/* �˵���Ϣ */
typedef struct
{
	char 	OPTINO_NAME[3][20];//ҳ����
	char  ADS_str[3][4][20]; //�Ƕ�ѡ��
}MENU_MSG_s;
typedef struct
{

	/* �¶Ȳ����ṹ */
	struct 
	{
		unsigned int  	Max_Temp;  	
		unsigned int 	Slp_Temp; 
		unsigned int   Default_Temp;
		unsigned char  Default_CH;
		unsigned int   Last_Temp;
		unsigned char  Last_CH;		
	}Temp_s;
	/* ʱ������ṹ */
	struct 
	{
		unsigned int 	Slp_Time;
		unsigned int 	ScrSaver_Time;
		unsigned int 	Shutdown_Time;
	}Time_s;
	/* ���������ṹ*/
	struct 
	{
		unsigned char 	Work_Mode;  
		unsigned char 	Heater_Tip;
		unsigned char 	Shock_Sensor; 
		unsigned char 	Beep_Volume;  
	}Other_s;
} MENU_DATA_s;
//����ģʽ�µĲ˵��ṹ��
typedef struct
{
	/* ����ģʽ�˵���Ϣ */
	struct
	{
		char 	OPTINO_NAME[3][20];//ҳ����
		char  ADS_str[3][4][15]; //�Ƕ�ѡ��
	}FAC_MSG_s;

	/* ���������ṹ */
	struct 
	{
		unsigned char 	Iron;
		unsigned int 	Voltage; 
		unsigned char 	Encoder;
	}Adjust_s;
	struct 
	{
		unsigned int 	Ch1;
		unsigned int 	Ch2; 
		unsigned int 	Ch3;
		unsigned int 	Ch4;
	}Channel_s;
	struct 
	{
		unsigned char 	Temp_Step;
		unsigned char 	Lim_Voltage;
		unsigned char	Recovery;
	}Other_s;
	unsigned int Heater_Tip[8][14];
} FAC_DATA_s;
/********************************************************/
extern MENU_DATA_s 				MENU_DATA_Table;
extern FAC_DATA_s					FAC_DATA_Table;
extern EC11_State_s				ec11_state;
extern unsigned char ShutdownCounterFlag;
/* �������� */

/* ��������׽ */
void EC11_Capture(void);

/* �޸�ֵ */
unsigned char set_var(void);

/* ѡ���� */
unsigned char option_check_data(void);

/* ҳ���ѯ */
extern unsigned char Menu_check_data(void); 

/* �����趨 */
unsigned int ParaSet(void);
unsigned int ParaSet_Fac(void);

/* OLED */
void Display_cursor(void);
void OLED_printf(unsigned char x,unsigned char y,char *str,float var_num);
void DisPlay(unsigned char num);



#endif