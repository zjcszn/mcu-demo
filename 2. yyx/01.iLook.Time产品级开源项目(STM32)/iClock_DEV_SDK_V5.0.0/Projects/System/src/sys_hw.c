/**

  ******************************************************************************
  * @file    sys_hw.c
  * @author  SZQVC
  * @version V0.2.8
  * @date    2015.2.14
  * @brief   �����ƻ�.��Х��Ŀ (QQ��49370295)
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * �ļ���Ȩ�顰����Ȩ�ɰ��ӿƼ����޹�˾��(���SZQVC�����С�                   *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
  
**/
#include "sys_includes.h"

/* define */

/* public */

/* extern */

/* private */

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* ����                                                                       */
/*                                                                            */
/*----------------------------------------------------------------------------*/

//���ܺ��CPUID  
volatile static uint32_t *CPUIDEncrypt = (uint32_t*)0x0801F800;  
  
//д���������  
void WriteEncrypt(void)  
{   
   uint32_t EncryptCode;
  
    //��һ����д:��UIDд�뵽Flash��  
    if(*CPUIDEncrypt==0xFFFFFFFF)  
   {  
        uint32_t CpuID[3];         
        //��ȡCPUΨһ��ID  
        CpuID[0]=*(uint32_t*)ID1;  
        CpuID[1]=*(uint32_t*)ID2;  
        CpuID[2]=*(uint32_t*)ID3;          
  
        //�����㷨,�ܼ򵥵ļ����㷨  
        EncryptCode=CpuID[0]^(CpuID[1]>>1)^CpuID[2];     
        FLASH_Unlock();  
        FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);  
        FLASH_ProgramWord((uint32_t)CPUIDEncrypt, EncryptCode);  
        FLASH_Lock();  
    }  
}

//�жϼ���  
char JudgeEncrypt(void)  
{         
    uint32_t CpuID[4];         
    //��ȡCPUΨһ��ID
    CpuID[0]=*(uint32_t*)ID1;  
    CpuID[1]=*(uint32_t*)ID2;  
    CpuID[2]=*(uint32_t*)ID3;      
    //�����㷨,�ܼ򵥵ļ����㷨  
    CpuID[3]=CpuID[0]^(CpuID[1]>>1)^CpuID[2];     
    //���Flash�е�UID�Ƿ�Ϸ�   
    return (*CPUIDEncrypt == CpuID[3]);  
} 


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* ARM Sleepģʽ                                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void myPWR_EnterSleepMode(void)
{
/*
ͨ��ִ��WFI��WFEָ�����˯��״̬������Cortex.-M3ϵͳ���ƼĴ����е�SLEEPONEXIT
λ��ֵ��������ѡ�������ѡ��˯��ģʽ������ƣ�
�� SLEEP-NOW�����SLEEPONEXITλ���������WRI��WFE��ִ��ʱ��΢������������
��˯��ģʽ��
�� SLEEP-ON-EXIT�����SLEEPONEXITλ����λ��ϵͳ��������ȼ����жϴ����������
��ʱ��΢����������������˯��ģʽ��
*/
    
    USART_Cmd(USART1, DISABLE);
//    USART_Cmd(USART2, DISABLE);
//    USART_Cmd(USART3, DISABLE);
    SysTick_Ctrl(DISABLE);
    //
    PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);        //PWR_Regulator_ON,PWR_Regulator_LowPower
    //
    SysTick_Ctrl(ENABLE);
    USART_Cmd(USART1, ENABLE);
//    USART_Cmd(USART2, ENABLE);
//    USART_Cmd(USART3, ENABLE);
    SysTick_Ctrl(ENABLE);
    SystemInit();
    delay_us(10000);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* ARM Stopģʽ                                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void myPWR_EnterPowrOff(void)
{
    DEV_PWR_OFF();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 
//  RCC_APB2PeriphResetCmd(0X01FC,DISABLE);    //��λ����IO��
    PWR_WakeUpPinCmd(ENABLE);
    PWR_EnterSTANDBYMode();
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* ARM �����λ                                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void SoftReset(void)
{
    __set_FAULTMASK(1);     // �ر��쳣�ж�
    NVIC_SystemReset();     // ��λ
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* USB�򿪻�ر�                                                               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void USB_Ctrl(char onoff)
{
    if( onoff==1 ){
        Set_USBClock();
        USB_Interrupts_Config();
        USB_Init();
        MAL_Config();
    }else if( onoff==0 ){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
    }
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* �ⲿ�ж�����                                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void EXTI_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    //
    EXTI_ClearFlag(EXTI_Line0);
    EXTI_ClearFlag(EXTI_Line5);

    //PB5: XYZ intr
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);   //ѡ�� GPIO�ܽ������ⲿ�ж���·    

    EXTI_InitStructure.EXTI_Line    = EXTI_Line5;                 //�ⲿ�ж���
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;        //�ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //�жϴ�����ʽ
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //���ж�
    EXTI_Init(&EXTI_InitStructure);    
    //PA0: KEY_WAKUP intr
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);   //ѡ�� GPIO�ܽ������ⲿ�ж���·

    EXTI_InitStructure.EXTI_Line    = EXTI_Line0;                 //�ⲿ�ж���
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;        //�ж�ģʽ
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//�жϴ�����ʽ
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     //���ж�
    EXTI_Init(&EXTI_InitStructure);    
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* �жϿ���������                                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    //PB5: XYZ intr    
    NVIC_InitStructure.NVIC_IRQChannel            = EXTI9_5_IRQn; //ָ���ж�Դ
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     //�ж�ռ�ȵȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // ָ����Ӧ���ȼ���1
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    //PA0: KEY_WAKUP intr
    NVIC_InitStructure.NVIC_IRQChannel            = EXTI0_IRQn;   //ָ���ж�Դ
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     //�ж�ռ�ȵȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;            // ָ����Ӧ���ȼ���1
    NVIC_InitStructure.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* �жϷ������                                                                */
/* EXTI9_5_IRQHandler -> gSensor�ж�                                          */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void EXTI9_5_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line5) != RESET)
  {
      //����жϴ������
	    //
      EXTI_ClearFlag(EXTI_Line5);
  }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* �жϷ������                                                                */
/* EXTI0_IRQHandler -> WAKEUP KEY�ж�                                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
extern KEY_INFO gKey;

void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    EXTI_ClearFlag(EXTI_Line0);
    gKey.wkup_int++;
  }
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* GPIO����                                                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);

  /* USB_DISCONNECT_PIN used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
  
  /* DEV POWER */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PWR_DEV_CTRL, ENABLE);

  /* USB_DISCONNECT_PIN used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = PWR_DEV_CTRL_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;

  GPIO_Init(PWR_DEV_CTRL, &GPIO_InitStructure);
  DEV_PWR_ON();
  //
    //spi flash cs
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SPI_CS_FL, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = SPI_CS_FL_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI_CS_FL, &GPIO_InitStructure);

  //spi lcm cs
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_SPI_CS_LCM, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = SPI_CS_LCM_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI_CS_LCM, &GPIO_InitStructure);

  //lcm power ctrl
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PWR_LCM_CTRL, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = PWR_LCM_CTRL_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWR_LCM_CTRL, &GPIO_InitStructure);

  //lcm reset
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LCM_RST, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = LCM_RST_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCM_RST, &GPIO_InitStructure);

  //lcm data/command
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_LCM_DC, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = LCM_DC_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LCM_DC, &GPIO_InitStructure);
  
  
  //SPI bus
  /* Configure SPI1 pins: PA5-SCK, PA6-MISO and PA7-MOSI */
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //Charge State
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CHRG_ST, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = CHRG_ST_PIN ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CHRG_ST, &GPIO_InitStructure);  

  //WORK LED
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_WORK_LED, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = WORK_LED_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WORK_LED, &GPIO_InitStructure);
  //
  LED_ON();   //GPIO_SetBits(WORK_LED,WORK_LED_PIN);

  //Beep 
#if 0
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_BEEP_HZ, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = BEEP_HZ_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(BEEP_HZ, &GPIO_InitStructure);
  //
  GPIO_ResetBits(BEEP_HZ,BEEP_HZ_PIN);
#endif
  
  //Temperature
#if 0
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_TEMP_IO, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = TEMP_IO_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(TEMP_IO, &GPIO_InitStructure);
  //
  GPIO_ResetBits(TEMP_IO,TEMP_IO_PIN);
#endif
  //RTC_CLK
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_RTC_CLK, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = RTC_CLK_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(RTC_CLK, &GPIO_InitStructure);
  //
  GPIO_ResetBits(RTC_CLK,RTC_CLK_PIN);
  //RTC_DAT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_RTC_DAT, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = RTC_DAT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; //GPIO_Mode_Out_OD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(RTC_DAT, &GPIO_InitStructure);
  //
  GPIO_ResetBits(RTC_DAT,RTC_DAT_PIN);
  //RTC_RST
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_RTC_RST, ENABLE);

  GPIO_InitStructure.GPIO_Pin   = RTC_RST_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(RTC_RST, &GPIO_InitStructure);
  //
  GPIO_ResetBits(RTC_RST,RTC_RST_PIN);
  //WKUP KEY
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_WKUP_KEY, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = WKUP_KEY_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(WKUP_KEY, &GPIO_InitStructure);
  
  //GPS POWER CTRL
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PWR_GPS_CTRL, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = PWR_GPS_CTRL_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWR_GPS_CTRL, &GPIO_InitStructure);
  //GPIO_ResetBits(PWR_GPS_CTRL,PWR_GPS_CTRL_PIN);
  GPIO_SetBits(PWR_GPS_CTRL,PWR_GPS_CTRL_PIN);
  //XYZ INT
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_XYZ_INT, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = XYZ_INT_PIN;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;  //GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(XYZ_INT, &GPIO_InitStructure);
  
  GPIO_SetBits(XYZ_INT,XYZ_INT_PIN);
  
  //���õ�IO����
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PORTA_NULL, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PORTA_NULL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(PORTA_NULL, &GPIO_InitStructure);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_PORTB_NULL, ENABLE);
  GPIO_InitStructure.GPIO_Pin = PORTB_NULL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(PORTB_NULL, &GPIO_InitStructure);
  

}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* SPI������                                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void SPI_Config(uint8_t type)
{
    SPI_InitTypeDef SPI_InitStructure;

    /* Disable all spi device */
    GPIO_SetBits(SPI_CS_FL,SPI_CS_FL_PIN);
    GPIO_SetBits(SPI_CS_LCM,SPI_CS_LCM_PIN);
/*    
    if( (type==CFG_SPI_FLASH)
      ||(type==CFG_SPI_LCM)
      ||(type==CFG_SPI_COMM) )
*/
    {
      SPI_Cmd(SPI1, DISABLE);
      /* Enable SPI1 Periph clock */
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA
                             | RCC_APB2Periph_SPI1,
                             ENABLE);
  
      /*------------------------ SPI1 configuration ------------------------*/
      SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//SPI_Direction_1Line_Tx;
      SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
      SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
      SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;          //ʱ�����յ�
      SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;        //���ݲ����ڵ�һ��ʱ����
      SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;
      //SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;/* 72M/64=1.125M */
      SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;    //check lcm spi & spi flash
      SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
      SPI_InitStructure.SPI_CRCPolynomial = 7;
  
      SPI_I2S_DeInit(SPI1);
      SPI_Init(SPI1, &SPI_InitStructure);
  
      /* Enable SPI_MASTER */
      SPI_Cmd(SPI1, ENABLE);
      SPI_CalculateCRC(SPI1, DISABLE);
    }
}

uint8_t SPI_RdWrByte(uint8_t dt)
{
  uint8_t tmp = dt;

    //Wait until the transmit buffer is empty
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    // Send the byte
    SPI_I2S_SendData(SPI1, tmp);

    //Wait until a data is received
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    // Get the received data
    tmp = SPI_I2S_ReceiveData(SPI1);

    // Return the shifted data
    return tmp;
}

void SPI_WriteByte(uint8_t dt)
{
  uint8_t tmp = dt;

    //Wait until the transmit buffer is empty
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    // Send the byte
    SPI_I2S_SendData(SPI1, tmp);
    delay_us(10);
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* I2C������                                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#define I2C_TIMEOUT   0xffff;
void I2C_Config(void)
{
	I2C_Init();
}

uint8_t I2C_Read(uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num)
{
  uint16_t i2c_timeout,i=0;
  //
  i2c_timeout = I2C_TIMEOUT;
  while( (i2c_timeout-->0)&&(I2C_CheckBusy()!=I2C_IDLE) );
  //
  if( i2c_timeout>0){
    //-->I2C START
    I2C_Start();
    //-->Slave Addr | Write 
    I2C_SendByte(I2C_Addr);
    if( I2C_WaitAck()==I2C_ACK ){
      //-->Reg Addr
      I2C_SendByte(addr);
      if( I2C_WaitAck()==I2C_ACK ){
        //-->I2C START
        I2C_Start();          
        //-->Slave Addr | Read
        I2C_SendByte(I2C_Addr|0x01);
        if( I2C_WaitAck()==I2C_ACK ){
          while(i<num){
              buf[i] = I2C_ReceiveByte();
              i++;
              if(num-i>0)
                I2C_Ack();
              else{
                I2C_NoAck();
                I2C_Stop();
              }
          }
          I2C_Stop();
          return i;
        }
      }
    }
  }
  //
  I2C_Stop();  
  return i;
} 



uint8_t I2C_Write(uint8_t I2C_Addr,uint8_t addr,uint8_t value)
{
  uint16_t i2c_timeout;
  //
  i2c_timeout = I2C_TIMEOUT;
  while( (i2c_timeout-->0)&&(I2C_CheckBusy()!=I2C_IDLE) );
  //
  if( i2c_timeout>0){ 
    //-->I2C START
    I2C_Start();
      
    //-->Slave Addr | Write 
    I2C_SendByte(I2C_Addr);
    if( I2C_WaitAck()==I2C_ACK ){
      //-->Reg Addr
      I2C_SendByte(addr);
      if( I2C_WaitAck()==I2C_ACK ){
        //-->Data
        I2C_SendByte(value);
        if( I2C_WaitAck()==I2C_ACK ){
          I2C_Stop();
          return 1;
        }
      }
    }
  }
  I2C_Stop();
  return 0;
}

#if 0
void test_i2c_dev(void)
{
  uint8_t tmp;
    
  I2C_Read(I2C_DEV_BMP280,0xd0,&tmp,1);
}
#endif


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* UART1                                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
UART_INFO Uart1;

void USART1_Interrupts_Config(char flg)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;   //ͨ������Ϊ����1
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //�ж�ռ�ȵȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�ж���Ӧ���ȼ�
    if( flg==0 )
      NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;          //�ر��ж�
    else
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //���ж�
    NVIC_Init(&NVIC_InitStructure);
}

void UART1_Configuration(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef  USART_ClockInitStructure;
    
    /* ��ʼ�����ڲ��� */
    if( (Uart1.RxBfSize==0)||(Uart1.TxBfSize==0)||(Uart1.RxBuffer==NULL)||(Uart1.TxBuffer==NULL) ){
      Uart1.State=0;
      return;
    }
    
    /* ��UART1ʹ��ʱ��,UART1��APB1�� */
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);//Ҫ��UART1ӳ�䵽GPIOA��,�Ƿ�Ҳ��Ҫͬʱ��GPIOA��ʹ��ʱ����?
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE);
    //
    /*  USART1_TX -> PA9 , USART1_RX ->  PA10 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);    
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;         
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;                    // ʱ�ӵ͵�ƽ�
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;                          // ʱ�ӵ͵�ƽ
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;                        // ʱ�ӵڶ������ؽ������ݲ���
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;                // ���һλ���ݵ�ʱ�����岻��SCLK���
    /* Configure the USART1 synchronous paramters */
    USART_ClockInit(USART1, &USART_ClockInitStructure);                            // ʱ�Ӳ�����ʼ������
                                                                                                                                             
    USART_InitStructure.USART_BaudRate =9600;                                      // ������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                    // 8λ����
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                         // ��֡��β����1��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No ;                           // ��żʧ��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;// Ӳ��������ʧ��
    
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                // ����ʹ��+����ʹ��
    /* Configure USART1 basic and asynchronous paramters */
    USART_Init(USART1, &USART_InitStructure);
    
    /* Enable USART1 */
    USART_ClearFlag(USART1, USART_IT_RXNE);                                        //���жϣ�����һ�����жϺ����������ж�
    USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);                                  //ʹ��USART1�ж�Դ
    USART_Cmd(USART1, ENABLE);                                                     //USART1�ܿ��أ�����
    
    /* Enable USART1 Interrupt */
    USART1_Interrupts_Config(1);
    //
    Uart1.RxCounter = 0;
    Uart1.TxCounter = 0;
    Uart1.State = 1;
}


// for printf() �����ײ�
int fputc(int ch, FILE *f)
{ 
    USART1->DR = (u8) ch; 

    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET) ;

    return ch; 
}


int fgetc(FILE *f)
{
   while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET));
    
   return (USART_ReceiveData(USART1));
}

/*******************************************************************************
* Function Name  : USART1_IRQHandler
* Description    : This function handles USART1 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
    unsigned char tmp;
    
    
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        tmp = USART_ReceiveData(USART1);
        //���ݰ�ʱ���ǩ
        Uart1.IdelTickMask = GetSysTick_ms();
        //��һ���ַ�������'$'��ʼ
        if( (Uart1.RxCounter==0) ){
            if( tmp=='$' )
                Uart1.RxBuffer[Uart1.RxCounter++] = tmp;
            return;
        }
        //�洢
        if( Uart1.RxCounter<Uart1.RxBfSize ){
            Uart1.RxBuffer[Uart1.RxCounter++] = tmp;
        }else{
          Uart1.RxCounter++;  //���
        }
    }
    
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {   
        /* Write one byte to the transmit data register */
        USART_SendData(USART1, Uart1.TxBuffer[Uart1.TxCounter++]);


        if(Uart1.TxCounter >= Uart1.TxBfSize)
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);        
    }
}

/*----------------------------------------------------------------------------------------*/
/*                             GPS UART�ӿں���                                           */
/*----------------------------------------------------------------------------------------*/
#define GPS_Uart    Uart1

int gps_comm_get_len(void)
{
    if( GPS_Uart.RxCounter==0 )
      return 0;
    else if( TimeOutCheck_ms(GPS_Uart.IdelTickMask+10)==1 )
      return GPS_Uart.RxCounter;
    else
      return 0;
}


int gps_comm_read(char *buff, int length)
{
    int rd_len = length;
    
    if(length>GPS_Uart.RxCounter)
        rd_len = GPS_Uart.RxCounter;

//  strcpy( buff,(char*)Uart2.RxBuffer );
  memcpy( buff,(char*)GPS_Uart.RxBuffer, rd_len );

  return rd_len;
}

void gps_comm_clr(void)
{
    GPS_Uart.RxCounter = 0;
}


void gps_comm_disable(void)
{
  USART1_Interrupts_Config(0);
}

void gps_comm_enable(void)
{
  USART1_Interrupts_Config(1);
}


