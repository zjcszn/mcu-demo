#include "modules_init.h"

extern u8 SelfID_Buf[16];
float VoltageValue = 0.0;

//Ӳ��ģ���ʼ��
void ModulesInit(void)
{
	SysNVIC_Config();				//�����жϷ���Ϊ2(2:2)
	delay_init();					//��ʼ����ʱ����(���õδ�ʱ��)
	LED_Init();						//��ʼ��LEDӲ��		
//	WatchDogPinInit();				//���Ź���ʼ��
	DIP_SwitchInit();				//���뿪�س�ʼ��
	ComStatusChangeKeyInit();		//��ʼ������״̬�л�����
	AlamKeyPinInit();				//��ʼ����������
//	Adc_Init();						//++
	OrdinaryTimerInit(3,10,7200);	//��ʼ����ʱ��3
	AdvanceTimerInit(1,10,7200);	//��ʼ����ʱ��1(1ms)
	RTC_Init();						//��ʼ���ڲ�ʱʱ��
	LoadWriteAddrPowerOnDevice();	//������һ�ε���־д���ַ
	W25QxxFlashInit();				//��ʼ��W25QxxоƬ
	HostComSoftWareInit();			//��ʼ����λ�����
	MyLoRaInit();					//��ʼ��LoRaģ��
	HSC32i1Init(SelfID_Buf,16);		//��ʼ��HSC32�Լ�SM2��ԿЭ��
	TimerDelay_Ms(500);
//	VoltageValue = Get_Adc_Average(15,10);
	//****����ϵͳ��־(�豸�ϵ�)
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,DEVICE_POWER_CODE,0,Log_Successful);
}






