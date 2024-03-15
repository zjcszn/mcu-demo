#include "loraprotocol.h"


/**********************************************************************
							������
***********************************************************************/
	extern u16  LoRaTransCounter;
	extern u8 	SelfID_Buf[16];		//���������ʶ��
	extern u8 	OtherID_Buf[16];	//����Է���ʶ��
	LoRaStruct	LoRaPact;			//����lora����Э��ṹ��
	u8	LoRaRxBuf[LORA_RX_SIZE];	//������ջ�����
	u8	LoRaTxBuf[LORA_TX_SIZE];	//���巢�ͻ�����
	static u8 RelayStatus = ENABLE;	//�̵�����״ֵ̬
	//---------------------------------------------------------
	u8 	EncryptedData[1100] = {0,};	//������ܺ������
	
	u8 	PackSum = 0x00;				//�ܵİ���
	u16 AfterEncryptSzie = 0x00;	//���ܺ�����ݳ���
	u16 PackSumSize = 0x00;			//�ܵ����ݰ�����
	u8 	CountPackNum = 0x00;		//���ݰ�����
/**********************************************************************/


/***************************************************
	* ����������	����LoRa�ṹ�����
	* ��ڲ�����	��
	* ����ֵ��		��
****************************************************/
void SetLoRaStruPara(void)
{
	LoRaPact.RxCnt 	    	= 0x00;
	LoRaPact.TxCnt 	    	= 0x00;
	LoRaPact.RxBuf 	    	= LoRaRxBuf;	
	LoRaPact.TxBuf 	    	= LoRaTxBuf;
	LoRaPact.DeviceAddr 	= ReadDeviceID();	//��ȡ�����豸��
	LoRaPact.DstDeviceAddr 	= 0x09;
	//****3������־(�����豸��ַ)
	RestoreOperateLog(OperateLogBuf,SET_DEVICE_ADDR_CODE,SEC_CODE,Log_Successful);
}


/******************************************
	* �������������Ƽ̵���ָ��
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaGetIDCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;						//֡ͷ
	data[1] = 26;						//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);
	crc 	= CalCrc16(data,24,0xffff);	//��ȡCRCУ��ֵ
	data[24] = crc>>8;					//���ֽ���ǰ
	data[25] = crc;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,26);			//LoRa��������
}


/******************************************
	* �������������Ƽ̵���ָ��
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaCtrlRelayCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x50;						//֡ͷ
	data[1] = 0x09;						//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	//ָ��� 0/1
	if( RelayCounter > 200 )
	{
		RelayCounter = 0;
		data[6] = RelayStatus;			//����ָ��0/1	
		RelayStatus  = !RelayStatus;	//�ı����ָ���ֵ
	}
	
	crc 	= CalCrc16(data,7,0xffff);	//��ȡCRCУ��ֵ
	data[7] = crc>>8;					//���ֽ���ǰ
	data[8] = crc;						//���ֽ��ں�
	EncryptTheText(data,0x09);
	TimerDelay_Ms(100);
	GetEncryptedText(data,16);
	SX1276TxDataCRC32(data,16);			//LoRa��������
}


//�ڷ��ͼ�������֮ǰ
void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5E;							//֡ͷ
	data[1] = 0x0b;							//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRaĿ���豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;						//�ܵİ�����
	data[7] = PackSumSize>>8;				//���ݰ�����
	data[8] = PackSumSize;
	
	crc 	 = CalCrc16(data,9,0xffff);		//��ȡCRCУ��ֵ
	data[9]  = crc>>8;						//���ֽ���ǰ
	data[10] = crc;							//���ֽ��ں�
	
	SX1276TxDataCRC32(data,11);				//LoRa��������
}


void LoRaSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5F;								//֡ͷ
	data[1] = datalen+10;						//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;		//�ܵİ�����
	data[7] = CountPackNum;	//�˿̵İ���
	
	memcpy(&data[8],&EncryptedData[CountPackNum*224],datalen);
	
	crc 	= CalCrc16(data,datalen+8,0xffff);	//��ȡCRCУ��ֵ
	data[datalen+8] = crc>>8;					//���ֽ���ǰ
	data[datalen+9] = crc;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,datalen+10);			//LoRa��������
}


//���������ݲ��ɼ��ܺ������
void SplitComDataToEncryptedData(void)
{
	u8 	i = 0x00;
	u8 	RemainSize = 0x00;
	
	RemainSize = LoRaPact.TxCnt % LORA_PACK_SIZE;		//����ʣ�����ݴ�С
	if( LoRaPact.TxCnt>LORA_PACK_SIZE )
	{
		PackSum = LoRaPact.TxCnt / LORA_PACK_SIZE;		//�õ��ܵİ���
		
		for(i=0; i<PackSum; i++)
		{
			EncryptTheText(&hostcom_struct.Comx_RxBuf[i*224],LORA_PACK_SIZE);
			TimerDelay_Ms(50);
			GetEncryptedText( &EncryptedData[i*224],LORA_PACK_SIZE );
		}
		
		EncryptTheText(&hostcom_struct.Comx_RxBuf[PackSum*224],RemainSize);
		TimerDelay_Ms(50);
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		PackSumSize = PackSum*LORA_PACK_SIZE + AfterEncryptSzie;	//�ܵ����ݳ���
	}
	
	else
	{
		EncryptTheText(hostcom_struct.Comx_RxBuf,LoRaPact.TxCnt);
		TimerDelay_Ms(50);
		
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( EncryptedData,RemainSize );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( EncryptedData,((RemainSize/16)+1)*16 );
		}
		PackSumSize = AfterEncryptSzie;
	}
	//�洢���ܺ������(��д)
	RestoreEncryptLog(EncryptLogBuf,Log_Successful);
}


void LoRaHandleEncryptedData(void)
{
	u8 size = 0x00;
	
	SplitComDataToEncryptedData();					//��ȡ��������
	LoRaBeforeSendEncryptedData(LoRaPact.TxBuf,0);	//�ڷ��ͼ�������ǰ�����ݰ�
	TimerDelay_Ms(5);
	
	//����1��
	if(PackSum <= 0)
	{
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			//LoRa���ͼ�������
		RestoreCommunicateLog(CommunicateLogBuf,Log_Successful);	//�洢LoRa���͵�����
	}
	
	//����Ϊ1��
	else
	{
		while( CountPackNum<PackSum+1 )
		{
			if(CountPackNum >= PackSum )
			{	
				size = PackSumSize - PackSum*LORA_PACK_SIZE;				//�������һ��
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);					//LoRa�������1����������
				RestoreCommunicateLog(CommunicateLogBuf,Log_Successful);	//�洢LoRa���͵�����
				break;
			}
			else
			{
				//��������
				LoRaSendEncryptedData(LoRaPact.TxBuf,LORA_PACK_SIZE);
				CountPackNum ++;
				TimerDelay_Ms(20);
			}
		}
	}
	
}



