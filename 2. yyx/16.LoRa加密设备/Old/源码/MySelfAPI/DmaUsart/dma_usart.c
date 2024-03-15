#include "dma_usart.h"


/****************************************************
	* ����������ͨ��DMA1���ڷ�������
	* ��ڲ�����Str -- �ַ�����ַ
				Size -- �ַ�����С
	* ����ֵ��	��
	* ��ע��	��󳤶�(Ӣ��)ΪHOST_TX_SIZE
*****************************************************/
void DMA1_UartSendData(u8 *Str,u16 Size,u8 which_uart)
{
	//���ݳ��Ȳ��Ϸ�
	if(Size<HOST_TX_SIZE)
	{
		switch(which_uart)
		{
			case 1:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//�������ݵ����ͻ�����
				DMA_SetCurrDataCounter(DMA1_Channel4,Size);	//����DMA1�������ݴ�С
				DMA_Cmd(DMA1_Channel4, ENABLE);				//ʹ��ͨ��4����
			break;
			
			case 2:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//�������ݵ����ͻ�����
				DMA_SetCurrDataCounter(DMA1_Channel7,Size);	//����DMA1�������ݴ�С
				DMA_Cmd(DMA1_Channel7, ENABLE);				//ʹ��ͨ��4����
			break;
			
			case 3:
				memcpy(hostcom_struct.Comx_TxBuf,Str,Size);	//�������ݵ����ͻ�����
				DMA_SetCurrDataCounter(DMA1_Channel2,Size);	//����DMA1�������ݴ�С
				DMA_Cmd(DMA1_Channel2, ENABLE);				//ʹ��ͨ��4����
			break;
			
			default:
				//... ...
			break;
		}
	}
	
}




