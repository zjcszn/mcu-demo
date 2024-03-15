#ifndef SERVER_H_
#define SERVER_H_

#include "stm32f10x.h"


#define  SERVER_SATUS_IDLE    1  //����״̬  ��ʾû���ڳ��
#define  SERVER_SATUS_CHGING  2  //���ڳ��  ���ͳ�����ݱ���
#define  SERVER_SATUS_CHGEND  3  //������  ���ͳ����㱨��
#define  SERVER_SATUS_ERR     4  //��������  ���ͳ������ϱ���

typedef struct
{
	int (*send)(u8 *buf, u16 len);  //��̨���ͺ���  �ں�̨�����е���   ������ ��̫��  gprs wifi ���ͷ�ʽ
	void (*recv_callback)(u8 *buf, u16 len);  //���շ��������ݻص�����    ���������   ������̫��   gprs  wifi
}server_handler_def;


#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���

//ͨѶ����֡ͷ
typedef struct
{
	u8  fh;  //֡ͷ  �̶�0x5A
	u16 len;  //�������ݳ���  fh+len+id+crc32+֡β = 12bytes
	u32 id;   //���׮ID��
	u8 cmd;
	u8  data[1];  //����  ���Ȳ���
}com_header_def;


typedef struct
{
	u8 y;  //��  ��15����   y==15 ��ʾ2015��
	u8 m;  //��
	u8 d;  //��
	u8 h;  //ʱ
	u8 mm; //��
	u8 s;  //��
}com_time_def;

//���������
typedef struct
{
	u32 id;    //IC����
	u16 v;      //����ѹ  0.1V
	u16 i;     //������    0.1A
	u16 kwh;   //���ĵ���    0.01kwh
	u16 amount;  //���Ľ��  0.01Ԫ
	u16 time;    //�ѳ�ʱ��  ����
	u8  soc;     //���soc
	com_time_def start_time;  //��ʼ���ʱ��
	u8  gun_id;  //ǹ��
	u16 max_s_v;  //��ߵ����ѹ  0.01V
}com_chging_def;

//����������
typedef struct
{
	u32 id;    //IC����
	u32 balances;  //���
	u16 kwh;   //���ĵ���    0.01kwh
	u16 amount;  //���Ľ��  0.01Ԫ
	u16 time;    //�ѳ�ʱ��  ����
	u8  soc;     //���soc
	com_time_def start_time;  //��ʼ���ʱ��
	u8  gun_id;  //ǹ��
}com_chgend_def;


//��������ȡ������Ϣ
typedef struct
{
	u8  ver[5];    //�̼��汾��
	u16 price;  //���
	u32 kwh;   //������    0.01kwh
}machine_info_def;

#pragma pack(pop) //�ָ�����״̬


//���������Ϣ
typedef struct
{
	u32 id;    //IC����
	u32 balances;  //���
	u32 kwh;   //���ĵ���    0.01kwh
	u32 amount;  //���Ľ��  0.01Ԫ
	u32 time;    //�ѳ�ʱ��  ����
	u8  soc;     //���soc
	u8  status;  //״̬   1:����״̬  2:���ڳ��  3:������
	com_time_def start_time;  //��ʼ���ʱ��
	u8  gun_id;  //ǹ��
}chg_info_def;



typedef struct
{
	u8 send_cnt;  //���ݷ��ʹ���
	u8 rsp_cnt;   //��������Ӧ����

	u8  state;   //��ǰ�׶�
	u8  status;  //�����������״̬  1:����   0:����
}server_info_def;


extern server_handler_def server_handler;
extern chg_info_def chg_info;
extern server_info_def server_info;

void task_server(void);

void server_send_idle(void);
void server_send_chging(void);
void server_send_chgend(void);

void server_recv_callback(u8 *buf, u16 len);

#endif
