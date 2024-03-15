#ifndef BMS_YL_H_
#define BMS_YL_H_

#include "stm32f10x.h"

//��¡Э��

#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
//�ܵ�ѹ����
typedef struct
{
	u16 i;  //��λ1A  ƫ���� 32767A
	u16 v;  //��λ1V  ƫ����  0V
	u8  SOC;  // 1%  �����ٷֱ�
}bms_yl_bbab_def;

//���ֹͣ����
typedef struct
{
	u8 onoff;  //�ػ���� ����1ʱ���ػ���Ĭ��Ϊ0x00.
}bms_yl_a8a8_def;

#pragma pack(pop) //�ָ�����״̬

typedef struct
{
	//�����ص�ѹ����
	//��һά �����
	//�ڶ�ά һ������5��CAN��Ϣ
	//����ά һ��CAN��Ϣ8�ֽ�
	u8 msg_01A0[3][5][8];


	//�����Ϣ ����VIN
	u8 msg_ACAF[4][8];
}bms_yl_info_def;

#endif
