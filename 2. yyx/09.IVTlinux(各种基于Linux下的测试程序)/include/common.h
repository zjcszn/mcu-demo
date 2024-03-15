
#ifndef _COMMON_H
#define _COMMON_H
#include "glabtypes.h"
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define BIT0                    (1 << 0)
#define BIT1                    (1 << 1)
#define BIT2                    (1 << 2)
#define BIT3                    (1 << 3)
#define BIT4                    (1 << 4)
#define BIT5                    (1 << 5)
#define BIT6                    (1 << 6)
#define BIT7                    (1 << 7)
#define BIT8                    (1 << 8)
#define BIT9                    (1 << 9)
#define BIT10                   (1 << 10)
#define BIT11                   (1 << 11)
#define BIT12                   (1 << 12)
#define BIT13                   (1 << 13)
#define BIT14                   (1 << 14)
#define BIT15                   (1 << 15)
#define BIT16                   (1 << 16)
#define BIT17                   (1 << 17)
#define BIT18                   (1 << 18)
#define BIT19                   (1 << 19)
#define BIT20                   (1 << 20)
#define BIT21                   (1 << 21)
#define BIT22                   (1 << 22)
#define BIT23                   (1 << 23)
#define BIT24                   (1 << 24)
#define BIT25                   (1 << 25)
#define BIT26                   (1 << 26)
#define BIT27                   (1 << 27)
#define BIT28                   (1 << 28)
#define BIT29                   (1 << 29)
#define BIT30                   (1 << 30)
#define BIT31                   (1 << 31)

#define MAX_SHORT               0x7FFF
#define MAX_USHORT              0xFFFF

#define MAX_LONG                0x7FFFFFFF
#define MAX_ULONG               0xFFFFFFFF

#define GET_BIT(x, n)           ( (x) & (1 << (n)) )
#define SET_BIT(x, n)           ( (x) |= (1 << (n)) )
#define XOR_BIT(x, n)           ( (x) ^= (1 << (n)) )
#define CLR_BIT(x, n)           ( (x) &= ~(1 << (n)) )

/* ����ģʽ */
#define ATTRIB_NOTIFY           BIT0
#define GET_MODULE(x)           ((uint16_t) (((uint32_t) (x)) >> 16))
#define GET_ATTRIB(x)           ((uint8_t) (((uint32_t) (x)) >> 8))
#define CMD_IS_NOTIFY(x)        (GET_ATTRIB(x) & ATTRIB_NOTIFY)
#define GENCOMMAND(m, t, n)     ((uint32_t) (((uint32_t) ((uint16_t)(m))<<16) | (uint16_t) (((uint8_t)(t))<<8 | (uint8_t)(n))))

/* ��������*/
#define OPERATE_READ            0x0001          /* ������ */
#define OPERATE_WRITE           0x0002          /* д���� */
#define OPERATE_START           0x0004          /* ����(��) */
#define OPERATE_STOP            0x0008          /* ֹͣ(�ر�) */
#define OPERATE_NOTIFY          0x0010          /* ֪ͨ */
#define RETURN_READ             0x8001          /* ������ */
#define RETURN_WRITE            0x8002          /* д���� */
#define RETURN_START            0x8004          /* ����(��)���� */
#define RETURN_STOP             0x8008          /* ֹͣ(�ر�)���� */

/* ��ģ������*/
//ģ��ע��
#define MODULE_REGISTER						GENCOMMAND(MAIN_MANAGE_MODULE, 0, 0x00)
//����������ʼ
#define NET_SYS_UPDATE_START				GENCOMMAND(MAIN_MANAGE_MODULE, 0, 0x01)
//������������
#define NET_SYS_UPDATE_STOP					GENCOMMAND(MAIN_MANAGE_MODULE, 0, 0x02)

/* GPSģ������*/
//GPS�����ϱ�
#define GPS_DATA_UPDATA						GENCOMMAND(GPS_MODULE, 0, 0x00)
//GPSû�ҵ�
#define GPS_NOFIND							GENCOMMAND(GPS_MODULE, 0, 0x01)
//GPS��ȡ���ݳ���
#define GPS_GET_DATA_ERROR					GENCOMMAND(GPS_MODULE, 0, 0x02)

typedef enum _module_tag
{
	MAIN_MANAGE_MODULE = 1,	/* ��ģ�� */
	COMMUNI_MANAGE_MODULE,	/* ��������ģ�� */
	GPS_MODULE,				/* GPSģ�� */
	NET_TRANS_MODULE,		/* ���紫��ģ�� */
	VOICE_MODULE,			/* ����ģ��*/
	VIDEO_MODULE,			/* ��Ƶģ�� */
	RECORD_MANAGE_MODULE,	/* ¼�����ģ�� */	
	LOCAL_DISPLAY_MODULE,	/* ����Ԥ���ط�ģ�� */
	SIGN_MODULE,			/* ǩ��ģ�� */
	CAN_MODULE,				/* CANģ�� */
}module_t;

/* ����ͷ */
typedef struct {
    uint32_t cmdTag;                            /* �������� */
    uint16_t cmdChannel;                        /* ����ͨ������Ҫ��Ϊ��������Ƶ���� */
    uint16_t cmdOperate;                        /* �������� OPERATE_READ ~ RETURN_STOP */
    uint32_t cbSize;                            /* ���ݴ�С */
} CMDHEADER, *PCMDHEADER;

typedef struct{
	uint16_t	srcModule;		/*��Ϣ�ķ���ģ����*/
	uint16_t	tagModule;		/*��Ϣ��Ŀ��ģ����*/
}ipc_msg_t;

typedef struct
{
	ipc_msg_t msg_header;
	CMDHEADER cmd_header;
} module_msg_t;

#define MSG_CMD_SZ				(sizeof(ipc_msg_t)+sizeof(CMDHEADER))

#define SERVER_FILE 			"/tmp/server"	/* server's well-known name */
#define CLI_MAIN				"/tmp/main"	
#define CLI_COMMUNI 			"/tmp/cli_communi"
#define CLI_GPS 				"/tmp/gps"
#define CLI_NET_TRANS 			"/tmp/net_trans"
#define CLI_VOICE 				"/tmp/voice"
#define CLI_VIDEO 				"/tmp/video"
#define CLI_RECORD				"/tmp/record"
#define CLI_LOCAL_DISPLAY 		"/tmp/local_display"
#define	CLI_SIGN   				"/tmp/sign"
#define CLI_CAN					"/tmp/can"

#define MAX_MODULE_CONN_NUM 	16
#define LOCAL_BUFSIZE	 		1024

typedef struct {	/* one Client struct per connected client */
  int		fd;			/* fd, or -1 if available .server fd*/
  int		clifd;		/*client fd.*/	
  int 		mod_num;
} localSock;
localSock gModSocket[MAX_MODULE_CONN_NUM];


#endif
