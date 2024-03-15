#include "RS485.h"
#include "usart.h"	
#include "CRC.h"
#include "BL8025.h"
#include "Current.h"
#include "w25qxx.h"

uint8_t u8_AlarmStatus;//����״̬

uint8_t u8_RelayStatus1_8;  //�̵�������״̬
uint8_t u8_RelayStatus9_16;
uint8_t u8_RelayStatus17_24;
uint8_t u8_RelayStatus25_32;

uint8_t u8_RelayStatus1_8Bak;
uint8_t u8_RelayStatus9_16Bak;
uint8_t u8_RelayStatus17_24Bak;
uint8_t u8_RelayStatus25_32Bak;

uint8_t u8_RelayOverCurrentStatus1_8;  //�̵��������رձ��� 
uint8_t u8_RelayOverCurrentStatus9_16;
uint8_t u8_RelayOverCurrentStatus17_24;
uint8_t u8_RelayOverCurrentStatus25_32;

uint8_t u8_RemoveStatus1_8;  //�жϼ̵����Ƿ��Ƴ�
uint8_t u8_RemoveStatus9_16;
uint8_t u8_RemoveStatus17_24;
uint8_t u8_RemoveStatus25_32;

uint8_t u8_RemoveStatus1_8Bak;  //�����豸�Ƴ��̵����رձ���
uint8_t u8_RemoveStatus9_16Bak;
uint8_t u8_RemoveStatus17_24Bak;
uint8_t u8_RemoveStatus25_32Bak;

uint8_t u8_RemoveStatus1_8Qury; //�Ƿ������ڱ�־λ
uint8_t u8_RemoveStatus9_16Qury;
uint8_t u8_RemoveStatus17_24Qury;
uint8_t u8_RemoveStatus25_32Qury;

uint8_t u8_TrickleCurrentStatus1_8; // �̵������״̬
uint8_t u8_TrickleCurrentStatus9_16;
uint8_t u8_TrickleCurrentStatus17_24;
uint8_t u8_TrickleCurrentStatus25_32;

uint8_t u8_SendBuf[50];
uint8_t u8_SendLength = 0;

void RS485_Init(void)
{
	RS485_GPIO_Init();
	
	if(uni_SysStatus.Str.u8_CurrentBRT == 1)
	{
		uart_init(19200);
	}
	else if(uni_SysStatus.Str.u8_CurrentBRT == 2)
	{
		uart_init(9600);
	}
	else if(uni_SysStatus.Str.u8_CurrentBRT == 3)
	{
		uart_init(4800);
	}
	else if(uni_SysStatus.Str.u8_CurrentBRT == 4)
	{
		uart_init(2400);
	}
	
	u8_RemoveStatus1_8Qury= 0xff;
	u8_RemoveStatus9_16Qury= 0xff;
	u8_RemoveStatus17_24Qury= 0xff;
	u8_RemoveStatus25_32Qury= 0xff;
}
void RS485_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ�� 

	RS485_Recv_En();
//	RS485_Send_En();
}
void RS485_Process(void)  
{
	uint16_t CheckSum = 0;
	uint8_t RcvAddress;
	uint8_t BRTChange = 0;
	uint8_t i;
	
	if(USART_RX_STA == 0x02)
	{
		USART_RX_STA = 0;
		
		CheckSum = CRC_Check(USART_RX_BUF,Rcv_Length-2);		//����CheckSum   У���
		//�ж��Ƿ���CheckSumһ��
		Rcv_Length = 0;
		if(((uint8_t)(CheckSum >> 8) == USART_RX_BUF[Rcv_Length - 2]) && ((uint8_t)(CheckSum & 0x00ff) == USART_RX_BUF[Rcv_Length - 1]))
		{
			RcvAddress = USART_RX_BUF[1];
			
			if((RcvAddress != uni_SysStatus.Str.u8_DeviceAddress) && (RcvAddress != 0))	return;
			
			CmdCode = USART_RX_BUF[2];		//��ȡ�������
			
			switch(CmdCode)
			{
				case 0x81:	//��ѯ����״̬
				{
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x01;
					u8_SendBuf[3] = u8_AlarmStatus;
					u8_SendBuf[4] = u8_RelayStatus1_8;
					u8_SendBuf[5] = u8_RelayStatus9_16;
					u8_SendBuf[6] = u8_RelayStatus17_24;
					u8_SendBuf[7] = u8_RelayStatus25_32;
					u8_SendBuf[8] = u8_RelayOverCurrentStatus1_8;
					u8_SendBuf[9] = u8_RelayOverCurrentStatus9_16;
					u8_SendBuf[10] = u8_RelayOverCurrentStatus17_24;
					u8_SendBuf[11] = u8_RelayOverCurrentStatus25_32;
//					u8_SendBuf[12] = u8_RemoveStatus1_8Bak;
//					u8_SendBuf[13] = u8_RemoveStatus9_16Bak;
//					u8_SendBuf[14] = u8_RemoveStatus17_24Bak;
//					u8_SendBuf[15] = u8_RemoveStatus25_32Bak;
					
					u8_SendBuf[12] = u8_RemoveStatus1_8Bak & u8_RemoveStatus1_8Qury;
					u8_SendBuf[13] = u8_RemoveStatus9_16Bak & u8_RemoveStatus9_16Qury;
					u8_SendBuf[14] = u8_RemoveStatus17_24Bak & u8_RemoveStatus17_24Qury;
					u8_SendBuf[15] = u8_RemoveStatus25_32Bak & u8_RemoveStatus25_32Qury;

					
					u8_SendBuf[16] = u8_TrickleCurrentStatus1_8;
					u8_SendBuf[17] = u8_TrickleCurrentStatus9_16;
					u8_SendBuf[18] = u8_TrickleCurrentStatus17_24;
					u8_SendBuf[19] = u8_TrickleCurrentStatus25_32;	

					u8_SendLength = 20;		
					u8_RemoveStatus1_8Bak = 0;
					u8_RemoveStatus9_16Bak = 0;
					u8_RemoveStatus17_24Bak = 0;
					u8_RemoveStatus25_32Bak = 0;
					
					
					u8_RelayOverCurrentStatus1_8 = 0;
					u8_RelayOverCurrentStatus9_16 = 0;
					u8_RelayOverCurrentStatus17_24 = 0;
					u8_RelayOverCurrentStatus25_32 = 0;
					
					u1_isOnlineDectEndF = 0;
					
				}break;
				case 0x82://���ƿ���
				{
					if(USART_RX_BUF[3] <= 8)  //�ж����ĸ��̵���
					{
						u8_RelayStatus1_8 |= (0x01 << (USART_RX_BUF[3]-1));
					}
					else if(USART_RX_BUF[3] <= 16)
					{
						u8_RelayStatus9_16 |= (0x01 << (USART_RX_BUF[3]-9));
					}
					else if(USART_RX_BUF[3] <= 24)
					{
						u8_RelayStatus17_24 |= (0x01 << (USART_RX_BUF[3]-17));
					}
					else if(USART_RX_BUF[3] <= 32)
					{
						u8_RelayStatus25_32 |= (0x01 << (USART_RX_BUF[3]-25));
					}
					else
					{
						
					}
					
					uni_SysStatus.Str.u8_RelayWorkModeBuf[USART_RX_BUF[3] - 1] = USART_RX_BUF[4]; //�ж������ֹ���ģʽ
					
					if(uni_SysStatus.Str.u8_RelayWorkModeBuf[USART_RX_BUF[3] - 1] == 0x02)	 //��ʱģʽ
					{
						uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[USART_RX_BUF[3] - 1] = USART_RX_BUF[5];//��ʱʱ��
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[USART_RX_BUF[3] - 1] = 0;
					}
					else
					{
//						uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[USART_RX_BUF[3] - 1] = 0; //��ͣģʽ
					}
					
					if((USART_RX_BUF[6] >0) && (USART_RX_BUF[6] < 0x13)) //�ж������
					{
							uni_SysStatus.Str.u8_MaxPowerSupply = USART_RX_BUF[6];		//20181022 ��λ����Ϊ0.1A
					}
					
					u16_CurrentCheckTimeMs = 0;		//20181022
					
					//�ж��Ƿ����ɹ��󷵻�����
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x02;	
					u8_SendBuf[3] = USART_RX_BUF[3];			//���Ӽ̵������
					
					if(	(u8_RelayStatus1_8Bak ^ u8_RelayStatus1_8)  //���Ƿ�ռ��
						|| (u8_RelayStatus9_16Bak ^ u8_RelayStatus9_16) 
						|| (u8_RelayStatus17_24Bak ^ u8_RelayStatus17_24) 
						|| (u8_RelayStatus25_32Bak ^ u8_RelayStatus25_32))
					{
						u8_SendBuf[4] = 0x00;		//�����ɹ�
						
//						u16_CurrentCheckTimeMs = 0;		//20181022
					}
					else
					{
						u8_SendBuf[4] = 0x01;	//��ռ��
						
						if(0)	//����ԭ���²��ɹ�
						{
							u8_SendBuf[4] = 0x02;
						}
					}
					
					//u8_SendLength = 5;			//20181022
					u8_RelayStatus1_8Bak 		= u8_RelayStatus1_8;
					u8_RelayStatus9_16Bak 	= u8_RelayStatus9_16;
					u8_RelayStatus17_24Bak 	= u8_RelayStatus17_24;
					u8_RelayStatus25_32Bak 	= u8_RelayStatus25_32;
					
					u1_isOnlineDectF = 1;  //�����ж��Ƿ��������
//				u1_isOnlineDectEndF = 1;
					u16_IsOnlineCheckTimeS = 0;//�����ж��Ƿ��������
					u1_isOnlineUpDataF = 0;
					
				}break;
				case 0x83:	//���ù������
				{
					if(USART_RX_BUF[3] !=0 )
					{
						uni_SysStatus.Str.u8_TrickleDetectionThreshold 	= USART_RX_BUF[3];
					}
					if(USART_RX_BUF[4] !=0 )
					{
					uni_SysStatus.Str.u8_TricklePowerSupplyTime 		= USART_RX_BUF[4];
					}
					if(USART_RX_BUF[5] !=0 )
					{
					uni_SysStatus.Str.u8_MaxPowerSupplyTime 				= USART_RX_BUF[5];
					}
					if(USART_RX_BUF[6] !=0 )
					{
					uni_SysStatus.Str.u8_JitterTime 								= USART_RX_BUF[6];
					} 
					
					
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x03;
					u8_SendBuf[3] = uni_SysStatus.Str.u8_TrickleDetectionThreshold;
					u8_SendBuf[4] = uni_SysStatus.Str.u8_TricklePowerSupplyTime;
					u8_SendBuf[5] = uni_SysStatus.Str.u8_MaxPowerSupplyTime;
					u8_SendBuf[6] = uni_SysStatus.Str.u8_JitterTime;
					
					u8_SendLength = 7;	

				}break;
				case 0x84:	//����ϵͳ����
				{
					if(USART_RX_BUF[3] != 0)
					{
						if(USART_RX_BUF[3] == 0x01)
						{
							uni_SysStatus.Str.u8_KeyAlarmEnable = 1;						//�Ž�����ʹ��
						}
						else if(USART_RX_BUF[3] == 0x02)
						{
							uni_SysStatus.Str.u8_KeyAlarmEnable = 0;
						}
					}
					if(USART_RX_BUF[4] != 0)
					{
						if(USART_RX_BUF[4] == 0x01)
						{
							uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable = 1;	//�������籨��ʹ��
						}
						else if(USART_RX_BUF[4] == 0x02)
						{
							uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable = 0;
						}
					}
					if(USART_RX_BUF[5] != 0)
					{
						if(USART_RX_BUF[5] == 0x01)
						{
							uni_SysStatus.Str.u8_WatchDogEnable = 1;						//���Ź�����ʹ��
						}
						else if(USART_RX_BUF[5] == 0x02)
						{
							uni_SysStatus.Str.u8_WatchDogEnable = 0;	
						}
					}
					if(USART_RX_BUF[6] != 0)
					{
						uni_SysStatus.Str.u8_CurrentDetectionPowerTime = USART_RX_BUF[6];		//�������ͨ��ʱ��
					}

					if(USART_RX_BUF[7] != 0)
					{
						SetDate_Year = USART_RX_BUF[7];
					}
					if(USART_RX_BUF[8] != 0)
					{
						SetDate_Month = USART_RX_BUF[8];
					}
					if(USART_RX_BUF[9] != 0)
					{
						SetDate_Day = USART_RX_BUF[9];	
					}
					if(USART_RX_BUF[10] != 0)
					{
						SetTime_Hour = USART_RX_BUF[10];	
					}
					if(USART_RX_BUF[11] != 0)
					{
						SetTime_Minute = USART_RX_BUF[11];	
					}
					if(USART_RX_BUF[12] != 0)
					{
						SetTime_Second = USART_RX_BUF[12];	
					}
					
					for(i=3;i<13;i++)
					{
						if(USART_RX_BUF[i] != 0)
						{
							break;
						}
					}
					if(i<13)
					{
						i=0;
						BL8025SetDate();
						BL8025SetTime();
						
						u8_SendBuf[7] = SetDate_Year;
						u8_SendBuf[8] = SetDate_Month;
						u8_SendBuf[9] = SetDate_Day;
						u8_SendBuf[10] = SetTime_Hour;
						u8_SendBuf[11] = SetTime_Minute;
						u8_SendBuf[12] = SetTime_Second;
					}
					else
					{
						u8_SendBuf[7] = Current_Year;
						u8_SendBuf[8] = Current_Month;
						u8_SendBuf[9] = Current_Day;
						u8_SendBuf[10] = Current_Hour;
						u8_SendBuf[11] = Current_Minute;
						u8_SendBuf[12] = Current_Second;
					}
					
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x04;
					u8_SendBuf[3] = uni_SysStatus.Str.u8_KeyAlarmEnable;
					u8_SendBuf[4] = uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable;
					u8_SendBuf[5] = uni_SysStatus.Str.u8_WatchDogEnable;
					u8_SendBuf[6] = uni_SysStatus.Str.u8_CurrentDetectionPowerTime;					
					
					u8_SendLength = 13;	
					
				}break;
				case 0x85:	//����ͨ�Ų���
				{
					if(USART_RX_BUF[3] != 0x00)	//��ַ����0
					{
						uni_SysStatus.Str.u8_DeviceAddress = USART_RX_BUF[3];	//�������õĵ�ַ
					}
					else	//����յ��ĵ�ַ��0���򷵻ص�ǰ��ַ
					{
							
					}
					//���ò�����
					if((USART_RX_BUF[4] > 0) && (USART_RX_BUF[4] < 5))
					{							
						if(uni_SysStatus.Str.u8_CurrentBRT != USART_RX_BUF[4])
						{
							uni_SysStatus.Str.u8_CurrentBRT = USART_RX_BUF[4];
							
							BRTChange = 1;
						}
					}
					
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x05;
					u8_SendBuf[3] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[4] = uni_SysStatus.Str.u8_CurrentBRT;	

					u8_SendLength = 5;
					
				}break;
				case 0x86:	//�������ָ��
				{
					if((USART_RX_BUF[3] > 0) && (USART_RX_BUF[3] < 33))		//�ж��Ƿ��ڵ�ַ��Χ��
					{	
						if(	((u8_RelayStatus1_8 & (0x01<<(USART_RX_BUF[3]-1))) && (USART_RX_BUF[3] < 9))  //�жϼ̵����Ƿ���
								||((u8_RelayStatus9_16 & (0x01<<(USART_RX_BUF[3]-9))) && (USART_RX_BUF[3] < 17))
								||((u8_RelayStatus17_24 & (0x01<<(USART_RX_BUF[3]-17))) && (USART_RX_BUF[3] < 25))
								||((u8_RelayStatus25_32 & (0x01<<(USART_RX_BUF[3]-25))) && (USART_RX_BUF[3] < 33)))
						{
							u8_SendBuf[0] = 0x7e;
							u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
							u8_SendBuf[2] = 0x06;
							u8_SendBuf[3] = USART_RX_BUF[3];
							u8_SendBuf[4] = (uint8_t)(u16_RelayCurrentBuf[USART_RX_BUF[3] - 1]/100);	
							u8_SendLength = 5;
						}
						else //�̵���û�п��������
						{
							u1_NeedDectCurrentF = 1; //���뿪���̵���ģʽ��������
							u8_NeedDectCh = USART_RX_BUF[3];
						}
					}						
				}break;
				case 0x89:	//������λ
				{
					if(USART_RX_BUF[3] == 1)	//ϵͳ������λ
					{
						uni_SysStatus.Str.u8_TrickleDetectionThreshold = 13;
						uni_SysStatus.Str.u8_TricklePowerSupplyTime = 2;
						uni_SysStatus.Str.u8_MaxPowerSupplyTime = 14;
						uni_SysStatus.Str.u8_JitterTime = 3;
						uni_SysStatus.Str.u8_DeviceAddress = 1;
						uni_SysStatus.Str.u8_CurrentBRT = 2;	
						uni_SysStatus.Str.u8_KeyAlarmEnable = 1;
						uni_SysStatus.Str.u8_ACPowerOutagesAlarmEnable = 1;
						uni_SysStatus.Str.u8_WatchDogEnable = 1;
						uni_SysStatus.Str.u8_CurrentDetectionPowerTime = 2;						
					}
					if(USART_RX_BUF[4] == 1)		//RTC��λ
					{
						SetDate_Year = 18;
						SetDate_Month = 1;
						SetDate_Day = 1;
						SetDate_Week = 2;
						SetTime_Hour = 0;
						SetTime_Minute = 0;
						SetTime_Second = 0;		

						BL8025SetDate();
						BL8025SetTime();
					}
					if(USART_RX_BUF[5] != 0)	//�̵���״̬��λ
					{
						if(USART_RX_BUF[5] < 9)
						{
							u8_RelayStatus1_8 &= ~(1<<(USART_RX_BUF[5]-1));
						}
						else if(USART_RX_BUF[5] < 17)
						{
							u8_RelayStatus9_16 &= ~(1<<(USART_RX_BUF[5]-9));
						}
						else if(USART_RX_BUF[5] < 25)
						{
							u8_RelayStatus17_24 &= ~(1<<(USART_RX_BUF[5]-17));
						}
						else if(USART_RX_BUF[5] < 33)
						{
							u8_RelayStatus25_32 &= ~(1<<(USART_RX_BUF[5]-25));
						}
						else if(USART_RX_BUF[5] == 0x22)
						{
							u8_RelayStatus1_8 = 0;
							u8_RelayStatus9_16 = 0;
							u8_RelayStatus17_24 = 0;
							u8_RelayStatus25_32 = 0;
						}
					}
					
					u8_SendBuf[0] = 0x7e;
					u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
					u8_SendBuf[2] = 0x09;
					u8_SendBuf[3] = USART_RX_BUF[3];
					u8_SendBuf[4] = USART_RX_BUF[4];	
					u8_SendBuf[5] = USART_RX_BUF[5];
					
					u8_SendLength = 6;	
					
				}break;
				
				default:break;
			}
						
			if(BRTChange == 1)		//�����Ҫ���Ĳ����ʣ���ԭ�����ʷ������ݺ�Ÿ���ϵͳ�Ĳ�����
			{
				if(uni_SysStatus.Str.u8_CurrentBRT == 1)
				{
					uart_init(19200);
				}
				else if(uni_SysStatus.Str.u8_CurrentBRT == 2)
				{
					uart_init(9600);
				}
				else if(uni_SysStatus.Str.u8_CurrentBRT == 3)
				{
					uart_init(4800);
				}
				else if(uni_SysStatus.Str.u8_CurrentBRT == 4)
				{
					uart_init(2400);
				}
			}
		}
	}
	if(u1_ForceDectCurrentEndF)//�����������������ֵ
	{
		u1_ForceDectCurrentEndF = 0;

		u8_SendBuf[0] = 0x7e;
		u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
		u8_SendBuf[2] = 0x06;
		u8_SendBuf[3] = u8_NeedDectCh;
		u8_SendBuf[4] = (uint8_t)(u16_RelayCurrentBuf[u8_NeedDectCh - 1]/100);	
		u8_SendLength = 5;		
	}
	
	
	if(u1_isOnlineDectF && (u16_IsOnlineCheckTimeS >= 1))////���������ж��û�������Ƿ����
	{
		u1_isOnlineDectF = 0;
		u16_IsOnlineCheckTimeS = 0;
		u1_isOnlineDectEndF = 0;
			
			u8_SendBuf[0] = 0x7e;
			u8_SendBuf[1] = uni_SysStatus.Str.u8_DeviceAddress;
			u8_SendBuf[2] = 0x02;	
			u8_SendBuf[3] = USART_RX_BUF[3];			//���Ӽ̵������
			
			if(u16_RelayCurrentBuf[USART_RX_BUF[3] - 1] < MIN_CURRENT_CHECK)//�жϵ����Ƿ�С����С����ֵ
			{
				u8_SendBuf[4] = 0x03; //δ��⵽�����
				
				if(USART_RX_BUF[3] < 9)
				{					
					u8_RemoveStatus1_8Qury &= ~(1<<(USART_RX_BUF[3]-1));
				}
				else if(USART_RX_BUF[3] < 17)
				{
					u8_RemoveStatus9_16Qury &= ~(1<<(USART_RX_BUF[3]-9));
				}
				else if(USART_RX_BUF[3] < 25)
				{
					u8_RemoveStatus17_24Qury &= ~(1<<(USART_RX_BUF[3]-17));
				}
				else if(USART_RX_BUF[3] < 33)
				{
					u8_RemoveStatus25_32Qury &= ~(1<<(USART_RX_BUF[3]-25));
				}
			}
			else
			{
				if(USART_RX_BUF[3] < 9)
				{
					u8_RemoveStatus1_8Qury |= 1<<(USART_RX_BUF[3]-1); //��⵽�����
				}
				else if(USART_RX_BUF[3] < 17)
				{
					u8_RemoveStatus9_16Qury |= 1<<(USART_RX_BUF[3]-9);
				}
				else if(USART_RX_BUF[3] < 25)
				{
					u8_RemoveStatus17_24Qury |= 1<<(USART_RX_BUF[3]-17);
				}
				else if(USART_RX_BUF[3] < 33)
				{
					u8_RemoveStatus25_32Qury |= 1<<(USART_RX_BUF[3]-25);
				}
					
			}
			u8_SendLength = 5;		
	}
	
	if(u8_SendLength != 0)		//�����Ҫ�������ݣ��򷵻�
	{
		CheckSum = CRC_Check(&u8_SendBuf[1],(u8_SendLength-1));		//����CheckSum
		
		u8_SendBuf[u8_SendLength] = (uint8_t)(CheckSum>>8);
		u8_SendBuf[u8_SendLength+1] = (uint8_t)(CheckSum&0xff);
		
		USART_Send_Buf(u8_SendBuf,(u8_SendLength + 3));		//�෢��һ�����ݣ������Ͳ�ȫ
		
		u8_SendLength = 0;	//���㣬�����´��жϷ���
	}
}




