#ifndef LCD_H_
#define LCD_H_

#include "stm32f10x.h"

typedef struct PAGE
{
	u8 (*poll_handler)(u32 poll_cnt);  //ҳ����ѯ������  ���ڸ�����ʾ����   NULL��ˢ��   ��Ҫ�л�ҳ�� ����ҳ��ID����  pool_cnt���ִ��� ���л�����ǰҳ�濪ʼ����
	void (*vchange_handler)(u8 offset, u32 data);  //ҳ��������ݱ仯������  ���統�û���������ʱ�Զ�������������  ������ô˺�������
	u8 (*button_handler)(u8 offset);
	void (*page_init)();   //�л�ҳ��֮ǰ����  ���ڳ�ʼ��ҳ��
	void* wo;              //ֻд����
	void* ro;              //ֻ������
	u16 poll_time;         //��ѯʱ����   ��λms
	u8  wo_len;		         //ֻд��������
	u8  ro_len;		         //ֻ����������
	u8 prev_id;            //ǰһ��ҳ��  ���ڴӵ�ǰҳ�淵��ǰһ��
}page_t;

//DGUSʱ��
//HEX��ʾ  ����2015�� ��  YY == 0x15
typedef struct
{
	u8 YY;  //�� 2000 + YY
	u8 MM;  //��
	u8 DD;  //��
	u8 WW;  //����
	u8 h;   //ʱ
	u8 m;   //��
	u8 s;   //��
}dgus_time_t;


void task_lcd(void);

//��Чҳ��ID
#define PAGE_ID_INVALID 0xff

#define PAGE_PREV 0xfe

#endif /* LCD_H_ */
