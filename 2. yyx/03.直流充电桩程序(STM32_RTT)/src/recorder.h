#ifndef RECORDER_H_
#define RECORDER_H_

#include "stm32f10x.h"

#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
typedef struct
{
	unsigned short year;
	unsigned char  mon;
	unsigned char  day;
	unsigned char  hour;
	unsigned char  min;
	unsigned char  sec;
}record_time_def;

//�����ʷ��¼
typedef struct
{
	record_time_def time;  //��ʼʱ��
	u32 sum;  //��� 0.01Ԫ
	u32 kwh;  //���� 0.01��
	u32 sec;  //ʱ��  ��
	u32 card_id;  //IC��ID
	//u8  id;  //���ǹID
	u8  status;  //״̬   1:����  2:δ����
}record_def;

#pragma pack(pop) //�ָ�����״̬


int recd_write(u16 addr, record_def* recd);
int recd_read(u16 addr, record_def* recd);
void recd_init(void);
u16 recd_get_cnt(void);
void recd_clr(void);
u16 recd_add(void);
int recd_read_new(u16 id, record_def* recd);


#endif /* ETH_H_ */
