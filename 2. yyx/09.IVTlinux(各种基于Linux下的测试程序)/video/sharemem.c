#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "sharemem.h"

#define FRAME_SHM_KEY		0x1234

typedef struct _frameShareMem_tag{
	int key;				//�����ڴ�key
	int shmID;				//�����ڴ�id
	void *shm_addr;			//�����ڴ��ַ
	int memSize;			//�ֿ��ڴ��С
	int memCount;			//����
}frameShareMem_t;
static frameShareMem_t gFrameShareMem;

/*�����ڴ��ʼ��*/
int shareMemInit(int key, int memSize, int memCount)
{
	int shmID = -1;

	if((!memSize)||(!memCount))
	{
		return -1;
	}
	shmID = shmget(key,(memSize*memCount),IPC_CREAT | 0666);
	if(shmID < 0 ){
		shmID = shmget(key, 0, 0);
	}
	
	if(shmID < 0 )
		return -1;
	return shmID;
}

/*�ӹ����ڴ���ָ����ƫ������д����*/
void shareMemWrite(int mid,int offset,void *buf,int length)
{
	char *pDst = shmat(mid,0,0);
	if(pDst == NULL)
		return;
	memcpy(pDst+offset,buf,length);
	shmdt(pDst);
}

/*�ӹ����ڴ���ָ����ƫ��������ȡ����*/
void ShareMemRead(int mid,int offset,void *buf,int length)
{
	char *pSrc = shmat(mid,0,0);
	if(pSrc == NULL)
		return;
	memcpy(buf,pSrc+offset,length);
	shmdt(pSrc);
}

int frameShareMemInit(size_t size,size_t count)
{
	int shmID = -1;
	
	shmID = shareMemInit(FRAME_SHM_KEY,size,count);
	if(shmID > 0)
	{
		gFrameShareMem.key 		= FRAME_SHM_KEY;
		gFrameShareMem.shmID 	= shmID;
		gFrameShareMem.memSize 	= size;
		gFrameShareMem.memCount = count;
		return shmID;
	}
	else
	{
		printf("frameShareMemInit fail\n");
		return -1;
	}
}

void frameShareMemWrite(int index, uint8_t *yuv_frame, size_t yuv_length)
{
	if(index < gFrameShareMem.memCount)
	{
		shareMemWrite(gFrameShareMem.shmID,index*gFrameShareMem.memSize,yuv_frame,yuv_length);
	}
}

