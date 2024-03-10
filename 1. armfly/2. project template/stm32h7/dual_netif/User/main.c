/*
*********************************************************************************************************
*
*	ģ������ : ������ģ��
*	�ļ����� : main.c
*	��    �� : V1.0
*	˵    �� : RTX5 + RL-TCPnet��˫����ʵ�֡�
*              ʵ��Ŀ�ģ�
*                1. ѧϰRTX5 + RL-TCPnet��˫����ʵ�֡�
*              ʵ�����ݣ�
*                1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*                   ��ȡ��IP��ַ��ͨ�����ڴ�ӡ������
*                2. DM9000��DM9162�ֱ𴴽���һ��TCP Server��DM9000ʹ�ö˿ں�1002����DM9162ʹ�ö˿ں�1001��
*                   �û������ڵ��Զ�����������������TCP Client���Ӵ˷������ˡ�
*                3. ����K1���£�DM9162�������ݸ�TCP Client��
*                4. ����K2���£�DM9000�������ݸ�TCP Client��
*                5. ��������ʵ�ֵĹ������£�
*                   AppTaskUserIF����      : ������Ϣ����
*                   AppTaskLED����         : LED��˸��
*                   AppTaskDM9000Pro����   : DM9000��TCPnetӦ������
*                   AppTaskDM9162Pro����   : DM9162��TCPnetӦ������
*                   AppTaskEthCheck        : ���߲��״̬��⡣
*                   AppTaskStart����       : ��������Ҳ��������ȼ�������������BSP����������
*                   netCore_Thread����     : TCPnet�ں�����
*                   netEth0_Thread����     : TCPnet��̫���ӿ�����
*                   osRtxTimerThread����   : ��ʱ������TCPnetʱ���׼��
*              ע�����
*                1. Ϊ��RTX5�ĵ����������ʹ�ã�RTX5�Ĺ���·���в�Ҫ�����ģ�·����Ҫ̫����
*                   �����˳�����״̬Ҫ������жϵ㡣
*                2. ��ʵ���Ƽ�ʹ�ô������SecureCRT�鿴��ӡ��Ϣ��������115200������λ8����żУ��λ�ޣ�ֹͣλ1��
*                3. ��ؽ��༭��������������TAB����Ϊ4���Ķ����ļ���Ҫ��������ʾ�����롣
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2020-11-30   Eric2013     1. CMSIS����汾 V5.7.0
*                                         2. HAL��汾 V1.9.0
*                                         3. RTX5�汾5.5.1
*                                         4. Event Recorder�汾1.4.0
*                                         5. RL-TCPnet�汾V7.X
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_taskdbg printf
#else
	#define printf_taskdbg(...)
#endif

/*
**********************************************************************************************************
											��������
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
											 ����
**********************************************************************************************************
*/
/* ������������� */
const osThreadAttr_t ThreadStart_Attr = 
{
	/* δʹ�� */
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

/* ������ */
osThreadId_t ThreadIdTaskUserIF = NULL;
osThreadId_t ThreadIdTaskEthCheck = NULL;
osThreadId_t ThreadIdTaskDM9000Pro = NULL;
osThreadId_t ThreadIdTaskDM9162Pro = NULL;
osThreadId_t ThreadIdTaskLED = NULL;
osThreadId_t ThreadIdStart = NULL;


/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int main (void) 
{	
	/* HAL�⣬MPU��Cache��ʱ�ӵ�ϵͳ��ʼ�� */
	System_Init();

	/* �ں˿���ǰ�ر�HAL��ʱ���׼ */
	HAL_SuspendTick();
	
	/* �ں˳�ʼ�� */
	osKernelInitialize();                                  

	/* ������������ */
	ThreadIdStart = osThreadNew(AppTaskStart, NULL, &ThreadStart_Attr);  

	/* ���������� */
	osKernelStart();
	
	while(1);
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskUserIF
*	����˵��: ������Ϣ����		
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
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
				/* K1�����£�ֱ�ӷ����¼���־������AppTaskDM9162Pro������bit0 */
				case KEY_DOWN_K1:
					printf("K1�����£�ֱ�ӷ����¼���־������AppTaskDM9162Pro��bit0������\r\n");
					osThreadFlagsSet(ThreadIdTaskDM9162Pro, KEY1_BIT0);					
					break;	

				/* K2�����£�ֱ�ӷ����¼���־������AppTaskDM9000Pro������bit1 */
				case KEY_DOWN_K2:
					printf("K2�����£�ֱ�ӷ����¼���־������AppTaskDM9000Pro��bit1������\r\n");
					osThreadFlagsSet(ThreadIdTaskDM9000Pro, KEY2_BIT1);
					break;

				/* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
		
		osDelay(20);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskLED
*	����˵��: LED��˸��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal1 
*********************************************************************************************************
*/
void AppTaskLED(void *argument)
{
	const uint16_t usFrequency = 200; /* �ӳ����� */
	uint32_t tick;

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		bsp_LedToggle(2);
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskDM9000Pro
*	����˵��: TCPnetӦ������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal2  
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
*	�� �� ��: AppTaskDM9162Pro
*	����˵��: TCPnetӦ������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal2  
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
*	�� �� ��: AppTaskEthCheck
*	����˵��: ������߲��״̬��
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal3  
*********************************************************************************************************
*/
void AppTaskEthCheck(void *argument)
{
    uint8_t ip4_dm9000addr[NET_ADDR_IP4_LEN];
    uint8_t ip4_dm9162addr[NET_ADDR_IP4_LEN]; 
    uint32_t *ip4_dm9000 = (uint32_t *)ip4_dm9000addr;
    uint32_t *ip4_dm9162 = (uint32_t *)ip4_dm9162addr; 
    uint8_t ucDM9000Link = 0, ucDM9162Link = 0;             /* ������־������������*/
    uint8_t ucDM9000LinkStatus = 0, ucDM9162LinkStatus = 0; /* ������־������������*/

    /* ��ʼ������ */
    ThreadIdTaskDM9162Pro = NULL;
    ThreadIdTaskDM9000Pro = NULL;
    g_ucDM9162LinkStatus = 0;
    g_ucDM9000LinkStatus = 0;

	osDelay(1000);
	
    /* ��ʼ������ */
    netInitialize();

    while(1)
    {
        /* ���߲�δ���������ֲ�����Ҳ���Ը�����Ҫ���������¼���־������ */
        switch (g_ucDM9162LinkStatus)
        {
            /* �����ʱ״̬�����账�� */
            case 0:
            case 1:	
            break;	

            /* ���߲��룬��Ӧ������ */
            case 2:
                if(ucDM9162Link == 0)
                {
                netIF_GetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_dm9162addr, sizeof (ip4_dm9162addr));

                printf_taskdbg("DM9162���߲��룬�ȴ���ȡIP��ַ....\r\n");
                /* ��DHCP��������ȡ��IP��ַ */
                if(*ip4_dm9162 != 0)
                {
                    printf("DM9162��IP��ַ��%d.%d.%d.%d\r\n", ip4_dm9162addr[0],ip4_dm9162addr[1],
                    ip4_dm9162addr[2], ip4_dm9162addr[3]);

                    printf_taskdbg("DM9162����Ӧ������\r\n");

                    if(ThreadIdTaskDM9162Pro == NULL)
                    {
                        ThreadIdTaskDM9162Pro = osThreadNew(AppTaskDM9162Pro, NULL, &ThreadDM9162Pro_Attr);
                    }
                    ucDM9162Link = 2;
                    ucDM9162LinkStatus = 2;
                }
                }
                break;	

            /* ���߰ε�����λ���磬ɾ��Ӧ������ */
            case 3:
                printf_taskdbg("DM9162���߰ε�\r\n");
                g_ucDM9162LinkStatus = 0;
                ucDM9162Link = 0; 
                ucDM9162LinkStatus = 3;            
                break;

            /* �����ļ�ֵ������ */
            default:                     
                break;
        }
        
        switch (g_ucDM9000LinkStatus)
        {
            /* �����ʱ״̬�����账�� */
            case 0:
            case 1:	
            break;	

            /* ���߲��룬��Ӧ������ */
            case 2:
                if(ucDM9000Link == 0)
                {	
                    netIF_GetOption (NET_IF_CLASS_ETH | 1, netIF_OptionIP4_Address, ip4_dm9000addr, sizeof (ip4_dm9000addr));

                    printf_taskdbg("DM9000���߲��룬�ȴ���ȡIP��ַ....\r\n");
                    /* ��DHCP��������ȡ��IP��ַ */
                    if(*ip4_dm9000 != 0)
                    {
                        printf("DM9000��IP��ַ��%d.%d.%d.%d\r\n", ip4_dm9000addr[0],ip4_dm9000addr[1],
                        ip4_dm9000addr[2], ip4_dm9000addr[3]);


                        printf_taskdbg("DM9000����Ӧ������\r\n");
                        if(ThreadIdTaskDM9000Pro == NULL)
                        {
                            ThreadIdTaskDM9000Pro = osThreadNew(AppTaskDM9000Pro, NULL, &ThreadDM9000Pro_Attr);
                        }
                        ucDM9000Link = 2;
                        ucDM9000LinkStatus = 2;
                    }
                }
                break;	

            /* ���߰ε�����λ���磬ɾ��Ӧ������ */
            case 3:
                printf_taskdbg("DM9000���߰ε�\r\n");
                g_ucDM9000LinkStatus = 0;
                ucDM9000Link = 0;
                ucDM9000LinkStatus = 3;
                break;

            /* �����ļ�ֵ������ */
            default:                     
                break;
        }

            /* ���߰ε�����λ���磬ɾ��Ӧ������ */
        if((ucDM9000LinkStatus == 3)&&(ucDM9162LinkStatus == 3))
        {
            printf_taskdbg("���߶����ε�����λ���磬ɾ��Ӧ������\r\n");

            /* �ͷ�����������Դ����TCPnet�ں������ETH�ӿ����� */	
            netUninitialize();

            printf_taskdbg("netUninitialize\r\n");

            /* ɾ��TCPnetӦ������ */
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

            /* ���³�ʼ�� */
            netInitialize();

            printf_taskdbg("netInitialize\r\n");
        }

        osDelay(500);
    }	
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����������������BSP����������
*	��    ��: ��
*	�� �� ֵ: ��
*   �� �� ��: osPriorityNormal4  
*********************************************************************************************************
*/
void AppTaskStart(void *argument)
{
	const uint16_t usFrequency = 1; /* �ӳ����� */
	uint32_t tick;
	
	/* ��ʼ������ */
	HAL_ResumeTick();
	bsp_Init();

	/* �������� */
	AppTaskCreate();

	/* ��ȡ��ǰʱ�� */
	tick = osKernelGetTickCount(); 
	
    while(1)
    {
		/* ��Ҫ�����Դ���ĳ��򣬶�Ӧ������̵��õ�SysTick_ISR */
		bsp_ProPer1ms();
		
		/* ����ӳ� */
		tick += usFrequency;                          
		osDelayUntil(tick);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
	ThreadIdTaskEthCheck = osThreadNew(AppTaskEthCheck, NULL, &ThreadEthCheck_Attr);  	
	ThreadIdTaskLED = osThreadNew(AppTaskLED, NULL, &ThreadLED_Attr);  
	ThreadIdTaskUserIF = osThreadNew(AppTaskUserIF, NULL, &ThreadUserIF_Attr);  
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
