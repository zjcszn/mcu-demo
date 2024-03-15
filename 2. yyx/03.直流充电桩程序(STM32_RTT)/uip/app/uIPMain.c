#include "uip.h"
#include "uip_arp.h"
#include "tapdev.h"
#include "timer.h"
#include "stm32f10x.h"
#include"udp.h"
#include"dbg.h"
#include "sys_cfg.h"


#if 1
#define ETH_MSG dbg_msg
#else
#define ETH_MSG (void)
#endif

#if 1
#define ETH_ERROR dbg_error
#else
#define ETH_ERROR (void)
#endif


#define BUF ((struct uip_eth_hdr *)&uip_buf[0])

//uIP�¼�����
//�����յ����������ݰ�
void uip_polling(void);

static struct timer arp_timer, link_timer;


#define Device_ID1  (*(u32*)(0x1FFFF7E8))
#define Device_ID2  (*(u32*)(0x1FFFF7EC))
#define Device_ID3  (*(u32*)(0x1FFFF7F0))


/*
 * @brief           д�Ĵ���(������0x10)
 * @retval 0 ��������   < 0 ʧ��
 */
int link_ok = -1;
int eth_link_isok()
{
	return link_ok;
}



//uip�¼�������
void uIPMain()
{
	//����MAC��ַ
	uip_ethaddr.addr[0] = 0x88;  //����Ϊż��
	uip_ethaddr.addr[1] = (u8)(Device_ID1);
	uip_ethaddr.addr[2] = (u8)(Device_ID1 >> 16);
	uip_ethaddr.addr[3] = (u8)(Device_ID2);
	uip_ethaddr.addr[4] = (u8)(Device_ID2 >> 16);
	uip_ethaddr.addr[5] = (u8)(Device_ID3);

eth_restart:

	tapdev_init();  //STM32 ETH ��ʼ��

    //��ʼ��uIPʱ�� 10ms�ж�
	uIP_ClockInit();

	uip_init();  //uIP��ʼ��

#if UIP_CONF_DHCP  //�Զ���ȡIP
	dhcpc_init((const void*)&uip_ethaddr, 6);
#else
	{
		uip_ipaddr_t ipaddr;

		//���ñ�������IP��ַ
		uip_ipaddr(ipaddr, sys_cfg.lip1, sys_cfg.lip2, sys_cfg.lip3, sys_cfg.lip4);
		uip_sethostaddr(ipaddr);
		//��������IP��ַ
		uip_ipaddr(ipaddr, sys_cfg.drip1, sys_cfg.drip2, sys_cfg.drip3, sys_cfg.drip4);
		uip_setdraddr(ipaddr);
		//������������
		uip_ipaddr(ipaddr, sys_cfg.netmask1, sys_cfg.netmask2, sys_cfg.netmask3, sys_cfg.netmask4);
		uip_setnetmask(ipaddr);
	}
#endif

	//uip_listen(HTONS(1200));	 //����1200�˿�,����TCP Server
	//uip_listen(HTONS(80));	 //����80�˿�,����Web Server
    //tcp_client_reconnect();	 //�������ӵ�TCP Server��,����TCP Client

	udp_Init();


	//����1��10��Ķ�ʱ��
	timer_set(&arp_timer, CLOCK_SECOND * 10);

	timer_set(&link_timer, CLOCK_SECOND * 3);

	while(1)
	{
		uip_polling();

//		if(tcp_server_sta & (1 << 6))  //�յ��ͻ�������
//		{
//			//tcp_server_sta &= ~(1 << 6);
//		}
//
//		if(tcp_client_sta & (1 << 6))  //�յ����������
//		{
//			//tcp_client_sta &= ~(1 << 6);
//		}

		//ÿ3����һ����������״̬
		if(timer_expired(&link_timer))
		{
			static u8 link_off_cnt;

			timer_reset(&link_timer);
			if(!tapdev_link_up())  //���߶Ͽ�
			{
				link_off_cnt++;
				if(link_off_cnt == 3)  //�������μ�⵽
				{
					link_ok = -1;
					ETH_ERROR("eth link down restart now\n");
					goto eth_restart;
				}
			}
			else
			{
				link_ok = 0;
				link_off_cnt = 0;
			}
		}
	}
}

//uIP�¼�����
//�����յ����������ݰ�
void uip_polling()
{
	u8 i;

	uip_len = tapdev_read();	//�������豸��ȡһ��IP��,�õ����ݳ���uip_len��uip.c�ж���
	if(uip_len > 0) 			//������
	{
		//����IP���ݰ�(ֻ��У��ͨ����IP���Żᱻ����)
		if(BUF->type == htons(UIP_ETHTYPE_IP))//�Ƿ���IP��?
		{
			uip_arp_ipin();	//ȥ����̫��ͷ�ṹ������ARP��
			uip_input();   	//IP������
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ��� uip_len > 0
			//��Ҫ���͵�������uip_buf, ������uip_len  (����2��ȫ�ֱ���)
			if(uip_len > 0)//��Ҫ��Ӧ����
			{
				uip_arp_out();  //����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();  //�������ݵ���̫��
			}

			//ETH_MSG("eth rx ip\n");
		}
		else if(BUF->type == htons(UIP_ETHTYPE_ARP))  //����arp����,�Ƿ���ARP�����?
		{
			uip_arp_arpin();
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len(����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				tapdev_send();  //��Ҫ��������,��ͨ��tapdev_send����
			}

			//ETH_MSG("eth rx arp\n");
		}
		else
		{
			//ETH_MSG("eth rx other\n");
		}
	}

	//ÿ��10�����1��ARP��ʱ������ ���ڶ���ARP����,ARP��10�����һ�Σ��ɵ���Ŀ�ᱻ����
	else if(timer_expired(&arp_timer))  //10�붨ʱ����ʱ
	{
		timer_reset(&arp_timer);
		uip_arp_timer();
	}
	else
	{

#if UIP_CONF_TCP
		//��������ÿ��TCP����, UIP_CONNSȱʡ��40��
		for(i = 0; i < UIP_CONNS; i++)
		{
			uip_periodic(i);	//����TCPͨ���¼�
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len>0)
			{
				uip_arp_out();  //����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();  //�������ݵ���̫��
			}
		}
#endif

		#if UIP_UDP	//UIP_UDP
		//��������ÿ��UDP����, UIP_UDP_CONNSȱʡ��10��
		for(i = 0; i < UIP_UDP_CONNS; i++)
		{
			uip_udp_periodic(i);	//����UDPͨ���¼�
			//������ĺ���ִ�к������Ҫ�������ݣ���ȫ�ֱ���uip_len>0
			//��Ҫ���͵�������uip_buf, ������uip_len (����2��ȫ�ֱ���)
			if(uip_len > 0)
			{
				uip_arp_out();//����̫��ͷ�ṹ������������ʱ����Ҫ����ARP����
				tapdev_send();//�������ݵ���̫��
			}
		}
		#endif
	}
}
