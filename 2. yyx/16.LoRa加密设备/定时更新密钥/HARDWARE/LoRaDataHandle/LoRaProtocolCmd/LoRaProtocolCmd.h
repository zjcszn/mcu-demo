#ifndef __LORAPROTOCOLCMD_H
#define __LORAPROTOCOLCMD_H

#include "modules_init.h"


void 	LoRaCtrlNetLedCmd(u8 *data,u16 datalen);			//����LEDӦ��
void 	LoRaCtrlNetLedCmdACK(u8 *data,u16 datalen);			//����LEDӦ��
void 	LoRaRequestIDIDCmd(u8 *data,u16 datalen);			//��ȡIDָ��
void 	LoRaRequestIDACK(u8 *data,u16 datalen);				//�ɹ�����IDӦ��
void 	LoRaTimeSynchronizationCmd(u8 *data,u16 datalen);	//ʱ��ͬ��ָ��
void 	LoRaBeforeSendEncryptedData(u8 *data,u16 datalen);	//�������ݰ���Ϣָ��
void 	LoRaSendEncryptedData(u8 *data,u16 datalen);		//���ͼ�������ָ��
void 	LoRaRevEncryptdDataACK(u8 *data,u16 datalen);		//������ɼ�������Ӧ��
void 	SendTheDecryptedData(u8 *buf,u16 datalen);			//���ڴ�ӡ��������
void 	LoRaRequestUpdateSM4KeyCmd(u8 *data,u16 datalen);	//����SM4��Կָ��
void 	LoRaRequestUpdateSM4KeyACK(u8 *data,u16 datalen);	//������ԿӦ��

#endif