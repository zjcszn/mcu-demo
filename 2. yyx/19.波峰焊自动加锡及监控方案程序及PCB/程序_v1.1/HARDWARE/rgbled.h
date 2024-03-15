#ifndef _RGBLED_H 
#define _RGBLED_H

#include "sys.h"
#include "delay.h"
#include "main.h"

#define LED_RGB     PBout(7)

#define RGB_NOCHANGE    1

//RGB��ɫ����
#define COLOR_RED		0xFF0000	//-����
#define COLOR_GREEN		0x008000	//-����
#define COLOR_BLUE		0x0000FF	//-����
#define COLOR_YELLOW	0xFFFF00	//-����   //׼���׶�
#define COLOR_WHITE		0xFFFFFF	//-����
#define COLOR_BLACK		0x000000	//-����

void RGB_Set_Down(void); //����0
void RGB_Set_Up(void);   //����1
void RGB_Led_Init(void);
void RGB_SendData(u32 dat);  //ֵ��RGB˳��洢������ʱ����GRB˳����
void RGB_SendAllData(u32 dat[],u8 size);
void RGB_Control(u32 rgb_dev,u32 rgb_warning);  

#endif



