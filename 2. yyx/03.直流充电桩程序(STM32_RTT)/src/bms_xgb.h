#ifndef BMS_XGB_H_
#define BMS_XGB_H_

#include"stm32f10x.h"

#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
//�������Э��ṹ
typedef struct
{
	u8  ctl_byte;   //�����ֽ�
	u16 msg_lenth;  //��Ϣ�ܳ���  �ֽ�
	u8  pkg_cnt;  //���ݰ�����
	u8  not_use;  //����  �̶� 0xff
	u8  pgn[3];  //PGN
}TP_CM_RTS_Def;
#pragma pack(pop) //�ָ�����״̬


//���ڽ���BMS�������
typedef struct
{
	u16 len;
	u16 received_len;  //ʵ���յ����ֽ�
	u8  pkg_cnt;
	u8  pgn[3];
	u8  *buf;
}Mult_pkg_info_Def;



#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
typedef struct
{
	u8 version[3];

	//01Ǧ��
	//02����
	//03�������
	//04�����
	//05�����
	//06��Ԫ����
	//07�ۺ��������
	//08�����
	//FF����
	u8 battery_type;
	u16 Ah;  //����  ��λ0.1Ah
	u16 V;   //���ѹ  ��λ0.1V
	u8 manufacturer[4]; //����������  ASCII
	u32 No;  //��������
	u8 year;  //�����������  1985ƫ��
	u8 month;  //����������
	u8 day;  //����������
	u8 cycle_cnt[3];  //������
	u8 property;  //��Ȩ��ʶ  0����   1������
	u8 not_use;  //����
	u8 VIN[17];  //������ʶ
}PGN512_Def;

typedef struct
{
	u16 limt_v_single;  //���嶯����������������ѹ  ��λ0.01V  ��Χ 0~24V
	u16 limt_i;  //��߳�����  ��λ0.1A  -400Aƫ����    ��Χ  -400~0V
	u16 max_kWh;  //���������  ��λ0.1kWh   ��Χ 0~1000kWh
	u16 limt_v;   //����������ܵ�ѹ  ��λ0.1V  ��Χ 0~750V
	u8  limt_t;   //��������¶�  ��λ��    -50��ƫ��   ��Χ  -50�� ~ +200��
	u16 SOC;  //������ص��״̬  ��λ0.1%  ��Χ 0% ~ 100%
	u16 v;  //��������ܵ�ѹ  ��λ0.1V  ��Χ 0~750V
}PGN1536_Def;

typedef struct
{
	u16 v;  //��ѹ����  ��λ0.1V  ��Χ 0~750V
	u16 i;  //��������  ��λ0.1A  -400Aƫ����    ��Χ  -400~0A
	u8  cc_cv;  //���ģʽ   0x01:��ѹ   0x02����
}PGN4096_Def;

typedef struct
{
	u16 v;  //��ѹ����ֵ  ��λ0.1V  ��Χ 0~750V
	u16 i;  //��������ֵ  ��λ0.1A  -400Aƫ����    ��Χ  -400~0V
	u16 max_v_single;  //��ߵ��嶯����ص�ѹ������� 1~12λ��ѹ ��λ0.01V ��Χ0~24V  13~16λ ��ѹ������� 1ƫ����  ��Χ 1~16
	u8  SOC;  //��ǰ���״̬  ��λ1%  ��Χ 0% ~ 100%
	u16 remain_t;  //����ʣ����ʱ��  ��λ����  ��Χ0~600min
}PGN4352_Def;

typedef struct
{
	u8 SOC;  //��λ1%
	u16 min_v_single;  //��͵����ѹ
	u16 max_v_single;  //��ߵ����ѹ
	u8  min_temp_single;  //��͵�����¶�
	u8  max_temp_single;  //��ߵ�����¶�
}PGN7168_Def;



#pragma pack(pop) //�ָ�����״̬


#endif
