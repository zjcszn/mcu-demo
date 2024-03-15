/***********************************************************************************************
	* �ļ�˵�������ļ���Դ�������Ƶ�����ͣ���ʱ�����ݻ����������Դ��������ͣ�
				ʵ��ԭ��Ϊ���в�����
	* ���ߣ�	ZWQ
	* ʱ�䣺	2019-3-1
	* ��ע��	ע����LoRaΪ��˫����������ʵ��α��˫����Ҫ��MCU�Ĵ��ھ߱�ȫ˫�����ܡ�
				�ⲿ�����豸�߱�ȫ˫�����ڴ�ǰ����ʹ��ѭ�����л������ݣ�����ʱ��Ƭ��ԭ��
				�������ݣ�����λ��Э����ʱ�䡣
*************************************************************************************************/
#include "HostComDataHandle.h"


/**************************************************************************
						������
***************************************************************************/
		_ComDataQueue com_data_queue;
/**************************************************************************/


/***************************************************
	* �����������������ݶ��г�ʼ��
	* ��ڲ�����_ComDataQueue -- �ṹ��ָ��
	* ����ֵ��	
	* ��ע��	
****************************************************/
void ComDataQueueParaInit(_ComDataQueue *_com_data_queue)
{
	_com_data_queue->QueueCount      = 0x00;
	_com_data_queue->QueueNowPoint   = 0x00;
	_com_data_queue->EnterOffsetAddr = 0x00;
	_com_data_queue->OutOffsetAddr   = 0x00;
}


/*********************************************
	* �����������������
	* ��ڲ�����
	* ����ֵ��
	* ��ע��
**********************************************/
void EnterComDataQueue(u8 *SourceBuf,_ComDataQueue *_com_data_queue,u16 DataLen)
{
	//�ж϶�������
	if( _com_data_queue->QueueCount >= TEMP_COM_PACK_SIZE )
	{
		_com_data_queue->QueueCount = 0;
		_com_data_queue->EveryPackSize[0] = 0x00;
		_com_data_queue->EnterOffsetAddr = 0x00;
	}
	
	//��¼��������
	_com_data_queue->QueueCount ++;
	
	//���洮������
	if(DataLen<130)
	{
		if(_com_data_queue->QueueCount<=1)
		{
			memcpy( &_com_data_queue->TempComDataBuffer[0],
					SourceBuf,
					DataLen);
		}
		else
		{
			_com_data_queue->EnterOffsetAddr += _com_data_queue->EveryPackSize[_com_data_queue->QueueCount-2];
			
			//ʹ����һ����ֵ���е�ַƫ��
			memcpy( &_com_data_queue->TempComDataBuffer[_com_data_queue->EnterOffsetAddr],
					SourceBuf,
					DataLen);
		}
	}
		
	//��¼��ǰ���ݰ��Ĵ�С
	_com_data_queue->EveryPackSize[_com_data_queue->QueueCount-1] = hostcom_struct.Comx_RxCnt;
}



/****************************************************
	* ����������������
	* ��ڲ�����
	* ����ֵ��
	* ��ע
******************************************************/
void OutComDataQueue(_ComDataQueue *_com_data_queue,u8 *DestBuf)
{
	//�ж�ȫ��������
	if(_com_data_queue->QueueNowPoint >= TEMP_COM_PACK_SIZE)
	{
		_com_data_queue->QueueNowPoint = 0x00;
		_com_data_queue->OutOffsetAddr = 0x00;
	}
	
	//ƫ�ƶ���ָ��
	_com_data_queue->QueueNowPoint ++;
	
	if(_com_data_queue->QueueNowPoint<=1)
	{
		memcpy(	DestBuf,
				&_com_data_queue->TempComDataBuffer[0],
				_com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-1]);
	}

	else
	{
		_com_data_queue->OutOffsetAddr += _com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-2];
		
		//ʹ����һ�ε�ƫ�ƣ���СΪ��ǰ�ļ�¼
		memcpy(	DestBuf,
				&_com_data_queue->TempComDataBuffer[_com_data_queue->OutOffsetAddr],
				_com_data_queue->EveryPackSize[_com_data_queue->QueueNowPoint-1]);
	}	
}


//���洮������
void SaveHostComDataToQueueHandle()
{
	if(hostcom_struct.Comx_RevCompleted == ENABLE)
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;
		EnterComDataQueue(hostcom_struct.Comx_RxBuf,&com_data_queue,hostcom_struct.Comx_RxCnt);
		
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





