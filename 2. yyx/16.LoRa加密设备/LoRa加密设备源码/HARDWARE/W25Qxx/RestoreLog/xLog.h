#ifndef __XLOG_H
#define	__XLOG_H

#include "sys.h"
//#include "logs.h"
#include "xLog.h"
#include "sx1276-LoRa.h"
#include "MasterLoRaHandle.h"
#include "w25qxx_flash.h"



//������־����
#define		SYS_CODE						0			//ϵͳ����Ա����
#define		AUDIT_CODE						1			//��ȫ����Ա����
#define		SEC_CODE						2			//��ƹ���Ա����
//--------------------------------------------------------------------
#define		USER_CONFIRM_CODE				0x00		//�û�ȷ��
#define		CHANGE_PWD_CODE					0x01		//��������
#define		SET_SYS_TIME_CODE				0x02		//����ϵͳʱ��
#define		READ_SYS_TIME_CODE				0x03		//��ȡϵͳʱ��
#define		SET_LORA_PARA_CODE				0x04		//����LoRa����
#define		READ_LORA_PARA_CODE				0x05		//��ȡLoRa����
#define		READ_SM4_KEY_CODE				0x06		//��ȡSM4��Կ
#define		READ_RANGDOM_CODE				0x07		//��ȡ�����
#define		READ_OL_CODE					0x08		//��ȡ������־
#define		READ_SL_CODE					0x09		//��ȡϵͳ��־
#define		READ_CL_CODE					0x0A		//��ȡͨ����־
#define		READ_EL_CODE					0x0B		//��ȡ������־
#define		SET_DEST_ID_CODE				0x0C		//����Ŀ���豸��ID
#define		READ_DEST_ID_CODE				0x0D		//��ȡĿ���豸��ID
#define		READ_UART_PARA_CODE				0x0E		//��ȡ���ڲ���
#define		USER_REST_CODE					0x0F		//�ָ���������
#define		STOP_READ_LOG_CODE				0x10		//ֹͣ��ȡ��־
#define		UPDATE_FIRM_CODE				0X11		//�����̼�


//ϵͳ��־����
#define		DEVICE_POWER_CODE				0x00		//�豸�ϵ�
#define		HSC32_CHECK_CODE				0x01		//����оƬ�Լ�
#define		SET_DEVICE_ADDR_CODE			0x03		//�����豸��ַ
#define		START_WTACH_DOG_CODE			0x04		//�������Ź�����
#define		START_HOST_MANGE_CODE			0x05		//������λ��������
#define		STATR_LORA_COMM_CODE			0x06		//����LORAͨ�Ź���
#define 	SET_UART_PARA_CODE				0x07		//���ô��ڲ���
#define		CHECK_ILLEGAL_DISABLE_CODE		0x13		//��⵽�Ƿ���ж
#define		DEVICE_RESET_CODE				0x16		//�ָ���������
#define		LORA_MOD_ERROR					0x17		//LoRaģ���쳣


/***************************************************************************************************
										�洢��־��ַ����
****************************************************************************************************/
#define		MIN_LOG_PACK_SIZE				16		//С��־����С
#define		MAX_LOG_PACK_SIZE				1100	//����־����С
#define		ENCRYPT_DATA					1
#define		DECRYPT_DATA					0

//�������ò�����ַ
#define		SAVE_SET_PARA_ADDR				0X00
//�������ò������ô洢�ռ��С
#define		SAVE_SET_PARA_SIZE				(SAVE_SET_PARA_ADDR+0X19000)			//100KB

#define		SETED_USER_INFO					0x0088
#define		SYS_INFO_ADDR					0X0000
#define  	AUDIT_INFO_ADDR					0X0400
#define		SEC_INFO_ADDR					0X0800
#define		LORA_INFO_ADDR					0X0C00
#define		SAVE_OPERATE_ADDR				0X


//����APP��ַ
#define		UPDATE_APP_ADDR					0X19000
//����APP���ô洢�ռ��С
#define		UPDATE_APP_SIZE					(UPDATE_APP_ADDR+0x7D000)				//500KB

//������־����ַ
#define		OPERATE_LOG_BASE_ADDR			0x96000	
//������־��С
#define		OPERATE_LOG_SZIE				(OPERATE_LOG_BASE_ADDR + 0x35000)		//212KB

//ϵͳ��־����ַ
#define		SYSTEM_LOG_BASE_ADDR			0xCB400		//0XCB000 + 0X400
//ϵͳ��־��С(100*100=10000)
#define		SYSTEM_LOG_SZIE					(SYSTEM_LOG_BASE_ADDR + 0X35000) 		//212KB

//ͨ����־����ַ
#define		COMMUNICATE_LOG_BASE_ADDR		0X100800	//0X100400 +0X400
//ͨ����־��С
#define		COMMUNICATE_LOG_SIZE			(COMMUNICATE_LOG_BASE_ADDR+0x177FA0)	//1100*1400
//#define		COMMUNICATE_LOG_SIZE			(COMMUNICATE_LOG_BASE_ADDR+1100*10)

//������־����ַ
#define		ENCRYPT_LOG_BASE_ADDR			0X278BA0 	//0X2787A0 + 0X400
//������־��С
#define		ENCRYPT_LOG_SZIE				(ENCRYPT_LOG_BASE_ADDR+0x177FA0)		//1100*1400
//#define		ENCRYPT_LOG_SZIE				(ENCRYPT_LOG_BASE_ADDR+1100*10)
/***************************************************************************************************/

typedef enum
{
	Log_Successful = 1,
	Log_Wrong = 0,
	Log_None = 0xf
}_LogResult;


//��־����
typedef enum
{
	OperateLogFlag = 0xDE,
	SystemLogFlag,
	CommunicateLogFlag,
	EncryptFlag,
	NoneLogFlag,
}_LogTypes;


//��־����
typedef enum
{
	LogWriteAddr_Operate = 0x1E,
	LogWriteAddr_System,
	LogWriteAddr_Communicate,
	LogWriteAddr_Encrypt,
	LogWriteAddr_None,
}_LogWriteAddr;


//��������
extern u8 	OperateLogBuf[16];			
extern u8 	SystemLogBuf[16];	
extern u8 	CommunicateLogBuf[1100];		
extern u8  	EncryptLogBuf[1100];			
extern u32 	OperateWriteAddr;			
extern u32	OperateReadAddr;				
extern u32 	SystemWriteAddr;				
extern u32	SystemReadAddr;				
extern u32 	CommunicateWriteAddr;		
extern u32	CommunicateReadAddr;			
extern u32 	EncryptWriteAddr;			
extern u32	EncryptReadAddr;				


void 	ExportWhichLog(u8 LogType,u8 *DestBuf);
void 	RestoreWhichLog(u8 LogType,u8 *SourceBuf,u16 RestoreCode,u8 AdminCode,u8 Result);
void 	RestoreWriteAddr(u8 WhichWriteAddr);
void 	LoadWriteAddrPowerOnDevice(void);
void 	RestoreUserPassword(u8 *PwdBuf, u8 PwdSize,u32 where_addr);

#endif







