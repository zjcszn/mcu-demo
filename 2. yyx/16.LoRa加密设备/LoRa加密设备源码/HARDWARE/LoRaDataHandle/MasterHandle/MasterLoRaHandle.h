#ifndef __MASTERLORAHANDLE_H
#define __MASTERLORAHANDLE_H


#include "modules_init.h"
#include "LoRaProtocolCmd.h"


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
void 	LoRaMasterHandleSm2KeyAgreement(void);
void 	LoRaSendEncryptedData(u8 *data,u16 datalen);
void 	LoRaHandleEncryptedData(void);
void 	SplitComDataToEncryptedData(void);
void 	LoRaBeforeSendEncryptedData(u8 *data,u16 datalen);
void 	LoRaNodeHandleHostComData(void);
void 	LoRaMasterHandleCommunicateData(void);

#endif



