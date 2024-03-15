/**
  ******************************************************************************
  * @file    ui.c
  * @author  SZQVC
  * @version V0.0.1
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
#include "task.h"
#include "sys_includes.h"
#include "ui.h"

/* define */

/* public */
char gPowerOnFlg;

/* extern */
extern KEY_INFO gKey;

/* private */
uint32_t last_rt_tick;
uint32_t timer_rt_tick;
//
void win_debug_init(void);
char win_debug_process(void);
//
void win_time_init(void);
char win_time_process(void);
//
void win_timer_init(void);
char win_timer_process(void);
//
void display_system_bar(int x, int y, int type);
//
static char tmp_str[150];     //110 �����ַ�������

//������Ϣ���ýӿ�
POWER_TSK_MSG_T    *pwr;
DISPLAY_TSK_MSG_T  *displayer;

//
tm *now;      //��ǰʱ��ָ��



/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                UI TASK                                     */
/*                             ϵͳ�������                                  */
/*                                                                            */
/* ����һ�������������Ҫ����                                                   */
/* 1.����һ�����������ṹ,���������ʼ��ʱ�趨��������:TASK_CTRL_INFO UiTskInfo   */
/* 2.���������ʼ����������: void UiTask_Init(void)                             */
/* 3.�����������庯�����磺char UiTask(void)                                   */
/*----------------------------------------------------------------------------*/
TASK_CTRL_INFO UiTskInfo;   //ui������ƽṹ
UI_TSK_MSG_T UiTskMsg;      //ui������Ϣ�ṹ
KEY_INFO ui_key;

void UiTask_Init(void)
{
    //��ʼ��������Ϣ
    UiTskInfo.TickGap = 0;        //UI���������ٶȣ�=0 �������ٶȿ���
    UiTskInfo.TickMsk = 0;        //UI��������ʱ���
    UiTskInfo.Msg = 0;            //UI��Ϣ�����ָ�룬=0 Ϊ����Ϣ����ṹ
    UiTskInfo.MsgFlg = 0;         //UI��Ϣ�����־λ
    UiTskInfo.Process = UiTask;   //UIע����������������ָ��
    //������ģ��ӿ�ָ���ʼ��
  	pwr=(POWER_TSK_MSG_T *)QstGetMsg(&PowerTskInfo);              //��ȡ��Դ����������Ϣָ��
  	displayer = (DISPLAY_TSK_MSG_T*)QstGetMsg(&DisplayTskInfo);   //��ȡ��ʾ��������Ϣָ��
  	//
  	now = sys_time(0x0);          //��ȡϵͳʱ��ָ��
  	//
    QstCtrl(&UiTskInfo,UI_TIME);                //UI������ƣ�����ʵʱʱ�����
}

//��һ����������������
int next_win(void)
{
  int next_state = UiTskInfo.State;
  char flg=0;
  
  while(1){
    next_state++;
    switch( next_state ){
      case UI_DEBUG:          //���Խ���
      case UI_TIME:           //ʱ�����
      case UI_TIMER:          //��ʱ������
        flg = 1;
        break;

      case UI_COUNTDOWN:       //����ʱ����        
      case UI_WEATHER:         //��������
        flg = 0;
        break;
      
      case UI_END_WIN:
      default:
        next_state = UI_START_WIN+1;
        flg = 1;
        break;
    }
    if( flg==1 )
      break;
  }
  //
  return next_state;
}


char UiTask(void)
{
  //��ȡ��������(������������ж����ȿ����Ƿ��п�����Ϣ����)
  char tsk_cmd = QstGetCmd(&UiTskInfo);

  //��������
  if( (QstGetState(&DisplayTskInfo)==DISPLAY_WORKING)&&(gKey.type!=KEY_NULL) ){
    memcpy(&ui_key,&gKey,sizeof(KEY_INFO));
    ClrKey();
  }else{
    ui_key.type = 0;
    ui_key.value = 0;
  }

  //��������(UiTask��������Ҫ���û�������ת)
  if( tsk_cmd != T_NULL ){
    switch(UiTskInfo.Ctrl){
      case UI_TIME:
        win_time_init();
        QstEnter(&UiTskInfo,UI_TIME);
        break;
                    
      case UI_DEBUG:
        win_debug_init();
        QstEnter(&UiTskInfo,UI_DEBUG);
        break;
              
      case UI_TIMER:
        win_timer_init();
        QstEnter(&UiTskInfo,UI_TIMER);
        break;
      
      default:
        break;
    }
  }
  
  //�������(��������������״̬���ṹ����Ļ�ر�״̬������
  if( QstGetState(&DisplayTskInfo)==DISPLAY_WORKING ){
    switch(UiTskInfo.State){
      case UI_DEBUG:
        if( win_debug_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
          
      case UI_TIME:
        if( win_time_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
              
      case UI_TIMER:
        if( win_timer_process()==1 )
          QstCtrl(&UiTskInfo,next_win());
        break;
    }
  }
  
  return 1;
}


//------------------------------------------------------------------------------------//
//                                  debug����                                          //
//------------------------------------------------------------------------------------//
void win_debug_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  
  UiTskMsg.ui_reflash_tick = 0;
}

char win_debug_process(void)
{
  if((ui_key.value==1)&&(ui_key.type==KEY_CLICK)){
    return 1;
  }
  
  if( TimeOutCheck_ms(UiTskMsg.ui_reflash_tick+100) ){
    UiTskMsg.ui_reflash_tick = GetSysTick_ms();

  }

  return 0;
}

//------------------------------------------------------------------------------------//
//                                  ��ʱ����ʾ����                                      //
//------------------------------------------------------------------------------------//
void win_timer_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  //ϵͳ�˵���ʼ��
  display_system_bar(84,24,0);
  //
  UiTskMsg.ui_reflash_tick = 0;
  last_rt_tick = 0;
}

char win_timer_process(void)
{
  char rslt=0;
  uint32_t counter;
  
  //��������
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_CLICK) ){
    return 1;
  }
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_DB_CLICK) ){
    timer_rt_tick = now->tick;
  }
 
  //ʱ����ʾ
  if( now->tick != last_rt_tick ){
    //��ʱ��
    counter = now->tick-timer_rt_tick;
    if( (counter>0)&&(counter<59999) ){
      sprintf(tmp_str,"%03d:%02d",counter/60,counter%60);
      Glph_Print(0,0,LED_14X32,tmp_str);
    }else{
      Glph_Print(0,0,LED_14X32,"000:00");
	}
    //��ǰʱ��
    sprintf(tmp_str,"%02d:%02d:%02d",now->hour,now->min,now->sec);
    if( strlen(tmp_str)==8 ){
      Glph_Print(88,0,LED_5X7,tmp_str);
    }
    //
    display_system_bar(84,24,0);
  //
    last_rt_tick = now->tick;
  }

  return rslt;
}

//------------------------------------------------------------------------------------//
//                                  ʱ����ʾ����                                      //
//------------------------------------------------------------------------------------//
void display_week(int pos_x, int pos_y);


void win_time_init(void)
{
  QstCtrl( &DisplayTskInfo,DISPLAY_PWR_ON);
  //ϵͳ�˵���ʼ��
  display_system_bar(84,24,0);
  //
  UiTskMsg.ui_reflash_tick = 0;
  last_rt_tick = 0;
}

char win_time_process(void)
{
  char rslt=0;
//  char disp_str[20];
  
  //��������
  if( (ui_key.value==MODE_KEY)&&(ui_key.type==KEY_CLICK) ){
    return 1;
  }
 
  //ʱ����ʾ
  if( now->tick != last_rt_tick ){
    //ʱ��
    sprintf(tmp_str,"%02d:%02d",now->hour,now->min);
    if( strlen(tmp_str)==5 ){
      Glph_Print(0,0,LED_14X32,tmp_str);
    }
    sprintf(tmp_str,"%02d",now->sec);
    if( strlen(tmp_str)==2){
      Glph_Print(72,21,LED_5X7,tmp_str);
    }
    //����
    sprintf(tmp_str,"%02d/%02d",now->month,now->mday);
    if( strlen(tmp_str)==5){
      Glph_Print(88,0,LED_8X16,tmp_str);
    }
    //ϵͳ״̬
    display_system_bar(84,24,0);
    //
    display_week(88,16);
    //
    last_rt_tick = now->tick;
  }

  return rslt;
}

void init_system_bar(int x, int y, int type)
{
  DrawBitmap(103+4*5,16,BIT_MAP_VERT,5,8,(uint8_t*)ICO_V_m_5X8);  //m����
  DrawBitmap(83+3*5,16,BIT_MAP_VERT,5,8,(uint8_t*)ICO_V_c_5X8);   //���϶ȷ���
}

void display_system_bar(int x, int y, int type)
{
//  char tmp_str[10];
  int x1=x+4;

  //��Դ״̬
  if( pwr->PlugIn ){
    if( pwr->BatteryVol>=4.13 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_4);
    else
      Glph_DrawBattery(x1,y,2,ICO_BAT_CHARGE);
  }else{
    if( pwr->BatteryVol>3.95 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_4);
    else if( pwr->BatteryVol>3.8 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_3);
    else if( pwr->BatteryVol>3.65 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_2);
    else if( pwr->BatteryVol>3.5 )
      Glph_DrawBattery(x1,y,2,ICO_BAT_1);
    else
      Glph_DrawBattery(x1,y,2,ICO_BAT_0);
  }
}

void display_week(int pos_x, int pos_y)
{
  Glph_DrawRectangle(72,3,79,13,0,1);
  Glph_Print(74,4,LED_5X7,(char*)Int2DecStr(now->wday));
}



/*----------------------------------------------------------------------------------------*/
/*                                                                                        */
/*                                ARM Flash������                                         */
/*                                                                                        */
/* ��FLASH_REC_ADDR��ַ��ʼ��0x801F800)����С2K��                                          */
/* �洢FL_REC_TYPE�Ľṹ����                                                               */
/*----------------------------------------------------------------------------------------*/
#define FLASH_REC_ADDR  0x0801F800

char read_fl_rec_info(FL_REC_TYPE *out)
{
  int i;
  uint8_t *fl_p = (uint8_t *)FLASH_REC_ADDR;
  uint8_t *out_p = (uint8_t *)out;

  for( i=0; i<sizeof(FL_REC_TYPE); i++ ){
    *out_p++ = *fl_p++;
  }
  return 1;
}


char write_fl_rec_info(FL_REC_TYPE *in )
{
  int i,struct_size;
  uint32_t fl_p = (uint32_t)FLASH_REC_ADDR;
  uint32_t *in_p = (uint32_t *)in;
  
  //
  FLASH_Unlock();
  FLASH_ErasePage( FLASH_REC_ADDR );        /* ����ҳ */
  //
  struct_size = sizeof(FL_REC_TYPE);
  if(struct_size%4)
    struct_size = struct_size/4+1;
  else
    struct_size = struct_size/4;
  //
  for( i=0; i<struct_size; i++ ){
    FLASH_ProgramWord( fl_p,*in_p++);
    fl_p += 4;
  }
  
  return 1;
}


/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/

