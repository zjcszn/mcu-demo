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
	if(LoRaDeviceIs == MASTER)
	{
		ScanComStatusKey();
		HostComTimeOutHandle();				//��λ����ʱ�ȴ�
		ExportLogHandle();					//������־����
		LoRaMasterMainProcess();			//LoRa������������
		HostComProtocol(&hostcom_struct);	//����λ����������
		IndicatorLightMainHandle();			//����ϵͳָʾ��
	}
	
	//Slave��������
	else if( LoRaDeviceIs == SLAVE )
	{
		ExportLogHandle();					//������־����
		LoRaSlaveMainPorcess();				//LoRa�ӻ�Э���������
		SlaveHandleSm2KeyAgreement();		//�ӻ���ԿЭ��
		HostComProtocol(&hostcom_struct);	//������λ��ָ������
		IndicatorLightMainHandle();			//����ϵͳָʾ��
	}
}



