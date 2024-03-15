/**********************************************************
						���ߣ����زر���
						���̣�ILoveMCU.taobao.com

	�����̽��������ڱ��깺��ģ����û�ʹ�ã������������̵ĵ�����Ϊ
	��Ȩ���У�����ؾ�������
	SIM800 GSM/GPRSģ��
	https://item.taobao.com/item.htm?id=521599424969
	����ʹ��STM32��Сϵͳ��
	https://item.taobao.com/item.htm?id=523336310868
	STC12C5A60S2��Сϵͳ��
	https://item.taobao.com/item.htm?id=524701473371
	USB-TTLģ�飺
	https://item.taobao.com/item.htm?id=39481188174
	�ƶ���Դ﮵����װ
	https://item.taobao.com/item.htm?id=530904849115
	�ƶ����Ϲ�����﮵����װ��
	https://item.taobao.com/item.htm?id=530904849115
	
	����˵����
	STM32					SIM800C
	3.3V	------>	V_MCU
	GND		<----->	GND
	PA3		<------	T_TX
	PA2		------>	T_RX
	

	//���ڵ��Կɲ���
	STM32					USB-TTLģ��
	GND		------>	GND
	TX1		------>	RXD
***********************************************************/

#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "usart2.h"
#include "timer.h"
#include "exti.h"
#include "key.h"
//#include "iostream"
//using namespace std;


//����
#define Success 1U
#define Failure 0U

//�������
unsigned long  Time_Cont = 0;       //��ʱ��������
unsigned int count = 0;

extern u8 locknum;  //����Ƕȼ���
extern u8 lockclose;   //�ֶ�������־
u8 lockflag=0;     //��������ɣ��ֶ�������Ч��־
int i;

void errorLog(int num);
void lock_turn(void);
void lock_stop(void);
void open_lock(void);
int web_flag(void);
unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
unsigned int sendCommand_(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
unsigned int sendCommand_1(char *Command, char *Response, unsigned long Timeout, unsigned char Retry);
void Sys_Soft_Reset(void);
void close_lock(void);
void web_ready(void);
void beep(void);  //������


int main(void)
{	
	int openflag = 0;
	u16 led0pwmval=0;
	u8 dir=1;	
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
 	TIM3_PWM_Init(899,0);	 //����Ƶ��PWMƵ��=72000000/900=80Khz

	delay_init();
	uart_init(9600);	 //���ڳ�ʼ��Ϊ9600
	USART2_Init(9600);	//����2������9600
	KEY_Init();         	//��ʼ���밴�����ӵ�Ӳ���ӿ�
	EXTIX_Init();		 	//�ⲿ�жϳ�ʼ��
	u2_printf("AT\r\n");
	Init_LEDpin();
	Init_LEDpin_1();
	
	
	printf("Welcome to use!\r\n");
	printf("ILoveMcu.taobao.com!\r\n");
	web_ready();   //������ʼ��
	beep();
	delay_ms(50);
	TIM_SetCompare2(TIM3,450);   //��pwm
	while(1)
	{	
		
		if(locknum>3)           //��飬��ֹ����
		locknum=0;
		if(lockflag==0)
		{
		    openflag = web_flag();  //����Ƿ���Ҫ����
				if(openflag==1)         //�綯����
				{
						printf("\r\n we need open the lock now!\r\n");
						open_lock();
						delay_ms(1000);
				}
				else if(openflag==0)
					printf("\r\n we don't need open the lock now!\r\n");
		}
	
		if(lockclose==1&&lockflag==1)  //�ֶ�����
			{
				beep();
				lockclose=0;
			  close_lock();
			}
			printf("\r\n openflag: %d, lockclose: %d, lockflag: %d \r\n",openflag,lockclose,lockflag);
		delay_ms(1000);
		
	}  //ѭ������
	delay_ms(5000);
	
}
///////////////////////////////////////////////////////
void open_lock(void) //�����ɹ���ҳ
{
	beep();
	lock_turn();
	delay_ms(2);
	
	while(locknum<=3)
	{
		printf("locknum:  %d\r\n",locknum);
		if(locknum==3)
		{
		 lock_stop();
     delay_ms(10);		
     locknum=0;
			lockflag=1;		
    printf("�����ͣ\r\n");			
		 break;
		}
		delay_ms(2);
	}
	 printf("web_open\r\n");
	while(1)
	{
			if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/open.php\"\r\n","OK",5000,2) == Success);
			else errorLog(12);
			if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success)
				break;
			else errorLog(13);
			delay_ms(20);
	}
}

void close_lock(void) //������ҳ
{
	lockclose=0;
	lockflag=0;
	printf("���ֶ�����\r\n");
	beep();
	while(1)
	{
	   if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/close.php\"\r\n","OK",5000,2) == Success);
		 else errorLog(12);
		if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success)
			break;
		else errorLog(13);
		 delay_ms(20);
	}
}


 int web_flag() //����flag��ҳ
{
	
	 if(sendCommand("AT+HTTPPARA=\"URL\",\"www.qyc2018.top/lock/flag.php\"\r\n","OK",5000,2) == Success);
		else errorLog(12);
		delay_ms(20);
	 if(sendCommand_1("AT+HTTPACTION=0\r\n","HTTPACTION",20000,10) == Success);
		else errorLog(13);
		delay_ms(5000);
		
	 if(sendCommand_("AT+HTTPREAD\r\n","HTTPREAD",20000,10) == Success);  // ��ȡ����
		else errorLog(14);
		delay_ms(4000);
	 if (strstr(USART2_RX_BUF,"lockflag1") != NULL)
	 {
	    printf("web_success\n lockflag1\n");
		 return 1;
	 }
		else if(strstr(USART2_RX_BUF,"lockflag0") != NULL)
		{
		   printf("web_success\n lockflag0\n");
			return 0;
		}
		else
		{
		  printf("web_fail \r\n");
			printf("\n vlaue\r\n");
			printf(USART2_RX_BUF);
			delay_ms(500);
			return 2;
		}	
		
}


unsigned int sendCommand_1(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//����GPRSָ��
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			Time_Cont += 100;
			for(i=0;i<6;i++)
			{
			    delay_ms(1000);
					while(strncmp(USART2_RX_BUF,USART2_RX_FLAG,200)<=0);
					if (strstr(USART2_RX_BUF, Response) != NULL)
					{				
						printf("\r\n***************receive1****************\r\n");
						printf(USART2_RX_BUF);
						USART2_CLR_Buf();
						return Success;
					}
			}
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive2****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}


unsigned int sendCommand_(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//����GPRSָ��
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			Time_Cont += 100;
			for(i=0;i<6;i++)
			{
			   delay_ms(300);
				if (strstr(USART2_RX_BUF, Response) != NULL)
				{				
					printf("\r\n***************receive1****************\r\n");
					printf(USART2_RX_BUF);
					return Success;
				}
			}
			
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive2****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}

unsigned int sendCommand(char *Command, char *Response, unsigned long Timeout, unsigned char Retry)
{
	unsigned char n;
	USART2_CLR_Buf();
	for (n = 0; n < Retry; n++)
	{
		u2_printf(Command); 		//����GPRSָ��
		
		printf("\r\n***************send****************\r\n");
		printf(Command);
		
		Time_Cont = 0;
		while (Time_Cont < Timeout)
		{
			Time_Cont += 100;
			for(i=0;i<5;i++)
			{
			  delay_ms(100);
				if (strstr(USART2_RX_BUF, Response) != NULL)
				{				
					printf("\r\n***************receive****************\r\n");
					printf(USART2_RX_BUF);
					USART2_CLR_Buf();
					return Success;
				}
			}
		}
		Time_Cont = 0;
	}
	printf("\r\n***************receive****************\r\n");
	printf(USART2_RX_BUF);
	USART2_CLR_Buf();
	return Failure;
}

void web_ready(void) //������ʼ��
{
    //����HTTP����
	if (sendCommand("AT\r\n", "OK\r\n", 3000, 10) == Success);
	else errorLog(1);
	delay_ms(10);

	if (sendCommand("AT+CPIN?\r\n", "READY", 1000, 10) == Success);
	else errorLog(2);
	delay_ms(10);


	if (sendCommand("AT+CGCLASS=\"B\"\r\n", "OK\r\n", 1000, 10) == Success);
	else errorLog(3);
	delay_ms(10);

	if (sendCommand("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n", "OK\r\n", 5000, 2) == Success);
	else errorLog(4);
	delay_ms(10);

	if (sendCommand("AT+CGATT=1\r\n","OK\r\n", 1000, 10) == Success);
	else errorLog(5);
	delay_ms(10);
			
	if(sendCommand("AT+CGATT?\r\n","OK\r\n",1000,10) == Success);
	else errorLog(8);
	delay_ms(10);
	
	if(sendCommand("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n","OK\r\n",1000,10) == Success);
	else errorLog(6);
	delay_ms(10);
					
	if(sendCommand("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r\n","OK\r\n",1000,10) == Success);
	else errorLog(7);
	delay_ms(10);

	if(sendCommand_("AT+SAPBR=1,1\r\n","OK\r\n",1000,10) == Success);
	else errorLog(8);
	delay_ms(10);
	
	//��ʼ��http����
	if(sendCommand("AT+HTTPINIT\r\n","OK\r\n",5000,2) == Success);
	else errorLog(11);
	delay_ms(10);
}

void lock_turn(void)
{
	  locknum=0;
		GPIO_SetBits(GPIOB,GPIO_Pin_6);
    GPIO_ResetBits(GPIOB,GPIO_Pin_7);
}

void lock_stop(void)
{
	  GPIO_ResetBits(GPIOB,GPIO_Pin_6);
    GPIO_ResetBits(GPIOB,GPIO_Pin_7);
	  lockclose=0;
	  lockflag=1;		
}

void beep(void)  //������
{
   GPIO_SetBits(GPIOB,GPIO_Pin_8);
	delay_ms(100);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	delay_ms(100);
	GPIO_SetBits(GPIOB,GPIO_Pin_8);
	delay_ms(100);
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	delay_ms(10);
}

void errorLog(int num)
{
	printf("ERROR%d\r\n",num);
	while (1)
	{
		if (sendCommand("AT\r\n", "OK\r\n", 100, 10) == Success)
		{
			Sys_Soft_Reset();
		}
		delay_ms(200);
	}
}

void Sys_Soft_Reset(void)
{  
    SCB->AIRCR =0X05FA0000|(u32)0x04;      
}