/*************************************************************************
	* �ļ����������ļ��Ƕ����ݵļӽ��ܴ�����������Ϊ����оƬ��
	* ���ߣ�	ZWQ
	* ʱ�䣺	2019-1-4
	* ��ע��	API�����޸�
**************************************************************************/
#include "my_sm2.h"

extern u8 SelfID_Buf[16];
static u8 Send[2] = {0x30,};
static u8 Rev[2]  = {0x31,};
u8 KeyTempBuf[20] = {0,};


/*************************************************
	* ����������HSC32������ʼ���Լ�
				sm2��ԿЭ�̳�ʼ��
	* ��ڲ�����id_buf  -- ��ʶ����ַ
				id_size -- ��ȡ���ݰ��ĳ���
	* ����ֵ��	��
	* ��ע��	��ʶ���ݶ�Ϊ64B
**************************************************/
void HSC32i1Init(u8 *id_buf,u16 id_size)
{
	HSC32I1BaseInit();
	delay_ms(10);
	SetSM2AgreementProtocol();
	delay_ms(10);
	GetTheAID(id_buf,id_size);
	
#if TEST_AGREE
	//���ͱ�����ʶ��
	RS485Uart1SendData(hostcom_struct.WhichUart,SelfID_Buf,16);//++
#endif
	
	//****1����оƬ�Լ�
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,HSC32_CHECK_CODE,0,Log_Successful);
}


void GetSM4Key( u8 *key_buf,u8 key_size)
{
	//��ȡЭ�̺��sm4��Կ(�ݶ�Ϊ64Bytes)
	HSC32I1_ReadLenByte(SM4_KEYADDR,key_buf,key_size);
}


void GetTheAID(u8 *AID_Buf,u16 AID_Size)
{
	//��ȡ���ͷ��ı�ʶ��(�ݶ�Ϊ64Bytes)
	HSC32I1_ReadLenByte(AID_ADDR,AID_Buf,AID_Size);
}

 
void SetTheBID(u8 *BID_Buf,u16 BID_Size)
{
	//�����������
	HSC32I1_WriteLenByte(BID_ADDR,BID_Buf,BID_Size);
}
 
void EncryptTheText(u8 *Buf,u16 TextSize)
{
	//�����������
	HSC32I1_WriteLenByte(BEFORE_ENCRYPTED_ADDR,Buf,TextSize);
}


void GetEncryptedText(u8 *Buf,u16 TextSize)
{
	//��ȡ���ܺ������
	HSC32I1_ReadLenByte(AFTER_ENCRYPTED_ADDR,Buf,TextSize);
}


void DecryptTheText(u8 *Buf,u16 TextSize)
{
	//�����������
	HSC32I1_WriteLenByte(BEFORE_DECRYPTED_ADDR,Buf,TextSize);
}


void GetDecryptedText(u8 *Buf,u16 TextSize)
{
	//��ȡ���ܺ������
	HSC32I1_ReadLenByte(AFTER_DECRYPTED_ADDR,Buf,TextSize);
}


/*******************************************
	* �����������������ӻ�����ԿЭ��Э��
	* ��ڲ�����TheDeviceIs -- �豸����
	* ����ֵ��	��
	* ��ע��
********************************************/
void SetAgreementX(u8 TheDeviceIs)
{
	switch( TheDeviceIs )
	{
		//������ԿЭ��Э��
		case MASTER:
			HSC32I1_WriteLenByte(WHO_IS_SEND_REV_ADDR,Send,1);
		break;
		
		//�ӻ���ԿЭ��Э��
		case SLAVE:
			HSC32I1_WriteLenByte(WHO_IS_SEND_REV_ADDR,Rev,1);
		break;
		
		default:
		break;
	}
	
}


/**********************************************
	* ��������������SM2Э�̺����ӿ�����
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��
***********************************************/
void SetSM2AgreementProtocol(void)
{
	switch( LoRaDeviceIs )
	{
		case MASTER:
			SetAgreementX(MASTER);
		break;
		
		case SLAVE:
			SetAgreementX(SLAVE);
		break;
		
		default:
		break;
	}
}


void SendIdKeyInfo(u8 *buf,u8 size)
{
	memcpy(KeyTempBuf,buf,size);
	KeyTempBuf[size]   = 0x0d;
	KeyTempBuf[size+1] = 0x0a;
	RS485Uart1SendData(hostcom_struct.WhichUart,KeyTempBuf,size+2);
}


