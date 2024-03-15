#include "datetime.h"
#include "stdio.h"
#include "memory.h"
#include "string.h"

static char time_str[40];
static tm local_time;

/*
��������: time

����ԭ��: time_t sys_time(time_t *timer)
��������: �õ�ϵͳ��ǰʱ��,���趨ϵͳ��ǰʱ��
��������: ϵͳ��ǰʱ��
����˵��: t=NULLʱ�õ���������ʱ�䣬tΪ��Чָ��ʱ������tΪϵͳ��ǰʱ�䣬t��һ��tm����
*/
tm *sys_time(void *p)
{
    tm *t = (tm *)p;
    
    if(t==NULL)
        return &local_time;

    local_time.year = t->year;
    local_time.month = t->month;
    local_time.mday = t->mday;
    local_time.hour = t->hour;
    local_time.min = t->min;
    local_time.sec = t->sec;
	local_time.wday = t->wday;
    
    local_time.tick = t->hour*3600+t->min*60+t->sec;
    
    return &local_time;
}

//"YYYY-MM-DDThh:mm:ss"
char *xml_time(void *set_t)
{
    tm *t = (tm *)set_t;

    if(t==NULL){
        t = &local_time;
        sprintf(time_str,"%04d%s%02d%s%02d%s%02d%s%02d%s%02d",
        t->year,"-",t->month,"-",t->mday,
        "T",
        t->hour,":",t->min,":",t->sec
        );
    }
    return time_str;
}

char *tm2str(tm *time)
{
    tm *t = (tm *)time;

    sprintf(time_str,"%04d%s%02d%s%02d%s%02d%s%02d%s%02d",
    t->year,"-",t->month,"-",t->mday,
    "T",
    t->hour,":",t->min,":",t->sec
    );

    return time_str;
}
   
void UTCTime2LocalTime(tm *l_time, tm *utc_time, int iTimeZone)
{
    int iHourInterval,iInterval,iDays=0;
    tm lc_time;
    
    memset(&lc_time,0,sizeof(tm));   
    memcpy(&lc_time, utc_time, sizeof(tm)); 


    //��UTCʱ����ϱ���ʱ��Сʱ��   
    lc_time.hour += iTimeZone;
   
    //���ݼ���ʱ�����ʱ��ˢ�±���ʱ��   
    //����Сʱ���ж������Ƿ���Ҫ��λ   
    iHourInterval = lc_time.hour/24;
    
    lc_time.hour %= 24;   
    if(iHourInterval>0)   
    {   
        //Сʱ���Ѿ�����24��������Ҫ��1   
        lc_time.mday += 1;   
   
        //Сʱ����1������ж������Ƿ���Ҫ��1   
        //Ҫ���ݵ�ǰ�·��ж�   
        switch(lc_time.month)   
        {   
        case 1:   
        case 3:   
        case 5:   
        case 7:   
        case 8:   
        case 10:   
        case 12:   
            {   
                iDays = 31;   
            }   
            break;   
        case 2:   
            {   
                //2�·ݱȽ����⣬��Ҫ�����ǲ����������жϵ�����28�컹29��   
                iDays = IsLeapYear(utc_time->year)?29:28;   
            }   
            break;   
        case 4:   
        case 6:   
        case 9:   
        case 11:   
            {   
                iDays = 30;   
            }   
            break;   
        }   
   
        iInterval = lc_time.mday-iDays;
        if(iInterval>0)   
        {   
            lc_time.mday = iInterval;   
            lc_time.month += 1;   
   
            //�ж��·��Ƿ����12���������12���������Ҫ�ټ�1   
            iInterval = lc_time.month/12;   
            lc_time.month %= 12;   
            if(iInterval>0)   
            {   
                lc_time.year += 1;   
            }   
        }   
    }
    
    memcpy(l_time, &lc_time, sizeof(tm)); 
   
    //printf("��ǰ UTC ����ʱ�䣺 %04d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);   
    //printf("��ǰ ��������ʱ�䣺 %04d-%02d-%02d %02d:%02d:%02d\n",lc_time.wYear,lc_time.wMonth,lc_time.wDay,lc_time.wHour,lc_time.wMinute,lc_time.wSecond);   
}   

//�жϸ������Ƿ�����
/*  bool IsLeapYear(int iYear)
    {
	    if (iYear % 100 == 0)
		    return ((iYear % 400 == 0));
	    else
		    return ((iYear % 4 == 0));
    }
*/   
char IsLeapYear(unsigned int uiYear)   
{   
    return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);   
}


//�жϸ��������Ƿ�Ϸ�
unsigned char mday_cnt[13]={0xff,31,28,31,30,31,30,31,31,30,31,30,31};

char IsValidDate(tm *t)
{
  char rslt = 0;
  
	if( t->year>9999 ) 
	  return 0;

	switch (t->month)
	{
  	case 1: case 3: case 5: case 7: case 8: case 10: case 12:
  	  if( t->mday<=31 )  rslt = 1;
  		break;
  	case 4: case 6: case 9: case 11:
  	  if( t->mday<=30 )  rslt = 1;
  		break;
  	case 2:
  	  if( t->mday <= (IsLeapYear(t->year) ? 29 : 28) )
  	    rslt = 1;
  		break;
  	default:
  		return 0;
	}
	return rslt;
}

__inline void swap_int(unsigned int *a, unsigned int *b)
{
  *a ^= *b;
  *b ^= *a;
  *a ^= *b;
}

//��ȡ2������֮����������

int CalDays(tm *tm1, tm *tm2)
{
  int k;
  int iDays = 0;
  int i;
  tm T1,T2;
  tm *t1=&T1,*t2=&T2;
  
  //
  memcpy( &T1,tm1,sizeof(tm) );
  memcpy( &T2,tm2,sizeof(tm) );
	//1. ȷ�� ����1 < ����2

	if ((t1->year * 10000 + t1->month * 100 + t1->mday)>(t2->year * 10000 + t2->month * 100 + t2->mday))
	{
	  k = -1;
	  swap_int(&t1->year,&t2->year);
	  swap_int(&t1->month,&t2->month);
	  swap_int(&t1->mday,&t2->mday);
	}else
	  k = 1;
	
	//2. ��ʼ��������
	//���� �� iYear1��1��1�� �� iYear2��1��1��ǰһ�� ֮�������
	for (i = t1->year; i < t2->year; i++)
	{
		iDays += (IsLeapYear(i) ? 366 : 365);
	}
	
	//��ȥiYear1��ǰiMonth1�µ�����
	for (i = 1; i < t1->month; i++)
	{
		if( (i==2)&&IsLeapYear(t2->year) )
		  iDays -= 29;
		else
		  iDays -= mday_cnt[i];
	}
	//��ȥiYear1��iMonth1��ǰiDay1������
	iDays -= (t1->mday - 1);
	
	//����iYear2��ǰiMonth2�µ�����
	for (i = 1; i < t2->month; i++)
	{
		if( (i==2)&&IsLeapYear(t2->year) )
		  iDays += 29;
		else
		  iDays += mday_cnt[i];
	}
	//����iYear2��iMonth2��ǰiDay2������
	iDays += (t2->mday - 1);
	
	return iDays*k;
}


//������������
long CalSeconds(tm *t1, tm *t2)
{
  if( (t1->year!=t2->year)||(t1->month!=t2->month)||(t1->mday!=t2->mday) ){
    return (CalDays(t1,t2)*24*60*60);
  }else
    return (t2->tick - t1->tick);
}


