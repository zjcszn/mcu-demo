/**

  ******************************************************************************
  * @file    main.c
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


/* Includes ------------------------------------------------------------------*/
#include "sys_includes.h"
#include "ui.h"

/* version */
const char prj_info[]={"SZQVC iLook.Time"};
const char prj_version[]={"V5.0.0 2015/8/15"};

/* define */

/* public */

/* extern */
extern bool disc_mount(void);

/* private */


/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
extern POWER_TSK_MSG_T gPowerSt;
uint16_t  task_onoff_flg;

int main(void)
{
  uint8_t *logo_ptr;
  int logo_width,logo_height;
  
  //
  GPIO_Config();
  //�͵�ѹ����ֱ�ӹػ�
  PowerMonitorTask_Init();
  if( gPowerSt.BatteryVol<3.4 ){
        LCM_PWR_OFF();
        GPS_PwrOff();
        DEV_PWR_OFF();
        myPWR_EnterPowrOff();
  }
  //ARM��ʼ��
  UART1_Configuration();
  EXTI_Configuration();
  NVIC_Configuration();
  I2C_Config();
  //
  delay_us(2000);
  //Qst��ʼ��
  SysTick_Init();
  QstMonitor_Init();
  //������ʼ��
  spiflash_init();
  //�����ļ�ϵͳ,����ȡϵͳ����
  disc_mount();
  ilook_cfg_load();   //��ȡ�����ļ�
  //�����ʼ��
  RealTime_Init();    //���������������
  UsbMonitorTask_Init();
  DisplayTask_Init();
  //������ʾ
  QstCtrl(&DisplayTskInfo,DISPLAY_PWR_ON);
  if( (logo_ptr=load_logo(&logo_width,&logo_height))!=0 ){
    Glph_DrawBitmap(0,0,BMP_FILE|BIT_MAP_REVERSE,logo_width,logo_height,logo_ptr);
  }else{
    Glph_Print(0,0,MS_GOTHIC_8X16,(char*)prj_info);
    Glph_Print(0,24,ASCII_5X7,(char*)prj_version);
  }
  //������ʾ
  while(TimeOutCheck_Sec(iLookCfg.T_LogoDisplay)==0)
  {
    //��ʾLOGO
    DisplayTask();
    //����������ɿ���������ػ�
    if( TimeOutCheck_Sec(1)==0 ){
      if( GPIO_ReadInputDataBit(WKUP_KEY,WKUP_KEY_PIN)==0 ){
          LCM_PWR_OFF();
          GPS_PwrOff();
          DEV_PWR_OFF();
          myPWR_EnterPowrOff();
      }
    }
  }
  ClrScreen();
  LED_OFF();
  //����ϵͳ
  KeyTask_Init();
  //
  sys_log_write("POWER ON","OK");
  //
  UiTask_Init();
  //
  while(1)
  {
    QstMonitor();
    //
    PowerMonitorTask();
    //
    if( UsbMonitorTask()==1 )
      continue;
    //
    RealTime_Task();
    //
    DisplayTask();
    //
    UiTask();
  }
}


#ifdef USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {}
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


