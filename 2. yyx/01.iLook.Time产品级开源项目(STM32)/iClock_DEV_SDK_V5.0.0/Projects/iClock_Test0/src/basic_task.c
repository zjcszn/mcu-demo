/**
  ******************************************************************************
  * @file    basic_task.c
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   �����ƻ�.��Х��Ŀ (QQ��49370295)
  *          ϵͳ��������
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
#include "basic_task.h"

/* define */

/* public */

/* extern */

/* private */


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* ��Դ����                                                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/
char ADC_Process(void);

#define AD_Scale 3.66/0x8D8

TASK_CTRL_INFO  PowerTskInfo;
POWER_TSK_MSG_T gPowerSt;

static char adc_process_st;

void ADC_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  ADC_InitTypeDef ADC_InitStructure;
  
  /* GPIO CONFIG */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* -------------------------------------------------------------------------*/
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
    
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                   /* ����ģʽ */
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                        /* ������ͨ��ģʽ */
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;                  /* ����ת�� */
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  /* ת������������ */
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;               /* �Ҷ��� */
  ADC_InitStructure.ADC_NbrOfChannel = 1;                              /* ɨ��ͨ���� */
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel8 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);  /* ͨ��X,����ʱ��Ϊ55.5����,1�������ͨ����1�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);

  /* У׼ */
  ADC_Cmd(ADC1, ENABLE);
  ADC_StartCalibration(ADC1);
  while( ADC_GetCalibrationStatus(ADC1)!= RESET );
  ADC_Cmd(ADC1, DISABLE);
}

char ADC_Process(void)
{
    switch(adc_process_st)
    {
      case ADC_CONVERT: 
      //����ADC
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
      ADC_Cmd(ADC1, ENABLE);                  /* Enable ADC1 */
      ADC_SoftwareStartConvCmd(ADC1,ENABLE);  /* ʹ��ת����ʼ */
      //
      adc_process_st = ADC_CONVERT_WAIT;
      break;
      
      case ADC_CONVERT_WAIT:
      if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET){
         gPowerSt.BatteryVol = ADC_GetConversionValue(ADC1)*AD_Scale;
         adc_process_st = ADC_READ;
      }
      break;
      
      case ADC_READ:
      //�ر�ADC
      ADC_Cmd(ADC1, DISABLE);
      ADC_SoftwareStartConvCmd(ADC1,DISABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, DISABLE);
      //
      adc_process_st = ADC_CONVERT_FINISHED;
      break;
      
      case ADC_CONVERT_FINISHED:
      break;
    }
    
    return adc_process_st;
}

void GetBatteryAtOnce(void)
{
  adc_process_st = ADC_CONVERT;
  
  while(ADC_Process()!=ADC_CONVERT_FINISHED);
  
}

void PowerMonitorTask_Init(void)
{
    PowerTskInfo.Msg = (uint8_t*)&gPowerSt;
    PowerTskInfo.TickGap = 10000;
    PowerTskInfo.TickMsk = 0;
    QstCtrl(&PowerTskInfo,PWR_MONITOR_INIT);
    //
    ADC_Configuration();
    adc_process_st = ADC_CONVERT;
    while( ADC_Process()!=ADC_CONVERT_FINISHED );
    //
    if( GetChargeState() ){
      gPowerSt.PlugIn = 1;
    }else{
      gPowerSt.PlugIn = 0;
    }
}

void PowerMonitorTask(void)
{
    //
    QstMsgClr(&PowerTskInfo);
    //
    if( PowerTskInfo.Ctrl&TASK_CMD ){
      PowerTskInfo.Ctrl &= ~TASK_CMD;
    }
    //30���һ�ε�ѹ
    if( TimeOutCheck_ms(PowerTskInfo.TickMsk+PowerTskInfo.TickGap)==1 ){
      adc_process_st = ADC_CONVERT;
      PowerTskInfo.TickMsk = GetSysTick_ms();
    }
    if( ADC_Process()==ADC_READ ){
      PowerTskInfo.MsgFlg = 1;
    }
    //ʵʱ����ⲿ��Դ
    if( gPowerSt.PlugIn ){
      if( GetChargeState()==0 ){
        //sys_log_write("DC POWER","pull out");
        gPowerSt.PlugIn = 0;
        PowerTskInfo.MsgFlg = 1;
      }
    }else{
      if( GetChargeState()!= 0 ){
        sys_log_write("DC POWER","charging");
        gPowerSt.PlugIn = 1;
        PowerTskInfo.MsgFlg = 1;
      }
    }
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* usb����                                                                    */
/*                                                                            */
/*----------------------------------------------------------------------------*/
extern uint8_t   g_UsbLinkState;   //0 û���ӣ�1 ����״̬ 2 �Ͽ�����

TASK_CTRL_INFO  UsbMonitorTsk;

void UsbMonitorTask_Init(void)
{
  //��ʼ��������Ϣ
  UsbMonitorTsk.Process = UsbMonitorTask;
  //
  QstEnter(&UsbMonitorTsk,USB_UNLINK);
  USB_Ctrl(0);
}

char UsbMonitorTask(void)
{
  char rslt=0;
  
  if( (gPowerSt.PlugIn==1)&&(QstGetState(&UsbMonitorTsk)==USB_UNLINK) ){
    QstEnter(&UsbMonitorTsk,USB_OPEN);
  }
  //
  switch(UsbMonitorTsk.State){
    case USB_UNLINK:
      break;
    case USB_OPEN:
      USB_Ctrl(1);
      QstEnter(&UsbMonitorTsk,USB_CHECK);
      break;
    case USB_CHECK:
      if( g_UsbLinkState==1 ){
        sys_log_write("USB DISC","linking");
        disc_unmount();
        //
        ClrScreen();
        Glph_Print(0,0,ASCII_5X7,"USB Linking...");
        display_reflash(1);
        QstEnter(&UsbMonitorTsk,USB_LINKING);
      }
      break;
    case USB_LINKING:
      if( gPowerSt.PlugIn==0 ){
        QstEnter(&UsbMonitorTsk,USB_CLOSE);
      }
      rslt = 1;
      break;
    case USB_CLOSE:
      USB_Ctrl(0);
      SoftReset();
      break;
  }
  
  return rslt;
}


/*----------------------------------------------------------------------------*/
/*                                                                            */
/* key                                                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/
static int8_t keytask_state;
static uint32_t press_t,release_t;
static int8_t key_count;


KEY_INFO gKey;

void KeyTask_Init(void)
{
  keytask_state = 0;
  gKey.value = 0;
  gKey.type = 0;
  gKey.wkup_int = 0;
  key_count = 0;
}

void ClrKey(void)
{
  gKey.value = 0;
  gKey.type = 0;
  gKey.wkup_int = 0;
}

void KeyTask(void)
{
  uint8_t key_value=NULL_KEY;
  
  if( RD_WKUP_KEY()==1 )
      key_value = WAKUP_KEY;
      //
  switch(keytask_state){
    case 0: //����Ƿ��а���
      if( key_value==0 ){
        keytask_state++;
      }
      break;
    
    case 1:
      if( key_value==1 ){
        keytask_state++;
        press_t = 0;
      }
      break;      
    
    case 2: //ȷ�ϰ�������
      press_t++;
      //
      if( key_value ){
          if( press_t>10 )
            keytask_state++;
      }else
        keytask_state = 0;
      break;

    case 3: //����or�̰�
      if( key_value==1 ){
        press_t++;
        if( press_t>3000 ){
          key_count++;
          gKey.value = 1;
          gKey.type = KEY_LONG_PRESS;
          //
          release_t = 0;
          keytask_state = 10;
        }
      }else{
        release_t = 0;
        keytask_state++; 
      }
      break;
      
    case 4: //����
      if( key_value==0 ){
        if( release_t++> 200 ){
          gKey.value = 1;
          key_count++;
          gKey.type = KEY_CLICK;
          keytask_state = 0;
        }
      }else{
        keytask_state = 5;
        press_t = 0;
        release_t = 0;
      }
      break;
      
    case 5: //˫��
      if( key_value ){
        press_t++;
        release_t=0;
        if(press_t>10){
          keytask_state = 6;
        }
      }else{
        press_t=0;
        release_t++;
        if(release_t>100 )
          keytask_state = 0;
      }
      break;

    case 6:
      if( key_value==0 ){
        release_t++;
        if( release_t>100 ){
          gKey.value = 1;
          key_count++;
          gKey.type = KEY_DB_CLICK;
          keytask_state = 0;
        }
      }
      break;
    //�ȴ������ͷ�ͨ�ó���
    case 10:
      if( key_value==0 ){
        release_t++;
        if( release_t>100 ){
          keytask_state = 0;
        }
      }else{
        press_t++;
        //
        if( press_t==6000 ){
          gKey.value = PWR_KEY;
          gKey.type = KEY_HOLD;
        }else if( press_t>10000 ){    //���reset
          SoftReset();
        }
      }
  }
  
  //��Ļ�ر�״̬,���а���������Ӧ
  if( DisplayTskInfo.State!=DISPLAY_WORKING ){
      keytask_state = 0;
      gKey.value = 0;
      gKey.type = 0;
  }
  return;  
}



/*----------------------------------------------------------------------------*/
/*                                                                            */
/* rtc_task                                                                   */
/* ��ȡʵʱʱ�Ӳ����������գ�ʱ����                                           */
/*                                                                            */
/*----------------------------------------------------------------------------*/
extern tm rtc_tm;
uint32_t rl_time_t_msk;

void RealTime_Init(void)
{
  rl_time_t_msk = 0;
  DrvInit_rtc();
  sys_time(&rtc_tm);
}

void UpdateSysTime(void)
{
    GetTime();
    sys_time(&rtc_tm);  
}

void RealTime_Task(void)
{
  if( TimeOutCheck_ms(rl_time_t_msk) ){
    rl_time_t_msk = GetSysTick_ms()+100;
    GetTime();
    sys_time(&rtc_tm);
  }
}

