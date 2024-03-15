#ifndef __TCP_H__
#define __TCP_H__
/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"
#include "psock.h"
#include "stm32f10x.h"	

//ͨ�ų���״̬��(�û������Լ�����)  
enum
{
	STATE_CMD		= 0,	//�������״̬ 
	STATE_TX_TEST	= 1,	//�����������ݰ�״̬(�ٶȲ���)  
	STATE_RX_TEST	= 2		//�����������ݰ�״̬(�ٶȲ���)  
};	 
//���� uip_tcp_appstate_t �������ͣ��û��������Ӧ�ó�����Ҫ�õ�
//��Ա��������Ҫ���Ľṹ�����͵����֣���Ϊ����������ᱻuip���á�
//uip.h �ж���� 	struct uip_conn  �ṹ���������� uip_tcp_appstate_t		  
struct tcp_appstate
{
	u8_t state;
	u8_t *textptr;
	int textlen;
};	 
typedef struct tcp_appstate uip_tcp_appstate_t;

void tcp_appcall(void);
void tcp_client_appcall(void);
void tcp_server_appcall(void);

//����Ӧ�ó���ص����� 
#ifndef UIP_APPCALL
#define UIP_APPCALL tcp_appcall //����ص�����Ϊ tcp_ppcall
#endif
/////////////////////////////////////TCP SERVER/////////////////////////////////////
extern u8 tcp_server_databuf[];   		//�������ݻ���	 
extern u8 tcp_server_sta;				//�����״̬   
//tcp server ����
void tcp_server_aborted(void);
void tcp_server_timedout(void);
void tcp_server_closed(void);
void tcp_server_connected(void);
void tcp_server_newdata(void);
void tcp_server_acked(void);
void tcp_server_senddata(void);
/////////////////////////////////////TCP CLIENT/////////////////////////////////////
extern u8 tcp_client_databuf[];   		//�������ݻ���	 
extern u8 tcp_client_sta;				//�ͻ���״̬   
void tcp_client_reconnect(void);
void tcp_client_connected(void);
void tcp_client_aborted(void);
void tcp_client_timedout(void);
void tcp_client_closed(void);
void tcp_client_acked(void);
void tcp_client_senddata(void);
////////////////////////////////////////////////////////////////////////////////////


#endif
























