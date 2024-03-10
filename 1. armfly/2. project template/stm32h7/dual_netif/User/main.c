/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块
*	文件名称 : main.c
*	版    本 : V1.0
*	说    明 : RTX5 + RL-TCPnet的双网口实现。
*              实验目的：
*                1. 学习RTX5 + RL-TCPnet的双网口实现。
*              实验内容：
*                1. 强烈推荐将网线接到路由器或者交换机上面测试，因为已经使能了DHCP，可以自动获取IP地址。
*                   获取的IP地址会通过串口打印出来。
*                2. DM9000和DM9162分别创建了一个TCP Server，DM9000使用端口号1002，而DM9162使用端口号1001。
*                   用户可以在电脑端用网络调试软件创建TCP Client连接此服务器端。
*                3. 按键K1按下，DM9162发送数据给TCP Client。
*                4. 按键K2按下，DM9000发送数据给TCP Client。
*                5. 各个任务实现的功能如下：
*                   AppTaskUserIF任务      : 按键消息处理。
*                   AppTaskLED任务         : LED闪烁。
*                   AppTaskDM9000Pro任务   : DM9000的TCPnet应用任务。
*                   AppTaskDM9162Pro任务   : DM9162的TCPnet应用任务。
*                   AppTaskEthCheck        : 网线插拔状态检测。
*                   AppTaskStart任务       : 启动任务，也是最高优先级任务，这里用作BSP驱动包处理。
*                   netCore_Thread任务     : TCPnet内核任务。
*                   netEth0_Thread任务     : TCPnet以太网接口任务。
*                   osRtxTimerThread任务   : 定时器任务，TCPnet时间基准。
*              注意事项：
*                1. 为了RTX5的调试组件正常使用，RTX5的工程路径切不要有中文，路径不要太长。
*                   而且退出调试状态要清除所有断点。
*                2. 本实验推荐使用串口软件SecureCRT查看打印信息，波特率115200，数据位8，奇偶校验位无，停止位1。
*                3. 务必将编辑器的缩进参数和TAB设置为4来阅读本文件，要不代码显示不整齐。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2020-11-30   Eric2013     1. CMSIS软包版本 V5.7.0
*                                         2. HAL库版本 V1.9.0
*                                         3. RTX5版本5.5.1
*                                         4. Event Recorder版本1.4.0
*                                         5. RL-TCPnet版本V7.X
*
*	Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"



/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_taskdbg printf
#else
	#define printf_taskdbg(...)
#endif

/*
**********************************************************************************************************
											函数声明
**********************************************************************************************************
*/
static void AppTaskCreate (void);
void AppTaskUserIF(void *argument);
void AppTaskLED(void *argument);
void AppTaskDM9162Pro(void *argument);
void AppTaskDM9000Pro(void *argument);
void AppTaskStart(void *argument);


/*
**********************************************************************************************************
											 变量
**********************************************************************************************************
*/
/* 任务的属性设置 */
const osThreadAttr_t ThreadStart_Attr = 
{
	/* 未使用 */
//	.cb_mem = &worker_thread_tcb_1,
//	.cb_size = sizeof(worker_thread_tcb_1),
//	.stack_mem = &worker_thread_stk_1[0],
//	.stack_size = sizeof(worker_thread_stk_1),
//	.priority = osPriorityAboveNormal,
//	.tz_module = 0
	
	.name = "osRtxStartThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal4,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadEthCheck_Attr = 
{
	.name = "osRtxEthCheckThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal3,
	.stack_size = 1024,
};

const osThreadAttr_t ThreadDM9162Pro_Attr = 
{
	.name = "osRtxDM9162ProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal2,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadDM9000Pro_Attr = 
{
	.name = "osRtxDM9000ProThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal2,
	.stack_size = 2048,
};

const osThreadAttr_t ThreadLED_Attr = 
{
	.name = "osRtxLEDThread",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal1,
	.stack_size = 512,
};

const osThreadAttr_t ThreadUserIF_Attr = 
{
	.name = "osRtxThreadUserIF",
	.attr_bits = osThreadDetached, 
	.priority = osPriorityNormal,
	.stack_size = 1024,
};

/* 任务句柄 */
osThreadId_t ThreadIdTaskUserIF = NULL;
osThreadId_t ThreadIdTaskEthCheck = NULL;
osThreadId_t ThreadIdTaskDM9000Pro = NULL;
osThreadId_t ThreadIdTaskDM9162Pro = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;


/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
int main (void) 
{	
	/* HAL库，MPU，Cache，时钟等系统初始化 */
	System_Init();

	/* 内核开启前关闭HAL的时间基准 */
	HAL_SuspendTick();
	
	/* 内核初始化 */
	osKernelInitialize();                                  

	/* 创建启动任务 */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

	/* 开启多任务 */
	osKernelStart();
	
	while(1);
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskUserIF
*	功能说明: 按键消息处理		
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal (数值越小优先级越低，这个跟uCOS相反)
*********************************************************************************************************
*/
void AppTaskUserIF(void *argument)
{
	uint8_t ucKeyCode;

    while(1)
    {
		ucKeyCode = bsp_GetKey();
		
		if (ucKeyCode != KEY_NONE)
		{
			switch (ucKeyCode)
			{
				/* K1键按下，直接发送事件标志给任务AppTaskDM9162Pro，设置bit0 */
				case KEY_DOWN_K1:
					printf("K1键按下，直接发送事件标志给任务AppTaskDM9162Pro，bit0被设置\r\n");
					osThreadFlagsSet(ThreadIdTaskDM9162Pro, KEY1_BIT0);					
					break;	

				/* K2键按下，直接发送事件标志给任务AppTaskDM9000Pro，设置bit1 */
				case KEY_DOWN_K2:
					printf("K2键按下，直接发送事件标志给任务AppTaskDM9000Pro，bit1被设置\r\n");
					osThreadFlagsSet(ThreadIdTaskDM9000Pro, KEY2_BIT1);
					break;

				/* 其他的键值不处理 */
				default:                     
					break;
			}
		}
		
		osDelay(20);
	}
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskLED
*	功能说明: LED闪烁。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal1 
*********************************************************************************************************
*/
void AppTaskLED(void *argument)
{
	const uint16_t usFrequency = 200; /* 延迟周期 */
	uint32_t tick;

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		bsp_LedToggle(2);
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskDM9000Pro
*	功能说明: TCPnet应用任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal2  
*********************************************************************************************************
*/
void AppTaskDM9000Pro(void *argument)
{
	while(1)
	{
		DM9000TCPnetTest();
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskDM9162Pro
*	功能说明: TCPnet应用任务
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal2  
*********************************************************************************************************
*/
void AppTaskDM9162Pro(void *argument)
{
	while(1)
	{
		DM9162TCPnetTest();
	}	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskEthCheck
*	功能说明: 检查网线插拔状态。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal3  
*********************************************************************************************************
*/
void AppTaskEthCheck(void *argument)
{
    uint8_t ip4_dm9000addr[NET_ADDR_IP4_LEN];
    uint8_t ip4_dm9162addr[NET_ADDR_IP4_LEN]; 
    uint32_t *ip4_dm9000 = (uint32_t *)ip4_dm9000addr;
    uint32_t *ip4_dm9162 = (uint32_t *)ip4_dm9162addr; 
    uint8_t ucDM9000Link = 0, ucDM9162Link = 0;             /* 变量标志，方便任务处理*/
    uint8_t ucDM9000LinkStatus = 0, ucDM9162LinkStatus = 0; /* 变量标志，方便任务处理*/

    /* 初始化变量 */
    ThreadIdTaskDM9162Pro = NULL;
    ThreadIdTaskDM9000Pro = NULL;
    g_ucDM9162LinkStatus = 0;
    g_ucDM9000LinkStatus = 0;

	osDelay(1000);
	
    /* 初始化网络 */
    netInitialize();

    while(1)
    {
        /* 网线插拔处理，方便移植，大家也可以根据需要发送任务事件标志做处理 */
        switch (g_ucDM9162LinkStatus)
        {
            /* 插拔临时状态，无需处理 */
            case 0:
            case 1:	
            break;	

            /* 网线插入，创应用任务 */
            case 2:
                if(ucDM9162Link == 0)
                {
                netIF_GetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_dm9162addr, sizeof (ip4_dm9162addr));

                printf_taskdbg("DM9162网线插入，等待获取IP地址....\r\n");
                /* 从DHCP服务器获取了IP地址 */
                if(*ip4_dm9162 != 0)
                {
                    printf("DM9162的IP地址：%d.%d.%d.%d\r\n", ip4_dm9162addr[0],ip4_dm9162addr[1],
                    ip4_dm9162addr[2], ip4_dm9162addr[3]);

                    printf_taskdbg("DM9162创建应用任务\r\n");

                    if(ThreadIdTaskDM9162Pro == NULL)
                    {
                        ThreadIdTaskDM9162Pro = osThreadNew(AppTaskDM9162Pro, NULL, &ThreadDM9162Pro_Attr);
                    }
                    ucDM9162Link = 2;
                    ucDM9162LinkStatus = 2;
                }
                }
                break;	

            /* 网线拔掉，复位网络，删除应用任务 */
            case 3:
                printf_taskdbg("DM9162网线拔掉\r\n");
                g_ucDM9162LinkStatus = 0;
                ucDM9162Link = 0; 
                ucDM9162LinkStatus = 3;            
                break;

            /* 其他的键值不处理 */
            default:                     
                break;
        }
        
        switch (g_ucDM9000LinkStatus)
        {
            /* 插拔临时状态，无需处理 */
            case 0:
            case 1:	
            break;	

            /* 网线插入，创应用任务 */
            case 2:
                if(ucDM9000Link == 0)
                {	
                    netIF_GetOption (NET_IF_CLASS_ETH | 1, netIF_OptionIP4_Address, ip4_dm9000addr, sizeof (ip4_dm9000addr));

                    printf_taskdbg("DM9000网线插入，等待获取IP地址....\r\n");
                    /* 从DHCP服务器获取了IP地址 */
                    if(*ip4_dm9000 != 0)
                    {
                        printf("DM9000的IP地址：%d.%d.%d.%d\r\n", ip4_dm9000addr[0],ip4_dm9000addr[1],
                        ip4_dm9000addr[2], ip4_dm9000addr[3]);


                        printf_taskdbg("DM9000创建应用任务\r\n");
                        if(ThreadIdTaskDM9000Pro == NULL)
                        {
                            ThreadIdTaskDM9000Pro = osThreadNew(AppTaskDM9000Pro, NULL, &ThreadDM9000Pro_Attr);
                        }
                        ucDM9000Link = 2;
                        ucDM9000LinkStatus = 2;
                    }
                }
                break;	

            /* 网线拔掉，复位网络，删除应用任务 */
            case 3:
                printf_taskdbg("DM9000网线拔掉\r\n");
                g_ucDM9000LinkStatus = 0;
                ucDM9000Link = 0;
                ucDM9000LinkStatus = 3;
                break;

            /* 其他的键值不处理 */
            default:                     
                break;
        }

            /* 网线拔掉，复位网络，删除应用任务 */
        if((ucDM9000LinkStatus == 3)&&(ucDM9162LinkStatus == 3))
        {
            printf_taskdbg("网线都被拔掉，复位网络，删除应用任务\r\n");

            /* 释放所有网络资源，含TCPnet内核任务和ETH接口任务 */	
            netUninitialize();

            printf_taskdbg("netUninitialize\r\n");

            /* 删除TCPnet应用任务 */
            if(ThreadIdTaskDM9162Pro != NULL)
            {
                ThreadIdTaskDM9162Pro = NULL;
                osThreadTerminate(ThreadIdTaskDM9162Pro);
            }

            if(ThreadIdTaskDM9000Pro != NULL)
            {
                ThreadIdTaskDM9000Pro = NULL;
                osThreadTerminate(ThreadIdTaskDM9000Pro);
            }

            g_ucDM9162LinkStatus = 0;
            g_ucDM9000LinkStatus = 0;
            ucDM9162LinkStatus = 0;
            ucDM9000LinkStatus = 0;
            ucDM9162Link = 0;
            ucDM9000Link = 0;

            /* 重新初始化 */
            netInitialize();

            printf_taskdbg("netInitialize\r\n");
        }

        osDelay(500);
    }	
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务，这里用作BSP驱动包处理。
*	形    参: 无
*	返 回 值: 无
*   优 先 级: osPriorityNormal4  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 1; /* 延迟周期 */
	uint32_t tick;
	
	/* 初始化外设 */
	HAL_ResumeTick();
	bsp_Init();

	/* 创建任务 */
	AppTaskCreate();

	/* 获取当前时间 */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		/* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
		bsp_ProPer1ms();
		
		/* 相对延迟 */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr);  	
	ThreadIdTaskLED = osThreadNew(AppTaskLED, NULL, &ThreadLED_Attr);  
	ThreadIdTaskUserIF = osThreadNew(AppTaskUserIF, NULL, &ThreadUserIF_Attr);  
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
