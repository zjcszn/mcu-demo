#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define MCU_RED    PBout(5)// PB5
#define MCU_GREEN  PBout(6)// PB6	
#define MCU_YELLOW PBout(7)// PB7
#define MCU_BEEP   PBout(12)// PB12
#define test_mcu   PBout(15)// PB15

void LED_Init(void);//��ʼ��

		 				    
#endif
