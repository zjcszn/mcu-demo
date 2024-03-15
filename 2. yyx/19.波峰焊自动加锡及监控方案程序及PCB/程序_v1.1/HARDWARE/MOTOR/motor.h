#ifndef __motor_H
#define __motor_H	 
#include "sys.h"

#define ENA PAout(11) 		 //PA11���ʹ���ź�
#define PUL PAout(12) 		 //PA12���ʱ���ź�
#define DIR PBout(0)  		 //PB0 ��������ź�
#define MCU_LOCK PBout(1)    //PB1 ��������ϵ�ɲ��

#define MCU_TEST PCout(0)    //PC0 ����

void motor_Init(void);		   //��ʼ��
u32 motor_P(u32 MP_delay);	   //��ת
u32 motor_N(u32 MN_delay);	   //��ת
u8 motor_R(u32 MP_delay,u8 PN);
void motor_S(void);			   //ֹͣת��
					    
#endif
