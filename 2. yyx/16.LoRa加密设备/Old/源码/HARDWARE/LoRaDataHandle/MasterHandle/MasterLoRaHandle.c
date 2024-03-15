#include "MasterLoRaHandle.h"


/**********************************************************************
							������
***********************************************************************/
	void GetSlaveID(u8 *SlaveID_Buf);
	void LoRaCtrlRelayCmd(u8 *data,u16 datalen);
	void LoRaGetIDCmd(u8 *data,u16 datalen);
	void LoRaHandleEncryptedData(void);
	void SplitComDataToEncryptedData(void);
	void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen);
	void LoRaSendEncryptedData(u8 *data,u16 datalen);
	//----------------------------------------------------------
	LoRaStruct	LoRaPact;			//����lora����Э��ṹ��
	u8	LoRaRxBuf[LORA_RX_SIZE];	//������ջ�����
	u8	LoRaTxBuf[LORA_TX_SIZE];	//���巢�ͻ�����
	_ST st = Tx_ST;					//����������״̬ö��
	u16	LoRaTransCounter = 0x00;	//�����ʱ��(��/����ʱ��)
	u16 WaitIDCounter = 0x00;		//�ȴ��Է�ID��ʱ��
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
void MasterHandleSm2KeyAgreement(void)
{
	//��ԿЭ�̳ɹ����˳�����
	if(AgreeStatus != DISABLE)
		return ;
	
	GetSlaveID(OtherID_Buf);	//�����ȡ�ӻ���ID
	if( GetBidStatus )			//�õ��˴ӻ���ID
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
		TimerDelay_Ms(500);
	}
}


/********************************************************
	* ������������ȡ�ӻ���ʶ��
	* ��ڲ�����SlaveID_Buf -- �洢�ӻ���ʶ��������
	* ����ֵ��	��
	* ��ע��	��
*********************************************************/
void GetSlaveID(u8 *SlaveID_Buf)
{
	u8 size;
	
	LoRaGetIDCmd(LoRaPact.TxBuf,0);
	SX1276RxStateEnter();
	
	//�����ʱ����Ӧ�������µȴ�ʱ��
	if(WaitIDCounter > 5000)
		WaitIDCounter = 0x00;
	while( (WaitIDCounter < 5000) && (!DIO0) );
	
	if( DIO0==1 )
	{
		SX1276RxDataReadCRC32( LoRaPact.RxBuf, &size );
		if( (LoRaPact.RxBuf[6]=='O')&&
			(LoRaPact.RxBuf[7]=='K') )
		{
			memcpy(SlaveID_Buf,&LoRaPact.RxBuf[8],16);
			memset(LoRaPact.RxBuf,0,size);
			GetBidStatus = ENABLE;
		}
	}
	//�������ģʽ
	SetLoRaStandBy();
}


/*************************************************
	* ����������LoRa������������
	* ��ڲ�����
	* ����ֵ��
	* ��ע��
**************************************************/
void LoRaMasterMainProcess(void)
{
	//��Կ��δƥ��ɹ�
	if(AgreeStatus !=ENABLE)
		return ;
	
	//δ�л���͸��ģʽ
	if(com_work_mode != ComMode_Trans)
		return;
	
	IsComChangeToAT_WorkMode();
	
	//���ڽ��յ������ҹ����ڴ�͸ģʽ
	if( (hostcom_struct.Comx_RevCompleted == ENABLE) && (com_work_mode == ComMode_Trans) )
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;		//ʹ����һ�����ݴ���
		LoRaPact.TxCnt = hostcom_struct.Comx_RxCnt;		//�õ����յ������ݳ���
		
		//״̬����
		switch(st)
		{
			//��������
			case Tx_ST:
//				LoRaTransCounter = 0x00;
				LoRaHandleEncryptedData();	//���ͼ�������
				st = Rx_ST;					//�������״̬
				SX1276RxStateEnter();		//�������״̬
//				printf("��ʼ����-->����״̬��ʱ��Ϊ --  t3:%dms\r\n", LoRaTransCounter);
//				printf("\r\n");
			break;

			default:
			break;
		}
		
		PackSum 	 = 0;
		PackSumSize  = 0;
		CountPackNum = 0;
		AfterEncryptSzie = 0;
		
		//���������ݺ󣬴�����DMA1���... ...
		hostcom_struct.Comx_RxCnt = 0;							//�����ռ���������
		memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);		//�����ջ��������
		if(hostcom_struct.WhichUart == UART1)
		{
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);	//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ						
			DMA_Cmd(DMA1_Channel5,ENABLE);  					//��DMA����
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);	//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ						
			DMA_Cmd(DMA1_Channel3,ENABLE);  					//��DMA����
		}
	}
	
	if(st == Rx_ST)
	{
		//�ȴ���ʱ(0.5s)���յ����������˳�ѭ��
		while( (LoRaTransCounter < 4500) && (!DIO0) );
//		if(LoRaTransCounter >=4500)
//		{
//			printf("*************��ʱ�ȴ�*************��\r\n");
//		}

		ReceiveData(LoRaPact.RxBuf,&LoRaPact.RxCnt);

		//���Լ1s����һ������
//		LoRaTransCounter = 0;
		st = Tx_ST;
	}
	
}


/*********************************************
	* ��������
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
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			//LoRa���ͼ�������
		
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
				size = PackSumSize - PackSum*LORA_PACK_SIZE;				//�������һ��
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);					//LoRa�������1����������
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


/******************************************
	* ����������
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaGetIDCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;							//֡ͷ
	//���ӳ��ȣ�
	data[1] = 26;							//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);
	crc 	 = CalCrc16(data,24,0xffff);	//��ȡCRCУ��ֵ
	data[24] = crc>>8;						//���ֽ���ǰ
	data[25] = crc;							//���ֽ��ں�
	
	SX1276TxDataCRC32(data,26);				//LoRa��������
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
	
	data[0] = 0x50;							//֡ͷ
	data[1] = 0x09;							//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 1;							//����ָ��0/1	
	crc 	= CalCrc16(data,7,0xffff);		//��ȡCRCУ��ֵ
	data[7] = crc>>8;						//���ֽ���ǰ
	data[8] = crc;							//���ֽ��ں�
	
	SX1276TxDataCRC32(data,16);				//LoRa��������
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






