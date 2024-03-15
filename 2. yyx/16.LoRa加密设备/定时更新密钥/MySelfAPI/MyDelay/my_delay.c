#include "my_delay.h"


static u16 Counter = 0x00;
u16 ExportLogTimer = 0;		//������־��ʱ��(ʱ����Ϊ1s��Ϊ������λ��Э��)

void TimerDelay_Ms(u16 counter)
{
	Counter = 0x00;
	while( Counter<counter );
}


void RunDelayCounter(void)
{
	if( Counter<DELAY_MAX_VALUE )
		Counter ++;
	
	if(ExportLogTimer< 10000)
		ExportLogTimer++;
}


//��ȡ������־��ʱ����ֵ
u16 GetExportLogTimerValue(void)
{
	return ExportLogTimer;
}


//��յ�����־��ʱ����ֵ
void ClearExportLogTimer(void)
{
	ExportLogTimer = 0;
}





