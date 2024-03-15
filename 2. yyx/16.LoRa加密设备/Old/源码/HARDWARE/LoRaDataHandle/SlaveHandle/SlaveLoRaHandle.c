#include "SlaveLoRaHandle.h"



/**********************************************************************
						������
***********************************************************************/
	void LoRaCtrlRelayACK(u8 *data,u16 datalen);
	void LoRaRequestIDACK(u8 *data,u16 datalen);
	void LoRaRevEncryptdDataACK(u8 *data,u16 datalen);
	void SplitLoRaDataToDecryptedData(void);

	//----------------------------------------------------------
	extern LoRaStruct	LoRaPact;			//����lora����Э��ṹ��
	extern u8	LoRaRxBuf[LORA_RX_SIZE];	//������ջ�����
	extern u8	LoRaTxBuf[LORA_TX_SIZE];	//���巢�ͻ�����
	extern _ST st;							//����������״̬ö��
	extern u16	LoRaTransCounter;			//�����ʱ��(��/����ʱ��)
	extern u16 	WaitIDCounter;				//�ȴ��Է�ID��ʱ��
	//------------------------------------------------
	extern u8 	SelfID_Buf[16];				//���������ʶ��
	extern u8 	OtherID_Buf[16];			//����Է���ʶ��
	extern u8 	SM4_KEY[16];				//SM4��Կ
	extern u8 	GetBidStatus;				//�õ��Է���ʶ��
	extern u8 	AgreeStatus;				//sm2��ԿЭ�̳ɹ�
	//------------------------------------------------
	extern u8 	EncryptedData[1100];		//������ܺ������
	extern u8 	DecryptedData[1100];		//������ܺ������
	extern u8 	PackSum;					//�ܵİ���
	extern u8 	CountPackNum;				//���ݰ�����
	extern u16 	AfterEncryptSzie;			//���ܺ�����ݳ���
	extern u16 	PackSumSize ;				//�ܵ����ݰ�����
/**********************************************************************/


/**********************************************
	* ����������LoRa�ӻ����䴦��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
***********************************************/
void LoRaSlaveMainPorcess(void)
{
	ReceiveData(LoRaPact.RxBuf, &LoRaPact.RxCnt);			//�������ݰ�
	HandleSlaveLoRaProtocol(LoRaPact.RxBuf,LoRaPact.RxCnt);	//�������ݰ�		
	//�������
	if( LoRaPact.RxCnt )				
	{
		LoRaPact.RxCnt = 0;		//�����ռ���������
		SX1276RxStateEnter();	//�������״̬
	}
	
}


/***************************************************
	* ����������	LoRaЭ�鴦��
	* ��ڲ�����	��
	* ����ֵ��		��
****************************************************/
void HandleSlaveLoRaProtocol(u8 *data,u16 datalen)
{
	u32 tempAddr = 0x00;	
	u16 tempCrc  = 0x00;

	//��ȡ�豸��ַ
	tempAddr = (data[2]<<24)|(data[3]<<16)|(data[4]<<8)|data[5];	

	//���ݰ�����Ϊ0
	if(!datalen)
		return;

	//�ж��豸��ַ�Ƿ�һ��
	if(tempAddr != LoRaPact.DeviceAddr)	
		return;

	//�ж�CRCУ���Ƿ�ͨ��
	tempCrc = CalCrc16(data,datalen-2,0xFFFF);				 
	if(tempCrc != ( (data[datalen - 2]<<8)|data[datalen-1]))
		return;

	//��Ҫ������ƺ��������Ƶ��ƺ��״�
	switch(data[0])
	{
		//LoRaͨѶ����
		case 0x50:
			if(data[6] == 1)						//Ӧ����
				NET_LED = !NET_LED;					//�ı�NET�Ƶ�״̬
			LoRaCtrlRelayACK(LoRaPact.TxBuf,0);		//Ӧ��
		break;
		
		//��ԿЭ��
		case 0x51:
			if( (data[6] == 'I')&&
				(data[7] == 'D') )
			{
				memcpy(OtherID_Buf,&LoRaPact.RxBuf[8],16);
				GetBidStatus = ENABLE;
				LoRaRequestIDACK(LoRaPact.TxBuf,0);
			}
		break;
		
		//���ݴ���ǰ��1��
		case 0x5E:			
			PackSum 	= data[6];				//�ܵİ���
			PackSumSize = (data[7]<<8)|data[8];	//�ܵ����ݰ�����
		break;
		
		//���ռ��ܺ��͸������
		case 0x5F:
			if(  PackSum <= 0 )
			{
				memcpy(EncryptedData,&data[8],data[1]-10);
				
				RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
				SplitLoRaDataToDecryptedData();
				
				PackSumSize = 0;
				LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
			}
			else
			{
				if(CountPackNum <= PackSum)
				{
					memcpy(&EncryptedData[CountPackNum*224],&data[8],data[1]-10);
					CountPackNum ++;
				}
				//�������
				if(CountPackNum > PackSum)
				{
					RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
					SplitLoRaDataToDecryptedData();
					
					//�������
					//�洢������־
					PackSum = 0;
					PackSumSize = 0;
					CountPackNum = 0;
					LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
				}
			}
		break;
		
		//����
		default:
		break;
	}
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);
}


/****************************************
	* ����������SM2��ԿЭ��
	* ��ڲ�����
	* ����ֵ��
	* ��ע��	
*********************************************/
void SlaveHandleSm2KeyAgreement(void)
{
	//��ԿЭ�̳ɹ����˳�����
	if(AgreeStatus != DISABLE)
		return ;
	
	if(GetBidStatus)
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
	}
}


//���������ݲ��ɼ��ܺ������
void SplitLoRaDataToDecryptedData(void)
{
	u8 	i = 0x00;
	u8 	pack_sum = 0x00;
	u8 	remain_Size = 0x00;
	u16 after_decrypt_size= 0x00;
	u32 pack_sum_size = 0x00;
	
	remain_Size = PackSumSize % LORA_PACK_SIZE;		//����ʣ�����ݴ�С
	if( PackSumSize > LORA_PACK_SIZE )
	{
		pack_sum = PackSumSize / LORA_PACK_SIZE;	//�õ��ܵİ���
		
		for(i=0; i<pack_sum; i++)
		{
			DecryptTheText(&EncryptedData[i*224],LORA_PACK_SIZE);
			TimerDelay_Ms(50);
			GetDecryptedText( &DecryptedData[i*224],LORA_PACK_SIZE );
		}
		
		DecryptTheText(&EncryptedData[pack_sum*224],remain_Size);
		TimerDelay_Ms(50);
		if( remain_Size%16 == 0 )
		{
			after_decrypt_size = remain_Size;
			GetDecryptedText( &DecryptedData[pack_sum*224],after_decrypt_size );
		}
		else
		{
			after_decrypt_size = ((remain_Size/16)+1)*16;
			GetDecryptedText( &DecryptedData[pack_sum*224],after_decrypt_size );
		}
		pack_sum_size = pack_sum*LORA_PACK_SIZE + after_decrypt_size;	//�ܵ����ݳ���
	}
	
	else
	{
		DecryptTheText(EncryptedData,PackSumSize);
		TimerDelay_Ms(50);
		
		if( remain_Size%16 == 0 )
		{
			after_decrypt_size = remain_Size;
			GetDecryptedText( DecryptedData,after_decrypt_size );
		}
		else
		{
			after_decrypt_size = ((remain_Size/16)+1)*16;
			GetDecryptedText( DecryptedData,after_decrypt_size );
		}
		pack_sum_size = after_decrypt_size;
	}
	
	//�洢���ܺ������(��д)
	memcpy(EncryptedData,DecryptedData,1100);
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
	
	memset(EncryptedData,0,1100);
	memset(DecryptedData,0,1100);
	memset(EncryptLogBuf,0,1100);
}


void LoRaCtrlRelayACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x50;						//֡ͷ
	data[1] = 0x09;						//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 1;						//ָ��� 0/1
	
	crc 	= CalCrc16(data,7,0xFFFF);	//��ȡCRCУ��ֵ
	data[7] = crc>>8;					//���ֽ���ǰ
	data[8] = crc;						//���ֽ��ں�
	
	EncryptTheText(data,0x09);
	TimerDelay_Ms(50);		
	GetEncryptedText(data,16);
	
	SX1276TxDataCRC32(data,16);			//LoRa��������
}


//��������IDӦ��
void LoRaRequestIDACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;						//֡ͷ
	data[1] = 26;						//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	data[6] = 'O';
	data[7] = 'K';
	memcpy(&data[8],SelfID_Buf,24);
	
	crc 	= CalCrc16(data,24,0xFFFF);//��ȡCRCУ��ֵ
	data[24] = crc>>8;					//���ֽ���ǰ
	data[25] = crc;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,26);			//LoRa��������
}


//������LoRa���ݵ�Ӧ���
void LoRaRevEncryptdDataACK(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5F;								//֡ͷ
	data[1] = 0x08;								//���ݰ�����
	
	data[2] = LoRaPact.DeviceAddr>>24; 		//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;

	crc 	= CalCrc16(data,6,0xFFFF);			//��ȡCRCУ��ֵ
	data[6] = crc>>8;							//���ֽ���ǰ
	data[7] = crc;								//���ֽ��ں�
	
	SX1276TxDataCRC32(data,8);					//LoRa��������
}




 
 

