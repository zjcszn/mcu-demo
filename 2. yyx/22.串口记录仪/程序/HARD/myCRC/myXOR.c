#include "myXOR.h"

fileRXStr filerx = {0};

// ���У��
u8 XOR(u8* data, u32 len)
{
	u8 xord = 0;
	for (int i = 0; i < len; i++)
	{
		xord ^= data[i];
	}
	return xord;
}

// ��������
enumError_XOR ResolveRxData(fileRXStr* filerx,u8* rxbuf,u32 len)
{
	enumError_XOR erro = XOR_OK;
	if(len > (BYTE2SEND + 2)) return ERRO_LEN;
	if(rxbuf[len-1] != XOR(rxbuf,len-1)) return ERROR_XOR;
	u8 datlen = rxbuf[0] & 0x3f;
	if(datlen != (len -2)) return ERRO_LEN;
	if(rxbuf[0] & ENDFILE )	// �����ź�
	{
		if(rxbuf[0] & CONNECT)	// ����
		{
		
		}
		else					// ����
		{
			// ��ȡ����
			filerx->counter ++;
			filerx->rxlen = datlen;
			for(u8 i =0;i < filerx->rxlen;i++) filerx->filebuf[i] = rxbuf[1+i];
		}
	}
	
	return erro;
}

// �ظ���λ��
void RespectUp(fileRXStr* filerx,u8 isok)
{
	u8 buf[4]= {0};
	buf[0] = 2|ENDFILE;
	buf[1] = filerx->counter;
	buf[2] = isok;
	buf[3] = XOR(buf,3);
	MYXOR_SEND(buf,4);
}

// ����ʱ�� 
extern RTC_DateTypeDef sDate ;
extern RTC_TimeTypeDef sTime ;
extern RTC_HandleTypeDef hrtc;
void SynchronizationTime(u8* timebuf,u8 buflen)
{
	u8 i = 0;
	u8 j = 0;
	u8 buf[2] = {0};
	sDate.Year = (timebuf[2] - '0')*10 + (timebuf[3] - '0');
	while(timebuf[i]!='/') i++;
	i++;
	while(timebuf[i]!='/')
	{
		buf[j] = timebuf[i];
		i++;
		j++;		
	}		
	i++;
	if(j>1) sDate.Month = (buf[0] - '0')*10 + (buf[1] - '0');
	else sDate.Month = buf[0] - '0';
	j=0;
	while(timebuf[i]!=' ')
	{
		buf[j] = timebuf[i];
		i++;
		j++;		
	}
	i++;
	if(j>1) sDate.Date = (buf[0] - '0')*10 + (buf[1] - '0');
	else sDate.Date = buf[0] - '0';
	j=0;
	while(timebuf[i]!=':')
	{
		buf[j] = timebuf[i];
		i++;
		j++;		
	}
	i++;
	if(j>1) sTime.Hours = (buf[0] - '0')*10 + (buf[1] - '0');
	else sTime.Hours = buf[0] - '0';
	j=0;
	while(timebuf[i]!=':')
	{
		buf[j] = timebuf[i];
		i++;
		j++;		
	}
	i++;
	if(j>1) sTime.Minutes = (buf[0] - '0')*10 + (buf[1] - '0');
	else sTime.Minutes = buf[0] - '0';
	j=0;
	while(i < buflen)
	{
		buf[j] = timebuf[i];
		i++;
		j++;		
	}
	i++;
	if(j>1) sTime.Seconds = (buf[0] - '0')*10 + (buf[1] - '0');
	else sTime.Seconds = buf[0] - '0';
	j=0;
	
	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}









