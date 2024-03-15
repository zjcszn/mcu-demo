void UTCTime2LocalTime(void);   
bool IsLeapYear(unsigned int uiYear);   
   
void UTCTime2LocalTime(tm *local_time, tm *utc_time, int iTimeZone)
{   
    tm *st;       //UTCʱ��
    tm *st_l      //LOCALʱ��
    int iHourInterval,iInterval,iDays=0;
    
    memset(&st,0,sizeof(tm));
    memset(&st_l,0,sizeof(tm));   
   
    memcpy(&st, utc_time, sizeof(tm)); 
    memcpy(&st_l, utc_time, sizeof(tm)); 


    //��UTCʱ����ϱ���ʱ��Сʱ��   
    st_l.hour += iTimeZone;
   
    //���ݼ���ʱ�����ʱ��ˢ�±���ʱ��   
    //����Сʱ���ж������Ƿ���Ҫ��λ   
    iHourInterval = st_l.hour/24;
    
    st_l.hour %= 24;   
    if(iHourInterval>0)   
    {   
        //Сʱ���Ѿ�����24��������Ҫ��1   
        st_l.mday += 1;   
   
        //Сʱ����1������ж������Ƿ���Ҫ��1   
        //Ҫ���ݵ�ǰ�·��ж�   
        switch(st_l.month)   
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
                iDays = IsLeapYear(st.year)?29:28;   
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
   
        iInterval = st_l.mday-iDays);
        if(iInterval>0)   
        {   
            st_l.mday = iInterval;   
            st_l.month += 1;   
   
            //�ж��·��Ƿ����12���������12���������Ҫ�ټ�1   
            iInterval = st_l.month/12;   
            st_l.wMonth %= 12;   
            if(iInterval>0)   
            {   
                st_l.year += 1;   
            }   
        }   
    } 
#if 0   
    printf("��ǰ UTC ����ʱ�䣺 %04d-%02d-%02d %02d:%02d:%02d\n",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);   
    printf("��ǰ ��������ʱ�䣺 %04d-%02d-%02d %02d:%02d:%02d\n",st_l.wYear,st_l.wMonth,st_l.wDay,st_l.wHour,st_l.wMinute,st_l.wSecond);   
#endif
}   
   
bool IsLeapYear(unsigned int uiYear)   
{   
    return (((uiYear%4)==0)&&((uiYear%100)!=0))||((uiYear%400)==0);   
}