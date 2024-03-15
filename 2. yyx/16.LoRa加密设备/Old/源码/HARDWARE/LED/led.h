#ifndef __LED_H
#define __LED_H	 

#include "sys.h"

//λ������
#define 	RUN_LED 				PAout(2)		//PA2
#define 	NET_LED 				PAout(1)		//PA1	

#define		RUN_LED_ON()			{RUN_LED=0;}
#define		RUN_LED_OFF()			{RUN_LED=1;}
#define		NET_LED_ON()			{NET_LED=0;}
#define		NET_LED_OFF()			{NET_LED=1;}

#define		LED_MAX_VALUE			5000
#define   	LED_FLASH_FRE			500


void 	LED_Init(void);					//��ʼ��
void 	RunLEDCounter(void);			//���м�����
void 	IndicatorLightMainHandle(void);	//ָʾ��������

#endif
