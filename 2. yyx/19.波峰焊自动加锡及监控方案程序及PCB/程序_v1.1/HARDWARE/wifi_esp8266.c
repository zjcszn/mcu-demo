#include "wifi_esp8266.h"

char WifiCmd_RST[]        ="AT+RST";
char WifiCmd_Mode[]       ="AT+CWMODE=3"; 
//char WifiCmd_SendSSID[]   ="AT+CWSAP=\"SMT-01\",\"12345678\",1,4";
char WifiCmd_SendSSID[]   ="AT+CWSAP=\"Dip5_BFHJX\",\"12345678\",1,4";
char WifiCmd_MuxMode[]    ="AT+CIPMUX=0";
//char WifiCmd_ConnetWIFI[] ="AT+CWJAP=\"kkk\",\"1234567890\""; 
char WifiCmd_ConnetWIFI[] ="AT+CWJAP=\"TPwarehouse\",\"123456789\""; 
//char WifiCmd_ConnetWIFI[] ="AT+CWJAP=\"SMT-01\",\"12345678\"";
//char WifiCmd_ConnetWIFI[] ="AT+CWJAP=\"ADV\",\"12345678\"";
char WifiCmd_ConnetSever[]="AT+CIPSTART=\"TCP\",\"172.21.140.114\",80";
char WifiCmd_CIPMODE[]    ="AT+CIPMODE=0";  //��ʹ��͸��
char WifiCmd_UrlHead[]    ="Equipment_Network/DataMonitor/Communicate.php?";
char WifiCmd_HostIP[]    ="192.168.0.22";
void delay(u16 nus)
{
    u16 yy = 0;
    while(nus--)
        for(yy=10;yy>0;yy--); //1us
}


//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//����,�ڴ�Ӧ������λ��(str��λ��)
u8 WIFI_ChkCmd(char* str,char* str1,char* str2)
{
	if(USART1_RX_STA&0x8000)		//���յ�һ��������
	{ 
        if(strlen(str) || strlen(str1) || strlen(str2))
        {
            if(0 != Str_Pos((char*)USART1_RX_BUF,(char*)str) )
            {
                return 1;
            }
            else if(0 != Str_Pos((char*)USART1_RX_BUF,(char*)str1) )
            {
                return 2;
            }
            else if(0 != Str_Pos((char*)USART1_RX_BUF,(char*)str2) )
            {
                return 3;
            }
        }
	} 
	return 0;
}

//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 WIFI_SendCmdOnce(char* cmd,char* ack,char* orack1,char* orark2,u16 waitTime)  
{
	u8 res=0; 
    char pCmd[128];
    //Str_UpperCase(cmd); 
    //Str_UpperCase(ack); 
    sprintf(pCmd,"%s\r\n",cmd);
	USART1_RX_STA=0;    
	printf("%s",pCmd);	//��������
	if(ack&&waitTime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waitTime)	//�ȴ�����ʱ
		{
			delay_ms(1);
            if(USART1_RX_STA&0x8000)//���յ��ڴ���Ӧ����
			{
				if(WIFI_ChkCmd(ack,orack1,orark2)) 
				{
					usart3_printf("cmd ack ok:%s\r\n",ack);
					break;//�õ���Ч���� 
				}
                USART1_RX_STA=0;
			}
		}
		if(waitTime==0)
            res=1; 
	}
    USART1_RX_STA=0;
    delay_ms(50);
	return res;
} 
u8 WIFI_SendCmd(char* cmd,char* ack,char* orack1,char* orark2,u16 waitTime)
{
    u8 i=0;
    TIM_DISABLE(); 
    for(i=0;i<5;i++)
    {
        if(0 == WIFI_SendCmdOnce(cmd,ack,orack1,orark2,waitTime) )
        {
            TIM_ENABLE(); 
            delay_ms(20); //�ȴ�TIM2�ж�
            return 0;
        }
        delay_ms(50);
    }
    TIM_ENABLE(); 
    return 1;
}

//
//��ATK-ESP8266����ָ������
//data:���͵�����(����Ҫ��ӻس���)
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
u8 WIFI_SendDataOnce(char* dat,char* ack,u16 waittime)
{
	//u8 res=0; 
    u16 iLocation=0; 
    char iTemp[]="AT+CIPSEND=";
    char iCmd[32];  
    char iDatas[1024];

    if(0 != WIFI_ConnetSever() )
    {
		MCU_RED=1; MCU_YELLOW=1; MCU_GREEN=1;
        return 1;
    }
    StrToCmd(iDatas,dat);     
    sprintf(iCmd,"%s%d",iTemp,strlen(iDatas));
	if(0 != WIFI_SendCmd(iCmd,"OK","","",2000) )
    {
        return 1;
    }    
    USART1_RX_STA = 0;
	printf("%s",iDatas);	//��������   
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(1);
			if(USART1_RX_STA&0x8000)//���յ��ڴ���Ӧ����
			{
				iLocation = WIFI_ChkCmd(ack,"","");
                if(iLocation)
				{					
                    iLocation = Str_Pos((char*)USART1_RX_BUF,"+IPD");
                    Str_SubString((char*)USART1_RX_BUF,iLocation,(USART1_RX_STA-iLocation+1));
                   // usart3_printf(iDatas,(u8*)ack);
                    usart3_printf((char*)USART1_RX_BUF);
                    return 0;
					//break;//�õ���Ч���� 
				}
                else
                    USART1_RX_STA=0;
			}
		}
		//if(waittime==0)res=1; 
	}
    //USART1_RX_STA=0;    
	return 1;
}

u8 WIFI_SendData(char* dat,char* ack,u16 waittime)
{
    u8 i=0;
    TIM_DISABLE();
    for(i=0;i<1;i++)
    {
        if(0 == WIFI_SendDataOnce(dat,ack,waittime) ) 
        {
            TIM_ENABLE();
            delay_ms(100); //�ȴ�TIM2�жϳ��������������ʱ�����˿��ܳ������ж���
            return 0;
        }
        delay_ms(50);
    }
    Wifi_ConnetRightFlag = 0;//wifi����ʧ�ܣ���������WiFi
    TIM_ENABLE();   
    return 1;
}

//wifi����,�ɹ�����0
u8 WIFI_ConfigOnce(void)
{
    //u8 iFlag=0;
    //TIM_DISABLE();   
    if(WIFI_SendCmd("AT","OK","","",2000) )
    {
        DeviceErrorCode |= ERRORCODE_CONNECT;
        return 1;
    }
        
    if(0 != WIFI_SendCmd(WifiCmd_RST,"ready","","",2000) ) 
    {
        //TIM_ENABLE();   
        return 1;
    }
    if(0 != WIFI_SendCmd(WifiCmd_Mode,"OK","","",2000) )
    {
        //TIM_ENABLE();   
        return 1;
    }
    if(0 != WIFI_SendCmd(WifiCmd_SendSSID,"OK","","",2000) )
    {
        //TIM_ENABLE();   
        return 1;
    }
    if(0 != WIFI_SendCmd(WifiCmd_ConnetWIFI,"WIFI CONNECTED","","",2000) )
    {
        if(0 == WIFI_SendCmd(WifiCmd_ConnetWIFI,"+CWJAP:","","",30000))  //wait 30s
        {
            DeviceErrorCode |= ERRORCODE_NOWIFI;             
        }               
        //TIM_ENABLE();   
        return 1;
    }   
    if(0 != WIFI_SendCmd("AT+CIPMUX=0","OK","","",2000) )
    {
        //TIM_ENABLE();   
        return 1;
    }

    DeviceErrorCode &= (~ERRORCODE_CONNECT);    //ȡ���ñ�־λ 
    DeviceErrorCode &= (~ERRORCODE_NOWIFI);     //ȡ���ñ�־λ  
    return 0;   
}
u8 WIFI_Config(void)
{
    u8 i=0;
    for(i=0;i<1;i++)
    {
        if(0 == WIFI_ConfigOnce() )
        {
            return 0;
        }
        delay_ms(50);
    }
    Wifi_ConnetRightFlag = 0;   //wifi����ʧ��,����WIFI�ѹر� 
    return 1;
}


//wifi���ӷ�����,�ɹ�����0
u8 WIFI_ConnetSever(void)
{
    u8 iFlag=0;
    TIM_DISABLE();   
    if(0 == WIFI_SendCmd(WifiCmd_ConnetSever,"OK","ALREADY","",2000) )
    {
        iFlag = 0;
    }        
    else
    {
        iFlag = 1;
    }    
    TIM_ENABLE();   
    return iFlag;
}
//��������--KYO
void StrToCmd(char* p,char* cmds) 
{
    char iCmd[512];
    sprintf(iCmd,"GET /%sCMD=%s HTTP/1.1\r\n",WifiCmd_UrlHead,cmds);
    sprintf(iCmd,"%sContent-Type: text/html;charset=utf-8\r\n",iCmd);
    sprintf(iCmd,"%sHost: %s\r\n",iCmd,WifiCmd_HostIP);
    sprintf(iCmd,"%sUser-Agent: abc\r\n",iCmd);
    sprintf(iCmd,"%sConnection: Keep Alive\r\n",iCmd);
    sprintf(p,"%s\r\n",iCmd);
}

//��doubleת��Ϊ�ַ��ͣ����Է����ַ���ָ��
//����ֵ-p,x-ת��ֵ��
//num-ת�����ȿ��ƣ�1-7λ
void Str_DoubleToStr(char* p, double x, u8 num)
{
    char pTemp[32]="";
    u8 iLoop_i = 0;
    sprintf(p, "%d.", (u16)x);
    x = x - (u16)x;
    if (x < 1e-7)
    {
        sprintf(p, "%s0", p);
        return;
    }
    if ((0 == num) || (7 < num)) num = 7;
    while (x > 1e-7)
    {
        x = x * 10;
        if (iLoop_i++ > num - 1)  break;
        if (0==(u16)x)
            sprintf(pTemp, "%s0", pTemp);
        else
        {
            sprintf(pTemp, "%s%d", pTemp, (u16)x);
            sprintf(p, "%s%s", p, pTemp);
            sprintf(pTemp, "");
        }
        x = x - (u16)x;

    }
}


