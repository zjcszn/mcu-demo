#include "LoRaDeviceTask.h"


/**************************************************
	* ����������LoRa�����豸������
	* ��ڲ�����
	* ����ֵ��
	* ��ע��
***************************************************/
void TheLoRaEncryptDeviceTask(void)
{	
	//Master��������
	if( LoRaDeviceIs == MASTER )
	{
//		WatchDogHandle();					//���Ź�����
		//-----------------------------------------------
		ScanComStatusKey();					//��⴮��״̬�л�����
		//---------------------------------------------------------
		HostComTimeOutHandle();				//��λ����ʱ�ȴ�
		//---------------------------------------------------
		ExportLogHandle();					//������־����
		//------------------------------------------------
		LoRaNodeHandleCommunicateData();	//LoRa�ڵ㴦��ͨ������
		LoRaNodeHandleHostComData();		//LoRa�ڵ㴦����λ����������
		//---------------------------------------------------------------
		HostComProtocol(&hostcom_struct);	//������λ��ָ������
		//-------------------------------------------------------
		IndicatorLightMainHandle();			//����ϵͳָʾ��
//		TheTimeUptoUpdateSM4KeyHandle();	//������Կ
	}
	
	//Slave��������
	else if( LoRaDeviceIs == SLAVE )
	{
//		WatchDogHandle();					//���Ź�����
		//-----------------------------------------------
		ScanComStatusKey();					//��⴮��״̬�л�����
		//---------------------------------------------------------
		HostComTimeOutHandle();				//��λ����ʱ�ȴ�
		//---------------------------------------------------
		ExportLogHandle();					//������־����
		//------------------------------------------------
		LoRaNodeHandleCommunicateData();	//LoRa�ڵ㴦��ͨ������
		LoRaNodeHandleHostComData();		//LoRa�ڵ㴦����λ����������
		LoRaSlaveHandleSm2KeyAgreement();	//�ӻ���ԿЭ��
		//---------------------------------------------------------------
		HostComProtocol(&hostcom_struct);	//������λ��ָ������
		//-------------------------------------------------------
		IndicatorLightMainHandle();			//����ϵͳָʾ��
	}
}





