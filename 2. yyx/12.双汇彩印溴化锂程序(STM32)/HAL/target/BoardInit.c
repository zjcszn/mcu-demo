//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* �� �� �� : BoardInit.c
//*
//* �ļ����� : STM32ϵͳĿ����ʼ��ģ��
//*
//* ��    �� : 
//* ��    �� : 
//* �� �� �� : IAR EWARM 5.20
//*********************************************************************************************************
//*

#include "ucos_ii.h"
#include "stm32f10x_lib.h"
#include "head.h"

//*
//*********************************************************************************************************
//* Function : 
//* Describe : 
//*--------------------------------------------------------------------------------------------------------
//* Author   : 
//* Date     : 
//*--------------------------------------------------------------------------------------------------------
//* Mender   :
//* Date     :
//* Describe :
//*********************************************************************************************************
//*
void Target_Init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    //--------------------------------------------------------------------
    //�����Ǹ���ģ�鿪��ʱ��    
    //����GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | \
                           RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE,\
                           ENABLE);
    //����AFIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	
	//����TIM2ʱ�ӡ�����TIM3ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2|RCC_APB1Periph_TIM3|RCC_APB1Periph_SPI2,ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
    //--------------------------------------------------------------------
    //��ʼ�����ڵ���ӦIO�ܽŵ�
    UART_Configuration();
    
    Init_Ki();
    Init_Relay();
    InitKey();
    
    //��ʼ�����ж�
    //--------------------------------------------------------------------
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);


    //����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);


    //����1�����жϴ�    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	//����2�����жϴ�    
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/*Enable the USART3 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable the USART4 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    /*Enable the USART5 Interrupt*/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM2 Interrupt*/ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Enable the TIM3 Interrupt*/ 
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//*
//********************************************************************************************************
//* �� �� �� : OS_CPU_SysTickInit()
//**------------------------------------------------------------------------------------------------------
//* ��    �� : ��ʼ��ϵͳTick
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ������� : None.
//**------------------------------------------------------------------------------------------------------
//* ��   ��  : None.
//********************************************************************************************************
//*
void  OS_CPU_SysTickInit (void)
{
    RCC_ClocksTypeDef  rcc_clocks;

    // ��ȡϵͳƵ��
    RCC_GetClocksFreq(&rcc_clocks);

    // ����HCLK��ΪSysTickʱ��
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    
    // �����Զ�������ֵ
    SysTick_SetReload((rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC) - 1);
    
    // ʹ��SysTick
    SysTick_CounterCmd(SysTick_Counter_Enable);
    
    // ʹ��SysTick�ж�
    SysTick_ITConfig(ENABLE);
}

//
//*********************************************************************************************************
//**                            End Of File
//*********************************************************************************************************
//
