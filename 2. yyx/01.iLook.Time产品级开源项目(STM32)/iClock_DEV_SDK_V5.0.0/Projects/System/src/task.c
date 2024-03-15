/**
  ******************************************************************************
  * @file    task.c
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   �����ƻ�.��Х��Ŀ (QQ��49370295)
  *          QSTǰ��̨���̹���ϵͳ
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
#include "stm32f10x.h"
#include "sys_tick.h"
#include "task.h"

/* define */
struct _QST_STATE{
  uint32_t mloop_per_sec;
}Qst;

/* public */

/* extern */

/* private */
static unsigned long mon_task_tick,mon_task_loop_cnt;


/*******************************************************************************
* Function Name  : TaskCtrl
* Description    : ����״̬�л�
* Input          : - tsk: ����ṹָ��
*                  - ctrl: �л���ʲô״̬
* Output         : None
* Return         : None
*******************************************************************************/
void QstCtrl(TASK_CTRL_INFO *tsk, unsigned char ctrl)
{
  tsk->Ctrl = ctrl|TASK_CMD;
  tsk->TickMsk = GetSysTick_ms();  //��¼�����״̬��ʱ���ǩ
  //����ִ��һ������
  if( tsk->Process != 0x0 )
    tsk->Process();
}


/*******************************************************************************
* Function Name  : TaskEnter
* Description    : ����״̬��ת
* Input          : - tsk: ����ṹָ��
*                  - st: ����ֱ�ӽ��뵽ʲô״̬
* Output         : None
* Return         : None
*******************************************************************************/
void QstEnter( TASK_CTRL_INFO *tsk, unsigned char st )
{
  tsk->MsgFlg = TASK_MSG_ST_CHANGE;      //�����µ�״̬����ϢӦ�ñ�����  
  tsk->State = st;                       //�趨״̬
  tsk->TickMsk = GetSysTick_ms();        //��¼�����״̬��ʱ���ǩ
}

/*******************************************************************************
* Function Name  : QstRestTaskTick
* Description    : ��λ���������
* Input          : - tsk: ����ṹָ��
* Output         : None
* Return         : None
*******************************************************************************/
void QstRestTskTick( TASK_CTRL_INFO *tsk )
{
  tsk->TickMsk = GetSysTick_ms();        //��¼�����״̬��ʱ���ǩ
}


/*******************************************************************************
* Function Name  : QstGetCmd
* Description    : ��ȡ�����������
* Input          : - tsk: ����ṹָ��
* Output         : T_NULL or Command
* Return         : None
*******************************************************************************/
unsigned char QstGetCmd( TASK_CTRL_INFO *tsk )
{
  if( tsk->Ctrl&TASK_CMD ){
    tsk->Ctrl &= ~TASK_CMD;
    return tsk->Ctrl;
  }else
    return T_NULL;
}

/*******************************************************************************
* Function Name  : QstGetState
* Description    : ��ȡ����״̬
* Input          : - tsk: ����ṹָ��
* Output         : Task state
* Return         : None
*******************************************************************************/
unsigned char QstGetState( TASK_CTRL_INFO *tsk )
{
  return tsk->State;
}

/*******************************************************************************
* Function Name  : QstGetMsg
* Description    : ��ȡ������Ϣָ��
* Input          : - tsk: ����ṹָ��
* Output         : ���������Ϣָ��
* Return         : None
*******************************************************************************/
unsigned char *QstGetMsg( TASK_CTRL_INFO *tsk )
{
    return tsk->Msg;
}

/*******************************************************************************
* Function Name  : QstGetMsgState
* Description    : ��ȡ������Ϣ��־
* Input          : - tsk: ����ṹָ��
* Output         : 0 û����Ϣ
* Return         : None
*******************************************************************************/
unsigned char QstGetMsgState( TASK_CTRL_INFO *tsk )
{
  return tsk->MsgFlg;
}

/*******************************************************************************
* Function Name  : QstMsgClr
* Description    : ���������Ϣ
* Input          : - tsk: ����ṹָ��
* Output         : None
* Return         : None
*******************************************************************************/
void QstMsgClr( TASK_CTRL_INFO *tsk )
{
  tsk->MsgFlg = TASK_MSG_NULL;
}

/*******************************************************************************
* Function Name  : TaskMonitor
* Description    : ��ѭ��ÿ��ִ�д�����������Ϣ��������.
* Input          : - 
* Output         : None
* Return         : None
*******************************************************************************/
void QstMonitor_Init(void)
{
  mon_task_tick = 0;
}

void QstMonitor(void)
{
  //��ѭ���ٶ�
  if( GetSysTick_ms()>mon_task_tick+1000 ){
    mon_task_tick = GetSysTick_ms();
    Qst.mloop_per_sec = mon_task_loop_cnt;
    mon_task_loop_cnt = 0;
  }else{
    mon_task_loop_cnt++;
  }
  //
}

/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/
