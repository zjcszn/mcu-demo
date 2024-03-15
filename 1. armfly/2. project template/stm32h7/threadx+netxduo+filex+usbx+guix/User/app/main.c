/*
*********************************************************************************************************
*	                                  
*	ģ������ : ThreadX
*	�ļ����� : mian.c
*	��    �� : V2.0
*	˵    �� : ThreadXȫ��Ͱ��������ۺ�ģ�塣
*              ʵ��Ŀ�ģ�
*                1. ������ҪѧϰThreadXȫ��Ͱ��������ۺ�ģ�塣            
*              ʵ�����ݣ�
*                1. �����������¼�������ͨ�����°���K1����ͨ�����ڴ�ӡ�����ջʹ�����
*                    ===============================================================
*                    OS CPU Usage =  1.31%
*                    ===============================================================
*                     �������ȼ� ����ջ��С ��ǰʹ��ջ  ���ջʹ��   ������
*                       Prio     StackSize   CurStack    MaxStack   Taskname
*                        2         4092        303         459      App Task Start
*                       30         1020        303         303      App Task STAT
*                       31         1020         87          71      App Task IDLE
*                        5         4092        311         551      App Msp Pro
*                        7         4092        303         719      App Task UserIF
*                        8         4092        183         575      App Task GUI
*                        6         4092        255         359      App NETX Pro
*                       16         4092        259         851      GUIX System Thread
*                        3         4092        415         535      NetX IP Instance 0
*                        4         1020        391         463      ux_slave_storage_thread
*                        0         1020        191         235      System Timer Thread         
*                    �����������ʹ��SecureCRT��V7���������д�������鿴��ӡ��Ϣ��
*                    App Task Start����  ������������������BSP����������
*                    App Task MspPro���� ����Ϣ������������FileX�ļ�ϵͳ����
*                    App Task UserIF���� ��������Ϣ����
*                    App Task GUI����    ��GUIӦ������
*                    App Task STAT����   ��ͳ������
*                    App Task IDLE����   ����������
*                    App NETX Pro����    ��NetX����Ӧ�ô�������
*                    GUIX System Thread  ��GUIϵͳ����
*                    System Timer Thread����ϵͳ��ʱ������
*                    ux_slave_storage_thread����ģ��U������
*                    NetX IP Instance 0 �������紦������
*                2. (1) �����õ�printf������ȫ��ͨ������App_Printfʵ�֡�
*                   (2) App_Printf���������ź����Ļ�������������Դ�������⡣
*              ����˵����
*                1. ���ڳ���ʹ����DWTʱ�����ڼ��������������غ��뽫���������ϵ�ʹ�ã���ֹ
*                   DWTʱ�����ڼ�����û��������λ��
*                2. SD����FileX�ļ�ϵͳ��SD��ģ��U�̲����������������������������ַ����ɣ�	
*                   ��ѡ����������SD��ģ��U�̲����ڼ䲻֧���ٵ�������1-6:
*                   1 - ��ʾ��Ŀ¼�µ��ļ��б�
*                   2 - ����һ�����ļ�armfly.txt
*                   3 - ��armfly.txt�ļ�������
*                   4 - ����Ŀ¼
*                   5 - ɾ���ļ���Ŀ¼
*                   6 - ��д�ļ��ٶȲ���
*                   a - ��SD��ģ��U��
*                   b - �ر�SD��ģ��U��
*               3. NetX����Э��ջ������
*                  (1) Ĭ��IP��ַ192.168.28.245����demo_dm9162_netx.c��ͷ���壬�û��ɸ�����Ҫ�޸ġ�
*                  (2) �����ڵ��Զ�����������������TCP Client���Ӵ˷������ˣ��˿ں�1001��
*                  (3) ʵ����һ���򵥵Ļػ�ͨ�ţ��û�ʹ����λ�����͵�����ͨ�����ӷ��ص���λ����
*              ע�����
*                1. ��ʵ���Ƽ�ʹ�ô������SecureCRT��Ҫ�����ڴ�ӡЧ�������롣�������
*                   V7��������������С�
*                2. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��   ����         ����            ˵��
*       V1.0    2021-01-30   Eric2013    1. ST�̼���1.9.0�汾
*                                        2. BSP������V1.2
*                                        3. ThreadX�汾V6.0
*                                        4. GUIX�汾V6.0
*                                        5. FileX�汾V6.1.3
*                                        6. NetXDUO�汾V6.1.3
*                                        7. USBX�汾V6.1.3
*
*       V2.0    2023-12-20   Eric2013    1. ST�̼���1.11.1�汾
*                                        2. BSP������V1.2
*                                        3. ThreadXȫ��Ͱ6.3.0
*                                       
*	Copyright (C), 2021-2030, ���������� www.armbbs.cn
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
         


/*
*********************************************************************************************************
*                                 �������ȼ�����ֵԽС���ȼ�Խ��
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          2u
#define  APP_CFG_TASK_MsgPro_PRIO                         5u
#define  APP_CFG_TASK_NetXPro_PRIO                        29u 
#define  APP_CFG_TASK_USER_IF_PRIO                        7u
#define  APP_CFG_TASK_GUI_PRIO                            17u


/* ��demo_dm9162_netx.c���壬���߲�����28������3 */
//#define  APP_CFG_TASK_NETX_PRIO                         28u
//#define  APP_CFG_TASK_NETX_PRIO1                         3u

/* ��demo_dm9162_netx.c���壬���߲�����29������6 */
//#define  APP_CFG_TASK_NetXPro_PRIO                      29u 
//#define  APP_CFG_TASK_NetXPro_PRIO1                     6u

/* ��ux_port.h���壬*/
//#define UX_THREAD_PRIORITY_CLASS                        4u

/*
*********************************************************************************************************
*                                    ����ջ��С����λ�ֽ�
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    4096u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   4096u
#define  APP_CFG_TASK_NETXPRO_STK_SIZE                  4096u
#define  APP_CFG_TASK_GUI_STK_SIZE                      4096u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  4096u

/*
*********************************************************************************************************
*                                       ��̬ȫ�ֱ���
*********************************************************************************************************
*/                                                        
static  TX_THREAD   AppTaskStartTCB;
static  uint64_t    AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE/8];

static  TX_THREAD   AppTaskMsgProTCB;
static  uint64_t    AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE/8];

TX_THREAD   AppTaskNetXProTCB;
static  uint64_t    AppTaskNetXProStk[APP_CFG_TASK_NETXPRO_STK_SIZE/8];

static  TX_THREAD   AppTaskGUITCB;
static  uint64_t    AppTaskGUIStk[APP_CFG_TASK_GUI_STK_SIZE/8];

static  TX_THREAD   AppTaskUserIFTCB;
static  uint64_t    AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE/8];


/*
*********************************************************************************************************
*                                      ��������
*********************************************************************************************************
*/
static  void  AppTaskStart          (ULONG thread_input);
static  void  AppTaskMsgPro         (ULONG thread_input);
static  void  AppTaskUserIF         (ULONG thread_input);
static  void  AppTaskGUI			(ULONG thread_input);
static  void  AppTaskNetXPro(ULONG thread_input);
static  void  App_Printf (const char *fmt, ...);
static  void  AppTaskCreate         (void);
static  void  DispTaskInfo          (void);
static  void  AppObjCreate          (void);

/*
*******************************************************************************************************
*                               ����
*******************************************************************************************************
*/
static  TX_MUTEX   AppPrintfSemp;	/* ����printf���� */

__IO double OSCPUUsage;       	   /* CPU�ٷֱ� */

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main(void)
{
 	/* HAL�⣬MPU��Cache��ʱ�ӵ�ϵͳ��ʼ�� */
	System_Init();

	/* �ں˿���ǰ�ر�HAL��ʱ���׼ */
	HAL_SuspendTick();
	
    /* ����ThreadX�ں� */
    tx_kernel_enter();

	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: tx_application_define
*	����˵��: ThreadXר�õ����񴴽���ͨ�������������
*	��    ��: first_unused_memory  δʹ�õĵ�ַ�ռ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void  tx_application_define(void *first_unused_memory)
{
	/*
	   ���ʵ������CPU������ͳ�ƵĻ����˺���������ʵ����������ͳ������Ϳ����������������ں���
	   AppTaskCreate���洴����
	*/
	/**************������������*********************/
    tx_thread_create(&AppTaskStartTCB,              /* ������ƿ��ַ */   
                       "App Task Start",              /* ������ */
                       AppTaskStart,                  /* ������������ַ */
                       0,                             /* ���ݸ�����Ĳ��� */
                       &AppTaskStartStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_START_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_START_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_START_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               /* ������ʱ��Ƭ */
                       TX_AUTO_START);                 /* �������������� */	   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ��������
*	��    ��: thread_input ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (ULONG thread_input)
{
	EXECUTION_TIME TolTime, IdleTime, deltaTolTime, deltaIdleTime;
	uint32_t uiCount = 0;
	(void)thread_input;

	/* �ȹ���ʱ���� */
#ifndef TX_NO_TIMER
	tx_thread_suspend(&_tx_timer_thread);
#endif

	/* �ָ���ʱ���� */
#ifndef TX_NO_TIMER
	tx_thread_resume(&_tx_timer_thread);
#endif	

/* �ں˿����󣬻ָ�HAL���ʱ���׼ */
    HAL_ResumeTick();
	
    /* �����ʼ�� */
    bsp_Init();
	
	/* �������� */
    AppTaskCreate(); 

	/* ���������ͨ�Ż��� */
	AppObjCreate();	

	/* ����CPU������ */
	IdleTime = _tx_execution_idle_time_total;
	TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
    while (1)
	{  
		/* ��Ҫ�����Դ���ĳ��򣬶�Ӧ������̵��õ�SysTick_ISR */
        bsp_ProPer1ms();
		
		/* CPU������ͳ�� */
		uiCount++;
		if(uiCount == 200)
		{
			uiCount = 0;
			deltaIdleTime = _tx_execution_idle_time_total - IdleTime;
			deltaTolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total - TolTime;
			OSCPUUsage = (double)deltaIdleTime/deltaTolTime;
			OSCPUUsage = 100- OSCPUUsage*100;
			IdleTime = _tx_execution_idle_time_total;
			TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
		}
		
        tx_thread_sleep(1);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskMsgPro
*	����˵��: ��Ϣ������������FileX�ļ�ϵͳ
*	��    ��: thread_input ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 5
*********************************************************************************************************
*/
extern void DemoFileX(void);
static void AppTaskMsgPro(ULONG thread_input)
{
	(void)thread_input;

	while(1)
	{
        DemoFileX();
	}   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskNetXPro
*	����˵��: ��Ϣ������������NetX����������
*	��    ��: thread_input ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: �ϵ���29�����߲����������6
*********************************************************************************************************
*/
extern void NetXTest(void);
static void AppTaskNetXPro(ULONG thread_input)
{
    (void)thread_input;
    
	while(1)
	{
        NetXTest();
	}   
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ������Ϣ����
*	��    ��: thread_input ����������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 7
*********************************************************************************************************
*/
static void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* �������� */
	
	(void)thread_input;
    
		  
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				
				case KEY_DOWN_K1:			  /* K1������ӡ����ִ����� */
					case KEY_DOWN_K2:
					case KEY_DOWN_K3:
							case JOY_DOWN_OK:
					 DispTaskInfo();
					break;
				
				default:                     /* �����ļ�ֵ������ */
					break;
			}
		}

        tx_thread_sleep(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskGUI
*	����˵��: GUIӦ������
*	��    ��: thread_input ����������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 8
*********************************************************************************************************
*/
static void AppTaskGUI(ULONG thread_input)
{	
    MainTask();
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************����MsgPro����*********************/
    tx_thread_create(&AppTaskMsgProTCB,               /* ������ƿ��ַ */    
                       "App Msp Pro",                 /* ������ */
                       AppTaskMsgPro,                  /* ������������ַ */
                       0,                             /* ���ݸ�����Ĳ��� */
                       &AppTaskMsgProStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_MsgPro_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_MsgPro_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_MsgPro_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               /* ������ʱ��Ƭ */
                       TX_AUTO_START);                /* �������������� */
   

	/**************����USER IF����*********************/
    tx_thread_create(&AppTaskUserIFTCB,               /* ������ƿ��ַ */      
                       "App Task UserIF",              /* ������ */
                       AppTaskUserIF,                  /* ������������ַ */
                       0,                              /* ���ݸ�����Ĳ��� */
                       &AppTaskUserIFStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_USER_IF_STK_SIZE,  /* ��ջ�ռ��С */  
                       APP_CFG_TASK_USER_IF_PRIO,      /* �������ȼ�*/
                       APP_CFG_TASK_USER_IF_PRIO,      /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,               /* ������ʱ��Ƭ */
                       TX_AUTO_START);                 /* �������������� */

	/**************����GUIX����*********************/
    tx_thread_create(&AppTaskGUITCB,               /* ������ƿ��ַ */    
                       "App Task GUI",              /* ������ */
                       AppTaskGUI,                  /* ������������ַ */
                       0,                           /* ���ݸ�����Ĳ��� */
                       &AppTaskGUIStk[0],            /* ��ջ����ַ */
                       APP_CFG_TASK_GUI_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_GUI_PRIO,        /* �������ȼ�*/
                       APP_CFG_TASK_GUI_PRIO,        /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             /* ������ʱ��Ƭ */
                       TX_AUTO_START);               /* �������������� */
                       
	/**************����NetX��������*********************/
    tx_thread_create(&AppTaskNetXProTCB,               /* ������ƿ��ַ */    
                      "App NETX Pro",                   /* ������ */
                       AppTaskNetXPro,                  /* ������������ַ */
                       0,                           /* ���ݸ�����Ĳ��� */
                       &AppTaskNetXProStk[0],           /* ��ջ����ַ */
                       APP_CFG_TASK_NETXPRO_STK_SIZE,    /* ��ջ�ռ��С */  
                       APP_CFG_TASK_NetXPro_PRIO,    /* �������ȼ�*/
                       APP_CFG_TASK_NetXPro_PRIO,    /* ������ռ��ֵ */
                       TX_NO_TIME_SLICE,             /* ������ʱ��Ƭ */
                       TX_AUTO_START);               /* �������������� */
}

/*
*********************************************************************************************************
*	�� �� ��: AppObjCreate
*	����˵��: ��������ͨѶ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	 /* ���������ź��� */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
}

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ		  			  
*	��    ��: ͬprintf�Ĳ�����
*             ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static  void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1]; /* �ر�ע�⣬���printf�ı����϶࣬ע��˾ֲ������Ĵ�С�Ƿ��� */
    va_list   v_args;


    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

	/* ������� */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

    printf("%s", buf_str);

    tx_mutex_put(&AppPrintfSemp);
}
 
/*
*********************************************************************************************************
*	�� �� ��: DispTaskInfo
*	����˵��: ��ThreadX������Ϣͨ�����ڴ�ӡ����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* ����һ��������ƿ�ָ�� */

    p_tcb = &AppTaskStartTCB;
	
	/* ��ӡ���� */
	App_Printf("===============================================================\r\n");
	App_Printf("CPU������ = %5.2f%%\r\n", OSCPUUsage);
	App_Printf("����ִ��ʱ�� = %.9fs\r\n", (double)_tx_execution_thread_time_total/SystemCoreClock);
	App_Printf("����ִ��ʱ�� = %.9fs\r\n", (double)_tx_execution_idle_time_total/SystemCoreClock);
	App_Printf("�ж�ִ��ʱ�� = %.9fs\r\n", (double)_tx_execution_isr_time_total/SystemCoreClock);
	App_Printf("ϵͳ��ִ��ʱ�� = %.9fs\r\n", (double)(_tx_execution_thread_time_total + \
		                                               _tx_execution_idle_time_total +  \
	                                                   _tx_execution_isr_time_total)/SystemCoreClock);	
	App_Printf("===============================================================\r\n");
	App_Printf(" �������ȼ� ����ջ��С ��ǰʹ��ջ  ���ջʹ��   ������\r\n");
	App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* ������������б�TCB list)����ӡ���е���������ȼ������� */
	while (p_tcb != (TX_THREAD *)0) 
	{
		
		App_Printf("   %2d        %5d      %5d       %5d      %s\r\n", 
                    p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr,
                    p_tcb->tx_thread_name);


        p_tcb = p_tcb->tx_thread_created_next;

        if(p_tcb == &AppTaskStartTCB) break;
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
