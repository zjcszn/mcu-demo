/********************************************************************************
 *      Copyright:  (C) 2017 huiersen
 *                  All rights reserved.
 *
 *       Filename:  gps.h
 *    Description:  This head file
 *
 *        Version:  1.0.0(2017年06月19日)
 *         Author:  pengyu
 *      ChangeLog:  1, Release initial version on "2017年06月19日 12时25分59秒"
 *
 ********************************************************************************/

#ifndef __GPS_H__
#define __GPS_H__

typedef unsigned int UINT;
typedef int BYTE;

typedef struct __gprmc__
{
   UINT time;			/* gps��λʱ�� */
   char pos_state;		/*gps״̬λ*/
   char	ns;				/*γ�Ȱ���N(������)��S(�ϰ���)*/
   char	ew;				/*���Ȱ���E(����)��W(����)*/
   float latitude;		/*γ�� */ 
   float longitude;		/* ���� */  
   float speed;  		/* �ٶ� */ 
   float direction;		/*���� */  
   UINT date;  			/*����  */  
   float declination; 	/* ��ƫ�� */  
   char dd;
   char mode;			/* GPSģʽλ */ 
}GPRMC;

int initGpsUart();
int gpsModuleRun();

#endif
