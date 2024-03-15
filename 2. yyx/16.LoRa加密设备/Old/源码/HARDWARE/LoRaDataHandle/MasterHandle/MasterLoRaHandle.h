#ifndef __MASTERLORAHANDLE_H
#define __MASTERLORAHANDLE_H

#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "relay_init.h"
#include "sx1276.h"
#include "my_sm2.h"
#include "xLog.h"
#include "modules_init.h"

#define			LORA_RX_SIZE		255
#define			LORA_TX_SIZE		255
#define 		INFO_SAVE_ADDR		0x0800F000 		//Flash�洢��Ϣ��ַ
#define			LORA_PACK_SIZE		224


//���嵥�ƿ������ṹ��
typedef struct
{
	u32 DeviceAddr;		//LoRa�豸��ַ
	u32 DstDeviceAddr;	//Ŀ��������ַ
	u8 	*RxBuf;			//���ջ�������ַ
	u8 	RxCnt;			//���ռ�����
	u8 	*TxBuf;			//���ͻ�������ַ
	u16 TxCnt;			//���ͼ�����
} LoRaStruct;	
	
extern LoRaStruct	LoRaPact;	//�����ṹ��
extern u8 	EncryptedData[1100];//������ܺ������
extern u8 	PackSum;			//�ܵİ���
extern u16 	AfterEncryptSzie;	//���ܺ�����ݳ���
extern u16 	PackSumSize;		//�ܵ����ݰ�����
extern u8 	CountPackNum;		//���ݰ�����


void 	SetLoRaStruPara(void);
void 	HandleLoRaProto(u8 *data,u16 datalen);
void 	MasterHandleSm2KeyAgreement(void);
void 	LoRaMasterMainProcess(void);

#endif



