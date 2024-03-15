#include "main.h"
#include "adc.h"
//extern void Adc_Init(void);
//----------ȫ�ֱ���---------------
u16 DeviceErrorCode=0;  //�豸�������λ�ñ�־
u8 ResetLinitFlag=0;    //���������ޱ�־�������������ޱ�־,
u8 Wifi_ConnetRightFlag=0;  //wifi������������λ�ñ�־λ
u8 Wifi_ConnetSeverFlag=0;  //wifi���ӷ�������������λ�ñ�־λ
u8 ResetRomFlag=0;  //TIM2���յ���λROM��Ϣ�����ݻ���λ�ñ�־
u8 Cmd_Flag_OK=0;   //�����־,Ӧ��OK
u8 Cmd_SendErrorFlag=0;    //��������ʧ��ʱ��λ�ñ�־,���ܷ��Ͳ�������
u8 SendFluxDoingFlag=0; //���ڷ������������Ϣʱ����λ�ñ�־,��ֹ�ظ�������������ϢƵ������ROM
u8 Timer3_EnableFlag=0; //��ʱ��3ʹ�ܱ�־,��λ�ñ�־��ʹ�ܶ�ʱ��3����ͨ��ʱ��
u8 SolenoidValveOnFlag=0;   //��ŷ��򿪱�׼,��ŷ��򿪽���λ�ñ�־
u8 FluxStartFlag=0; //��ʼ����������ݱ�־,������ʱ����λ�ñ�־

char RecvicePN[32]="";  //������յ���PN
u16 FluxValue_Now=0;    //ʵʱ����ֵ
u16 Flowmeter_H=0;  //����������
u16 Flowmeter_L=0;  //����������
//--------------------------------

//---------��C�ļ���Ч֮����-------
u8 CommSuccess_Flag=0; //����һ,����ͨѶ�ɹ���־
u8 RightRomOk_Flag=0; //�����,ȷ��ROMֵ�ɹ���־
//--------------------------------

// LED_RUN
// LED_RGB
// V_SIGNAL
char data1[] = "CMD,PN9898989898E,H3009,L2001,LINE2,END";
char ActivePN[32]="";  //���ڴ洢��ǰ������Ϣ
int main(void)
{

    char iDataBuf[128];
    char iTemp_Buf[64];
    u16 iTemp=0;




    delay_init();	    	 //��ʱ������ʼ��
    NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	 	//���ڳ�ʼ��
    uart3_init(115200);
    TIMx_Int_Init(TIM3,2000,7200);	//��ʼ��3,������ͨ��ʱ���� 1000 = 100ms
    TIMx_Int_Init(TIM2,2000,7200);	//��ʼ��2,���ڼ���������,1000=100ms
    DeviceErrorCode = 0;
    ResetLinitFlag = 1; //Ϊ1����Ҫ��ȡ������
    ResetRomFlag = 1;   //��ʼʱ��Ҫ���л�ȡ
    CommSuccess_Flag=0; //����һ,����ͨѶ�ɹ���־,��ʼ����ΪOK
    RightRomOk_Flag=0;  //�����,ȷ��ROMֵ�ɹ���־,��ʼ����ΪOK
    Wifi_ConnetRightFlag = 0;//������Ϊ1
    Cmd_Flag_OK = 0;

    motor_Init();
    LED_Init();			     //LED�˿ڳ�ʼ��
    KEY_Init();              //��ʼ���밴�����ӵ�Ӳ���ӿ�
    Adc_Init();
    MCU_RED=1; MCU_YELLOW=1; MCU_GREEN=1; MCU_BEEP=0; MCU_LOCK=0;
    Cmd_SendErrorFlag=0;
    test_mcu=1;
    while(1)
    {

        while(0) //Debug
        {
			ENA=1;
		//	PUL=0;
			motor_R(1000,1);			
        }

        usart3_printf("Location 1\r\n");
        while(0 == Wifi_ConnetRightFlag)    //WIFI δ������������WiFi
        {
            iTemp = WIFI_Config();
            if(0 != iTemp)
            {
                DeviceErrorCode |= ERRORCODE_CONNECT;
            }
            else
            {
                DeviceErrorCode = 0;    //���������豸�쳣
                Wifi_ConnetRightFlag = 1; //����WiFi��������
                CommSuccess_Flag = 0;   //������һ����������
                TIM_Cmd(TIM2,ENABLE);	//������ʱ��
				MCU_RED=0; MCU_YELLOW=0; MCU_GREEN=1;
                break;
            }
            delay_ms(100);
        }
        while(Wifi_ConnetRightFlag)
        {
            while(1)
            {
				TIM_Cmd(TIM2,DISABLE);	//�رն�ʱ��
				MCU_YELLOW=1; MCU_RED=0; MCU_GREEN=0;
                if(IR_UP==0)							//��IR_UP��⵽����ʱ������ǰѭ��
                {
					
					
                    delay_ms(10);
                    if(IR_UP==0)
                    {
                        break;
                    }
				}
                else
				{
					while(motor_R(1000,1));				//��IR_UP��ⲻ������ʱ�����������ת����ֱ����⵽��������ѭ��
				}
					break;
            }
            motor_S();
            delay_ms(10);
            while(MCU_start!=0)
            {
                delay_ms(1);
                while(MCU_start!=0);
            }										//��ʼ����
            MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;
			delay_ms(10);
			
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);motor_R(4000,0);
			motor_S();
			
			delay_ms(1000);
            while(1)
            {
                ////////////////////////////////////////
                ////////////////////////////////////////
                /**************�����������************/
                weigh();											//����
                delay_ms(10);
                if(w<=0.2)											//�ж��Ƿ�<=1KG
                {

                    while(1)  //���λ��
                    {
                        if(IR_UP==0)
                        {
                            delay_ms(10);
                            if(IR_UP==0)
                            {
                                break;
                            }
                        }
                        else
						{
							delay_ms(10000);
                            while(motor_R(1000,1))
							{
							}
							break;
						}
							
                    }

                    motor_S();
                    delay_ms(10);
                    while(MCU_start!=0) 								//��⿪ʼ���п����Ƿ񴥷�
                    {
                        delay_ms(1);
                        while(MCU_start!=0) 								//��⿪ʼ���п����Ƿ񴥷�
                        {
                            MCU_YELLOW = 1; MCU_RED=0; MCU_GREEN = 0;
                        }
                    }
                    MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;
					motor_S();
					delay_ms(10);
					
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
					motor_S();
					delay_ms(1000);
                }
                ////////////////////////////////////////
                ////////////////////////////////////////
                /**************��λ���ģ��************/

                if(TBA_MCU==0 && TBB_MCU==0) 					//�����ﵽԤ��λ�ã������豸���ܴ��ڹ���
                {
                    motor_S();
                    MCU_RED=0; MCU_YELLOW=1; MCU_GREEN=1;		//�Ƶơ��̵������������ﵽ̽��A��B��
                }
                else if(TBA_MCU==0 && TBB_MCU==1)				//���������������ֹͣת��
                {
                    motor_S();
                    MCU_RED=0; MCU_YELLOW=0; MCU_GREEN=1;
                }
                else if(TBA_MCU==1 && TBB_MCU==0) 				//�����ﵽԤ��λ�ã���̽��A�쳣
                {
                    motor_S();
                    MCU_RED=1; MCU_YELLOW=1; MCU_GREEN=0;		//��ơ��Ƶ�������̽��A�쳣
                }

                else if(TBA_MCU==1 && TBB_MCU==1) 				//�������ͣ���Ҫ����
                {
                    MCU_GREEN = 1; MCU_RED=0; MCU_YELLOW=0;
                    motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
										motor_S();
										delay_ms(10);

					if(IR_DOWN==0)							//��IR_UP��⵽����ʱ������ǰѭ��
					{
						delay_ms(10);
						if(IR_DOWN==0)
						{
							while(motor_R(1000,1))				//��IR_UP��ⲻ������ʱ�����������ת����ֱ����⵽��������ѭ��
							{
							}
							motor_S();
							delay_ms(10);
							while(MCU_start!=0)
							{
								delay_ms(1);
								while(MCU_start!=0);
							}
						
						
							
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_S();
						}
					}							
				
					
                    weigh();                    delay_ms(10);//ʱ����˳���Ῠ��
                    //sprintf(iTemp_Buf,"CMD,OFF%dFAILH,LINED-L3,END",w);
                    Str_DoubleToStr(iTemp_Buf,w,2);
                    sprintf(iDataBuf,"TIN_WEIGHT,%s,LINED-L3,END",iTemp_Buf);

                    if(w<=0.2)
                    {

                        while(1)
                        {
                            if(IR_UP==0)
                            {
                                delay_ms(10);
                                if(IR_UP==0)
                                {

                                    break;
                                }
                            }
                            else
                                while(motor_R(1000,1))
								{
								}
								break;
                        }
                        motor_S();
                        delay_ms(10);
                        while(MCU_start!=0)
                        {
                            delay_ms(1);
                            while(MCU_start!=0)
                            {
                                MCU_YELLOW = 1; MCU_RED=0; MCU_GREEN=0;
                            }
                        }
                        MCU_YELLOW=0; MCU_RED=0; MCU_GREEN=1;  
						delay_ms(10);
						
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);motor_R(1000,0);
						motor_S();
					}
					delay_ms(10);
                    motor_S();
                    delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000); delay_ms(1000);	//���ֹͣ�ȴ������ڻ�ʱ��
                    WIFI_SendData(iDataBuf,"SEND OK",2000);
                    while(Cmd_SendErrorFlag)
                    {

                    }
                }

                motor_S();
            }


        }

    }
}



