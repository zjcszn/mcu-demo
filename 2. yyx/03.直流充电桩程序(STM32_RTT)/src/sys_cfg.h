#ifndef SYS_CFG_H_
#define SYS_CFG_H_

#include "stm32f10x.h"
//�ڲ�flash����16λ��д��  ������ 32 ����
typedef struct
{
	u16 valid;  //��־�Ƿ���Ч  == 0xAAAA ��Ч ���ڵ�һ���ϵ�

	u32 sys_pswd;  //ϵͳ����
	u32 id;        //���׮ID��
	u32 e_price;   //���  ��λ ��  100��ʾ1Ԫ
	u32 ic_down_pswd;  //��־�Ƿ����ع���������Կ  0xAAAAAAAAΪ���ع�

	u8 lip1;  //����IP
	u8 lip2;
	u8 lip3;
	u8 lip4;

	u8 drip1;  //����IP
	u8 drip2;
	u8 drip3;
	u8 drip4;

	u8 netmask1;  //��������
	u8 netmask2;
	u8 netmask3;
	u8 netmask4;

	u8 rip1;  //Զ��IP
	u8 rip2;
	u8 rip3;
	u8 rip4;

	u16 rport;  //Զ�̶˿�

	u16 gprs;  //0:�ƶ�   1:��ͨ
}sys_cfg_def;

typedef struct
{
	u16 valid;  //��־�Ƿ���Ч  == 0xAAAA ��Ч ���ڵ�һ���ϵ�

	u32 v_offset;  //��ѹADƫ����   12bit
	float v_gain;    //��ѹ����  ����1000��
	u32 i_offset;  //����ADƫ����   12bit
	float i_gain;    //��������  ����1000��
}meas_adj_def;

int sys_cfg_read(void);
int sys_cfg_save(void);
int meas_adj_read(void);
int meas_adj_save(void);

extern sys_cfg_def sys_cfg;
extern meas_adj_def meas_adj;
extern const sys_cfg_def sys_cfg_default;
extern const meas_adj_def meas_adj_default;

#endif /* CONFIG_H_ */
