#ifndef __MAIN_H
#define __MAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"
#include "stm32f10x.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
#include "max6675.h"
#include "pid.h"
#include "adc.h"
#include "beeper.h"
#include "flash.h"
#include "menu.h"
#include "oled.h"
#include "setting.h"

#define HEAT_Pin GPIO_Pin_0
#define HEAT_GPIO_Port GPIOA
#define HEAT PAout(0)

#define SLEEP_Pin GPIO_Pin_8
#define SLEEP_GPIO_Port GPIOA
#define SLEEP PAin(8)

// Ĭ���¶ȿ���ֵ�����麸���¶ȣ�300-380��C��
#define TEMP_MIN   150 // ��Ϳ�ѡ�¶�
#define TEMP_MAX   400 // ����ѡ�¶�
#define TEMP_SLEEP 100 // ˯��ģʽ�µ��¶�
#define TEMP_ADD	50 // ǿ��ģʽ�µ��¶�����ֵ

// Ĭ������ͷ�¶�У׼ֵ
#define TEMP100 790  // 100��ʱ��ADCֵ
#define TEMP200 1560 // 200��ʱ��ADCֵ
#define TEMP300 2830 // 300��ʱ��ADCֵ
#define TEMP420 4090 // 420��ʱ��ADCֵ

// Ĭ�ϼ�ʱ��ֵ��0=���ã�
#define TIME_SLEEP 5        // ����˯��ģʽ��ʱ�䣨���ӣ�
#define TIME_SHUT  20       // �رռ�������ʱ�䣨���ӣ�

#define TEMPARRLEN 20 //�¶�����Ԫ�ظ���
extern u16 volatile NTC_temp;//�ֱ��¶�
extern u16 volatile T12_temp;//����ͷ�¶�
extern float volatile VinVolt;//�����ѹ
extern u16 volatile tempArray[TEMPARRLEN];//�¶�����
#ifdef __cplusplus
}
#endif
#endif
