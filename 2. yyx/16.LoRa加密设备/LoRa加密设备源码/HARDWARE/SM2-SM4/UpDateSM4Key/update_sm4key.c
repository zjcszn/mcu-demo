#include "update_sm4key.h"


extern void GetTheAID(u8 *AID_Buf,u16 AID_Size);
extern u8 SelfID_Buf[16];
extern u8 	AgreeStatus;

u8 	UpdateSM4KeyFlag   = 0;
u16	CalcOneHourCounter = 0x00;
u16 UpdatedCounter = 0x00;		//��¼���´���
char UpdateTempBuf[30] ={ 0,};

/****************************************************
	* ��������������SM4��Կ
	* ��ڲ�����
	* ����ֵ��
	* ��ע��
*****************************************************/
void TheTimeUptoUpdateSM4KeyHandle(void)
{
	//��⵽������Կ��־
	if( UpdateSM4KeyFlag==ENABLE )
	{
		UpdateSM4KeyFlag = DISABLE;
		
		AgreeStatus = DISABLE;							//������ԿЭ�̱�־								
		LoRaRequestUpdateSM4KeyCmd(LoRaPact.TxBuf,0);	//֪ͨ�Է�����SM4��Կ
		RestHSC32Module();								//��λ����ģ��
		SetSM2AgreementProtocol();			 			//������ԿЭ�̷�
		TimerDelay_Ms(20);
		GetTheAID(SelfID_Buf,16);						//���»�ȡ���������
		TimerDelay_Ms(50);
		LoRaMasterHandleSm2KeyAgreement();				//������ԿЭ��
		
		//��ӡ���´���
		UpdatedCounter ++;
		sprintf(UpdateTempBuf,"Successful:%d\r\n",UpdatedCounter);
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8 *)UpdateTempBuf,25);
	}
}


void RunCalcOneHourCounter(void)
{
	if( CalcOneHourCounter>UPDATE_SM4_TIME_VALUE )
	{
		CalcOneHourCounter = 0x00;
//		HostComSoftwareReset();
		UpdateSM4KeyFlag = ENABLE;
	}
	else
		CalcOneHourCounter ++;
}


void DisableDMA1Channelx(u8 _which_uart )
{
	switch(_which_uart)
	{
		case UART1:
//			//�رմ���1
//			USART_Cmd(USART1,DISABLE);
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);		
			//�ر�ͨ��5����			
			DMA_Cmd(DMA1_Channel5,DISABLE);  
		break;
		
		case UART3:
//			//�رմ���3
//			USART_Cmd(USART3,DISABLE);
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);			
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
//			//��������1
//			USART_Cmd(USART1,ENABLE);
			//�ر�ͨ��5����			
			DMA_Cmd(DMA1_Channel5,ENABLE);  
		break;
		
		case UART3:
//			//��������3
//			USART_Cmd(USART3,ENABLE);	
			//�ر�ͨ��3����			
			DMA_Cmd(DMA1_Channel3,ENABLE);  	
		break;
		
		default:
		break;
	}
}


void HostComSoftwareReset(void)
{
	u16 Temp;
	
	Temp = USART1->SR;								//�ȶ�SR,Ȼ��DR�������
	Temp = USART1->DR;								//��USART_IT_IDLE��־
	DisableDMA1Channelx(hostcom_struct.WhichUart);
	hostcom_struct.Comx_RxCnt = 0;					//�����յ��ֽڳ��ȸ�ֵ
}


//���ݴ˿̴��ڽ��յ�������
void BackUpRevComDataRightNow()
{
	
}




