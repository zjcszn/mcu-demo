/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			ϵͳ��ʼ��
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef  __SYS_INIT_H_
#define  __SYS_INIT_H_

#include "Sys_Init.h"
/************************����ȫ�ֱ���*****************************/
#define	BEEP 				(P03)
#define BEEP_ON			(P03 = 0)
#define BEEP_OFF		(P03 = 1)

#define	EC11_A			(P32)
#define	EC11_B			(P31)
#define	EC11_KEY		(P30)

/************************�����ⲿ����*****************************/
void Sys_Init(void);
void Iron_Off(void);
void Iron_Run(void);

#endif