/**
  ******************************************************************************
  * @file    sys_tick.c
  * @author  SZQVC
  * @version V1.0.0
  * @date    2015.2.14
  * @brief   �����ƻ�.��Х��Ŀ (QQ��49370295)
  *          system tick,��CPU���
  ******************************************************************************
  * @attention                                                                 *
  *                                                                            *
  * <h2><center>&copy; COPYRIGHT 2015 SZQVC</center></h2>                      *
  *                                                                            *
  * �ļ���Ȩ�顰����Ȩ�ɰ��ӿƼ����޹�˾��(���SZQVC�����С�                      *
  *                                                                            *
  *        http://www.szqvc.com                                                *
  *                                                                            *
  ******************************************************************************
**/
#include "stm32f10x.h"
#include "sys_tick.h"

/* define */
struct _SYS_TICK_TYPE
{
    uint32_t ms;
    uint32_t ten_ms;
    uint32_t Sec;
}systick;

#define us        12      //@72MHz


/* public */

/* extern */

/* private */


/*******************************************************************************
* Function Name  : SysTick_Init
* Description    : ϵͳ��ʱ��ʱ�ӳ�ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Init(void)
{
    systick.ms = 0;
    systick.Sec =0;
    SysTick_Config(SystemCoreClock/1000);    //1ms�ж�һ��
}

/*******************************************************************************
* Function Name  : SysTick_Ctrl
* Description    : ϵͳ��ʱ��ʱ��ENABLE/DISABLE
* Input          : ENABLE/DISABLE
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Ctrl(uint16_t cmd)
{
    if( cmd==ENABLE ){
        SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    }else if( cmd==DISABLE){
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    }
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : ϵͳ��ʱ���ж�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern void KeyTask(void);

void SysTick_Handler(void)
{
    systick.ms++;
    if(systick.ms%1000==0)
      systick.Sec++;
    /*��Ҫ�ڶ�ʱ���д�������� */
    KeyTask();
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : ��ȡms������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetSysTick_ms(void)
{
    return systick.ms;
}

void MarkSysTick_ms(uint32_t *t)
{
  *t = systick.ms;
}

/*******************************************************************************
* Function Name  : GetSysTick_Sec
* Description    : ��ȡsec������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint32_t GetSysTick_Sec(void)
{
    return systick.Sec;
}

void MarkSysTick_Sec(uint32_t *t)
{
  *t = systick.Sec;
}

/*******************************************************************************
* Function Name  : delay_nus
* Description    : ��ʱn us
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
void delay_us(uint32_t i)
{
  i = i*us;
	while(i--);
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : ��ʱn ms
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
void delay_ms(uint32_t i)
{
    uint32_t end_t = systick.ms+i;
    
    while( systick.ms<end_t );
}

/*******************************************************************************
* Function Name  : TimeOutCheck_Sec, TimeOutCheck_ms
* Description    : ��ʱn ms
* Input          : i
* Output         : None
* Return         : None
*******************************************************************************/
char TimeOutCheck_Sec(uint32_t i)
{
  if( systick.Sec>=i )
    return 1;
  else
    return 0;
}

char TimeOutCheck_ms(uint32_t i)
{
  if( systick.ms>=i )
    return 1;
  else
    return 0;
}

