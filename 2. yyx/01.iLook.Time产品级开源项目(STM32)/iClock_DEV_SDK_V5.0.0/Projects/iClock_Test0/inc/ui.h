/**
  ******************************************************************************
  * @file    ui.h
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   ��������,�û�����
  ******************************************************************************
**/


#ifndef UI_H
#define UI_H

#define TEMP_UNKNOW   -10000
#define PRESS_UNKNOW  -10000
#define ALTITUDE_UNKNOW  -10000

/*----------------------------------------------------------------------------------------*/
typedef enum _UI_TASK_STATE
{
    UI_TASK_NULL    = T_NULL,
    UI_PWR_ON       = T_PWR_ON,
    //
    UI_START_WIN,
    //
    UI_TIME,            //ʱ��
    UI_TIMER,           //��ʱ������
    UI_COUNTDOWN,       //����ʱ����
    UI_WEATHER,         //͹���¶�+��ѹ+����
    UI_SPORT,           //�˶�
    UI_HERE,            //������
    //
    UI_DEBUG,           //���Խ���
    UI_END_WIN,
    //
    UI_PWR_OFF      = T_PWR_OFF,
    UI_HW_ERR       = T_HW_ERR,
}UI_TASK_STATE;

typedef enum _SMART_LIFE_STATE{
  GPS_AUTO_ON         = 1,                //GPS�Զ�����
  GPS_AUTO_OFF        = 1<<1,             //GPS�Զ��ػ�
  DISPLAY_AUTO_ONOFF  = 1<<2,             //��ʾ���Զ�����
}SMART_LIFE_STATE;


typedef struct _UI_TSK_MSG_T{
  char              deep_sleep;           //0.�����˯��״̬ 1: �������˯��״̬
  unsigned char     resume_ui_state;      //��������ǰ��ui״̬
  SMART_LIFE_STATE  smart_life;
  char              flg_wkup;             //����
  //
  char              new_day;
  tm                smart_life_tm;
  uint32_t          gps_sleep_t_msk;      //gps˯��ʱ���
  uint32_t          sysclock_t_msk;       //ϵͳʵʱʱ��ͬ��ʱ���
  uint32_t          ui_reflash_tick;      //����ˢ��ʱ���
  //
  double            altitude;
  int16_t           altitude_dis;
}UI_TSK_MSG_T;

void UiTask_Init(void);
char UiTask(void);

/*----------------------------------------------------------------------------------------*/
void SmartLifeTask_Init(void);
void SmartLifeTask(void);
/*----------------------------------------------------------------------------------------*/
typedef struct{
  //�̶��ڸ�λ��#define FLASH_REC_ADDR  0x0801F800
  uint32_t id;      //��ʶ0x5aa5a55a
  //
}FL_REC_TYPE;

#endif

/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/


