#ifndef __HOSTCOMDATAHANDLE_H
#define	__HOSTCOMDATAHANDLE_H

#include <string.h>
#include <stdio.h>
#include "sys.h"
#include "hostcomprotocol.h"


#define		TEMP_COM_PACK_SIZE			10
#define		TEMP_COM_BUFFER_SIZE		1300

typedef struct ComDataQueue
{
	u8 	TempComDataBuffer[TEMP_COM_BUFFER_SIZE];//��ʱ�������ݻ�����
	u8 	QueueNowPoint;							//��������ָ��
	u8 	QueueCount;								//�������м���
	u8  SharePara;								//�������
	u16	EveryPackSize[TEMP_COM_PACK_SIZE];		//��¼ÿһ���������ݵĴ�С
	u16 EnterOffsetAddr;						//������ƫ�Ƶ�ַ
	u16 OutOffsetAddr;							//������ƫ�Ƶ�ַ
}_ComDataQueue;

extern _ComDataQueue com_data_queue;

void 	EnterComDataQueue(u8 *SourceBuf,_ComDataQueue *_com_data_queue,u16 DataLen);
void	OutComDataQueue(_ComDataQueue *_com_data_queue,u8 *DestBuf);

#endif