/************************************************************************
	* �ļ�˵��������LoRaģ��֮��ͨ��Э�飻
				1.����LED״̬				--	0x50
				2.����LED״̬Ӧ��			--	0x51
				3.��ȡ�Է�ID��ʶ��			--	0x52
				4.��ȡ�Է�ID��ʶ��Ӧ��		--	0x53
				5.ʱ��ͬ��ָ��				--	0x54
				6.ʱ��ͬ��ָ��Ӧ��			--	0x55
				7.�����ܵļ������ݰ���Ϣ	--	0x56
				8.���ͼ�������				--	0x57
				9.�������ܵļ�������Ӧ��	--	0x58
				10.�������SM4��Կָ��		--	0x59

	* ���ߣ�	ZWQ
	* ʱ�䣺	2019-3-2
	* ��ע��
*************************************************************************/
#include "LoRaProtocolCmd.h"


/***************************************************
					������
****************************************************/
	extern u8  SelfID_Buf[16];
	extern u8  DecryptedData[1100];
	extern u8  NetLedStatus;
/***************************************************/


/******************************************
	* ��������������LED״ָ̬��
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaCtrlNetLedCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x50;							//֡ͷ
	data[1] = 0x09;							//���ݰ��ĳ���
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 1;							//����ָ��0/1	
	
	crc_value 	= CalCrc16(data,7,0xffff);	//��ȡCRCУ��ֵ
	data[7] = crc_value>>8;					//���ֽ���ǰ
	data[8] = crc_value;					//���ֽ��ں�
	
	SX1276TxDataCRC32(data,16);				//LoRa��������
}


/******************************************
	* ��������������LED״ָ̬��
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaCtrlNetLedCmdACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x51;							//֡ͷ
	data[1] = 0x09;							//���ݰ��ĳ���
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = NetLedStatus;					//����ָ��0/1	
	
	crc_value = CalCrc16(data,7,0xffff);	//��ȡCRCУ��ֵ
	data[7] = crc_value>>8;					//���ֽ���ǰ
	data[8] = crc_value;					//���ֽ��ں�
	
	SX1276TxDataCRC32(data,16);				//LoRa��������
}


/******************************************
	* ������������ȡ�Է�ID��ʶ��
	* ��ڲ�����data    -- ���ݵ�ַ
				datalen -- ���ݳ���
	* ����ֵ��	��
	* ��ע��	Э�������Ҫ�ı�
*******************************************/
void LoRaRequestIDIDCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x52;								//֡ͷ
	data[1] = 26;								//���ݰ��ĳ���
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';								//��ʶ��
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);				//���������
	
	crc_value 	 = CalCrc16(data,24,0xffff);	//��ȡCRCУ��ֵ
	data[24] = crc_value>>8;					//���ֽ���ǰ
	data[25] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,26);					//LoRa��������
}


//��������IDӦ��
void LoRaRequestIDACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x53;								//֡ͷ
	data[1] = 26;								//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'O';
	data[7] = 'K';
	memcpy(&data[8],SelfID_Buf,24);
	
	crc_value 	 = CalCrc16(data,24,0xFFFF);	//��ȡCRCУ��ֵ
	data[24] = crc_value>>8;					//���� ����ǰ
	data[25] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,26);					//LoRa��������
}


//ʱ��ͬ��ָ��
void LoRaTimeSynchronizationCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0x00;
	
	data[0]  = 0x54;						//֡ͷ
	data[1]  = 0x0F;						//���ݰ�����
	
	data[2]  = LoRaPact.DstDeviceAddr>>24; 	//LoRa�豸��ַ
	data[3]  = LoRaPact.DstDeviceAddr>>16;
	data[4]  = LoRaPact.DstDeviceAddr>>8;
	data[5]  = LoRaPact.DstDeviceAddr>>0;
	
	data[6]  = SysYear>>8;			//��
	data[7]  = SysYear;
	data[8]  = SysMonth;			//��
	data[9]  = SysDate;				//��
	data[10] = SysHour;				//Сʱ
	data[11] = SysMin;				//����
	data[12] = SysSec;				//��
	
	crc_value = CalCrc16(data,13,0xFFFF);
	data[13] = crc_value>>8;				//���ֽ�
	data[14] = crc_value;
	
	SX1276TxDataCRC32(data,15);				//LoRa��������
}


//�����ܵļ������ݰ���Ϣ
void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x56;								//֡ͷ
	data[1] = 0x0B;								//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRaĿ���豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;							//�ܵİ�����
	data[7] = PackSumSize>>8;					//���ݰ�����
	data[8] = PackSumSize;
	
	crc_value 	 = CalCrc16(data,9,0xffff);		//��ȡCRCУ��ֵ
	data[9]  = crc_value>>8;					//���ֽ���ǰ
	data[10] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,11);					//LoRa��������
}


//���ͼ�������
void LoRaSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x57;										//֡ͷ
	data[1] = datalen+10;								//��¼�������ݰ��ĳ���(�����ֽ���)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 				//LoRa�豸��ַ
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;									//�ܵİ�����
	data[7] = CountPackNum;								//�˿̵İ���
	
	memcpy(&data[8],&EncryptedData[CountPackNum*224],datalen);
	
	crc_value 	= CalCrc16(data,datalen+8,0xffff);		//��ȡCRCУ��ֵ
	data[datalen+8] = crc_value>>8;						//���ֽ���ǰ
	data[datalen+9] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,datalen+10);					//LoRa��������
}


//������������ݵ�Ӧ���
void LoRaRevEncryptdDataACK(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x58;								//֡ͷ
	data[1] = 0x08;								//���ݰ�����
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'E';
	data[7] = 'D';
	
	crc_value 	= CalCrc16(data,8,0xFFFF);		//��ȡCRCУ��ֵ
	data[8] = crc_value>>8;						//���ֽ���ǰ
	data[9] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,10);					//LoRa��������
}


//�������SM4��Կָ��
void LoRaRequestUpdateSM4KeyCmd(u8 *data,u16 datalen)
{
	u16 crc_value = 0;
	
	data[0] = 0x59;								//֡ͷ
	data[1] = 0x08;								//���ݰ�����
	
	data[2] = LoRaPact.DeviceAddr>>24; 			//LoRa�豸��ַ
	data[3] = LoRaPact.DeviceAddr>>16;
	data[4] = LoRaPact.DeviceAddr>>8;
	data[5] = LoRaPact.DeviceAddr>>0;
	
	data[6] = 'U';
	data[7] = 'P';
	data[8] = 'K';
	
	crc_value 	 = CalCrc16(data,9,0xFFFF);		//��ȡCRCУ��ֵ
	data[9]  = crc_value>>8;					//���ֽ���ǰ
	data[10] = crc_value;						//���ֽ��ں�
	
	SX1276TxDataCRC32(data,11);					//LoRa��������
}


//���ͽ��ܺ������
void SendTheDecryptedData(u8 *buf,u16 datalen)
{
	memcpy(buf,DecryptedData,datalen);
	
	if(hostcom_struct.WhichUart == UART1)
	{
//		//����1DMA1����
//		DMA_SetCurrDataCounter(DMA1_Channel4,datalen);
//		DMA_Cmd(DMA1_Channel4, ENABLE);
		RS485Uart1SendData(COM1,buf,datalen);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
//		//����3DMA1����
//		DMA_SetCurrDataCounter(DMA1_Channel2,datalen);
//		DMA_Cmd(DMA1_Channel2, ENABLE);
		RS485Uart1SendData(COM3,buf,datalen);
	}
}




