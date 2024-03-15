#ifndef _LCD_PROTOCOL_H
#define _LCD_PROTOCOL_H

#ifndef _LCD_PROTOCOL_C
#define EXT_LCD_PROTOCOL extern
#else
#define EXT_LCD_PROTOCOL
#endif



//
#define LCD_PROTOCOL_FRAME_MIN_LEN		8
#define SEND_RETRY_TIMES				3

#define SEQ_YC_CIRCLE_TIME				20		//400ms
#define FRAME_UNACK_TIMEOVER_TIME		50		//500ms
//----------------------------------------------------------------
//��Ҫ����֡���ͱ�־ UINT32 seq_state	,���ֵı�־����Ҫѭ�����ͣ����ֵı�־��Ҫѭ�����͡�

//	
#define STA_SEQ_DEVICE_TYPE_FLAG		0x00000001		//�豸���Ͳ�ѯ	
#define STA_SEQ_EVENT_CALL_FLAG		 	0x00000002		//�¼���¼�ٻ�
#define STA_SEQ_ADDR_FLAG				0x00000004		//��ַ��ѯ
#define STA_SEQ_COMM_FLAG				0x00000008		//ͨѶ������ѯ


#define STA_SEQ_PWD_CALL_FLAG           0x00000010		//�����ٻ�
#define STA_SAVE_PWD_FLAG	            0x00000020		//��������
#define STA_SEQ_DISP_FLAG				0x00000040		//�ٻ���ʾ����(����������ʱ����ʾһ�λ����)
#define STA_SAVE_COMM_FLAG				0x00000080		//ͨѶ�����̻�

#define STA_SAVE_ADDR_FLAG				0x00000100		//��ַ�̻�
#define STA_SAVE_TIME_FLAG				0x00000200		//ʱ��̻�
#define STA_SAVE_DISP_FLAG				0x00000400		//��ʾ�����̻�
#define STA_SEQ_FIX_VALUE_FLAG			0x00000800		//��ֵ��ѯ֡

#define STA_SAVE_FIX_VALUE_FLAG			0x00001000		//��ֵ�̻�֡
#define STA_SEQ_KI_PARA_FLAG			0x00002000		//�ٿ������
#define STA_SAVE_KI_PARA_FLAG			0x00004000		//��������̻�
#define STA_SEQ_ADJ_FLAG				0x00008000		//������ϵ��

#define STA_SAVE_ADJ_FLAG				0x00010000		//��������ϵ��
#define STA_SEQ_SOE_FLAG				0x00020000		//��SOE
#define STA_SET_SIGNAL_RST_FLAG			0x00040000		//�źŸ���
#define STA_SEQ_DEV_INFO_FLAG			0x00080000		//�ٻ�װ��˵��

#define STA_SEQ_ACK_FLAG				0x40000000		//����ȷ��֡
#define STA_SEQ_REAL_DATA_FLAG			0x80000000 		//ʵʱ���ݲ�ѯ

//---------------------------------------------------------------
//����֡���Ͷ���(�ϴ����·���֡��ʹ�ô˴���������)
//
#define FRAME_TYPE_SEQ_DEVICE		1	//�豸����
#define FRAME_TYPE_SEQ_YC			2	//ʵʱ����
#define FRAME_TYPE_SEQ_ADDR			3	//��ַ��ѯ
#define FRAME_TYPE_SET_TIME			4	//����ʱ��
#define FRAME_TYPE_SIGNAL_RST		5	//�źŸ���
#define FRAME_TYPE_SEQ_FIX			6	//��ֵ��ѯ,			�ϴ���ֵ(����)
#define FRAME_TYPE_SET_FIX			7	//��ֵ����
#define FRAME_TYPE_SEQ_PWD			8	//�����ѯ
#define FRAME_TYPE_SET_PWD			9	//��������
#define FRAME_TYPE_SET_YK			10	//ң������
#define FRAME_TYPE_SEQ_EVENT		11	//��SOE
#define FRAME_TYPE_SEQ_DISP			12	//��ʾ����
#define FRAME_TYPE_SEQ_COMM			13	//ͨѶ������ѯ,		�ϴ�ͨѶ����(����)
#define FRAME_TYPE_SET_COMM			14	//ͨѶ�����̻�
#define FRAME_TYPE_SET_ADDR			15	//��ַ����
#define FRAME_TYPE_SET_DISP			16	//��ʾ�����̻�
#define FRAME_TYPE_SEQ_KI			17	//���������ѯ
#define FRAME_TYPE_SET_KI			18	//��������̻�
#define FRAME_TYPE_SEQ_ADJ			19	//����ϵ����ѯ
#define FRAME_TYPE_SET_ADJ			20	//����ϵ���̻�
#define FRAME_TYPE_SEQ_DEV_INFO		21	//�ٻ�װ��˵��
//
#define FRAME_TYPE_TIME_UP			22	//ʱ���ϴ�
#define FRAME_TYPE_LIGHT_UP			23	//�����ϴ�
#define FRAME_TYPE_SOE_UP			24	//�¼������ϴ�
//
#define FRAME_TYPE_ACK				25	//ȷ��֡

//�������Ͷ���
#define PARA_TYPE_DEVICE			0X20	//װ�ò���
#define PARA_TYPE_ADDRESS			0X21	//��ַ����
#define PARA_TYPE_JIANKONG		   0X22	//���ͨѶ�ڲ���
#define PARA_TYPE_DISPYX			0X23	//��ʾң�Ų���
#define PARA_TYPE_SOE				0X24	//SOE����
#define PARA_TYPE_YC				0x25	//ң�����
#define PARA_TYPE_GPRS			    0x26	//GPRSͨ�Ų���
#define PARA_TYPE_SENSOR			0x27	//������ͨ�Ų���
#define PARA_TYPE_FIXED				0x28	//��ֵ����
#define PARA_TYPE_YK				0x29	//ң�ز���
#define PARA_TYPE_WORK				0x2a	//��������
#define PARA_TYPE_MODULPARA			0X2B	//����ϵ������
#define PARA_TYPE_YX				0X2C	//ң�Ų���
#define PARA_TYPE_MODUL				0x2D	//����ϵ��

//----------------------------------------------------------------
//������������ռ�ù������������� share_buf_use_type
#define SHARE_BUF_TYPE_REAL_DATA	0x01			//ʵʱ����
#define SHARE_BUF_TYPE_EVENT		0x02			//�¼���¼
#define SHARE_BUF_TYPE_COMM			0x03			//ͨѶ����
#define SHARE_BUF_TYPE_KI			0x04			//�ٻ��������
#define SHARE_BUF_ADJ				0x05			//������ϵ��
#define SHARE_BUF_FIX				0x06			//��ֵ����

//----------------------------------------------------------------	
//----------------------------------------------------
//�ٻ��ɹ��ı�־ valid_para_flag	(��ռ�ù���������������)
#define ENABLE_TYPE_DEVICE_FLAG		0x00000001		//װ�������ٻ����
#define ENABLE_TYPE_ADDRESS_FLAG	0x00000002		//��ַ�����ٻ����
#define ENABLE_TYPE_COMMPORT_FLAG	0x00000004		//ͨѶ�ڲ����ٻ����
#define ENABLE_TYPE_DISPYX_FLAG		0x00000008		//��ʾң�Ų����ٻ����
//
#define ENABLE_TYPE_PASSWORD_FLAG	0x00000010		//�����ٻ��ɹ�

#define ENABLE_TYPE_ACKED_FLAG		0x00000020		//�ɹ��յ�ȷ��֡

#define ENABLE_DATA_FLAG			0x80000000		//���û��������ݿ��ñ�־



//----------------------------------------------------
#define TASK_LCD_PROTOCOL_STK_SIZE		256

#define LCD_PROTOCOL_RECE_BUF_LEN		256
#define LCD_PROTOCOL_SEND_BUF_LEN		256

#define SHARE_BUF_LEN					256			//�������ݻ����������е��ٻ����������ݷŴ�
//

//----------------------------------------------------------------------------
typedef struct 
{
	UINT8 lcd_protocol_rece_buf[LCD_PROTOCOL_RECE_BUF_LEN];
	UINT8 rece_len;
}RECE_BUF;

typedef struct 
{
	UINT8 lcd_protocol_send_buf[LCD_PROTOCOL_SEND_BUF_LEN];
	UINT8 send_len;
}SEND_BUF;

typedef struct 
{	
 	UINT32 rece_byte_num;	//�������ֽڼ���
 	UINT32 send_byte_num;	//�������ֽڼ���	
}LCD_CONNECTED_COUNT;

//ѭ���ٻ�֡ʱ����Ӧ��ʱ����
typedef struct
{
	UINT16 yc_circle_time;
}STRU_SEQ_CIRCLE_TIME;

//--------------------------------------------------------------------------------
//��������������
//�豸���Ͳ���
typedef struct 
{
	UINT8 type;
	UINT8 dev_version; 
}PARA_DEVICE_STRUCT;

//ͨѶ�ڲ���
//��ʾң�Ų���
typedef struct
{
	UINT8 yx_num;
	UINT8 yx_type;
	UINT8 yx_lcdposition;
	UINT8 yx_name;
}DISP_YX_PARA_STRU;
//SOE����
//ң�����
typedef struct 
{
	UINT8 type;		//����
	UINT8 channel;	//·��
	UINT8 format;	//���ݸ�ʽ
	UINT8 unit;		//��λ
}PARA_YC_STRU;

//----------------------------------------------------------------------------------

EXT_LCD_PROTOCOL PARA_DEVICE_STRUCT dev_type;			//�豸����
//EXT_LCD_PROTOCOL UINT16  local_addr;						//������ַ
EXT_LCD_PROTOCOL UINT16 led_disp_data;					//��������
EXT_LCD_PROTOCOL UINT8 new_soe_num;						//���������ɵ�SOE�����������ϴ�����
														//0����û����SOE����


//
EXT_LCD_PROTOCOL LCD_CONNECTED_COUNT lcd_t_r_count;		//���շ��ͼ���
//��������
EXT_LCD_PROTOCOL UINT32 seq_status;						//����״̬��ͨ����״̬������֪�����ͺ�֡
EXT_LCD_PROTOCOL UINT32 valid_para_flag;				//����ʹ�õĲ������־
//
EXT_LCD_PROTOCOL UINT16  bkg_delay;						//������ʱ
EXT_LCD_PROTOCOL UINT16  yc_disp_mode;					//��ʾģʽ��1��һ��ֵ��0������ֵ�����ô����ֵ�λ


//�������ݻ�����
//EXT_LCD_PROTOCOL UINT16 share_buf[SHARE_BUF_LEN];
EXT_LCD_PROTOCOL UINT8 share_buf_use_type;				//��������ռ���˹��û�������0����ռ��
EXT_LCD_PROTOCOL UINT16 share_buf_data_len;				//�������������ݳ���
//-----------------------------------------------------

EXT_LCD_PROTOCOL OS_STK TaskLcdProtocolStk[TASK_LCD_PROTOCOL_STK_SIZE];
EXT_LCD_PROTOCOL void InitLcdProtocol(void);
EXT_LCD_PROTOCOL void TaskLcdProtocol(void *);

//
EXT_LCD_PROTOCOL void SetProRunStatus(UINT32 flag);
EXT_LCD_PROTOCOL void ResetProRunStatus(UINT32 flag);
EXT_LCD_PROTOCOL UINT32 GetProRunStatus(void);

EXT_LCD_PROTOCOL void SetParaStatus(UINT32 flag);
EXT_LCD_PROTOCOL void ResetParaStatus(UINT32 falg);
EXT_LCD_PROTOCOL UINT32 GetParaStatus(void);

extern const UINT8 fix_data[];


#endif