#include "update_sm4key.h"


extern void GetTheAID(u8 *AID_Buf,u16 AID_Size);

/*****************************************************************
							������
******************************************************************/
	extern u8 SelfID_Buf[16];
	extern u8 SM4_KEY[16];
	extern u8 AgreeStatus;
	extern u8 GetBidStatus;
	//-------------------------
	u8 	UpdateSM4KeyFlag    = 0;		//��ʼ������Կ��־
	u8  SoftWareResetFlag   = DISABLE;	//�����λ��־
	u8 	UpdateSM4KeyAckFlag = DISABLE;	//������ԿӦ���־(�ӻ�)
	u32	CalcOneHourCounter  = 0x00;		//������Կ��ʱ��
/*****************************************************************/


/****************************************************
	* ��������������SM4��Կ
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��
*****************************************************/
void TheTimeUptoUpdateSM4KeyHandle(void)
{
	//��⵽������Կ��־
	if( UpdateSM4KeyFlag!=ENABLE )
		return ;

	//�����λ����(���µ���ִֻ��1��)
	if( SoftWareResetFlag )
	{
		SoftWareResetFlag = DISABLE;	//���������λ��־
		AgreeStatus       = DISABLE;	//������ԿЭ�̱�־	

	#if 1
		HostComSoftwareReset(hostcom_struct.WhichUart);	//��λ��λ�����ڲ���
		LoRaSoftWareParaReset();						//��λLoRa�������
	#endif

		RestHSC32Module();				//��λ����ģ
		SetSM2AgreementProtocol();		//������ԿЭ�̷�
		TimerDelay_Ms(10);				//��ʱ10ms��ȡ����
		GetTheAID(SelfID_Buf,16);		//���»�ȡ���������

	#if TEST_AGREE
		//���ͱ�����ʶ��
		RS485Uart1SendData(hostcom_struct.WhichUart,SelfID_Buf,16);
	#endif
	}
	
	//��������ԿӦ���־
	if( UpdateSM4KeyAckFlag!=ENABLE )
	{
		//?֪ͨ�Է�����SM4��Կ(�����ʱ�ӻ����ڷ��ͼ������ݣ���ν����ͻ?)
		LoRaRequestUpdateSM4KeyCmd(LoRaPact.TxBuf,0);	
		SX1276RxStateEnter();
		TimerDelay_Ms(160);
	}
}


//�ط���ȡ�ӻ���ʶ���Ļ���
void ResendGetSlaveIdCmdProcess(void)
{
	//��������ԿӦ���־
	if( UpdateSM4KeyAckFlag!=ENABLE )
		return ;
	
	if( GetBidStatus==ENABLE )
		return;
	
	GetSlaveID();	//��ȡ�ӻ��ı�ʶ��	
	TimerDelay_Ms(100);
}


//����1Сʱ��ʱ��
void RunCalcOneHourCounter(void)
{
	if(LoRaDeviceIs!=MASTER)
		return;
	
	if( CalcOneHourCounter>UPDATE_SM4_TIME_VALUE )
	{
		CalcOneHourCounter = 0x00;		//��1Сʱ��ʱ������
		UpdateSM4KeyFlag   = ENABLE;	//��λ������Կ��־
		SoftWareResetFlag  = ENABLE;	//��λ�����λ��־	
	}
	else
		CalcOneHourCounter ++;			//��ʱ
}


void DisableDMA1Channelx(u8 _which_uart)
{
	switch(_which_uart)
	{
		case UART1:		
			//�ر�ͨ��5����			
			DMA_Cmd(DMA1_Channel5,DISABLE);  
		break;
		
		case UART3:		
			//�ر�ͨ��3����			
			DMA_Cmd(DMA1_Channel3,DISABLE);  	
		break;
		
		default:
		break;
	}
}


void EnableDMA1Channelx(u8 _which_uart)
{
	switch(_which_uart)
	{
		case UART1:
			Usart_X_Init(COM1);
			DMA_RX_Init(COM1,hostcom_struct.Comx_RxBuf);	//DMA1����1��ʼ��
			DMA_TX_Init(COM1,hostcom_struct.Comx_TxBuf);	//DMA1����1��ʼ�� 
		break;
		
		case UART3:	
			Usart_X_Init(COM3);
			DMA_RX_Init(COM3,hostcom_struct.Comx_RxBuf);	//DMA1����1��ʼ��
			DMA_TX_Init(COM3,hostcom_struct.Comx_TxBuf);	//DMA1����1��ʼ��	
		break;
		
		default:
		break;
	}
}


//��λ��λ�������������
void HostComSoftwareReset(u8 _which_uart)
{
	u16 Temp;
	
	if( _which_uart == UART1)
	{
		Temp = USART1->SR;			//�ȶ�SR,Ȼ��DR�������
		Temp = USART1->DR;			//��USART_IT_IDLE��־
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_Cmd(USART1,DISABLE);
	}
	else if( _which_uart == UART3)
	{
		Temp = USART3->SR;			//�ȶ�SR,Ȼ��DR�������
		Temp = USART3->DR;			//��USART_IT_IDLE��־
		USART_ClearFlag(USART3, USART_FLAG_TC);
		USART_Cmd(USART3,DISABLE);
	}
	
	hostcom_struct.Comx_RevCompleted = DISABLE;
	memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);
	hostcom_struct.Comx_RxCnt = 0;	//�����յ��ֽڳ��ȸ�ֵ
}


void LoRaSoftWareParaReset(void)
{
	LoRaPact.RxCnt = 0x00;
	LoRaPact.TxCnt = 0x00;
	memset(LoRaPact.RxBuf,0,LORA_RX_SIZE);
	memset(LoRaPact.TxBuf,0,LORA_TX_SIZE);
}




