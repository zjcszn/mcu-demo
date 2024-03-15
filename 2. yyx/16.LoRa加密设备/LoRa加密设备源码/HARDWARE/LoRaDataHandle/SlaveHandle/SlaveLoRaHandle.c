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
	extern u8  NetLedStatus;
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
	u8 	TimeSynchronizeFlag = DISABLE;		//ʱ��ͬ����־
	u32	AckCounter = 0x00;					//Ӧ�������
/**********************************************************************/


/**********************************************
	* ����������LoRa�ڵ㴦��ģ��ͨ������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
***********************************************/
void LoRaNodeHandleCommunicateData(void)
{
	ReceiveData(LoRaPact.RxBuf, &LoRaPact.RxCnt);					//�������ݰ�
	HandleLoRaCommunicateProtocol(LoRaPact.RxBuf,LoRaPact.RxCnt);	//�������ݰ�		
}


/***************************************************
	* ����������	LoRaͨ��Э�鴦��
	* ��ڲ�����	��
	* ����ֵ��		��
	* ��ע��	
****************************************************/
void HandleLoRaCommunicateProtocol(u8 *data,u16 datalen)
{
	u8  led_status = 0x00;
	u16 tempCrc  = 0x00;
	u32 tempAddr = 0x00;
	
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
		//����LED״̬
		case 0x50:
			if(data[6] == 1)		//Ӧ����
				NET_LED = !NET_LED;	//�ı�NET�Ƶ�״̬
			
			if(NET_LED==1)
				NetLedStatus = LED_OFF;
			else
				NetLedStatus = LED_ON;
			
			//Ӧ���
			LoRaCtrlNetLedCmdACK(LoRaPact.TxBuf,0);
		break;
		
		//����LEDӦ���
		case 0x51:	
			led_status = data[6];	//��ȡ�Է���LED״̬
		break;
		
		//��ԿЭ��
		case 0x52:
			if( (data[6] == 'I')&&
				(data[7] == 'D') )
			{
				memcpy(OtherID_Buf,&LoRaPact.RxBuf[8],16);
				GetBidStatus = ENABLE;
				LoRaRequestIDACK(LoRaPact.TxBuf,0);
			}
		break;
		
		//��ȡ�Է���IDӦ��
		case 0x53:
			//... ...
		break;
		
		//ʱ��ͬ��
		case 0x54:
			SysYear  = data[6]<<8 | data[7];
			SysMonth = data[8];
			SysDate  = data[9];
			SysHour	 = data[10];
			SysMin	 = data[11];
			SysSec	 = data[12];
			RTC_Set(SysYear,SysMonth,SysDate,SysHour,SysMin,SysSec);
			
			//��λʱ��ͬ����־
			TimeSynchronizeFlag = ENABLE;
		break;
		
		//ʱ��ͬ��Ӧ��
		case 0x55:
			//... ...
		break;
		
		//���ݴ���ǰ��1��
		case 0x56:			
			PackSum 	= data[6];				//�ܵİ���
			PackSumSize = (data[7]<<8)|data[8];	//�ܵ����ݰ�����
		break;
		
		//���ռ��ܺ��͸������
		case 0x57:
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
					//�洢ͨ����־
					RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
					
					//�������
					SplitLoRaDataToDecryptedData();
					
					//��������
					PackSum = 0;
					PackSumSize = 0;
					CountPackNum = 0;
					LoRaRevEncryptdDataACK(LoRaPact.TxBuf,0);
				}
			}
		break;
		
		//LoRa�������ʱ
		case 0x58:
			//����״̬��
			if( st==Rx_ST )
			{
				if( (data[6] == 'E')&&
					(data[7] == 'D') )
				{
					AckCounter ++;
//					hostcom_struct.Comx_TxBuf[0] = AckCounter>>24;
//					hostcom_struct.Comx_TxBuf[1] = AckCounter>>16;
//					hostcom_struct.Comx_TxBuf[2] = AckCounter>>8;
//					hostcom_struct.Comx_TxBuf[3] = AckCounter;
//					RS485Uart1SendData(hostcom_struct.WhichUart,hostcom_struct.Comx_TxBuf,4);
				}
			}
		break;
			
			
		//�������SM4��Կ
		case 0x59:
			if( (data[6] == 'U')&&
				(data[7] == 'P')&&
				(data[8] == 'K' ) )
			{
				AgreeStatus = DISABLE;							//����Э�̱�־
				
				HostComSoftwareReset();							//
				RestHSC32Module();								//��λ����ģ��
				SetSM2AgreementProtocol();						//������ԿЭ�̷�
				TimerDelay_Ms(50);
				GetTheAID(SelfID_Buf,16);						//��ȡ���������				
			}
		break;
			
		//����
		default:
		break;
	}
	
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);	//�����ջ���������
	LoRaPact.RxCnt = 0;						//�����ռ���������
	SX1276RxStateEnter();					//LoRa�л�Ϊ����״̬
}


/****************************************
	* ����������SM2��ԿЭ��
	* ��ڲ�����
	* ����ֵ��
	* ��ע��	
*********************************************/
void LoRaSlaveHandleSm2KeyAgreement(void)
{
	//��ԿЭ�̳ɹ����˳�����
	if(AgreeStatus != DISABLE)
		return ;
	
	if( GetBidStatus )
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);	//++
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
		
//		//�������״̬
//		SX1276RxStateEnter();							
		//��DMA1ͨ��
		EnableDMA1Channelx(hostcom_struct.WhichUart);
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
	
	SendTheDecryptedData(hostcom_struct.Comx_TxBuf,PackSumSize);	//��ӡ���ܺ������
	memcpy(EncryptedData,DecryptedData,1100);						//�洢���ܺ������(��д)
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
	
	memset(EncryptedData,0,1100);
	memset(DecryptedData,0,1100);
	memset(EncryptLogBuf,0,1100);
}





