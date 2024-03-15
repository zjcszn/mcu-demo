//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* �� �� �� : main.c
//*
//* �ļ����� : STM32ϵͳ���ļ�
//*
//* ��    �� : 
//* ��    �� : 
//* �� �� �� : IAR EWARM 5.20
//*********************************************************************************************************
//*
#define  OS_GLOBALS
#define _DLIB_FILE_DESCRIPTOR 0//ʹ��fputc()
#include "head.h"
#include <stdio.h>

//---
#define TASK_REF_DISPLAY_STK_SIZE 512
OS_STK	Task_Ref_Display_Stk[TASK_REF_DISPLAY_STK_SIZE];
void Task_Ref_Display(void *pdata);
//---
#define TASK_RUN_LED_STK_SIZE 512
OS_STK	Task_Run_Led_Stk[TASK_RUN_LED_STK_SIZE];
void Task_Run_Led(void *pdata);
//---
#define TASK_LOGIC_STK_SIZE 512
OS_STK	Task_logic_Stk[TASK_LOGIC_STK_SIZE];
void Task_Logic(void *pdata);



//int fputc(int ch, FILE *f);
//int fputc(int ch, FILE *f)
//{
 //   USART1->DR = (u8) ch;
    
    /* Loop until the end of transmission */
   // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
    //{
   // }
   // return ch;
//}

//*
//********************************************************************************************************
//* �� �� �� : static void APP_TaskStart(void *pdata)
//**------------------------------------------------------------------------------------------------------
//* ��    �� : main
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ��   ��  : None.
//********************************************************************************************************
//*
static void APP_TaskStart(void *pdata)
{
    pdata = pdata;
    
    //Ŀ����ʼ��,
    Target_Init();       
    
    // ����ϵͳTick
    OS_CPU_SysTickInit();
	
    //ͳ�������ʼ��
#if OS_TASK_STAT_EN > 0
    OSStatInit();
#endif
	
    //�¼��Ľ���
    CreateEvents();
	
    //ϵͳ������Ľ���
    //CreateMultiTask();
    
	
    InitPara();//���ڱ����������ȳ�ʼ��
    
    //��ʼ�����Զ�����
    //OSTaskSuspend(OS_PRIO_SELF);
    while(1)
    {        
		
        //ˢ��Һ����ʾ
        OSTaskCreateExt(Task_Ref_Display,
			(void *)0,
			&Task_Ref_Display_Stk[TASK_REF_DISPLAY_STK_SIZE-1],
			12,
			12,
			&Task_Ref_Display_Stk[0],
			TASK_REF_DISPLAY_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		//OSTaskNameSet(Task1_LED1_Prio, "Task1-LED1", &err);
		
		//���е�����
		
		OSTaskCreateExt(Task_Run_Led,
			(void *)0,
			&Task_Run_Led_Stk[TASK_RUN_LED_STK_SIZE-1],
			13,
			13,
			&Task_Run_Led_Stk[0],
			TASK_RUN_LED_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		//OSTaskNameSet(Task2_LED2_Prio, "Task2-LED2", &err);
		
        
        //�˻�������������,������Ķ�����lcd_interface.c��
		
		
        OSTaskCreateExt(TaskLcdInf,
			(void *)0,
			&TaskLcdInfStk[TASK_LCD_INF_STK_SIZE-1],
			14,
			14,
			&TaskLcdInfStk[0],
			TASK_LCD_INF_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
        //�߼��������ͬʱ���� KI
        OSTaskCreateExt(Task_Logic,
			(void *)0,
			&Task_logic_Stk[TASK_LOGIC_STK_SIZE-1],
			10,
			10,
			&Task_logic_Stk[0],
			TASK_LOGIC_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
        
        //�߼��������ͬʱ���� KI
        OSTaskCreateExt(TaskPAIWU,
			(void *)0,
			&TaskPAIWUStk[TASK_PAIWU_STK_SIZE-1],
			11,
			11,
			&TaskPAIWUStk[0],
			TASK_PAIWU_STK_SIZE,
			(void *)0,
			OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
		
	
        
        
        
        
        
		
        OSTimeDly(OS_TICKS_PER_SEC);
        OSTaskSuspend(OS_PRIO_SELF);
    }
}

//*
//********************************************************************************************************
//* �� �� �� : void main(void)
//**------------------------------------------------------------------------------------------------------
//* ��    �� : main
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ��   ��  : None.
//********************************************************************************************************
//*
void main(void)
{
#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    INT8U  err;
#endif
    
    OSInit();
    
    //���ÿ�����������
#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(OS_TASK_IDLE_PRIO, "uC/OS-II Idle", &err);
#endif
    //����ͳ����������
#if (OS_TASK_NAME_SIZE > 14) && (OS_TASK_STAT_EN > 0)
    OSTaskNameSet(OS_TASK_STAT_PRIO, "uC/OS-II Stat", &err);
#endif
    
    //������������
    OSTaskCreateExt(APP_TaskStart,                                        //void (*task)(void *pd) �����׵�ַ
		(void *)0,                                            //void *pdata            ����ָ��
		&APP_TaskStartStk[APP_TASK_START_STK_SIZE - 1],       //OS_STK *ptos           ָ�������ջջ����ָ��
		(INT8U)APP_TASK_START_PRIO,                           //INT8U prio             �������ȼ�
		(INT16U)APP_TASK_START_ID,                            //INT16U id              �����ID��
		&APP_TaskStartStk[0],                                 //OS_STK *pbos           ָ�������ջջ�׵�ָ��
		(INT32U)APP_TASK_START_STK_SIZE,                      //INT32U stk_size        ��ջ����
		(void *)0,                                            //void *pnext            ����ָ��
		OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);           //INT16U opt             �趨OSTaskCreateExt��ѡ��
	
#if OS_TASK_NAME_SIZE > 14
    OSTaskNameSet(APP_TASK_START_PRIO, "Task-Start", &err);
#endif
	
    OSStart();
}


//���LCDˢ����ʾ����
static void Task_Ref_Display(void *pdata)
{
    u16 base_ref;   //��������ʱ�����ȥˢ����Ļ
  
    pdata = pdata;    
	
    GUI_Initialize();
    
    base_ref = 0;
    
    
    while(1)
    {
		
      if ((lcd_need_refurbish) || (base_ref++ > 200)) //��Ҫˢ�£���ʱˢ��ʱ�䵽����ˢ��Ļ
        {
            lcd_need_refurbish = 0;
            base_ref = 0;
			
            LCD_DispRefurbish();           
            
        }
		
		OSTimeDly(OS_TICKS_PER_SEC / 20);  
        
        		
		//���߼�����ŵ���ͬ������
//		Control_Relay();//�����߼����Ƶ�ǰ�̵���
    }    
}

//������е���˸����
//������ �ɼ�AD,ͨ�Ŵ���
void  Task_Run_Led(void *pdata)
{   
    
  
    pdata = pdata;  
	

    Adc_Init(); 
    //InitModbus();
    Init_Mb_Mst();
    
	LED_Init();

    
    while(1)
    {
      
     
        
      
		//
        OSTimeDly(OS_TICKS_PER_SEC/10 );
        		        
       // TaskModbus(COM3);
        TaskModbus(COM1);     
        TaskModbus(COM2);
		TaskModbus(COM3);     
        TaskModbus(COM4);
#ifdef FOR_TEST     
        TaskModbus(COM5);
#else
        //����MODBUS��վ
        Pro_Mb_Mst(); //COM5
#endif	
		LED_Run();       
    }
	
    
}
//�߼����ܴ���
void  Task_Logic(void *pdata)
{      
	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
	//    OS_CPU_SR  cpu_sr = 0;
#endif
	
    pdata = pdata;
    
    
    
	Init_Relay();
	
    Init_Ki();    
        
    ProKi();//ɨ�迪��    
    	 
    while(1)
    {
        OSTimeDly(OS_TICKS_PER_SEC/40); 

        ProLogic();             
		
        ProKi();//ɨ�迪��
    }
    
}



