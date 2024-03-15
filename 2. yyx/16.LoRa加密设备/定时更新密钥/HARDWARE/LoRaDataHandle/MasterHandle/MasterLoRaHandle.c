#include "MasterLoRaHandle.h"


/**********************************************************************
							������
***********************************************************************/
	
	
	//----------------------------------------------------------
	extern u16	LoRaTransCounter;	//�����ʱ��(��/����ʱ��)
	extern u16 	WaitIDCounter;		//�ȴ��Է�ID��ʱ��
	extern u8 	UpdateSM4KeyAckFlag;
	LoRaStruct	LoRaPact;			//����LoRa����Э��ṹ��
	u8	LoRaRxBuf[LORA_RX_SIZE];	//������ջ�����
	u8	LoRaTxBuf[LORA_TX_SIZE];	//���巢�ͻ�����
	_ST st = Tx_ST;					//����������״̬ö��
	u8 	NetLedStatus = LED_ON;		//����Ƶ�״̬				
	//------------------------------------------------
	u8 	SelfID_Buf[16]  = {0,};		//���������ʶ��
	u8 	OtherID_Buf[16] = {0,};		//����Է���ʶ��
	u8 	SM4_KEY[16] = {0,};			//SM4��Կ
	u8 	GetBidStatus = DISABLE;		//�õ��Է���ʶ��
	u8 	AgreeStatus  = DISABLE;		//sm2��ԿЭ�̳ɹ�
	//------------------------------------------------
	
	//---------------------------------------------------
	u8 	EncryptedData[1100] = {0,};	//������ܺ������
	u8  DecryptedData[1100] = {0,};	//������ܺ������
	u8 	PackSum = 0x00;				//�ܵİ���
	u8 	CountPackNum = 0x00;		//���ݰ�����
	u16 AfterEncryptSzie = 0x00;	//���ܺ�����ݳ���
	u16 PackSumSize = 0x00;			//�ܵ����ݰ�����
	//---------------------------------------------------
/**********************************************************************/

	
/*************************************************
	* ����������LoRa�ڵ㴦����͸������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	���ӻ��豸����API�ӿ�
**************************************************/
void LoRaNodeHandleHostComData(void)
{
	//��Կ��δƥ��ɹ�
	if(AgreeStatus !=ENABLE)
		return ;
	
	//δ�л���͸��ģʽ
	if(com_work_mode != ComMode_Trans)
		return;
	
	//�ж���λ������״̬�Ƿ��Ѿ��л�
	IsComChangeToAT_WorkMode();
	
	//���ڽ��յ������ҹ����ڴ�͸ģʽ
	if( (hostcom_struct.Comx_RevCompleted == ENABLE) && (com_work_mode == ComMode_Trans) )
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;		//ʹ����һ�����ݴ���
		LoRaPact.TxCnt = hostcom_struct.Comx_RxCnt;		//�õ����յ������ݳ���
		
		LoRaHandleEncryptedData();	//���ͼ�������
		SX1276RxStateEnter();		//�������״̬

		LoRaPact.TxCnt = 0;
		PackSum 	 = 0;
		PackSumSize  = 0;
		CountPackNum = 0;
		AfterEncryptSzie = 0;
		
		//���������ݺ󣬴�����DMA1���... ...
		hostcom_struct.Comx_RxCnt = 0;						//�����ռ���������
		memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);	//�����ջ��������
		if(hostcom_struct.WhichUart == UART1)
		{
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);		
			//��ͨ��5����			
			DMA_Cmd(DMA1_Channel5,ENABLE);  					
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);			
			//��ͨ��3����			
			DMA_Cmd(DMA1_Channel3,ENABLE);  					
		}
	}
	
}


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
	
	//****����ϵͳ��־(�����豸��ַ)
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,SET_DEVICE_ADDR_CODE,0,Log_Successful);
}


/****************************************
	* ����������SM2��ԿЭ��
	* ��ڲ�����
	* ����ֵ��
	* ��ע��	
*********************************************/
void LoRaMasterHandleSm2KeyAgreement(void)
{
	//��ԿЭ�̳ɹ����˳�����
	if(AgreeStatus != DISABLE)
		return ;
	
	//�õ��˴ӻ���ID
	if( GetBidStatus )			
	{
		GetBidStatus        = DISABLE;	//���û�ȡ�Է���ʶ����־
		UpdateSM4KeyFlag    = DISABLE;	//���ø�����Կ��־
		UpdateSM4KeyAckFlag = DISABLE;	//���ø�����ԿӦ���־
		PackSumSize = 0;				//���ý������ݰ��ĳ���
		AgreeStatus = ENABLE;			//��λ��ԿЭ�̱�־
		
		SetTheBID(OtherID_Buf,16);						//��ʼ����SM4��Կ
		TimerDelay_Ms(150);								//���Լ���ʱ�䣿
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);	//����SM4��Կ
	
		EnableDMA1Channelx(hostcom_struct.WhichUart);	//��DMA1ͨ��
		SX1276RxStateEnter();
	#if 1
		//������Կ
		RS485Uart1SendData(hostcom_struct.WhichUart,SM4_KEY,16);
	#endif
	}
	
}


/********************************************************
	* ������������ȡ�ӻ���ʶ��
	* ��ڲ�����SlaveID_Buf -- �洢�ӻ���ʶ��������
	* ����ֵ��	��
	* ��ע��	��
*********************************************************/
void GetSlaveID(void)
{
	if( AgreeStatus != DISABLE )
		return ;
	
	LoRaRequestIDIDCmd(LoRaPact.TxBuf,0);
	SX1276RxStateEnter();
}


/*********************************************
	* ����������LoRa�����������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
**********************************************/
void LoRaHandleEncryptedData(void)
{
	u8 size = 0x00;
	
	SplitComDataToEncryptedData();					//��ȡ��������
	LoRaBeforeSendEncryptedData(LoRaPact.TxBuf,0);	//�ڷ��ͼ�������ǰ�����ݰ�
	TimerDelay_Ms(5);
	
	//����1��
	if(PackSum <= 0)
	{
		//LoRa���ͼ�������
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			
		
		//�洢LoRa���͵�����
		RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
	}
	
	//����Ϊ1��
	else
	{
		while( CountPackNum<PackSum+1 )
		{
			if(CountPackNum >= PackSum )
			{	
				size = PackSumSize - PackSum*LORA_PACK_SIZE;	//�������һ��
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);		//LoRa�������1����������
				
				//�洢LoRa���͵�����
				RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
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
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
}




