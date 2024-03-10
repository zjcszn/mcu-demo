/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : app_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*              2. ������һ��TCP Server������ʹ���˾���������NetBIOS���û�ֻ���ڵ��Զ�ping armfly
*                 �Ϳ��Ի�ð��ӵ�IP��ַ���˿ں�1001��
*              3. �û������ڵ��Զ�����������������TCP Client���Ӵ˷������ˡ�
*              4. ����K1���£�����8�ֽڵ����ݸ�TCP Client��
*              5. ����K2���£�����1024�ֽڵ����ݸ�TCP Client��
*              6. ����K3���£�����5MB�ֽڵ����ݸ�TCP Client��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2019-10-22   Eric2013     �׷�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	



/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_debug printf
#else
	#define printf_debug(...)
#endif


/*
*********************************************************************************************************
*	                                  �궨��
*********************************************************************************************************
*/
#define PORT_NUM       1001    /* TCP�����������˿ں� */


/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
int32_t tcp_sock;
int32_t tcp_sock1;

/* TCPnet API�ķ���ֵ */
static const char * ReVal_Table[]= 
{
	"netOK: Operation succeeded",
	"netBusy: Process is busy",
	"netError: Unspecified error",
	"netInvalidParameter: Invalid parameter specified",
	"netWrongState: Wrong state error",
	"netDriverError: Driver error",
	"netServerError: Server error",
	"netAuthenticationFailed: User authentication failed",
	"netDnsResolverError: DNS host resolver failed",
	"netFileError: File not found or file r/w error",
	"netTimeout: Operation timeout",
};

/*
*********************************************************************************************************
*	�� �� ��: tcp_cb_server
*	����˵��: TCP Socket�Ļص�����
*	��    ��: socket  ���
*             event   �¼�����
*             addr    NET_ADDR���ͱ�������¼IP��ַ���˿ںš�
*             buf     ptrָ��Ļ�������¼�Ž��յ���TCP���ݡ�
*             len     ��¼���յ������ݸ�����
*	�� �� ֵ: 
*********************************************************************************************************
*/
uint32_t tcp_cb_server (int32_t socket, netTCP_Event event,
                        const NET_ADDR *addr, const uint8_t *buf, uint32_t len) 
{
	switch (event) 
	{
		/*
			Զ�̿ͻ���������Ϣ
		    1������ptr�洢Զ���豸��IP��ַ��par�д洢�˿ںš�
		    2��������ֵ1�������ӣ�������ֵ0��ֹ���ӡ�
		*/
		case netTCP_EventConnect:
			if (addr->addr_type == NET_ADDR_IP4) 
			{
				printf_debug("Զ�̿ͻ�����������IP: %d.%d.%d.%d  �˿ں�:%d\r\n", 
																addr->addr[0], 
																addr->addr[1], 
																addr->addr[2], 
																addr->addr[3], 		
																addr->port);
				return (1);
			}
			else if (addr->addr_type == NET_ADDR_IP6)  
			{
				return (1);
			}
			
			return(0);

		/* SocketԶ�������Ѿ����� */
		case netTCP_EventEstablished:
			printf_debug("Socket is connected to remote peer\r\n");
			break;

		/* ���ӶϿ� */
		case netTCP_EventClosed:
			printf_debug("Connection has been closed\r\n");
			break;

		/* ������ֹ */
		case netTCP_EventAborted:
			break;

		/* ���͵������յ�Զ���豸Ӧ�� */
		case netTCP_EventACK:
			break;

		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case netTCP_EventData:
//			printf_debug("Data length = %d\r\n", len);
//			printf ("%.*s\r\n",len, buf);
			break;
	}
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: TCPnetTest
*	����˵��: TCPnetӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
void TCPnetTest(void)
{
	int32_t iCount;
	uint8_t *sendbuf;
	uint32_t maxlen;
	netStatus res;
	const uint16_t usMaxBlockTime = 2; /* �ӳ����� */
	uint32_t EvtFlag;
	
	uint8_t ip4_addr[NET_ADDR_IP4_LEN];
 
	netIF_GetOption (NET_IF_CLASS_ETH | 0, netIF_OptionIP4_Address, ip4_addr, sizeof (ip4_addr));
	
	for(iCount=0; iCount < 4; iCount++)
	{
		printf("--%d", ip4_addr[iCount]);
	}
	
	netIF_GetOption (NET_IF_CLASS_ETH | 1, netIF_OptionIP4_Address, ip4_addr, sizeof (ip4_addr));
	
	for(iCount=0; iCount < 4; iCount++)
	{
		printf("--%d", ip4_addr[iCount]);
	}
	
	
	netIF_SetDefault (NET_IF_CLASS_ETH | 1, netIF_VersionIP4);
	
	tcp_sock = netTCP_GetSocket (tcp_cb_server);
	
	if (tcp_sock > 0) 
	{
		res = netTCP_Listen (tcp_sock, PORT_NUM);
		printf_debug("tcp listen res = %s\r\n", ReVal_Table[res]);
		
		/* ʹ��TCP_TYPE_KEEP_ALIVE����һֱ�������� */
		//netTCP_SetOption (tcp_sock, netTCP_OptionKeepAlive, 1);
	}
	
	netIF_SetDefault (NET_IF_CLASS_ETH | 0, netIF_VersionIP4);
	
	tcp_sock1 = netTCP_GetSocket (tcp_cb_server);
	
	if (tcp_sock > 0) 
	{
		res = netTCP_Listen (tcp_sock1, PORT_NUM);
		printf_debug("tcp listen res = %s\r\n", ReVal_Table[res]);
		
		/* ʹ��TCP_TYPE_KEEP_ALIVE����һֱ�������� */
		//netTCP_SetOption (tcp_sock, netTCP_OptionKeepAlive, 1);
	}
	
	while (1) 
	{
		
		EvtFlag = osThreadFlagsWait(0x00000007U, osFlagsWaitAny, usMaxBlockTime);
		
		/* ������Ϣ�Ĵ��� */
		if(netTCP_GetState(tcp_sock) == netTCP_StateESTABLISHED)
		{
			switch (EvtFlag)
			{
				/* ���յ�K1�����£���Զ��TCP�ͻ��˷���8�ֽ����� */
				case KEY1_BIT0:			  
					iCount = 8;
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ��ô����û����� */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* ����ʹ��������ڴ�ռ� */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;

				/* ���յ�K2�����£���Զ��TCP�ͻ��˷���1024�ֽڵ����� */
				case KEY2_BIT1:	
					iCount = 1024;
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ��ô����û����� */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* ����ʹ��������ڴ�ռ� */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;					
					
					
				/* ���յ�K3�����£���Զ��TCP�ͻ��˷���5MB���� */
				case KEY3_BIT2:			  
					iCount = 5*1024*1024;				
					do
					{
						if(netTCP_SendReady(tcp_sock) == true )
						{
							maxlen  = netTCP_GetMaxSegmentSize (tcp_sock);

							iCount -= maxlen;
							
							if(iCount < 0)
							{
								/* ��ô����û����� */
								maxlen = iCount + maxlen;
							}
							
							sendbuf = netTCP_GetBuffer (maxlen);
							sendbuf[0] = '1';
							sendbuf[1] = '2';
							sendbuf[2] = '3';
							sendbuf[3] = '4';
							sendbuf[4] = '5';
							sendbuf[5] = '6';
							sendbuf[6] = '7';
							sendbuf[7] = '8';
							
							/* ����ʹ��������ڴ�ռ� */
							netTCP_Send (tcp_sock, sendbuf, maxlen);
						}
						
					}while(iCount > 0);
					break;
				
				 /* �����ļ�ֵ������ */
				default:                     
					break;
			}
		}
	}
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
