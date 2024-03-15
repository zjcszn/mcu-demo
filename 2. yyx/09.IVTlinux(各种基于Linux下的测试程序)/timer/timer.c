#include "timer.h"

#define LOG_SYSTIMER 					printf
#define MAX_FUNC_ARG_LEN 				1024

/* ��ʱ�����ݽṹ */
typedef struct _tagSysTimerList
{
	BOOL 				bIsUsed;                        /*��ʱ�����ݽṹ�Ƿ��ѱ�ʹ�ã�false��ʾ��δʹ��*/     
	BOOL 				bLoop;                          /*��ʱ���Ƿ�һֱѭ��,false��ʾ��ѭ��,ִ�д�������*/
	int  				nInterval;                     	/*��ʱ���Ķ�ʱʱ�䣬��λ���� */  
	int  				nTimediff;						/*��10MS ����ʱ��Ƭ��ƫ�ƴ���*/
	SYS_TIMER_HANDLER   pFunction;          			/*��ʱ��������ָ�� */
	char    			lpParameter[MAX_FUNC_ARG_LEN];  /*��ʱ������������ */
	int    				nParamLen;                      /*��ʱ�������������ĳ��� */
	int    				nLoopTimes;                     /*ѭ���Ĵ��� 0:ѭ��ֹͣ��n > 0:ѭ��n��.... */
	BOOL     			bEnable;						/*��ʱ����ʹ��״̬,�ܷ���ûص� */
	int    				nIndexId;                       /*����ʹ�õĶ�ʱ���Ķ�ʱ�������±� */ 
} tSysTimer;

/* ȫ�ֱ��� */
typedef struct _tagSysTimer
{
	BOOL   bTimerInit;						/*��ʼ�����б�־ */    
	int    nSysClockTime;              		/*��ʱ������С��ʱʱ�䣬��λ����*/
	int    nTimers;                    		/*��ʱ��ģ��ʹ�õĶ�ʱ���� */
	tSysTimer tTimer[MAX_SYS_TIMERS];   	/*��ʱ�����ݽṹ���� */
	int    nFirstTimer;              		/*��һ��ʹ�õĶ�ʱ�� */
	int    nLastTimer;               		/*���һ��ʹ�õĶ�ʱ��*/
	CRITICAL trCritical;                 	/*����������*/
} sysTimer_t;

static sysTimer_t  gSystimer;

/** \fn BOOL sysTimerRelease(INT32 nIndex)
 *  \brief  �Ӷ�ʱ���������õĶ�ʱ�������С�±�
 */
void setSysTimerMinMaxIndex()
{
	int index = 0 , nMin = -1 , nMax = 0;

	if(gSystimer.nTimers > 0)
	{
		for (index = 0;index < MAX_SYS_TIMERS;index++)
		{
			if(gSystimer.tTimer[index].bIsUsed)
			{
				if(nMin < 0)/*δ����ֵ*/
				{
					nMin = index;
				}
				nMin = min(nMin ,index);
				nMax = max(nMax,index);
			}
		}	
	}
	else
	{
		nMin = 0;
		nMax = 0;
	}

	gSystimer.nFirstTimer = nMin;
	gSystimer.nLastTimer  = nMax;
}

/** \fn int systimerAdd(tSysTimer *pSysTimer)
 *  \brief  ��ʱ���б����һ����ʱ����
 *  \param  pSysTimer    ��ӵĶ�ʱ���ṹָ�롣
 *  \return �ɹ�������ӵĶ�ʱ�����±꣬ʧ�ܷ���-1��
 */
int systimerAdd(tSysTimer *pSysTimer)
{
	int i = 0;
	
	for (i = 0; i < MAX_SYS_TIMERS; i++)
	{
		/* �б�ʱ���������Ǹ���ʱ����δʹ�� */
		if (!gSystimer.tTimer[i].bIsUsed)
		{
			memcpy(&gSystimer.tTimer[i], pSysTimer, sizeof(tSysTimer));	
			gSystimer.nTimers++;
			setSysTimerMinMaxIndex();
			return i;
		}
	}
	return -1;
}

/** \fn int trSysTimerAlloc(SYS_TIMER_HANDLER fn,
 *                               VOID* context,
 *                               INT32 nTimeOut)
 *  \brief  ��ʼ����ʱ����
 *  \param  fn              ��ʱ�Ļص�������
 *  \param  context         �ص������Ĳ�����
 *  \param  nParamLen       �������� 
 *  \param  nTimeOut        ��ʱ��ʱ��(��λ����)��
 *  \return �ɹ����ض�ʱ���ı�ʶ��ʧ�ܷ���-1��
 */
int trSysTimerAlloc(SYS_TIMER_HANDLER fn,void* context,int nParamLen,int nTimeOut)
{
	int   nRet = 0;
	tSysTimer LSysTimer;

	nRet = MAX_SYS_TIMERS;
	
	if (!gSystimer.bTimerInit)
	{
		LOG_SYSTIMER("timer is not init, system can not alloc a timer!\n");
		return -1;
	}
	
	if (MIN_SYS_CLOCK_TIME >= nTimeOut)
	{
		LOG_SYSTIMER(" system can't achieve timer <10ms!\n");
		return -1;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	
	if (gSystimer.nTimers == MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("timer is reach MAX_SYS_TIMERS :%d, can not add more!\n", nRet);
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return -1;
	}

	if (0 == gSystimer.nTimers)
	{
		gSystimer.nFirstTimer = 0;
		gSystimer.nLastTimer = 0;
	}

	/* Ҫ���õĶ�ʱ�����ݽṹ��ʼ�� */
	memset(&LSysTimer, 0, sizeof(tSysTimer));
	LSysTimer.bIsUsed = true;
	LSysTimer.nInterval = nTimeOut;
	LSysTimer.nTimediff = (nTimeOut/MIN_SYS_CLOCK_TIME);
	LSysTimer.pFunction = fn;
	if(nParamLen > 0)
	{
		memcpy(LSysTimer.lpParameter,context, nParamLen);
	}	
	LSysTimer.nParamLen = nParamLen;

	nRet = systimerAdd(&LSysTimer);		
	if (-1 == nRet)
	{
		LOG_SYSTIMER("timer is full!\n");
	}	
	CRITICAL_UNLOCK(&(gSystimer.trCritical));

	return nRet;
}

/** \fn BOOL trSysTimerStart(int nTimer, BOOL bLoop, int nLoopTimes)
 *  \brief  ������ʱ����
 *  \param  nTimer      �����Ķ�ʱ����ʶ��
 *  \param  bLoop       ��ʱ���Ƿ�ѭ����
 *  \param  nLoopTimes  ѭ��������0:ֹͣ��n > 0:ѭ��n�Ρ�
 *  \return �ɹ�����TRUE��ʧ�ܷ���FALSE��
 */
BOOL trSysTimerStart(int nTimer, BOOL bLoop, int nLoopTimes)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("trSysTimerStart timer[%d] is not existed!\n", nTimer);
		return false;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if (!gSystimer.tTimer[nTimer].bIsUsed) 
	{
		LOG_SYSTIMER("timer is not init, can not start!\n");
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	
	/* ����ѭ����־λ */
	if (bLoop)
	{
		gSystimer.tTimer[nTimer].bLoop = true;
	}
	else
	{
		gSystimer.tTimer[nTimer].bLoop = false;
		/* ����ѭ������ */     
		if(nLoopTimes > 0)
		{
		   	gSystimer.tTimer[nTimer].nLoopTimes = nLoopTimes; 
		}
		else
		{
			LOG_SYSTIMER("Loop timer is negative!\n");
		}
	}
	/* ����������־λ */	
	gSystimer.tTimer[nTimer].bEnable = true;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}


/** \fn BOOL trSysTimerStart(int nTimer, BOOL bLoop, INT32 nLoopTimes)
 *  \brief  ��ͣ��ʱ����
 *  \param  nTimer      �����Ķ�ʱ����ʶ��
 *  \return �ɹ�����TRUE��ʧ�ܷ���FALSE��
 */
BOOL trSysTimerCancel(int nTimer)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("trSysTimerCancel timer[%d] is not existed!\n", nTimer);
		return false;
	}
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if (!gSystimer.tTimer[nTimer].bIsUsed) 
	{
		LOG_SYSTIMER("trSysTimerCancel timer is not init, can not cancal!\n");
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	
	/* ����������ͣ��־λ */	
	gSystimer.tTimer[nTimer].bEnable = false;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}


/** \fn BOOL trSysTimerReset(int nTimer, int nTimeOut)
 *  \brief  ���¿�ʼ��ʱ���ļ�ʱ��
 *  \param  nTimer      ��Ҫ����Ķ�ʱ����ʶ��
 *  \return �ɹ�����TRUE��ʧ�ܷ���FALSE��
 */
BOOL trSysTimerReset(int nTimer, int nTimeOut)
{
	if (nTimer < 0 || nTimer >= MAX_SYS_TIMERS)
	{
		LOG_SYSTIMER("timer[%d] is not existed trSysTimerReset!\n", nTimer);
		return false;
	}
	/* ���ö�ʱ����������ֵΪ��ʼֵ */
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if(nTimeOut>MIN_SYS_CLOCK_TIME)
	{	
		gSystimer.tTimer[nTimer].nInterval = nTimeOut;
		gSystimer.tTimer[nTimer].nTimediff = nTimeOut%MIN_SYS_CLOCK_TIME;
	}
	CRITICAL_UNLOCK(&(gSystimer.trCritical));
	return true;
}

/** \fn BOOL sysTimerRelease(INT32 nIndex)
 *  \brief  �Ӷ�ʱ������ɾ��һ����ʱ����
 *  \param  nIndex      ��ɾ���Ķ�ʱ�����±ꡣ
 *  \return �ɹ�����TRUE��ʧ�ܷ���FALSE��
 */
BOOL sysTimerRelease(int nIndex)
{
	if(gSystimer.nTimers<=0)
	{
		return false;
	}
	/* �б�Ҫɾ���Ķ�ʱ���Ƿ���� */
	if (!gSystimer.tTimer[nIndex].bIsUsed)
	{
		return false;
	}

	gSystimer.tTimer[nIndex].bIsUsed = false;
	gSystimer.nTimers--;

	/*��Ҫ�������õ�һ��ʹ�õĶ�ʱ�������һ��ʹ�õĶ�ʱ��*/
	setSysTimerMinMaxIndex();
	return true;
}

/** \fn BOOL trSysTimerFree(int nTimer)
 *  \brief  �ͷŶ�ʱ����
 *  \param  nTimer      �ͷŵĶ�ʱ����ʶ��
 *  \return �ɹ�����TRUE��ʧ�ܷ���FALSE��
 */
BOOL trSysTimerFree(int *nTimer)
{
	BOOL    bRet;
	
	CRITICAL_LOCK(&(gSystimer.trCritical));
	if(*nTimer < 0 || *nTimer >= MAX_SYS_TIMERS)
	{
		CRITICAL_UNLOCK(&(gSystimer.trCritical));
		return false;
	}
	/* �ͷ�һ����ʱ�� */
	bRet = sysTimerRelease(*nTimer);
	*nTimer = -1;
	CRITICAL_UNLOCK(&(gSystimer.trCritical));	
	return bRet;
}

void printSysTimer()
{
	int    i = 0,j = 0;
	
	LOG_SYSTIMER("SystimerLib have %d timers.\n", gSystimer.nTimers);
	
	for (i = 0; i < MAX_SYS_TIMERS; ++i)
	{
		if (gSystimer.tTimer[i].bIsUsed)
		{
			LOG_SYSTIMER("Timer[%d] index = %d nInterval=%d nIndexId=%d\n",j,i,gSystimer.tTimer[i].nInterval,gSystimer.tTimer[i].nIndexId);
			j++;
		}
	}
}

/*��ʱ�������߳�*/
void* trSysTimerThread(void* param)
{
	struct timeval tv,tv_old,tv_now;
	int nIndex=0,nTimers=0,expires=0,time_diff=0,nSec=0,nUsec=0;
	
	nSec = gSystimer.nSysClockTime/1000;
	nUsec = 1000*(gSystimer.nSysClockTime%1000);
	gettimeofday(&tv_old, 0);
	gettimeofday(&tv_now, 0);
	
	while(1)
	{
		if(gSystimer.bTimerInit)
		{
			time_diff = 0;
			nTimers = gSystimer.nTimers;
			if (nTimers > 0)
			{
				/*����ִ�еľ���*/
				gettimeofday(&tv_now, 0);
				expires = ((tv_now.tv_sec-tv_old.tv_sec)*1000000+(tv_now.tv_usec-tv_old.tv_usec));
				/*����ִ�е�ʱ��*/
				if(expires > gSystimer.nSysClockTime*3*1000)
				{
					time_diff = expires/(gSystimer.nSysClockTime);
				}
				CRITICAL_LOCK(&(gSystimer.trCritical));
				for(nIndex=gSystimer.nFirstTimer;nIndex<=gSystimer.nLastTimer;nIndex++)/*��ʱ���б�*/
				{
					if((gSystimer.tTimer[nIndex].bEnable)&&(gSystimer.tTimer[nIndex].bIsUsed))/*��Ч�Ķ�ʱ��*/
					{
						if(time_diff>0)/*����ִ��ʱ�������ԭ�����ʱ����̫��*/
						{
							gSystimer.tTimer[nIndex].nTimediff -= time_diff;
						}
						else
						{
							gSystimer.tTimer[nIndex].nTimediff--;
						}
						if(0>=gSystimer.tTimer[nIndex].nTimediff)/*��ʱ��ʱ*/
						{
							gSystimer.tTimer[nIndex].nTimediff = gSystimer.tTimer[nIndex].nInterval/(gSystimer.nSysClockTime);	
							if(gSystimer.tTimer[nIndex].bLoop)/*һֱѭ��*/
							{
								gSystimer.tTimer[nIndex].pFunction(gSystimer.tTimer[nIndex].lpParameter,gSystimer.tTimer[nIndex].nParamLen);
							}
							else if(gSystimer.tTimer[nIndex].nLoopTimes > 0)/*ѭ������*/
							{
								if(true==gSystimer.tTimer[nIndex].pFunction(gSystimer.tTimer[nIndex].lpParameter,gSystimer.tTimer[nIndex].nParamLen))
								{
									gSystimer.tTimer[nIndex].nLoopTimes--;
								}
							}
							else
							{
								sysTimerRelease(nIndex);/*�Ӷ�ʱ���б���ɾ��*/
							}
						}
					}
					
				}
				CRITICAL_UNLOCK(&(gSystimer.trCritical));
				tv_old.tv_sec = tv_now.tv_sec;
				tv_old.tv_usec = tv_now.tv_usec;
				
			}	
		}
		
		tv.tv_sec = nSec;
		tv.tv_usec = nUsec;
		if(-1 == select(0, NULL, NULL, NULL, &tv))
		{
			perror("sysclockmain select");
			continue;		
		}
		
	}
	pthread_exit(NULL);
	
	return NULL;
}

int sysTimerInit(void)
{
	int ret;
	pthread_t pthrSysTimer;

	if (gSystimer.bTimerInit)
	{
		return false;
	}
	gSystimer.nSysClockTime = MIN_SYS_CLOCK_TIME;
	/* ������ʱ��ģ����Ҫ�Ļ�����   */
	CRITICAL_OPEN(&(gSystimer.trCritical));	
	/* create the systimer thread */
	ret = pthread_create(&pthrSysTimer, NULL, trSysTimerThread, NULL);
	if (ret)
	{
		printf("ERROR; return code from pthread_create() is:%d\n", ret);
		exit(-1);
	}
	gSystimer.bTimerInit = true;
	return 0;
}

#if 0

#define TIMER_TEST
#ifdef TIMER_TEST
unsigned int getTickCount()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec/1000000);
}

BOOL SLSysTimerLog1(void * lpParam,int nParamLen);
BOOL SLSysTimerLog2(void *lpParam,int nParamLen);
BOOL SLSysTimerLog3(void *lpParam,int nParamLen);

int systimerindex1;
int systimerindex2;
int systimerindex3;

BOOL SLSysTimerLog1(void * lpParam,int nParamLen)
{
	static unsigned int now;
	LOG_SYSTIMER("SLSysTimerLog1 %u\n",getTickCount()-now);
	now = getTickCount();
	return true;
}

BOOL SLSysTimerLog2(void *lpParam,int nParamLen)
{
	static unsigned int now;
	char buf[1024] = {0};

	memcpy(buf,lpParam,nParamLen);
	LOG_SYSTIMER("SLSysTimerLog2 %u nParamLen(%d) param:%s\n",getTickCount()-now,nParamLen,buf);
	now = getTickCount();
	return true;
}

BOOL SLSysTimerLog3(void *lpParam,int nParamLen)
{
	static int times;
	static unsigned int now;

	times++; 
	LOG_SYSTIMER("times %d SLSysTimerLog3 %u\n",times,getTickCount()-now);
	now = getTickCount();
	usleep(3000);
	return true;
}

void setsystimer1(void)
{
	systimerindex1 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog1,0,0,500);
	trSysTimerStart(systimerindex1, 1, 0);
}

void setsystimer2(void)
{
	char string[] = "setsystimer2testmytimer";
	
	systimerindex2 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog2,(void*)string ,sizeof(string), 600);
	trSysTimerStart(systimerindex2, 0, 10);
}

void setsystimer3(void)
{
	systimerindex3 = trSysTimerAlloc((SYS_TIMER_HANDLER)SLSysTimerLog3, 0, 0, 900);
	trSysTimerStart(systimerindex3, 0, 100);
}

int main()
{
	struct timeval tv;
	sysTimerInit();
	setsystimer1();
	setsystimer2();
	setsystimer3();
	while(1)
	{
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerReset(systimerindex1,1000);
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerCancel(systimerindex1);
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerStart(systimerindex1,1, 0);
		tv.tv_sec = 60;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);
		trSysTimerFree(&systimerindex1);
	}
	return 0;	
}

#endif
#endif
