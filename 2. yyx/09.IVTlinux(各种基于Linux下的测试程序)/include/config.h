#ifndef _CONF_H_
#define _CONF_H_

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#include <stdio.h>
#else

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#endif

#include "glabtypes.h"
#include "iniConfigFile.h"

#define CONLINELENGTH 1024 	//�����ļ�һ�еĳ������ֵ

/////////////////////////////////////////////////////////////////////////////////////
#define NONE                 ""
#define BOLD                 ""

#define VERSION_INFO  BOLD"\n [IVT V1.0]  "NONE  __DATE__ " (" __TIME__ ")" "\n\n"

/* the maximum number of ports we are willing to open */
#define MAX_PORTS 			5

#define DEBUG_MODE			0

/*��������֡����*/
#define MAX_SERIAL_LEN		(64*1024)
#define MAX_BUF_SIZE 		(10240)

/*this array hold information about each port we have opened */
struct PortInfo{
	int busy;
	char name[32];
	int handle;
	char buf[MAX_BUF_SIZE];
	int buflen;
	pthread_mutex_t lock;
};

//�����豸����
typedef struct _devName_tag
{
	char string[16];
}devName_t;

typedef struct _tag_Chinese_name {
    int index;
    char chineseName[64];
}PACKED chinese_name_t;

typedef struct _chinese_name_t{
	char name[64];
}Chinesename_t;

/* ������� */
typedef struct {
    uint32_t EthernetIP;                    /* ��̫��IP */
    uint32_t IPMask;                        /* �������� */
    uint32_t GatewayIP;                     /* Ĭ������ */
    uint32_t DNSIP;                         /* DNS IP */
	uint32_t ServerIP;                      /* ������ IP */
    uint16_t ServerPort;                    /* �������˿� */
    uint16_t HttpPort;                      /* HTTP�˿� */
    uint8_t  DHCPEnable;                    /* ����DHCP 0 �ر�; 1 �� */
    uint8_t  UPNPEnable;                    /* ����UPNP 0 �ر�; 1 �� */
    uint8_t  MacAddr[6];                    /* MAC��ַ */
    uint8_t  Reserve[8];					/* ����*/
} NETCONFIG, *PNETCONFIG;

/*����ͷ����*/
typedef struct _cameraPara_tag
{
	int 		width;//��Ƶ���
	int 		hight;//��Ƶ�߶�
}cameraPara_t;

/* ��Ƶ���� */
typedef struct _videoConfig_tag{
	
	cameraPara_t cameraPara;
	
} videoConfig_t;


/*this array hold information about each port we have opened */
struct PortInfo ports[MAX_PORTS];//��¼5�������豸�Ľṹ��

//char *ReadConfigfile(char *fileName, char *item,char * mydefault,char *dest);
//void get_key_string(char *section,char * file,char* key,char* dest,char *def);
//int WriteConfigfile(char *fileName, char *item,char *value);
void initMachineConf();
void loadConfig();


#endif


