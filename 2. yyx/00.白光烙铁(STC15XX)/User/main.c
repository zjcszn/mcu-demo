/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			�������ļ�
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#include "config.h"

void main(void)
{
	//ϵͳ��ʼ��
	Sys_Init();
	while(1)
	{
		//�����趨����
		ParaSet();
		//��ʾ������
		Sys_Monitor();
	}
}