#include "Relay.h"
#include "RS485.h"
#include "w25qxx.h"
#include "BL8025.h"
#include "Current.h"
#include "usart.h"

void Relay_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOD Eʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;//������
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ�� 
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ�� 
	
	GPIO_Write(GPIOD,0x0000);		 //All off
	GPIO_Write(GPIOE,0x0000);		//All off
}

void Relay_Out(void)
{
	uint8_t i;
	uint16_t temp = 0;
	
	temp = GPIO_ReadOutputData(GPIOE);
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOE);
		
		if(u8_RelayStatus1_8 & (1<<i))
		{
			GPIO_Write(GPIOE,(temp | (1<<i)));
		}
		else
		{
			GPIO_Write(GPIOE,(temp & (~(1<<i))));
		}
	}
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOE);
		if(u8_RelayStatus9_16 & (1<<i))
		{
			GPIO_Write(GPIOE,(temp | (0x0001<<(i+8))));
		}
				else
		{
			GPIO_Write(GPIOE,(temp & (~(0x0001<<(i+8)))));
		}
	}
	
	temp = GPIO_ReadOutputData(GPIOD);
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOD);
		if(u8_RelayStatus17_24 & (1<<i))
		{
			GPIO_Write(GPIOD,(temp | (1<<i)));
		}
		else
		{
			GPIO_Write(GPIOD,(temp & (~(1<<i))));
		}
	}
	
	for(i=0;i<8;i++)
	{
		temp = GPIO_ReadOutputData(GPIOD);
		if(u8_RelayStatus25_32 & (1<<i))
		{
			GPIO_Write(GPIOD,(temp | (0x0001<<(i+8))));
		}
		else
		{
			GPIO_Write(GPIOD,(temp & (~(1<<(i+8)))));
		}
	}	
}
void Relay_Judge(void)  
{
	uint8_t i;
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus1_8 & (1<<i)) //forѭ���ж�ÿһ���̵���״̬
		{
			if(u8_RemoveStatus1_8 & (1<<i))//�Ѿ��Ƴ��豸���رռ̵���
			{
//				printf("\r\n �жϺ��Ѿ��Ƴ�����\r\n");
				u8_RelayStatus1_8 &= ~(1<<i);//�̵���״̬����Ϊ0  �ر�
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;//�̵�������ʱ������Ϊ0
				u8_TrickleCurrentTimeBuf1_8[i] = 0;//���ʱ������Ϊ0
				u8_TrickleCurrentStatus1_8 &=~(1<<i);//���״̬����Ϊ0  ���� �����
				u8_RemoveStatus1_8 &= ~(1<<i);//�Ƴ�״̬����Ϊ0
				u8_RelayRemoveCntBuf[i] = 0;//���ڼ�¼�������С��3������ʱ�����30���ӵļ�������ֵ����10��˵���Ƴ��豸���������㣬���¼���
				u8_JitterRunTimeBuf[i] = 0;//������ʱ��
				u16_RelayCurrentBuf[i] = 0;//�̵�������ֵ		
			}
//			if((uint8_t)(u16_RelayCurrentBuf[i]/500) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//�õ����������󹩵繦��
			if((uint8_t)(u16_RelayCurrentBuf[i]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//�õ����������󹩵繦��
			{
//				printf("\r\n �жϺ������󹩵繦�ʣ���\r\n");
				u8_RelayStatus1_8 &= ~(1<<i);//�̵���״̬����Ϊ0  �ر�
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;  //�̵���״̬����ʱ������Ϊ0
				u8_TrickleCurrentTimeBuf1_8[i] = 0;//���ʱ������Ϊ0
				u8_TrickleCurrentStatus1_8 &= ~(1<<i);//���״̬����Ϊ0
				u8_RemoveStatus1_8 &= ~(1<<i); //�Ƴ�״̬����Ϊ0 
				u8_RelayRemoveCntBuf[i] = 0;  //
				u8_JitterRunTimeBuf[i] = 0;		//������ʱ��
				u8_RelayOverCurrentStatus1_8 |= (1<<i);//�̵�������״̬ 
				u16_RelayCurrentBuf[i] = 0;	//��ǰ�̵�������ֵ			
			}
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i] == 0x02)		//��ʱģʽ
			{
				//����ʱ������趨ʱ�䣬�رռ̵���
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i]) //����ʱ�����ϵͳ�����õ�ʱ��
					{
//					printf("\r\n ��ʱģʽ�жϺ��������ʱ�䣡��\r\n");
						u8_RelayStatus1_8 &= ~(1<<i);  //�̵���״̬����Ϊ0 �ر�
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0; //�̵�������ʱ��Ϊ�ر�
						u8_TrickleCurrentTimeBuf1_8[i] = 0;     //�̵������ʱ��
						u8_TrickleCurrentStatus1_8 &= ~(1<<i);  //�̵������״̬Ϊ0  ���� �������
						u8_RemoveStatus1_8 &= ~(1<<i);   //�̵����Ƴ�״̬����Ϊ0
						u8_RelayRemoveCntBuf[i] = 0;     //���ڼ�¼�������С��3������ʱ�����30���ӵļ�������ֵ����10��˵���Ƴ��豸���������㣬���¼���
						u8_JitterRunTimeBuf[i] = 0;      //������ʱ������Ϊ0
						u16_RelayCurrentBuf[i] = 0;      //�̵�����ǰ����ֵ����Ϊ0
					}
					
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i] == 0x01)		//��ͣģʽ
			{
				//����ʱ������������ʱ��������ʱ���������������ʱ�䣬�رռ̵���
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					|| (u8_TrickleCurrentTimeBuf1_8[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
//					printf("\r\n ��ͣģʽ�жϺ�����������ʱ����������������������ʱ�䣡��\r\n");
					u8_RelayStatus1_8 &= ~(1<<i); //�̵�������Ϊ0 �ر�״̬
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i] = 0;//�̵�������ʱ��
					u8_TrickleCurrentTimeBuf1_8[i] = 0; //�̵������ʱ��
					u8_TrickleCurrentStatus1_8 &= ~(1<<i);//�̵������״̬
					u8_RemoveStatus1_8 &= ~(1<<i);//�̵����Ƴ�״̬ Ϊ0����
					u8_RelayRemoveCntBuf[i] = 0;//
					u8_JitterRunTimeBuf[i] = 0;//�̵���������ʱ��
					u16_RelayCurrentBuf[i] = 0;//�̵�������ֵ
				}
			}
		}
	}
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus9_16 & (1<<i))
		{
			if(u8_RemoveStatus9_16 & (1<<i))		//�Ѿ��Ƴ��豸���رռ̵���
			{
				u8_RelayStatus9_16 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
				u8_TrickleCurrentTimeBuf9_16[i] = 0;
				u8_TrickleCurrentStatus9_16 &= ~(1<<i);
				u8_RemoveStatus9_16 &= ~(1<<i);	
				u8_RelayRemoveCntBuf[i+8] = 0;	
				u8_JitterRunTimeBuf[i+8] = 0;	
				u16_RelayCurrentBuf[i+8] = 0;				
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+8]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//�õ����������󹩵繦��
			{
				u8_RelayStatus9_16 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
				u8_TrickleCurrentTimeBuf9_16[i] = 0;
				u8_TrickleCurrentStatus9_16 &= ~(1<<i);
				u8_RemoveStatus9_16 &= ~(1<<i);	
				u8_RelayRemoveCntBuf[i+8] = 0;	
				u8_JitterRunTimeBuf[i+8] = 0;		
				u8_RelayOverCurrentStatus9_16 |= (1<<i);	
				u16_RelayCurrentBuf[i+8] = 0;					
			}			
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+8] == 0x02)	//��ʱģʽ
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+8]) 
					{
						u8_RelayStatus9_16 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
						u8_TrickleCurrentTimeBuf9_16[i] = 0;
						u8_TrickleCurrentStatus9_16 &= ~(1<<i);
						u8_RemoveStatus9_16 &= ~(1<<i);		
						u8_RelayRemoveCntBuf[i+8] = 0;
						u16_RelayCurrentBuf[i+8] = 0;	
					}
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+8] == 0x01)	//��ͣģʽ
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf9_16[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
						u8_RelayStatus9_16 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+8] = 0;
						u8_TrickleCurrentTimeBuf9_16[i] = 0;
						u8_TrickleCurrentStatus9_16 &= ~(1<<i);
						u8_RemoveStatus9_16 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+8] = 0;
						u8_JitterRunTimeBuf[i+8] = 0;	
						u16_RelayCurrentBuf[i+8] = 0;	
				}
			}
		}
	}	
	
	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus17_24 & (1<<i))
		{
		  if(u8_RemoveStatus17_24 & (1<<i))		//�Ѿ��Ƴ��豸���رռ̵���
			{
				u8_RelayStatus17_24 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
				u8_TrickleCurrentTimeBuf17_24[i] = 0;
				u8_TrickleCurrentStatus17_24 &= ~(1<<i);
				u8_RemoveStatus17_24 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+16] = 0;
				u8_JitterRunTimeBuf[i+16] = 0;	
				u16_RelayCurrentBuf[i+16] = 0;	
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+16]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//�õ����������󹩵繦��
			{
				u8_RelayOverCurrentStatus17_24 |= (1<<i);
				
				u8_RelayStatus17_24 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
				u8_TrickleCurrentTimeBuf17_24[i] = 0;
				u8_TrickleCurrentStatus17_24 &= ~(1<<i);
				u8_RemoveStatus17_24 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+16] = 0;
				u8_JitterRunTimeBuf[i+16] = 0;
				u16_RelayCurrentBuf[i+16] = 0;				
			}			
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+16] == 0x02)
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+16]) 
					{
						u8_RelayStatus17_24 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
						u8_TrickleCurrentTimeBuf17_24[i] = 0;
						u8_TrickleCurrentStatus17_24 &= ~(1<<i);
						u8_RemoveStatus17_24 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+16] = 0;
						u8_JitterRunTimeBuf[i+16] = 0;	
						u16_RelayCurrentBuf[i+16] = 0;
					}
			}	
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+16] == 0x01)
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf17_24[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
					u8_RelayStatus17_24 &= ~(1<<i);
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+16] = 0;
					u8_TrickleCurrentTimeBuf17_24[i] = 0;
					u8_TrickleCurrentStatus17_24 &= ~(1<<i);
					u8_RemoveStatus17_24 &= ~(1<<i);
					u8_RelayRemoveCntBuf[i+16] = 0;
					u8_JitterRunTimeBuf[i+16] = 0;	
					u16_RelayCurrentBuf[i+16] = 0;
				}
			}
	
		}
	}	

	for(i=0;i<8;i++)
	{
		if(u8_RelayStatus25_32 & (1<<i))		
		{
			if(u8_RemoveStatus25_32 & (1<<i))		//�Ѿ��Ƴ��豸���رռ̵���
			{
				u8_RelayStatus25_32 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
				u8_TrickleCurrentTimeBuf25_32[i] = 0;
				u8_TrickleCurrentStatus25_32 &= ~(1<<i);
				u8_RemoveStatus25_32 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+24] = 0;
				u8_JitterRunTimeBuf[i+24] = 0;	
				u16_RelayCurrentBuf[i+24] = 0;
			}
			if((uint8_t)(u16_RelayCurrentBuf[i+24]/100) >= (uni_SysStatus.Str.u8_MaxPowerSupply))	//�õ����������󹩵繦��
			{
				u8_RelayOverCurrentStatus25_32 |= (1<<i);
				
				u8_RelayStatus25_32 &= ~(1<<i);
				uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
				u8_TrickleCurrentTimeBuf25_32[i] = 0;
				u8_TrickleCurrentStatus25_32 &= ~(1<<i);
				u8_RemoveStatus25_32 &= ~(1<<i);
				u8_RelayRemoveCntBuf[i+24] = 0;
				u8_JitterRunTimeBuf[i+24] = 0;	
        u16_RelayCurrentBuf[i+24] = 0;				
			}				
			if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+24] == 0x02)
			{
					if(uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] >= uni_SysStatus.Str.u8_RelayWorkSetTimeBuf[i+24]) 
					{
						u8_RelayStatus25_32 &= ~(1<<i);
						uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
						u8_TrickleCurrentTimeBuf25_32[i] = 0;
						u8_TrickleCurrentStatus25_32 &= ~(1<<i);
						u8_RemoveStatus25_32 &= ~(1<<i);
						u8_RelayRemoveCntBuf[i+24] = 0;
						u8_JitterRunTimeBuf[i+24] = 0;
						u16_RelayCurrentBuf[i+24] = 0;
					}
			}		
			else if(uni_SysStatus.Str.u8_RelayWorkModeBuf[i+24] == 0x01)
			{
				if((uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] >= uni_SysStatus.Str.u8_MaxPowerSupplyTime)
					||(u8_TrickleCurrentTimeBuf25_32[i] >= uni_SysStatus.Str.u8_TricklePowerSupplyTime))
				{
					u8_RelayStatus25_32 &= ~(1<<i);
					uni_SysStatus.Str.u8_RelayWorkRunTimeBuf[i+24] = 0;
					u8_TrickleCurrentTimeBuf25_32[i] = 0;
					u8_TrickleCurrentStatus25_32 &= ~(1<<i);
					u8_RemoveStatus25_32 &= ~(1<<i);
					u8_RelayRemoveCntBuf[i+24] = 0;
					u8_JitterRunTimeBuf[i+24] = 0;
					u16_RelayCurrentBuf[i+24] = 0;
				}
			}
		}
	}


	if(u1_NeedDectCurrentF) //��Ҫ������
	{  //��������ʱ��С��ϵͳ�趨�ĵ������ͨ��ʱ��
		if(u16_CurrentForceCheckTimeS < uni_SysStatus.Str.u8_CurrentDetectionPowerTime)
		{
			if(u8_NeedDectCh < 9)  //����֮ǰ������ʱ������û�п��������ȿ��� �̵��� �����ڵ������ʱ���м��
			{
				u8_RelayStatus1_8 |= 0x01<<(u8_NeedDectCh-1);  
			}
			else 	if(u8_NeedDectCh < 17)
			{
				u8_RelayStatus9_16 |= 0x01<<(u8_NeedDectCh-9);
			}
			else 	if(u8_NeedDectCh < 25)
			{
				u8_RelayStatus17_24 |= 0x01<<(u8_NeedDectCh-17);
			}
			else 	if(u8_NeedDectCh < 33)
			{
				u8_RelayStatus25_32 |= 0x01<<(u8_NeedDectCh-25);
			}
		}
		else
		{
			u1_NeedDectCurrentF = 0;
			u1_ForceDectCurrentEndF = 1;  //��������ʱ�����ϵͳ�趨�ĵ������ͨ��ʱ��  ��ʾ������
			
			if(u8_NeedDectCh < 9)
			{
				u8_RelayStatus1_8 &= ~(0x01<<(u8_NeedDectCh-1)); //�رռ̵���
			}
			else 	if(u8_NeedDectCh < 17)
			{
				u8_RelayStatus9_16 &= ~(0x01<<(u8_NeedDectCh-9));
			}
			else 	if(u8_NeedDectCh < 25)
			{
				u8_RelayStatus17_24 &= ~(0x01<<(u8_NeedDectCh-17));
			}
			else 	if(u8_NeedDectCh < 33)
			{
				u8_RelayStatus25_32 &= ~(0x01<<(u8_NeedDectCh-25));
			}				
		}
	}	
}
void Relay_Process(void)
{
	Relay_Judge();  //�̵���״̬�ж�
	Relay_Out();//���Ƽ̵�������
	
	uni_SysStatus.Str.u8_RelayStatusBuf[0] = u8_RelayStatus1_8;
	uni_SysStatus.Str.u8_RelayStatusBuf[1] = u8_RelayStatus9_16;
	uni_SysStatus.Str.u8_RelayStatusBuf[2] = u8_RelayStatus17_24;
	uni_SysStatus.Str.u8_RelayStatusBuf[3] = u8_RelayStatus25_32;
	
	u8_RelayStatus1_8Bak = u8_RelayStatus1_8;
	u8_RelayStatus9_16Bak = u8_RelayStatus9_16;
	u8_RelayStatus17_24Bak = u8_RelayStatus17_24;
	u8_RelayStatus25_32Bak = u8_RelayStatus25_32;
}











