/*!
 *     COPYRIGHT NOTICE
 *     Copyright (c) 2017,BG0BGH
 *     All rights reserved.
 *
 *     ��ע�������⣬�����������ݰ�Ȩ������־�����У�δ����������������ҵ��;��
 *     �޸�����ʱ���뱣�����ߵİ�Ȩ������
 *
 * @file			�����ļ�
 * @brief			
 * @author		��־��
 * @version		V2.2
 * @date			2017-8-30
 */
#ifndef		__CONFIG_H
#define		__CONFIG_H


/************************����ϵͳʱ��*****************************/
//#define MAIN_Fosc		22118400L	//������ʱ��
//#define MAIN_Fosc		12000000L	//������ʱ��
//#define MAIN_Fosc		11059200L	//������ʱ��
//#define MAIN_Fosc		 5529600L	//������ʱ��
#define MAIN_Fosc		24000000L	//������ʱ��
#define Main_Fosc_KHZ	(MAIN_Fosc / 1000)

/****************************�ӿڶ���****************************/



/***************************�����⺯��***************************/
#include	"STC15Fxxxx.H"
#include 	"stdio.h"
#include 	"delay.h"
#include 	"GPIO.h"
#include 	"ADC.h"
#include 	"Exti.h"
#include 	"timer.h"
#include 	"PCA.h"
#include  "EEPROM.h"
//#include  "USART.h"

/***************************�����û�����***************************/
#include "Sys_Init.h"
#include "LQ12864.h"
#include "EC11.h"
#include "Control.h"
//#include "Kalman.h"

#endif
