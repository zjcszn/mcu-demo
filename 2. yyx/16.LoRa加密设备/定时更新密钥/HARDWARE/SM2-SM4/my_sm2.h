#ifndef __MY_SM2_H
#define __MY_SM2_H

#include "string.h"
#include "sys.h"
#include "hsc32i1.h"
//#include "logs.h"
#include "xLog.h"
#include "dip_switch.h"
/*************************************************
				�ڲ��Ĵ�����ַ����
			   ---------------------
	0	--	�����㷨(10Bytes)
	1	--	���������Կ(200Bytes)
	21	--	����������Կ(200Bytes)
	41	--	����Դ����,����(400bytes)
	81	--	���ܺ�����,����(400Bytes)
	121	--	����Դ����,����(400Bytes)
	161	--	���ܺ�����,����(400Bytes)
**************************************************/

//����Э���ַ����
#define   	SM4_KEYADDR					0		//SM4��Կ
#define		AID_ADDR					2		//������ʶ��
#define		BID_ADDR					4		//�Է���ʶ��
#define		BEFORE_ENCRYPTED_ADDR		6		//����ǰ������
#define		AFTER_ENCRYPTED_ADDR		32		//���ܺ������
#define		BEFORE_DECRYPTED_ADDR		58		//����ǰ������
#define		AFTER_DECRYPTED_ADDR		84		//���ܺ������
#define		WHO_IS_SEND_REV_ADDR		110		//�жϷ��ͷ����ǽ��շ�

typedef enum
{
	Agreement_A = 0x30,
	Agreement_B,
	Agreement_None
}_SM2_Agreement_x;


void 	HSC32i1Init(u8 *id_buf,u16 id_size);
void 	GetSM4Key(u8 *key_buf,u8 key_size);
void 	GetTheAID(u8 *AID_Buf,u16 AID_Size);
void 	SetTheBID(u8 *BID_Buf,u16 BID_Size);
void 	EncryptTheText(u8 *Buf,u16 TextSize);
void 	GetEncryptedText(u8 *Buf,u16 TextSize);
void 	DecryptTheText(u8 *Buf,u16 TextSize);
void 	GetDecryptedText(u8 *Buf,u16 TextSize);
void 	SetSM2AgreementProtocol(void);
void 	SendIdKeyInfo(u8 *buf,u8 size);

#endif




