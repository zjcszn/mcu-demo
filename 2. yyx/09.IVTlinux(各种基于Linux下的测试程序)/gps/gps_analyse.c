
/*********************************************************************************
 *      Copyright:  (C) 2017 zoulei
 *                  All rights reserved.
 *
 *       Filename:  gps_analyse.c
 *    Description:  This file
 *
 *        Version:  1.0.0(2017��06��19��)
 *         Author:  zoulei <zoulei121@gmail.com>
 *      ChangeLog:  1, Release initial version on "2017��06��19�� 12ʱ16��39��"
 *
 ********************************************************************************/
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "gps_analyse.h"

typedef struct _gpsName_tag
{
	char string[8];
}gpsName_t;

typedef enum nmeaPACKTYPE
{
	GPGGA,
	GPGLL,
	GPGSA,
	GPGSV,
	CPRMC,
	GPVTG
}nmea_t;

gpsName_t gGpsName[] = {{"$GPGGA"},{"$GPGLL"},{"$GPGSA"},{"$GPGSV"},{"$GPRMC"},{"$GPVTG"}}; 

int find_str(const char* string, const char* match_str)
{
	if(strstr(string,match_str) != NULL)
	{
		return 1;
	}
	return 0;
}

int gps_analyse (char *buff,GPRMC *gps_data)
{
    char *ptr=NULL;
	
	if(gps_data==NULL)
	{
		return -1;
	}
	
	if(strlen(buff)<10)
	{
		if(!strlen(buff))
		{
			return 0;
		}
		else
		{
			return -1;
		}	
	}
	//printf("buff:%s\n",buff);
	/* ���buff�ַ����а����ַ�"$GPRMC"��$GPRMC�ĵ�ַ��ֵ��ptr */
	if(!find_str(buff,"ANTENNA OK*"))
	{
		return -1;
	}
	if(NULL==(ptr=strstr(buff,"$GNRMC")))
	{
		return -1;
	}
	/* sscanf����Ϊ���ַ������룬��˼�ǽ�ptr�ڴ浥Ԫ��ֵ��Ϊ����ֱ����뵽����Ľṹ���Ա */
	if(10==sscanf(ptr,"$GNRMC,%d.000,%c,%f,%c,%f,%c,%f,%f,%d,,,%c*",&(gps_data->time),&(gps_data->pos_state),&(gps_data->latitude),&(gps_data->ns),&(gps_data->longitude),&(gps_data->ew),&(gps_data->speed),&(gps_data->direction),&(gps_data->date),&(gps_data->mode)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int print_gps (GPRMC *gps_data)
{
    printf("===========================================================\n");
    printf("==                   ȫ��GPS��λ����ģ��                 ==\n");
    printf("===========================================================\n");
    printf("                                                           \n");
    printf("===========================================================\n");
    printf("==   GPS state bit : %c  [A:��Ч״̬ V:��Ч״̬]              \n",gps_data->pos_state);
    printf("==   GPS mode  bit : %c  [A:������λ D:��ֶ�λ]               \n", gps_data->mode);
    printf("==   Date : 20%02d-%02d-%02d                                  \n",gps_data->date%100,(gps_data->date%10000)/100,gps_data->date/10000);
    printf("==   Time : %02d:%02d:%02d                                   \n",(gps_data->time/10000+8)%24,(gps_data->time%10000)/100,gps_data->time%100);
    printf("==   γ�� : %c:%d��%d��%5.3f��",gps_data->ns ,((int)gps_data->latitude) / 100, (int)(gps_data->latitude - ((int)gps_data->latitude / 100 * 100)), (float)(((gps_data->latitude - ((int)gps_data->latitude / 100 * 100)) - ((int)gps_data->latitude - ((int)gps_data->latitude / 100 * 100))) * 60.0));
    printf("���� : %c:%d��%d��%5.3f��\n",gps_data->ew,((int)gps_data->longitude) / 100, (int)(gps_data->longitude - ((int)gps_data->longitude / 100 * 100)), (float)(((gps_data->longitude - ((int)gps_data->longitude / 100 * 100)) - ((int)gps_data->longitude - ((int)gps_data->longitude / 100 * 100))) * 60.0));
    printf("==   �ٶ� : %.3f  m/s                                      \n",gps_data->speed);
    printf("==                                                       \n");
    printf("============================================================\n");

    return 0;
}
