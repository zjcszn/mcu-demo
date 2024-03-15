#ifndef __UIP_CONF_H__
#define __UIP_CONF_H__

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned short uip_stats_t;
typedef int uip_udp_appstate_t;

//���TCP������
#define UIP_CONF_MAX_CONNECTIONS 5

//���TCP�˿ڼ�����
#define UIP_CONF_MAX_LISTENPORTS 5

//uIP�����С
#define UIP_CONF_BUFFER_SIZE     1520

//CPU��С��ģʽ
//STM32��С��ģʽ��
#define UIP_CONF_BYTE_ORDER  UIP_LITTLE_ENDIAN

//��־����
#define UIP_CONF_LOGGING         0

//UDP֧�ֿ���
#define UIP_CONF_UDP             1

#define UIP_CONF_DHCP            0

//UDPУ��Ϳ���
#define UIP_CONF_UDP_CHECKSUMS   1	    


//uIPͳ�ƿ��� 	 
#define UIP_CONF_STATISTICS      0

											 
#include"tcp.h"

#if UIP_CONF_UDP
	#include"udp.h"

	#if UIP_CONF_DHCP
	#include"dhcpc.h"
	#endif
#endif

#endif 























