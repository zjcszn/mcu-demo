#ifndef _LCD_INTERFACE_H
#define _LCD_INTERFACE_H

#ifndef _LCD_INTERFACE_C
#define EXT_LCD_INF extern
#else
#define EXT_LCD_INF
#endif


#define TOKEN_LOGO   0xFFFF     //����

#define TOKEN_MAIN   0x0000     //���˵�

#define TOKEN_PARA   0x1000     //�������ò˵�
#define TOKEN_PORT   0x2000     //�˿����ò˵�
#define TOKEN_SERIAL 0x3000     //������ʾ�˵�
#define TOKEN_CAN    0x4000     //CAM��ʾ�˵�
#define TOKEN_ETH    0x5000     //��̫����ʾ�˵�
#define TKOEN_TERM   0x6000     //�ն���ʾ�˵�

#define TOKEN_TIME   0x1100     //ʱ��������ʾ
#define TOKEN_BKG    0x1200     //������ʱ��ʾ
#define TOKEN_COMM   0x1300     //ͨѶ������ʾ
#define TOKEN_FAULT  0x1400     //���ϵƶ�����ʾ
#define TOKEN_ALARM  0x1500     //����������ʾ
#define TOKEN_PWD    0x1600     //����������ʾ

#define TOKEN_TERMCS 0x2100     //�����ն�������ʾ
#define TOKEN_TERMCE 0x2200     //��̫���ն�������ʾ

#define TOKEN_PORTS  0x3100     //���п�״̬��ʾ
#define TOKEN_FRAME1 0x3200     //���Ĺ�����ʾ
#define TOKEN_RECE1  0x3300     //���пڽ���������ʾ
#define TOKEN_SEND1  0x3400     //���пڷ���������ʾ

#define TOKEN_CANS   0x4100     //CAN״̬��ʾ
#define TOKEN_FRAME2 0x4200     //CAN���Ĺ���
#define TOKEN_RECE2  0x4300     //CAN����������ʾ
#define TOKEN_SEND2  0x4400     //CAN����������ʾ

#define TOKEN_ETHS   0x5100     //��̫��socket�׽ӿ�״̬��ʾ
#define TOKEN_RECE3  0x5200     //��̫�����ձ�����ʾ
#define TOKEN_SEND3  0x5300     //��̫�����ͱ�����ʾ

#define TOKEN_TERMS  0x6100     //�ն�״̬��ʾ
#define TOKEN_YX     0x6200     //ң����ʾ
#define TOKEN_YC     0x6300     //ң����ʾ
#define TOKEN_KWH    0x6400     //���������ʾ
#define TOKEN_SOE    0x6500     //SOE����ʾ

#define TOKEN_YXC    0x2110     //ң��������ʾ
#define TOKEN_YCC    0x2120     //ң��������ʾ


#define M_MENU_ITEM  0x01       //�˵���
#define M_DLIST_ITEM 0x02       //ö�����б����
#define M_NUM_ITEM   0x03       //������ʾ��
#define M_IP_ITEM    0x04       //IP��ַ��




//---------------------------------------
#define TASK_LCD_INF_STK_SIZE	256

EXT_LCD_INF OS_STK TaskLcdInfStk[TASK_LCD_INF_STK_SIZE];

EXT_LCD_INF void TaskLcdInf(void *pdata);

EXT_LCD_INF void WaitKey(s16 time_long,u16 key);


#endif