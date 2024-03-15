#ifndef EVC_H_
#define EVC_H_


#include"stm32f10x.h"



#define MAX_MODULE_CNT 8  //���������ģ����


//����������ȫ�����ò���
typedef struct
{
	u8  onoff;   //0xff:on  0x00:off
	u16 set_v;   //���õ�ѹ (0.1v)
	u16 set_c;	 //���õ��� (0.01A)
}config_info_def;

//���������Ļ�ȡ����
typedef struct
{
	u16 v;  //��λ0.1V
	u16 i;  //��λ0.01A
}read_info_def;


#define READ_SAMP_CNT 3  //��ѹ���������˲�ȡ������   �������1

//����ģ�����
typedef struct
{
	u8 id;  //ID��
	u8 is_err;  //��־�Ƿ��й���  1:�й���   0:����
	u8 link_off;  //����⵽����ʱ��1 ������3�����жϹ���
	u8 cfg_end;  //����ͨѶ��ʱ�Ƿ����
	u8 ocp_reset_cnt;  //������λ����
	u32 ocp_reset_t;   //��λʱ��
	u32 status;  //������״̬
	u16 v;  //��ȡ�ĵ�ѹ ��λ0.1V
	u16 i;  //��ȡ�ĵ��� ��λ0.01A
	u16 v_temp[READ_SAMP_CNT];  //��ȡ�ĵ�ѹ ��λ0.1V
	u16 i_temp[READ_SAMP_CNT];  //��ȡ�ĵ��� ��λ0.01A
}module_info_def;


typedef struct
{
	u8 err_ps;  //1:�������   0:����
	u8 module_cnt;  //������������ģ����
	u8 module_cnt_no_err;  //�������޹���ģ����
	u16 max_v;  //��������ѹ   0.1V
	u16 max_i;  //����������   0.01A
	config_info_def config;
	read_info_def read;
	module_info_def module_info[MAX_MODULE_CNT];
}evc_info_def;



void evc_out_onoff(u8 onoff);
void evc_set_v_c(unsigned short v, unsigned short c);

extern evc_info_def evc_info;
void task_evc(void);


#endif /* CHGMODULE_H_ */
