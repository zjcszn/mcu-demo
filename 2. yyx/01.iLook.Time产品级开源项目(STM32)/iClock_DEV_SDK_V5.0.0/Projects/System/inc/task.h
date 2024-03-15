/**
  ******************************************************************************
  * @file    task.h
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   QSTǰ��̨���̹���ϵͳ
  ******************************************************************************
**/
#ifndef __TASK_H

#define __TASK_H


/*---------------------------------------------------------------------*/
/* QST���̹���ϵͳ���� */
#define TASK_CMD            0x80      //�����־
#define TASK_MSG_NULL       0x00      //����Ϣ
#define TASK_MSG_ST_CHANGE  0x80      //״̬�л���Ϣ

typedef struct _TASK_CTRL_INFO{
  unsigned char Ctrl;       //�����������룬��8λ������1��TASK_CMD|New State
  unsigned char State;      //����ǰ״̬
  unsigned long TickMsk;    //����ʱ���
  unsigned long TickGap;    //����ʱ����
  unsigned int  MsgFlg;     //��������Ϣ��־λ
  unsigned char *Msg;       //������Ϣָ��
  char (*Process)(void);         //������ָ��
}TASK_CTRL_INFO;

//������Ϣ����
void QstMsgClr( TASK_CTRL_INFO *tsk );
unsigned char QstGetMsgState( TASK_CTRL_INFO *tsk );
unsigned char *QstGetMsg( TASK_CTRL_INFO *tsk );

//����״̬������
void QstCtrl( TASK_CTRL_INFO *tsk, unsigned char ctrl );

//����״̬����ת
void QstEnter( TASK_CTRL_INFO *tsk, unsigned char st );

//��ȡ�ⲿ��������
unsigned char QstGetCmd( TASK_CTRL_INFO *tsk );

//��ȡ����״̬
unsigned char QstGetState( TASK_CTRL_INFO *tsk );

//��λ�����ʱ��
void QstRestTskTick( TASK_CTRL_INFO *tsk );

//QST���ؽ���
void QstMonitor_Init(void);
void QstMonitor(void);

//������״̬����
#define T_NULL      0x00    //��״̬
#define T_PWR_ON    0x01    //�����״̬
#define T_PWR_OFF   0x70    //����ر�״̬
#define T_HW_ERR    0x71    //�������Ӳ������״̬


/*---------------------------------------------------------------------*/
/* ��Ŀ���漰��TASK����ȫ���ŵ����� */
extern TASK_CTRL_INFO UiTskInfo;        //ϵͳ��������

extern TASK_CTRL_INFO CompassTskInfo;   //ָ������������

extern TASK_CTRL_INFO DisplayTskInfo;   //��ʾ��������

extern TASK_CTRL_INFO gSensorTskInfo;   //���ٶȴ�������������

extern TASK_CTRL_INFO GpsTskInfo;       //GPS��������

extern TASK_CTRL_INFO PowerTskInfo;     //��Դ��������

extern TASK_CTRL_INFO BaroTskInfo;      //��ѹ����������
/*---------------------------------------------------------------------*/

#endif


/*************************** (C) COPYRIGHT SZQVC ******************************/
/*                              END OF FILE                                   */
/******************************************************************************/


