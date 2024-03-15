/*
*********************************************************************************************************
*
*	ģ������ : ����Э��ջ����
*	�ļ����� : demo_dm9162_netx.c
*	��    �� : V1.0
*	˵    �� : ���ԵĹ���˵��
*              1. Ĭ��IP��ַ192.168.28.245���ڱ��ļ����ã��û��ɸ�����Ҫ�޸ġ�
*              2. �����ڵ��Զ�����������������TCP Client���Ӵ˷������ˣ��˿ں�1001��
*              3. ʵ����һ���򵥵Ļػ�ͨ�ţ��û�ʹ����λ�����͵�����ͨ�����ӷ��ص���λ����
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2021-01-30   Eric2013     �׷�
*
*	Copyright (C), 2018-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	



/*
*********************************************************************************************************
*	                                    IP���
*********************************************************************************************************
*/
#define DEFAULT_PORT                    1001    /* TCP�����������˿ں� */

#define IP_ADDR0                        192
#define IP_ADDR1                        168
#define IP_ADDR2                        28
#define IP_ADDR3                        245

#define  MAX_TCP_CLIENTS                1

/*
*********************************************************************************************************
*	                                    NetX�����ͨ�����
*********************************************************************************************************
*/
/* �ϵ��Ƚ������õ������ȼ��������߲�����������ȼ���3 */
#define  APP_CFG_TASK_NETX_PRIO                           28u    
#define  APP_CFG_TASK_NETX_STK_SIZE                     4096u
static   uint64_t  AppTaskNetXStk[APP_CFG_TASK_NETX_STK_SIZE/8];

/* ����NetX�����������ȼ���3 */
#define  APP_CFG_TASK_NETX_PRIO1                           3u 

/* ����NetXӦ���������ȼ���6 */
#define  APP_CFG_TASK_NetXPro_PRIO1                        6u

NX_TCP_SOCKET TCPSocket;
TX_SEMAPHORE  Semaphore;


/*
*********************************************************************************************************
*	                                    NetX�����ͨ�����
*********************************************************************************************************
*/
UCHAR data_buffer[4096];

NX_PACKET_POOL    pool_0;
NX_IP             ip_0;  

/* ���ݰ��ڴ�� */
#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 20)
ULONG  packet_pool_area[NX_PACKET_POOL_SIZE/4 + 4];

/* ARP���� */
ULONG    arp_space_area[512 / sizeof(ULONG)];                                                      
ULONG    error_counter;


#define PRINT_DATA(addr, port, data)        do {                                            \
                                                  printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \r\n", \
                                                  (addr >> 24) & 0xff,                      \
                                                  (addr >> 16) & 0xff,                      \
                                                  (addr >> 8) & 0xff,                       \
                                                  (addr & 0xff), port, data);               \
                                               } while(0)

extern TX_THREAD   AppTaskNetXProTCB;
extern TX_THREAD   *netx_thread_ptr;                                               
extern VOID  nx_driver_stm32h7xx(NX_IP_DRIVER *driver_req_ptr);
static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port);
  
                                               
/*
*********************************************************************************************************
*	�� �� ��: NetXTest
*	����˵��: TCPnetӦ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/    
void NetXTest(void)
{
    UINT status;
    UINT ret;
    ULONG socket_state;
    UINT old_priority;

    ULONG source_ip_address;
    NX_PACKET *data_packet;

    UINT source_port;
    ULONG bytes_read;

    /* �����ź���������֪ͨTCP������������������ */
    tx_semaphore_create(&Semaphore, "App Semaphore", 0);
    

    /* ��ʼ��NetX */
    nx_system_initialize();

    /* �����ڴ�� */
    status =  nx_packet_pool_create(&pool_0, 
                                     "NetX Main Packet Pool", 
                                     1536,  (ULONG*)(((int)packet_pool_area + 15) & ~15) , 
                                     NX_PACKET_POOL_SIZE);

    /* ��ⴴ���Ƿ�ʧ�� */
    if (status) error_counter++;

    /* ����IP */
    status = nx_ip_create(&ip_0, 
                            "NetX IP Instance 0", 
                            IP_ADDRESS(IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3), 
                            0xFFFFFF00UL, 
                            &pool_0, nx_driver_stm32h7xx,
                            (UCHAR*)AppTaskNetXStk,
                            sizeof(AppTaskNetXStk),
                            APP_CFG_TASK_NETX_PRIO);
                            
            
    /* ��ⴴ���Ƿ�ʧ�� */
    if (status) error_counter++;

    /* ʹ��ARP�����ṩARP���� */
    status =  nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));

    /* ʹ��fragment */    
    status = nx_ip_fragment_enable(&ip_0);

    /* ���ʹ�ܳɹ� */
    if (status) error_counter++;

    /* ʹ��TCP */
    status =  nx_tcp_enable(&ip_0);

    /* ���ʹ�ܳɹ� */
    if (status) error_counter++;

    /* ʹ��UDP  */
    status =  nx_udp_enable(&ip_0);

    /* ���ʹ�ܳɹ� */
    if (status) error_counter++;

    /* ʹ��ICMP */
    status =  nx_icmp_enable(&ip_0);

    /* ���ʹ�ܳɹ� */
    if (status) error_counter++;   
    
    /* NETX��ʼ����Ϻ������������ȼ� */
    tx_thread_priority_change(netx_thread_ptr, APP_CFG_TASK_NETX_PRIO1, &old_priority);
    tx_thread_priority_change(&AppTaskNetXProTCB, APP_CFG_TASK_NetXPro_PRIO1, &old_priority);
    
    /////////////////////////////////////////////////////////////////////////////
    /* ����TCP Socket */
    ret = nx_tcp_socket_create(&ip_0, 
                               &TCPSocket, 
                               "TCP Server Socket", 
                               NX_IP_NORMAL, 
                               NX_FRAGMENT_OKAY,
                               NX_IP_TIME_TO_LIVE, 
                               4320, 
                               NX_NULL, 
                               NX_NULL);
    if (ret)
    {
		Error_Handler(__FILE__, __LINE__);    
    }

    /*
    * �����µ����ӡ�
    * �����ص�TCP_listen_callback��ʾ�����������ӡ�
    */
    ret = nx_tcp_server_socket_listen(&ip_0, DEFAULT_PORT, &TCPSocket, MAX_TCP_CLIENTS, tcp_listen_callback);

    if (ret)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    if(tx_semaphore_get(&Semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
    {

    }
    else
    {
        /* ����TCPͨ��ǰ������������ */
        ret = nx_tcp_server_socket_accept(&TCPSocket, TX_WAIT_FOREVER);

        if (ret)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    
    
	while(1)
	{
        TX_MEMSET(data_buffer, '\0', sizeof(data_buffer));

        /* ��ȡsocket״̬ */
        nx_tcp_socket_info_get(&TCPSocket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &socket_state, NULL, NULL, NULL);

        /* ������ӻ�û�н������������������ӣ��ɹ��Ļ������������� */
        if(socket_state != NX_TCP_ESTABLISHED)
        {
            ret = nx_tcp_server_socket_accept(&TCPSocket, NX_IP_PERIODIC_RATE);
        }

        if(ret == NX_SUCCESS)
        {
            /* ����TCP�ͻ��˷���TCP���ݰ� */
            ret = nx_tcp_socket_receive(&TCPSocket, &data_packet, NX_WAIT_FOREVER);

            if (ret == NX_SUCCESS)
            {
                /* ��ȡ�ͻ��˵�IP��ַ�Ͷ˿� */
                nx_udp_source_extract(data_packet, &source_ip_address, &source_port);

                /* ��ȡ�ͻ��˷��������� */
                nx_packet_data_retrieve(data_packet, data_buffer, &bytes_read);

                /* ��ӡ���յ����� */
                PRINT_DATA(source_ip_address, source_port, data_buffer);

                /* ���������յ������ݷ��ͻ�ȥ */
                ret =  nx_tcp_socket_send(&TCPSocket, data_packet, NX_IP_PERIODIC_RATE);

                if (ret == NX_SUCCESS)
                {

                }

                /* �ͷ����ݰ� */
                nx_packet_release(data_packet);
            }
            else
            {
                /* �Ͽ����ӣ����¼��� */
                nx_tcp_socket_disconnect(&TCPSocket, NX_WAIT_FOREVER);
                nx_tcp_server_socket_unaccept(&TCPSocket);
                nx_tcp_server_socket_relisten(&ip_0, DEFAULT_PORT, &TCPSocket);
            }
        }
        else
        {
            /* ���ڿ���״̬ */
            
        }
	}  	
}

/*
*********************************************************************************************************
*	�� �� ��: tcp_listen_callback
*	����˵��: �������������
*	��    ��: -
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    tx_semaphore_put(&Semaphore);
}   

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
