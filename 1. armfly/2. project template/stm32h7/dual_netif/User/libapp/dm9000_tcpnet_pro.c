/*
*********************************************************************************************************
*
*	ģ������ : TCPnet����Э��ջ����
*	�ļ����� : dm9000_tcpnet_lib.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. ǿ���Ƽ������߽ӵ�·�������߽�����������ԣ���Ϊ�Ѿ�ʹ����DHCP�������Զ���ȡIP��ַ��
*                 ��ȡ��IP��ַ��ͨ�����ڴ�ӡ������
*              2. DM9000��DM9162�ֱ𴴽���һ��TCP Server��DM9000ʹ�ö˿ں�1002����DM9162ʹ�ö˿ں�1001��
*                 �û������ڵ��Զ�����������������TCP Client���Ӵ˷������ˡ�
*              3. ����K1���£�DM9162�������ݸ�TCP Client��
*              4. ����K2���£�DM9000�������ݸ�TCP Client��
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2020-11-22   Eric2013     �׷�
*
*	Copyright (C), 2020-2030, ���������� www.armfly.com
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
#define PORT_NUM       1002     /* TCP�����������˿ں� */


/*
*********************************************************************************************************
*	                                     ����
*********************************************************************************************************
*/
static int32_t tcp_sock;

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
static uint32_t tcp_cb_server (int32_t socket, netTCP_Event event,
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
				printf_debug("DM9000Զ�̿ͻ�����������IP: %d.%d.%d.%d  �˿ں�:%d\r\n", 
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
			printf_debug("DM9000 Socket is connected to remote peer\r\n");
			break;

		/* ���ӶϿ� */
		case netTCP_EventClosed:
			printf_debug("DM9000 Connection has been closed\r\n");
			break;

		/* ������ֹ */
		case netTCP_EventAborted:
			break;

		/* ���͵������յ�Զ���豸Ӧ�� */
		case netTCP_EventACK:
			break;

		/* ���յ�TCP����֡��ptrָ�����ݵ�ַ��par��¼���ݳ��ȣ���λ�ֽ� */
		case netTCP_EventData:
			printf_debug("DM9000 Data length = %d\r\n", len);
			printf ("%.*s\r\n",len, buf);
			break;
	}
	return (0);
}

/*
*********************************************************************************************************
*	�� �� ��: DM9000TCPnetTest
*	����˵��: DM9000TCPnetTestӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
void DM9000TCPnetTest(void)
{
	int32_t iCount;
	uint8_t *sendbuf;
	uint32_t maxlen;
	netStatus res;
	const uint16_t usMaxBlockTime = 2; /* �ӳ����� */
	uint32_t EvtFlag;
	
	tcp_sock = netTCP_GetSocket (tcp_cb_server);
	
	if (tcp_sock > 0) 
	{
		res = netTCP_Listen (tcp_sock, PORT_NUM);
		printf_debug("DM9000 tcp listen res = %s\r\n", ReVal_Table[res]);
		
		/* ʹ��TCP_TYPE_KEEP_ALIVE����һֱ�������� */
		netTCP_SetOption (tcp_sock, netTCP_OptionKeepAlive, 1);
	}
	
	while (1) 
	{
		EvtFlag = osThreadFlagsWait(0x00000007U, osFlagsWaitAny, usMaxBlockTime);
		
		/* ������Ϣ�Ĵ��� */
		if(netTCP_GetState(tcp_sock) == netTCP_StateESTABLISHED)
		{
			switch (EvtFlag)
			{
				/* ���յ�K2�����£���Զ��TCP�ͻ��˷������� */
				case KEY2_BIT1:	
					iCount = 128*1024;
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
							sendbuf[0] = '8';
							sendbuf[1] = '7';
							sendbuf[2] = '6';
							sendbuf[3] = '5';
							sendbuf[4] = '4';
							sendbuf[5] = '3';
							sendbuf[6] = '2';
							sendbuf[7] = '1';
							
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
