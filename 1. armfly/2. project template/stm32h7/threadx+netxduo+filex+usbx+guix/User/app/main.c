/*
*********************************************************************************************************
*	                                  
*	模块名称 : ThreadX
*	文件名称 : mian.c
*	版    本 : V2.0
*	说    明 : ThreadX全家桶所有组件综合模板。
*              实验目的：
*                1. 本章主要学习ThreadX全家桶所有组件综合模板。            
*              实验内容：
*                1. 共创建了如下几个任务，通过按下按键K1可以通过串口打印任务堆栈使用情况
*                    ===============================================================
*                    OS CPU Usage =  1.31%
*                    ===============================================================
*                     任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名
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
*                    串口软件建议使用SecureCRT（V7光盘里面有此软件）查看打印信息。
*                    App Task Start任务  ：启动任务，这里用作BSP驱动包处理。
*                    App Task MspPro任务 ：消息处理，这里用作FileX文件系统处理。
*                    App Task UserIF任务 ：按键消息处理。
*                    App Task GUI任务    ：GUI应用任务。
*                    App Task STAT任务   ：统计任务
*                    App Task IDLE任务   ：空闲任务
*                    App NETX Pro任务    ：NetX网络应用处理任务
*                    GUIX System Thread  ：GUI系统任务
*                    System Timer Thread任务：系统定时器任务
*                    ux_slave_storage_thread任务：模拟U盘任务
*                    NetX IP Instance 0 任务：网络处理任务
*                2. (1) 凡是用到printf函数的全部通过函数App_Printf实现。
*                   (2) App_Printf函数做了信号量的互斥操作，解决资源共享问题。
*              操作说明：
*                1. 由于程序使用了DWT时钟周期计数器，程序下载后，请将板子重新上电使用，防止
*                   DWT时钟周期计数器没有正常复位。
*                2. SD卡的FileX文件系统和SD卡模拟U盘操作串口命令命令，仅需输入下面的字符即可：	
*                   请选择操作命令，打开SD卡模拟U盘操作期间不支持再调用命令1-6:
*                   1 - 显示根目录下的文件列表
*                   2 - 创建一个新文件armfly.txt
*                   3 - 读armfly.txt文件的内容
*                   4 - 创建目录
*                   5 - 删除文件和目录
*                   6 - 读写文件速度测试
*                   a - 打开SD卡模拟U盘
*                   b - 关闭SD卡模拟U盘
*               3. NetX网络协议栈操作：
*                  (1) 默认IP地址192.168.28.245，在demo_dm9162_netx.c开头定义，用户可根据需要修改。
*                  (2) 可以在电脑端用网络调试软件创建TCP Client连接此服务器端，端口号1001。
*                  (3) 实现了一个简单的回环通信，用户使用上位机发送的数据通过板子返回到上位机。
*              注意事项：
*                1. 本实验推荐使用串口软件SecureCRT，要不串口打印效果不整齐。此软件在
*                   V7开发板光盘里面有。
*                2. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者            说明
*       V1.0    2021-01-30   Eric2013    1. ST固件库1.9.0版本
*                                        2. BSP驱动包V1.2
*                                        3. ThreadX版本V6.0
*                                        4. GUIX版本V6.0
*                                        5. FileX版本V6.1.3
*                                        6. NetXDUO版本V6.1.3
*                                        7. USBX版本V6.1.3
*
*       V2.0    2023-12-20   Eric2013    1. ST固件库1.11.1版本
*                                        2. BSP驱动包V1.2
*                                        3. ThreadX全家桶6.3.0
*                                       
*	Copyright (C), 2021-2030, 安富莱电子 www.armbbs.cn
*
*********************************************************************************************************
*/
#include "includes.h"
#include "MainTask.h"
         


/*
*********************************************************************************************************
*                                 任务优先级，数值越小优先级越高
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          2u
#define  APP_CFG_TASK_MsgPro_PRIO                         5u
#define  APP_CFG_TASK_NetXPro_PRIO                        29u 
#define  APP_CFG_TASK_USER_IF_PRIO                        7u
#define  APP_CFG_TASK_GUI_PRIO                            17u


/* 在demo_dm9162_netx.c定义，网线插入后从28提升到3 */
//#define  APP_CFG_TASK_NETX_PRIO                         28u
//#define  APP_CFG_TASK_NETX_PRIO1                         3u

/* 在demo_dm9162_netx.c定义，网线插入后从29提升到6 */
//#define  APP_CFG_TASK_NetXPro_PRIO                      29u 
//#define  APP_CFG_TASK_NetXPro_PRIO1                     6u

/* 在ux_port.h定义，*/
//#define UX_THREAD_PRIORITY_CLASS                        4u

/*
*********************************************************************************************************
*                                    任务栈大小，单位字节
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    4096u
#define  APP_CFG_TASK_MsgPro_STK_SIZE                   4096u
#define  APP_CFG_TASK_NETXPRO_STK_SIZE                  4096u
#define  APP_CFG_TASK_GUI_STK_SIZE                      4096u
#define  APP_CFG_TASK_USER_IF_STK_SIZE                  4096u

/*
*********************************************************************************************************
*                                       静态全局变量
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
*                                      函数声明
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
*                               变量
*******************************************************************************************************
*/
static  TX_MUTEX   AppPrintfSemp;	/* 用于printf互斥 */

__IO double OSCPUUsage;       	   /* CPU百分比 */

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main(void)
{
 	/* HAL库，MPU，Cache，时钟等系统初始化 */
	System_Init();

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
    /* 进入ThreadX内核 */
    tx_kernel_enter();

	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无
*********************************************************************************************************
*/
void  tx_application_define(void *first_unused_memory)
{
	/*
	   如果实现任务CPU利用率统计的话，此函数仅用于实现启动任务，统计任务和空闲任务，其它任务在函数
	   AppTaskCreate里面创建。
	*/
	/**************创建启动任务*********************/
    tx_thread_create(&AppTaskStartTCB,              /* 任务控制块地址 */   
                       "App Task Start",              /* 任务名 */
                       AppTaskStart,                  /* 启动任务函数地址 */
                       0,                             /* 传递给任务的参数 */
                       &AppTaskStartStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_START_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_START_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_START_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                 /* 创建后立即启动 */	   
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 2
*********************************************************************************************************
*/
static  void  AppTaskStart (ULONG thread_input)
{
	EXECUTION_TIME TolTime, IdleTime, deltaTolTime, deltaIdleTime;
	uint32_t uiCount = 0;
	(void)thread_input;

	/* 先挂起定时器组 */
#ifndef TX_NO_TIMER
	tx_thread_suspend(&_tx_timer_thread);
#endif

	/* 恢复定时器组 */
#ifndef TX_NO_TIMER
	tx_thread_resume(&_tx_timer_thread);
#endif	

/* 内核开启后，恢复HAL里的时间基准 */
    HAL_ResumeTick();
	
    /* 外设初始化 */
    bsp_Init();
	
	/* 创建任务 */
    AppTaskCreate(); 

	/* 创建任务间通信机制 */
	AppObjCreate();	

	/* 计算CPU利用率 */
	IdleTime = _tx_execution_idle_time_total;
	TolTime = _tx_execution_thread_time_total + _tx_execution_isr_time_total + _tx_execution_idle_time_total;
    while (1)
	{  
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
        bsp_ProPer1ms();
		
		/* CPU利用率统计 */
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
*	函 数 名: AppTaskMsgPro
*	功能说明: 消息处理，这里用作FileX文件系统
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 5
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
*	函 数 名: AppTaskNetXPro
*	功能说明: 消息处理，这里用作NetX网络任务处理
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 上电是29，网线插入后提升至6
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
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 7
*********************************************************************************************************
*/
static void AppTaskUserIF(ULONG thread_input)
{
	uint8_t ucKeyCode;	/* 按键代码 */
	
	(void)thread_input;
    
		  
	while(1)
	{        
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				
				case KEY_DOWN_K1:			  /* K1键按打印任务执行情况 */
					case KEY_DOWN_K2:
					case KEY_DOWN_K3:
							case JOY_DOWN_OK:
					 DispTaskInfo();
					break;
				
				default:                     /* 其他的键值不处理 */
					break;
			}
		}

        tx_thread_sleep(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskGUI
*	功能说明: GUI应用任务
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 8
*********************************************************************************************************
*/
static void AppTaskGUI(ULONG thread_input)
{	
    MainTask();
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppTaskCreate (void)
{
	/**************创建MsgPro任务*********************/
    tx_thread_create(&AppTaskMsgProTCB,               /* 任务控制块地址 */    
                       "App Msp Pro",                 /* 任务名 */
                       AppTaskMsgPro,                  /* 启动任务函数地址 */
                       0,                             /* 传递给任务的参数 */
                       &AppTaskMsgProStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_MsgPro_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_MsgPro_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_MsgPro_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                /* 创建后立即启动 */
   

	/**************创建USER IF任务*********************/
    tx_thread_create(&AppTaskUserIFTCB,               /* 任务控制块地址 */      
                       "App Task UserIF",              /* 任务名 */
                       AppTaskUserIF,                  /* 启动任务函数地址 */
                       0,                              /* 传递给任务的参数 */
                       &AppTaskUserIFStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_USER_IF_STK_SIZE,  /* 堆栈空间大小 */  
                       APP_CFG_TASK_USER_IF_PRIO,      /* 任务优先级*/
                       APP_CFG_TASK_USER_IF_PRIO,      /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,               /* 不开启时间片 */
                       TX_AUTO_START);                 /* 创建后立即启动 */

	/**************创建GUIX任务*********************/
    tx_thread_create(&AppTaskGUITCB,               /* 任务控制块地址 */    
                       "App Task GUI",              /* 任务名 */
                       AppTaskGUI,                  /* 启动任务函数地址 */
                       0,                           /* 传递给任务的参数 */
                       &AppTaskGUIStk[0],            /* 堆栈基地址 */
                       APP_CFG_TASK_GUI_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_GUI_PRIO,        /* 任务优先级*/
                       APP_CFG_TASK_GUI_PRIO,        /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             /* 不开启时间片 */
                       TX_AUTO_START);               /* 创建后立即启动 */
                       
	/**************创建NetX处理任务*********************/
    tx_thread_create(&AppTaskNetXProTCB,               /* 任务控制块地址 */    
                      "App NETX Pro",                   /* 任务名 */
                       AppTaskNetXPro,                  /* 启动任务函数地址 */
                       0,                           /* 传递给任务的参数 */
                       &AppTaskNetXProStk[0],           /* 堆栈基地址 */
                       APP_CFG_TASK_NETXPRO_STK_SIZE,    /* 堆栈空间大小 */  
                       APP_CFG_TASK_NetXPro_PRIO,    /* 任务优先级*/
                       APP_CFG_TASK_NetXPro_PRIO,    /* 任务抢占阀值 */
                       TX_NO_TIME_SLICE,             /* 不开启时间片 */
                       TX_AUTO_START);               /* 创建后立即启动 */
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  AppObjCreate (void)
{
	 /* 创建互斥信号量 */
    tx_mutex_create(&AppPrintfSemp,"AppPrintfSemp",TX_NO_INHERIT);
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式		  			  
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
static  void  App_Printf(const char *fmt, ...)
{
    char  buf_str[200 + 1]; /* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
    va_list   v_args;


    va_start(v_args, fmt);
   (void)vsnprintf((char       *)&buf_str[0],
                   (size_t      ) sizeof(buf_str),
                   (char const *) fmt,
                                  v_args);
    va_end(v_args);

	/* 互斥操作 */
    tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

    printf("%s", buf_str);

    tx_mutex_put(&AppPrintfSemp);
}
 
/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将ThreadX任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
	TX_THREAD      *p_tcb;	        /* 定义一个任务控制块指针 */

    p_tcb = &AppTaskStartTCB;
	
	/* 打印标题 */
	App_Printf("===============================================================\r\n");
	App_Printf("CPU利用率 = %5.2f%%\r\n", OSCPUUsage);
	App_Printf("任务执行时间 = %.9fs\r\n", (double)_tx_execution_thread_time_total/SystemCoreClock);
	App_Printf("空闲执行时间 = %.9fs\r\n", (double)_tx_execution_idle_time_total/SystemCoreClock);
	App_Printf("中断执行时间 = %.9fs\r\n", (double)_tx_execution_isr_time_total/SystemCoreClock);
	App_Printf("系统总执行时间 = %.9fs\r\n", (double)(_tx_execution_thread_time_total + \
		                                               _tx_execution_idle_time_total +  \
	                                                   _tx_execution_isr_time_total)/SystemCoreClock);	
	App_Printf("===============================================================\r\n");
	App_Printf(" 任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名\r\n");
	App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

	/* 遍历任务控制列表TCB list)，打印所有的任务的优先级和名称 */
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

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
