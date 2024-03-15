//*
//*********************************************************************************************************
//*
//*                                                 LOGO
//*
//*
//* �� �� �� : low_level_init.c
//*
//* �ļ����� : ��������ϵͳ��λ��,���ݶγ�ʼ��֮ǰ���ײ����
//*
//*            ��Ҫ��;�ǳ�ʼ��һЩӲ������(��FSMC),ϵͳʱ��,���Ź�,�жϵ�
//*
//*            ע: ���ڶ�û�б���ʼ��,����
//*                ʹ��ȫ�ֱ���,��̬�����ͻ����, �޸�����Ҳ�ᱻ��ʼ���������¸���
//*                ʹ��0��ʼ���ı���Ҳ����ִ���, ��Ϊ��û�����г�ʼ������
//*                �˺����������ڵײ��ʼ��
//*                
//*                ϵͳ��λ��ʹ��MSP��ջ, ���ú���ע��MSP�ߴ�
//*
//* ��    �� : SuperMario 
//* ��    �� : 2008.11.1a
//* �� �� �� : IAR EWARM 5.20
//*********************************************************************************************************
//*
#ifdef __cplusplus
extern "C" {
#endif
  
#include "stm32f10x_lib.h"
  
#pragma language=extended

__interwork int __low_level_init(void);

__interwork int __low_level_init(void)
{
    // �ر��ж�
    asm("CPSID  I");
    
    //=====================================================================================================
    // ��λĿ��ϵͳ��NVIC,�ָ�Ϊ�ϵ�Ĭ��ֵ
    //=====================================================================================================
    NVIC_DeInit();
    NVIC_SCBDeInit();
    // �����RAM�е���, �޸��ж��������׵�ַ NVIC_VectTab_RAM = 0x20000000
    // NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
    // �����޸��ж���������Flash��, NVIC_VectTab_FLASH = 0x08000000
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
    //=====================================================================================================
    // ϵͳʱ�ӳ�ʼ��
    //=====================================================================================================
    // RCC system reset(for debug purpose)
    // ��RCC�Ĵ����ָ�Ϊ�ϵ�Ĭ��ֵ
    RCC_DeInit();
    // Enable HSE
    // ��high-speed external clock signal
    RCC_HSEConfig(RCC_HSE_ON);
    // �ȴ��ⲿ�ⲿ����ready
    RCC_WaitForHSEStartUp();
    // Enable Prefetch Buffer
    // ����Ԥȡ������
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    // FLASH_Latency_0 = 000: 0�ȴ�״̬, �� 0 < SYSCLK��24MHz
    // FLASH_Latency_1 = 001: 1�ȴ�״̬, �� 24MHz < SYSCLK�� 48MHz
    // FLASH_Latency_2 = 010: 2�ȴ�״̬, �� 48MHz < SYSCLK�� 72MHz
    // SYSCLK = 72MHz
    // Flash 2 wait state
    FLASH_SetLatency(FLASH_Latency_2);
    // HCLK   = SYSCLK
    // HCLK   = ������ʱ��
    // SYSCLK = PLL_CLK or HSE or HSI
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    // PCLK2 = HCLK
    // PCLK2 = 72MHz
    RCC_PCLK2Config(RCC_HCLK_Div1);
    // PCLK1 ��󲻿ɳ���36MHz
    // PCLK1 = 72MHz / 2 = 36MHz
    RCC_PCLK1Config(RCC_HCLK_Div2);
    // ADCCLK = PCLK2/2
    // ADCCLK = 12MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div2);
    // PLLCLK = 8MHz * 9 = 72 MHz
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
    // Enable PLL
    RCC_PLLCmd(ENABLE);
    // �ȴ�PLL Ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    // ����SYSCLK��PLL_CLK, SYSCLK = PLL_CLK
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    // �ȴ� SYSCLK �ȶ�
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
    
    
    
    /*
    // FSMC �ӿڳ�ʼ��
    // Enable FSMC clock, ����λ��RCC_AHBENR�Ĵ�����
    // AHB Peripheral Clock enable register (RCC_AHBENR:0x40021014) Reset value: 0x0000 0014
    // ����λ,��������:
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |Bit31 - Bit11 |Bit10|Bit9 |Bit8 |Bit7 |Bit6 |Bit5 |Bit4 |Bit3 |Bit2 |Bit1 |Bit0 |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // | Reserved     |SDIO |res. |FSMC |res. |CRCEN|res. |FLITF|res. |SRAM |DMA2 |DMA1 |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |     0        |  0  |  0  |  1  |  0  |  0  |  0  |  1  |  0  |  1  |  0  |  0  |
    // |--------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    *(vu32 *)0x40021014 = 0x00000114;
    
    // A0 - A18 ---- GPIOD, GPIOE, GPIOF and GPIOG
    // D0 - D15 ---- GPIOD, GPIOE
    // CE       ---- PG10 ( NE3 )
    // OE       ---- PD4
    // WE       ---- PD5
    // LB       ---- PE0
    // UB       ---- PE1
    // ʹ�� GPIOD, GPIOE, GPIOF and GPIOG ʱ��
    // APB2 Peripheral reset register (RCC_APB2RSTR:0x40021018) Reset value: 0x00000 0000
    // APB2 ��16λReserved, ��14λ���������λ,��������:
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |Bit15 |Bit14 |Bit13|Bit12|Bit11|Bit10|Bit9 |Bit8 |Bit7 |Bit6 |Bit5 |Bit4 |Bit3 |Bit2 |Bit1 |Bit0 |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |ADC3  |USART1|TIM8 |SPI1 |TIM1 |ADC2 |ADC1 |IOPG |IOPF |IOPE |IOPD |IOPC |IOPB |IOPA |res. |AFIO |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    // |  0   |   0  |  0  |  0  |  0  |  0  |  0  |  1  |  1  |  1  |  1  |  0  |  0  |  0  |  0  |  0  |
    // |------|------|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|-----|
    *(vu32 *)0x40021018 = 0x000001E0;
  
    //---------------  SRAM Data lines, NOE and NWE configuration ---------------*
    //----------------  SRAM Address lines configuration -------------------------*
    //----------------  NOE and NWE configuration --------------------------------*  
    //----------------  NE3 configuration ----------------------------------------*
    //----------------  NBL0, NBL1 configuration ---------------------------------*
  
    *(vu32 *)0x40011400 = 0x44BB44BB;
    *(vu32 *)0x40011404 = 0xBBBBBBBB;
  
    *(vu32 *)0x40011800 = 0xB44444BB;
    *(vu32 *)0x40011804 = 0xBBBBBBBB;
   
    *(vu32 *)0x40011C00 = 0x44BBBBBB;
    *(vu32 *)0x40011C04 = 0xBBBB4444;  

    *(vu32 *)0x40012000 = 0x44BBBBBB;
    *(vu32 *)0x40012004 = 0x44444B44;
  
    //----------------  FSMC Configuration ---------------------------------------*  
    //----------------  Enable FSMC Bank1_SRAM Bank ------------------------------*
  
    *(vu32 *)0xA0000010 = 0x00001011;
    *(vu32 *)0xA0000014 = 0x00000200;
    */

    //=====================================================================================================
    // �޸ķ���ֵ����ѡ�񲻳�ʼ�����ʼ�����ݶ�
    // Return: 0 ��ִ�жγ�ʼ��
    //         1 ִ�жγ�ʼ��
    //=====================================================================================================
    return 1;
}

#pragma language=default

#ifdef __cplusplus
}
#endif
