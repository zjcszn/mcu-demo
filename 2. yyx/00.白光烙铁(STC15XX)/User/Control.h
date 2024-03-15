/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			Control.h
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef	_CONTROL_H_
#define	_CONTROL_H_

/*��ֵ�˲�������������*/
#define NUM 34 //��������(�����Ƕ��ı�����+2)
#define DIV 5 //���ƴ������ �൱�� ����( z = y/(2^div) )

extern unsigned int IronTemp_Now;
extern unsigned int IronTemp_AM;
extern unsigned int chn;
extern unsigned char SetTempFlag;
extern unsigned char FtyModeFlag;

extern void IronTempControl(unsigned int temp);
extern void Sys_Monitor(void);


#endif