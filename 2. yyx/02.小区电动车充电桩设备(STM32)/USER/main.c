#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "spi.h"
#include "w25qxx.h"
#include "key.h" 
#include "timer.h"
#include "Current.h"
#include "WatchDog.h"
#include "BL8025.h" 
#include "RS485.h" 
#include "ACPowerDetection.h" 
#include "Relay.h"

void SysPara_Init(void)   //ϵͳ����
{
	uint8_t i;
	
	W25QXX_Read(uni_SysStatus.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		//��ȡFlash����
//W25QXX_Read(uni_SysStatusBak.u8_buf,FLASH_START_ADD,FLASH_LENGTH);
	if(uni_SysStatus.Str.u8_FirstPowerOn == 0xff)		//�ж��Ƿ��һ���ϴΣ�flashûд֮ǰΪ0xff
	{
		uni_SysStatus.Str.u8_FirstPowerOn = 0xAA;			//��λ�жϱ�־���´��ϵ��ȡʱ����Ϊ�ǵ�һ�Σ���ִ�иò�
		
		uni_SysStatus.Str.u8_CurrentBRT = 2;			//��ʼ��������Ϊ9600
		uni_SysStatus.Str.u8_DeviceAddress = 1;		//Ĭ�ϵ�ַΪ1
		uni_SysStatus.Str.u8_TrickleDetectionThreshold = 13;		//��������ֵ
		uni_SysStatus.Str.u8_TricklePowerSupplyTime = 2;			//�������ʱ��
		uni_SysStatus.Str.u8_MaxPowerSupply = 14;			//��󹩵�ʱ��
		uni_SysStatus.Str.u8_JitterTime = 3;			//������ʱ��
		
		uni_SysStatus.Str.u8_KeyAlarmEnable = 0x01;		//  �����Ž�����
		uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable = 0x01;		//���������ϵ籨��
		uni_SysStatus.Str.u8_WatchDogEnable = 0x01;			//�������Ź�����
		uni_SysStatus.Str.u8_CurrentDetectionPowerTime = 0x02;		//�������ʱ��
		
		uni_SysStatus.Str.u8_RelayStatusBuf[0] = 0;			//���м̵����ر�
		uni_SysStatus.Str.u8_RelayStatusBuf[1] = 0;	
		uni_SysStatus.Str.u8_RelayStatusBuf[2] = 0;	
		uni_SysStatus.Str.u8_RelayStatusBuf[3] = 0;	
		
		for(i=0;i<32;i++)			//�̵�������ʱ������
		{
			uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;
		}
	}
	
	W25QXX_Write(uni_SysStatus.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		 	//���������ֵ�Flash
	delay_ms(5);
	W25QXX_Read(uni_SysStatusBak.u8_buf,FLASH_START_ADD,FLASH_LENGTH);		//�ٴζ�������������
	
	u8_RelayStatus1_8 = uni_SysStatus.Str.u8_RelayStatusBuf[0];				//���̵���״̬��ֵ�����Ʊ���
	u8_RelayStatus9_16 = uni_SysStatus.Str.u8_RelayStatusBuf[1];
	u8_RelayStatus17_24 = uni_SysStatus.Str.u8_RelayStatusBuf[2];
	u8_RelayStatus25_32 = uni_SysStatus.Str.u8_RelayStatusBuf[3];
}
int main(void)
{ 	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);     //��ʼ����ʱ����
	TIM3_Int_Init(1000-1,84-1);	//��ʱ��ʱ��84M����Ƶϵ��84������84M/84=1Mhz�ļ���Ƶ�ʣ�����1000��Ϊ1ms     
		
	KEY_Init(); 				//������ʼ��    �Ž����
	Relay_GPIO_Init();
	Current_Init();
	ACPowerDetection_GPIO_Init();
	
	W25QXX_Init();			//W25QXX��ʼ��
//W25QXX_Erase_Chip();
	SysPara_Init();
	
	RS485_Init();	
	BL8025_Init();			//ʱ��оƬ��ʼ��

	WatchDogGPIO_Init();
	WatchDogGPIO_Enable();
	
	while(1)
	{
		BL8025_Process();  //��ʼ��ʵʱʱ�䣬����ʱ��,������ʱ,����ʱ���ʱ,�����ʱ,
		
		ACPowerDetection_Process(); //��������
		
		
	  Key_Process(); //����Ǹ����Ž��澯
		
		
		RS485_Process(); //��������������������  //�����䲻���
		
		
		Relay_Process(); //�жϼ̵�����״̬
		
		
		WatchDog_Feed(); //ι�����Ź�
		
		
		Current_Process();//��������
		
		
		W25QXX_Process();	//ϵͳ״̬д��FLASH
	}       
}

