#ifndef BMS_H_
#define BMS_H_

#include"stm32f10x.h"

#include"rtc.h"

typedef enum
{
	BMS_stage_handshake = 1,  //���ֽ׶�
	BMS_stage_config = 2,     //�������ý׶�
	BMS_stage_charging = 3,   //���׶�
	BMS_stage_end = 4,        //�������׶�
	BMS_stage_timeout = 5,
}bms_stage;


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
}TP_CM_RTS_def;
#pragma pack(pop) //�ָ�����״̬


//���ڹ������BMS���
typedef struct
{
	u16 len;  //��Ϣ�����ݳ���  �ֽ�
	u16 received_len;  //ʵ���յ����ֽ�
	u8  pkg_cnt;  //�ְ�����
	u8  recv_pkg_id;  //��һ�������յ��ķְ�id
	u8  pkg_rx_en;  //�Ƿ���շְ�  0:������   1:����
	u8  pgn[3];   //pgn���
	u8  *buf;  //���ݻ�����
}mult_pkg_info_def;


typedef struct
{
	void (*init)(void);  //Э���ʼ��
	void (*msg_handler)(u8 pgn, u8* msg, u16 len);  //���Ĵ���
	void (*timeout_handler)(void);   //can���ճ�ʱ����
	int (*poll)(void);  //��ѯ����   ���������������ݰ���ʱ���     ����oxff��ʾ������    0�������      (����1��ʾ������ԴҪ�л�Ϊ24V)
}bms_ptl_handler_def;


typedef struct
{
	u8 start_type;  //������ʽ    1:��̨     2:ˢ��      3:����Ա
	u8 chg_type;  //��緽ʽ  0:�Զ�����   1:������   2:ʱ��    3:�����
	u32 value;    //����   ����0.01kwh   ʱ��:1����       ���:0.01Ԫ
}bms_chg_info_def;

extern bms_chg_info_def bms_chg_info;


//BMSЭ������
#define BMS_PROTOCOL_GB 1  //����
#define BMS_PROTOCOL_XGB 2  //�¹���

typedef struct
{
	u16 out_v;  //BMS���صĵ�ѹ���ֵ  ��λ0.1V
	u16 out_i;  //BMS���صĵ������ֵ  ��λ0.1A

	u16 limt_v;  //��߳���ѹ ��λ0.1V
	u16 limt_i;  //��߳����� ��λ0.1A
	u16 limt_v_single;  //���嶯����������������ѹ  ��λ0.01V
	u16 limt_t;  //��߳����¶�

	u16 set_v;  //�����ѹ
	u16 set_i;  //�������
	u8  mode;  //��ѹ  ����


	u8  SOC;   //�����ٷֱ�

	//01Ǧ��
	//02����
	//03�������
	//04�����
	//05�����
	//06��Ԫ����
	//07�ۺ��������
	//08�����
	//FF����
	u8  battery_type;

	u8  timeout_cnt;  //�������ݽ��ճ�ʱ����

	u16 Ah;  //����  ��λ0.1Ah
	u16 V;   //���ѹ  ��λ0.1V

	u16 max_v_single;  //��ߵ��嶯����ص�ѹ  ��λ0.01V

	u16 min_v_single;  //��͵��嶯����ص�ѹ  ��λ0.01V

	u16 remain_t;  //����ʣ����ʱ��  ��λ����

	u16 max_kWh;  //���������  ��λ0.1kWh

	u8  max_temp;  //��ߵ�λ����¶�  ��λ1��  -50��ƫ��

	u8  min_temp;  //��ߵ�λ����¶�  ��λ1��  -50��ƫ��

	u8  protocol;  //BMSЭ��

	// 1 �ﵽSOCֵ   2 �ﵽ�ܵ�ѹ  3 �ﵽ�����ѹ
	// 4 ��Ե����     5 �������������  6 BMSԪ�� �������������
	// 7 �������������  8 ������¶ȹ���  9 ��������  10 ��������
	// 11 ��ѹ�쳣   12 ���ʹ���  13 �ֶ�ֹͣ
	// 14 ������ɳ��׼�����ĳ�ʱ  15 �������ֹͣ 16 ���ձ�ʶ���ĳ�ʱ  17 ���յ�س��������ĳ�ʱ
	// 18 ���յ�س����״̬���ĳ�ʱ  19 ���յ�س�������ĳ�ʱ    20 ������С   21 ������¶ȹ���2  22�����ѹ����        23 ���ձ�ʶ���ĳ�ʱ  24 ���յ�س��������ĳ�ʱ            50  ������      69���� 70 �ﵽ�趨����   71 �ﵽ�趨ʱ��   72  �ﵽ�趨���   73 ������ֹͣ   74 ����������
	u8  stop_reason;  //���ֹͣԭ��

	u8  bms_reuest_stop;  //BMS��������ֹͣ

	u8  bms_wait;  //BMS������ͣ���

	u8  reset_cnt;  //���ǳ������׶η������ĳ�ʱ��  ���׮Ӧ���½�������  �������Ӵ�������ʱ����

	u32 err_id;  //bms���صĴ���ID Ĭ��Ϊ0 ��ʾ�޴���

	u32 timer_tick;  //BMS��ʱ  ��λ10ms

	u8  can_out;

	bms_stage stage;  //��ǰ���׶�

	bms_ptl_handler_def *event;  //�¼�������
	
	u8 vin[17];  //������ʶ

	u32 no;  //��������

	u8 manufacturer[4]; //����������  ASCII

	u8 year;  //����������� 2000ƫ��
	u8 month;  //����������
	u8 day;  //����������

	time_t start_t;  //��ʼ���ʱ��
	time_t stop_t;   //�������ʱ��

	u8 v12_v24;  //��ǰ������Դ��ѹ   0:12V  1:24V

	u8 msg_id;  //��ǰ��ϢID
	u8 msg[50];  //bms �澯��Ϣ  ������Ϣ

}bms_info_def;

//�������֡
#define PGN_60160 0xEB
//����Э�����ӹ�����Ϣ
#define PGN_60416 0xEC


#define PGN_9984   0x27
#define PGN_256   0x01
#define PGN_512   0x02
#define PGN_1536  0x06
#define PGN_2304  0x09
#define PGN_4096  0x10
#define PGN_4352  0x11
#define PGN_4864  0x13
#define PGN_6400  0x19
#define PGN_7168  0x1C
#define PGN_7680  0x1E
#define PGN_5376  0x15
#define PGN_5632  0x16
#define PGN_5888  0x17



extern bms_info_def bms_info;
extern void CAN2_send(CanTxMsg *tx);
extern u8 get_pgn(u32 id);
extern void bms_stop_charge(u8 r);
extern void bms_gb_send_crm(u8 code);
extern void bms_pt_send_crm(u8 code);
extern int bms_start_charge(u8 start_type, u8 chg_type, u32 value);
extern u8 bms_charge_complete(void);
extern u8 bms_test_isok(void);
extern void bms_dc_out_dis(void);
extern void bms_dc_out_en(void);
extern void task_bms(void);

extern void bms_dc1_out_en(void);
extern void bms_dc1_out_dis(void);
extern void bms_sng1_en(void);
extern void bms_sng1_dis(void);

extern void bms_dc2_out_en(void);
extern void bms_dc2_out_dis(void);
extern void bms_sng2_en(void);
extern void bms_sng2_dis(void);

#endif /* BMS_H_ */
