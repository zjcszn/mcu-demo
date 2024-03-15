#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "main.h"

#define SOUND_NOWORK    (0x99)
#define SOUND_PASS      (0x01)
#define SOUND_FAIL      (0x02)

#define IO_BEEF()     PAout(8)

void TIM4_Int_Init(u16 arr,u16 psc);
void TIMx_Int_Init(TIM_TypeDef* TIMx,u16 arr,u16 psc);
void TIM_TemporaryClose(u8 NewState); //0-�رն�ʱ��,1-�ָ���ʱ��ԭ����״̬

//extern u8 BeefWorkWay;   //0-fail,1-pass,other-no work
extern u8 USART1_ReciveFinishFlag; //һ��ʱ����ʼ�����ڽ���
extern u8 USART1_ReciveFlag;
extern u8 CommFailFlag;

extern u16 TIM3_Times1;
extern u16 TIM3_Times2;
extern u16 TIM3_Times3;
extern u16 TIM3_Times4;
#endif




