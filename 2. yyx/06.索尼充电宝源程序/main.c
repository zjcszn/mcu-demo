//V1.1 ���µ�����ʾ����,���Ӱ���������
//V1.2 �����ֻ��źŸ���
//V1.3 ���ӱ߳�߷Ź���
#include "define.h"

//--IO��ʼ��-------------------------------------
void IO_Setting(void)
{		
  P0M = 0x0E;	//P0 Input-Output value   //P01��02��03���ģʽ��P00,04����ģʽ��
  P0UR = 0x01;	//P0 pull up value        //��P00����������
  P0 = P0&0X01; //P0 IO register-SET  ?

  P4M = 0x1A;	//P4 Input-Output value   //P41,43Ϊ���ģʽ��P4.2,P4.4,4.0Ϊ����ģʽ��
  P4UR = 0x04;  //P4 pull up value        //�ر�������������
  P4CON = 0x01;	//P4 Analog-Digital value //P4.0ֻ��ģ������
  P4 = 0X00;	//P4 register value       //P4�ڸ�ֵ0
                                                                                                                                                                                                                   
  P5M = 0x18;	//P5 Input-Output value   //P5.3,P5.4Ϊ���ģʽ.
  P5UR = 0x00;	//P5 pull up value        //�ر�����������  
  P5 = 0X00;	//P5 register value       //P5�ڸ�ֵ0
}

//--�û��Ĵ�����ʼ��-------------------------------
void UserInit(void)
{
	if (FirstPowerOn)
	{
		VoltageOffset=0;
		BatteryLevel=4;	
		BatteryEmptyFlag=0;
	}
	BatteryFullFlag=0;	
	KeyPressFlag=0;
	KeyTime=0;	
	BatteryCapUpdateTimeFlag=0;
	Counter1S=0;
	SleepWaitTime=0;
	KeyOutputSW=0;
	BatteryVchargeFlag=0;
	LEDTime=0;
	CurrentDetTime=0;	
	CurrentSmallTime=0;	
	CurrentMaxTime=0;
	KeyReleaseFlag=1;
	BatteryLowerCounter=0;
	BatteryFulltemp=0;
}

//--���ߴ�����-------------------------------
void EnterSleepMode(void)
{
	if (!(USBDET||OUTPUTSW))	
	{
		if (SleepWaitTime>=SleepWaitTimeSet)//if USB not insert and not 5V output
		{
			FGIE = 0;
			INTRQ=0;
			DCOUTOFF;
			LEDOFF;
			DisableADC;
			ADM=0;           //���;�̬����
			DelayUs(100);
			SLEEPMODE_MY;
			DelayUs(10);
			ADC_Setting();	
			UserInit();	
	       
            OUTPUTSW=1;      //�����
            LED2ON;
			delayS(250);
			LED2OFF;
            LED3ON;
            delayS(250);
			LED3OFF;
			LED4ON;
            delayS(250);
			LED4OFF;
			LED5ON;
			delayS(250);
			LED5OFF;

			KeyOutputSW=1; //�����

			FGIE = 1;  //�����ж�
		 }
	   }	
     }



//--��ص������³���-----------------------------
void UpdateBatteryCap(void)
{	
	uint8 batteryleveltemp;	  
	uint8 Offset;
	if (USBDET)     //���ʱ���벹��
	{
		Offset=0xC7;//offset the battery level 4 to 4.1V  0.136V
	}
	else            //�ŵ�ʱû�в���
	{
		Offset=0;
	}

	if (BatteryCapUpdateTimeFlag||FirstPowerOn)//1S update battery cap 1�����һ�ε�ص��������ߵ�һ�ο�������һ��
	{	
		BatteryCapUpdateTimeFlag=0;                         //�������±�־λ����

	  if (BatteryVoltage>=(BatteryVoltage4+Offset))        //�����ص�ѹ>=3.8V  //������3.936
		{
			batteryleveltemp=4;       //LED5��
		//	BatteryFullFlag=1;        //�����־λֵ1
		 }
		else
		{
		if (BatteryVoltage>=(BatteryVoltage3+Offset))        //�����ص�ѹ>=3.8V  //������3.936
		{
			batteryleveltemp=3;       //LED4��

		}
		else
		{ 
			if (BatteryVoltage>=(BatteryVoltage2+Offset))    //�����ص�ѹ>=3.6V  //������3.736
			{
				batteryleveltemp=2;		 //LED3��
			}
			else
			{
				if (BatteryVoltage>=(BatteryVoltage1+Offset))//�����ص�ѹ>=3.45V  //������3.586
				{
					batteryleveltemp=1;    //LED2�� ��ص�ѹ>=3.45V
				}
				else
				{
					if (BatteryVoltage>=BatteryVoltage0)      //�����ص�ѹ>=2.9
					{
                       batteryleveltemp=0;   //LED1�� ��ص�ѹ>=2.9									
					}
                    else
					{
                     	BatteryEmptyFlag=1;	 //�յ��־λֵ1																		 	  
					}
				  }
			    }
		      }
		    }

		if (USBDET)// if usb insert, battery level can up     ����ڳ��
		{
			if ((batteryleveltemp>=BatteryLevel)||FirstPowerOn) //�����ص���������ʾ����||��һ�ο�������һ��Ϊ��
			{
				BatteryLevel=batteryleveltemp;	                //���µ�����ʾ
			}
		}
		else	//if usb not insert, battery level can down     //����û���ڳ�磬���ڷŵ�
		{
			if (batteryleveltemp<BatteryLevel)                  //�����ص���С����ʾ����
			{
				BatteryLevel=batteryleveltemp;	                //������ʾ����
			}
		  }
	    }
      }

//--ADC��ʼ��-------------------------------------
void ADC_Setting(void)
{
  FADENB = 1;
  DelayUs(200); //User support DELAY function
//  ADR = 0x50;	//ADC clock: 2.000000MHZ,ADC resolution: -1
  ADR = ADR&0x0f;     //�趨ADCƵ�ʣ�FCPU/16��Ƶ��250000HZ��/1ms  ?
  VREFH = 0x03;	//Internal VREFH: VDD
  FEVHENB = 0;	//Internal VREFH disable
}

//--��紦��-------------------------------------
void Charge(void)
{
	if (USBDET)
	{
//	    KeyOutputSW=0;
		if (BatteryVoltage>=BatteryFullVoltage) //�����ص�ѹ>=4.16V
		{
		   if(BatteryFullDelayFlag==1)
		   {
		    BatteryFullDelayFlag=0;     //1S�����㣬
            BatteryFulltemp++;          //��������жϼĴ���
		   if(BatteryFulltemp>=200)      //����200S�����������ϡ�
		   {
            BatteryFulltemp=0;
			BatteryFullFlag=1;          //��������־λ
			}
		  }
		}
		else
		{
		    BatteryFulltemp=0;     //����������ʱ�ж�
			BatteryFullFlag=0;	   //���������־λ��Ϊ0.
		}
		if (!OUTPUTSW)             //�������ǹر�״̬
		{
			BatteryEmptyFlag=0;	   //��ؿյ��־λ���㡣		
		}
//		BatteryWarningFlag=0;      //ֻҪ��⵽��磬��ؾ����־λ�����㡣
	    }

}

//--�ŵ紦����----------------------------------------------
void Discharge(void)
{	
	if (BatteryEmptyFlag)    //����յ��־λΪ��
	{
		KeyOutputSW=OFF;     //����ر�
	}
	if (KeyOutputSW)         //����Ϊ��
	{
		BatteryFullFlag=0;	 //��������־λ����	
		DCOUTON;             //��ѹʹ�ܿ�
		DischargeCurrent=ADFilter(ChannelI);  //�ɼ��ŵ�������ݸ���DischargeCurrent ����
		if (DischargeCurrent>=DischargeCurrentPro)//if current larger protect current//����ŵ�������ڹ����㡣
		{
			CurrentMaxTime++;  //������ʱ��������
			if (CurrentMaxTime==CurrentMaxTimeSet) //�������ʱ��==�趨��������ʱ��
			{
				KeyOutputSW=0;     //�ر����
				CurrentMaxTime=0;  //������ʱ������
			}
		}
		else  //����ŵ����û�й���
		{
			CurrentMaxTime=0; //������ʱ������
		}

		if (CurrentDetTime)	  //1S detect 1 cycle //1Sʱ����һ�θ���
		{
			CurrentDetTime=0; //���㸺�ؼ���־λ
			if (DischargeCurrent<DischargeCurrentMin)//if discharge current is low 2.8/4096=0.00068359375��5=0.00341796875/0.05=0.068A
			{
				CurrentSmallTime++;  //�޸��ؼ�ʱ��++
				dischargeFlag=0;     //�޸���ʱ��־λΪ0
				if (CurrentSmallTime>=CurrentSmallTimeSet)  //�����ʱ�������趨ʱ��
				{
					KeyOutputSW=0;      //�ر����
					CurrentSmallTime=0; //�����ʱ��
					
				}
			}
			else    //����ŵ��������
			{
				CurrentSmallTime=0;	 //�����ʱ��
			}
		}
      if(DischargeCurrent>5) //���ؼ��  68ma
	   {
	    dischargeFlag=1;  //�и���ʱ��־λΪ1
					   
	    }
     
	 }
	else
	{
		DCOUTOFF;
		KeyOutputSW=0;
		CurrentMaxTime=0;
		CurrentSmallTime=0;
	}
}

//--���µ�ص�ѹ����---------------------------------
void UpdateBatteryVoltage(void)   //16��ƽ���˲�����
{
	uint8 loop=0;
	uint16 temp,result=0;
	for (loop=0;loop<16;loop++)
	{
	  temp=ADFilter(ChannelVbat);
	  result+=temp;
	   }
	result=result>>4;	
    BatteryVoltage=result;   //��ص�ѹ
}


//--��������-----------------------------------------
void KeyScan(void)
{
	if (!KEY)           //�����⵽����������
	{	
	  DelayUs(20);	    //��ʱ	
		if (!KEY)       //ȷ�ϰ���������
		{
			 KeyPressFlag=1;      //������־λ��1
             SleepWaitTime=0;
			if (DoubleKeyFlag)   //˫���������
			{
				if ((DoubleKeyTime>=50)&&(DoubleKeyTime<100))  //˫���ж�
				{
					DoubleKeyFlag=0;
					DoubleKeyTime=0;
//					LED=~LED;         //˫�����ֵ�Ͳ
				}
			   }			
		     }		
	       }
	else       //���û�м�⵽��������
	{
      	if (KeyPressFlag)
		{
		 	DoubleKeyFlag=1;   //˫��������־λ��1	
			if ((KeyTime>=ShortPressTime)&&(KeyTime<LongPressTime))
			{	
				KeyOutputSW=~KeyOutputSW;							
			}
		   }
		if (DoubleKeyTime>=100)  
		{
			DoubleKeyFlag=0;
			DoubleKeyTime=0;	
		}
		KeyPressFlag=0;
		KeyTime=0;
	    KeyPressFlag=0;	
		KeyReleaseFlag=1;	
	  }
    }

//--AD�ɼ�����----------------------------
uint16 CheckAD(uint8 AD_Channel)
{
	uint16 temp=0;
	uint8 loop;
	ADM=0x80+AD_Channel;
	FGCHS=1;			
	FADS=1;//AD start
	loop=250;
	while((!FEOC)&&loop)
	{
		loop--;
	}
	temp=ADB;
	temp=temp<<4;
	temp=temp|(ADR&0x0F);
	return temp;	
}
//--AD�����˲�����-------------------------
uint16 ADFilter(uint8 AD_Channel)
{
	uint8 loop;
	uint16 temp,max=0,min=0xFFF,result=0;
	FGIE=OFF;//disable interrupt
	for (loop=0;loop<18;loop++)
	{
		temp=CheckAD(AD_Channel);	
		if (temp>max)
		{
			max=temp;
		}
		if (temp<min)
		{
			min=temp;
		}
		result+=temp;
	}
	FGIE=ON;//Enable interrupt
	result=(result-max-min)>>4;	
	return result;
  }

//--��ʱ����ʼ��--------------------------------
void Timer_Setting(void)
{
   TC0M=0x74;      //timer 0 SET OSC  16.000000hz
   FTC0X8=1;       //interior osc frequency/1
   TC0C=0X00;      //starter value
   TC0R=0X00;
   FTC0ENB=1;      //enabled T0 timer
   
   TC1M=0X04;      //timer 1 SET Fcpu 
   FTC1X8=0;       //interior Fcpu frequency/256
   TC1C = 0xB2;	   //TC1C register value, time: 5.000000ms(200.000000HZ)
   TC1R = 0xB2;	   //TC1R register value
   FTC1IRQ = 0;
   FTC1IEN = 1;	   //TC1 Interrupt enable
   FTC1ENB = 1;	   //TC1 enable
   FGIE=1;         //Enable Whole Interrupt 
}

//--��ʱ����US-----------------------------
void DelayUs(uint8 time)
{
	while (time--);
}

void delayS(uint8 temp)
 {
     uint8 aa;
	for(aa=temp;aa>0;aa--)
	  {
	     DelayUs(255);
		 _ClearWatchDogTimer(); 	
	     DelayUs(255);
		  _ClearWatchDogTimer(); 
		 DelayUs(255);	   
	   }    
    }
		 

//--�жϴ�����---------------------------
__interrupt ISR_TC1(void)
{
  if(FTC1IRQ)//4ms              //��ʱ��1�жϣ�4MS�ж�һ��
  {
  	INTRQ=0;                    //�����������жϱ�־λ
	Counter1S++;                //1S��ʱ��++
	if (Counter1S==250)         //���1S��ʱ���ӵ�250��4MS=1000MS=1Sʱ�䵽
	{
    Counter1S=0;                //����1S��ʱ��
	BatteryFullDelayFlag=1;     //���������ʱ��־λ
	BatteryCapUpdateTimeFlag=1; //��ص�������ʱ���־λֵ1�����µ�ص���,1S����һ�Ρ�
	CurrentDetTime=1;           //Current detect flag bit  ����������ʱ���־λֵ1��		
	if (SleepWaitTime<0xFF)     //���߼�ʱ��
	{
    SleepWaitTime++;	        //���߼�ʱ������
	  }
      
	}

	if (KeyPressFlag)//key time det//���������ж�
	{
		KeyTime++;                 //����������ʱ������  
	}

    if (DoubleKeyFlag)    //˫������������
	{
		DoubleKeyTime++;
	}


//--LED1����-----------------------------------------------
	switch(LEDTime++)     //LEDѭ����ʾ����
	  {
	  	case 0:
		{
		    if(BatteryLevel==0)
			{
			LED1OFF;//�������Ȳ�һ
			if (USBDET||dischargeFlag) //������ڳ����߷ŵ�
			{
				if (BatteryLevel==0)   //�������==0 /��ص�ѹ>=2.9 /�ж�Ϊ��һ��LED���� 
				{
					if (Counter1S<125) //���1S��ʱС��0.5S
					{
						LED1ON;        //LED1��
					}		
					else               //����
					{
						LED1OFF;        //LED�ر� ����˸��
					}
				}
				else                   //�����ص�����=0 �ж�Ϊ��ѹ����2.9V-3.45V֮�� 
				{
					LED1ON;            //LED1����
				}	
			}
			else                  //���û���ڳ���ŵ�
			{       
			  LED1ON;             //LED1����
			  }
			  }
			  else
			  {
			   LED1OFF;           //LED�ر� ����˸��
			  }
			break;                //����
		}

//--LED2����---------------------------------------------
		case 1:
		{
			LED2OFF;
			if (BatteryLevel>0)            //�����ص���>0  �����ص�ѹ>=3.45V
			{
				if (USBDET) //������ڳ�磬�����ڷŵ�
				{
					if (BatteryLevel==1)   //�����ص���==1  �����ص�ѹ>=3.45V
					{
						if (Counter1S<125) //���Counter1SС��0.5S
						{
							LED2ON;        //LED2��
						}		
						else               //�������0.5S
						{
							LED2OFF;        //LED��������˸��
						}
					}
					else        //����==1
					{
						LED2ON; //LED2����
					}	
				}
			   else   //����û���ڳ�磬��ŵ�
				{
					LED2ON;  //LED2��
				}	
			}
			break;   //����
		}

//--LED3����----------------------------------------
		case 2:
		{
			LED3OFF;
			if (BatteryLevel>1)        //�����ص���>1  �����ص�ѹ>3.6V
			{
				if (USBDET)  //������ڳ���ŵ�
				{
					if (BatteryLevel==2)    //�����������2 �����ص�ѹ>=3.6V
					{
						if (Counter1S<125)  //���Counter1SС��0.5S
						{
							LED3ON;   //LED3��
						}		
						else          //����
						{
							LED3OFF;   //LED3�أ���˸��
						}
					}
					else     //�������������2
					{
						LED3ON;  //LED3��
					}	
				}
				else   //����û���ڳ�磬��ŵ�
				{
					LED3ON;  //LED3��
				}				
			}
			break;   //����
		}

//--LED4����----------------------------------------
		case 3:
		{
			LED4OFF;
			if (BatteryLevel>2)        //�����ص���>1  �����ص�ѹ>3.6V
			{
				if (USBDET)  //������ڳ���ŵ�
				{
					if (BatteryLevel==3)    //�����������2 �����ص�ѹ>=3.6V
					{
						if (Counter1S<125)  //���Counter1SС��0.5S
						{
							LED4ON;   //LED3��
						}		
						else          //����
						{
							LED4OFF;   //LED3�أ���˸��
						}
					}
					else     //�������������2
					{
						LED4ON;  //LED3��
					}	
				}
				else   //����û���ڳ�磬��ŵ�
				{
					LED4ON;  //LED3��
				}				
			}
			break;   //����
		}

//--LED5����------------------------------------------
		case 4:
		{
			LED5OFF;
			if (BatteryLevel>3)              //�����������2  �����ص�ѹ>3.6V
			{
				if (USBDET)   //������ڳ���߷ŵ�
				{
                  if (!BatteryFullFlag)      //��������־λΪ��
					{
						if (Counter1S<125)   //���Counter1SС��0.5S
						{
							LED5ON;         //LED4��
						}		
						else
						{
							LED5OFF;        //LED4�ء���˸��
						}
					}
					else          //�����������
					{
						LED5ON;   //LED4����
					}	
				}
				else   //�������û���ڳ�磬��ŵ�
				{
					LED5ON;   //LED4��
				}				
			}
			LEDTime=0;   //LEDɨ��ѭ����ʱ������	
			break;       //����
		}
	  }			 
   } 
  INTRQ=0; 
}
//--------------------------------------------------------------
void main(void)
{
  NORMALMODE_MY;
  FirstPowerOn=1;
  IO_Setting();
  Timer_Setting();
  ADC_Setting();
  UserInit();
  FGIE =1;
   
  while(!CurrentDetTime)
  {_ClearWatchDogTimer();}//wait 1S for battery voltage
  while(1)
  {
  	_ClearWatchDogTimer();
  	EnterSleepMode();
  	KeyScan();
  	UpdateBatteryVoltage();
	Charge();
	Discharge();	
	UpdateBatteryCap();
	FirstPowerOn=0;
  }
}
