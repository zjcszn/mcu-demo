#include "RTL.h"
#include "stm32f10x.h"
#include "rtc.h"
#include "dbg.h"

#define YEAR_BASE 2015

//�·�      1  2  3  4  5  6  7  8  9  10 11 12
//����     31 29 31 30 31 30 31 31 30 31 30 31
//������ 31 28 31 30 31 30 31 31 30 31 30 31

/**
  * @brief  �ж��Ƿ�������
  * @param  year: �������
  * @retval 1 ��   0 ����
  */
u8 Is_Leap_Year(u16 year)
{
	if (year % 4 == 0) //�����ܱ�4����
	{
		if (year % 100 == 0)
		{
			if (year % 400 == 0)
				return 1; //�����00��β,��Ҫ�ܱ�400����
			else
				return 0;
		}
		else
			return 1;
	}
	else
		return 0;
}



//�·����ݱ�
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
//ƽ����·����ڱ�
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

/**
  * @brief  ����ʱ��  �������ʱʱת��Ϊ���� ��1970.1.1Ϊ��׼  1970~2099��Ϊ�Ϸ����
  * @param  syear: ���
  * @param  smon:  �·�
  * @param  sday:  ��
  * @param  hour:  ʱ
  * @param  min:   ��
  * @param  sec:   ��
  * @retval 0 �ɹ�   ���� ����
  */
u8 TIME_Set(time_t *time)
{
	u16 t;
	u32 seccount = 0;
	u8 is_leap_year = Is_Leap_Year(time->year);

	if (time->year < 2000 || time->year > 2099)
		return 1; //syear��Χ1970-2099���˴����÷�ΧΪ2000-2099

	for (t = YEAR_BASE; t < time->year; t++) //��������ݵ��������
	{
		if (Is_Leap_Year(t))
			seccount += 31622400; //�����������
		else
			seccount += 31536000; //ƽ���������
	}

	time->mon -= 1;

	for (t = 0; t < time->mon; t++) //��ǰ���·ݵ����������
	{
		seccount += (u32) mon_table[t] * 86400; //�·����������
		if (is_leap_year && t == 1)
			seccount += 86400; //����2�·�����һ���������
	}

	seccount += (u32) (time->day - 1) * 86400; //��ǰ�����ڵ����������
	seccount += (u32) time->hour * 3600; //Сʱ������
	seccount += (u32) time->min * 60; //����������
	seccount += time->sec; //�������Ӽ���ȥ

	//����ʱ��
	/*  RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
	 RCC->APB1ENR|=1<<27;//ʹ�ܱ���ʱ��
	 PWR->CR|=1<<8;    //ȡ��������д����
	 //���������Ǳ����!*/

	RTC_WaitForLastTask();
	RTC_SetCounter(seccount);
	RTC_WaitForLastTask();
	return 0;
}




u8 TIME_Get_Week(u16 year, u8 month, u8 day);
/**
  * @brief  ��ȡʱ��
  * @param  year: �������
  * @retval 0 �ɹ�   ���� ����
  */
u8 TIME_Get(time_t *time)
{
	//static u16 daycnt = 0;
	u32 timecount = 0;
	u32 temp = 0;
	u16 temp1 = 0;
	timecount = RTC_GetCounter();  //�õ��������е�ֵ(������)
	temp = timecount / 86400; //�õ�����(��������Ӧ��)

	//dbg_msg("cnt  %d\n", timecount);

	//if (daycnt != temp) //����һ����
	//{
		//daycnt = temp;
		temp1 = YEAR_BASE; //��1970�꿪ʼ
		while (temp >= 365)
		{
			if (Is_Leap_Year(temp1)) //������
			{
				if (temp >= 366)
					temp -= 366; //�����������
				else
				{
					temp1++;
					break;
				}
			}
			else
				temp -= 365; //ƽ��
			temp1++;
		}
		time->year = temp1; //�õ����
		temp1 = 0;
		while (temp >= 28) //������һ����
		{
			if (Is_Leap_Year(time->year) && temp1 == 1) //�����ǲ�������/2�·�
			{
				if (temp >= 29)
					temp -= 29; //�����������
				else
					break;
			}
			else
			{
				if (temp >= mon_table[temp1])
					temp -= mon_table[temp1]; //ƽ��
				else
					break;
			}
			temp1++;
		}
		time->mon = temp1 + 1; //�õ��·�
		time->day = temp + 1; //�õ�����
	//}
	temp = timecount % 86400; //�õ�������
	time->hour = temp / 3600; //Сʱ
	time->min = (temp % 3600) / 60; //����
	time->sec = (temp % 3600) % 60; //����
	time->week = TIME_Get_Week(time->year, time->mon, time->day); //��ȡ����

	return 0;
}


/**
  * @brief  ��������
  * @param  year: ���
  * @param  mon:  �·�
  * @param  day:  ��
  * @retval ����
  */
u8 TIME_Get_Week(u16 year, u8 month, u8 day)
{
	u16 temp2;
	u8 yearH, yearL;
	yearH = year / 100;
	yearL = year % 100;

	// ���Ϊ21����,�������100

	if (yearH > 19)
		yearL += 100;
	// ����������ֻ��1900��֮���
	temp2 = yearL + yearL / 4;
	temp2 = temp2 % 7;
	temp2 = temp2 + day + table_week[month - 1];
	if (yearL % 4 == 0 && month < 3)
		temp2--;
	return (temp2 % 7);
}





static void RTC_Configuration()
{
	u32 timeout = 0x50000;

	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	//PWR_BackupAccessCmd(ENABLE);

	//BKP_DeInit();
	RCC_LSEConfig(RCC_LSE_ON);

	while (!RCC_GetFlagStatus(RCC_FLAG_LSERDY) && (--timeout));  //���ú���Ҫ�ȴ�����

	if(timeout == 0)
	{
		dbg_msg("LSE ON timeout\n");
		return;
	}

	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);  //ѡ��LSEΪRTC�豸��ʱ��
	RCC_RTCCLKCmd(ENABLE);  //ʹ��
	RTC_WaitForSynchro();  //�ȴ�ͬ��

	RTC_WaitForLastTask();
	RTC_SetPrescaler(32767);
	RTC_WaitForLastTask();

	//RTC_ITConfig(RTC_IT_SEC,ENABLE);
	//RTC_WaitForLastTask();
}


void RTC_Init()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);

	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
	{
		time_t time;
		time.year = 2015;
		time.mon  = 1;
		time.day  = 1;
		time.hour = 0;
		time.min  = 0;
		time.sec  = 0;

		RTC_Configuration();

		TIME_Set(&time);

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	}
	else
	{
		RTC_WaitForSynchro();
	}

	RCC_ClearFlag();
}
















