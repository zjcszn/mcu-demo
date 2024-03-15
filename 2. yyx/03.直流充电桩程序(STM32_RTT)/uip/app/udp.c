#include "uip.h"
#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "server.h"
#include "sys_cfg.h"
#include "string.h"

#if 1
#define UDP_MSG dbg_msg
#define UDP_ERROR dbg_error
#else
#define UDP_MSG (void)
#define UDP_ERROR (void)
#endif

#define UDP_SEND_BUF_LEN 100
u8 udp_send_buf[UDP_SEND_BUF_LEN];
static u16 udp_send_len;
static u8 udp_ready;




void udp_Init()
{
	struct uip_udp_conn* c;
	uip_ipaddr_t ipaddr = {0, 0};

	uip_ipaddr(ipaddr, sys_cfg.rip1, sys_cfg.rip2, sys_cfg.rip3, sys_cfg.rip4);
	c = uip_udp_new(&ipaddr, HTONS(sys_cfg.rport));


	uip_udp_bind(c, HTONS(1024));
	if(c != 0)
	{
		udp_ready = 1;
	}
}

void udp_appcall()
{
#if UIP_CONF_DHCP
	dhcpc_appcall();
#endif

	switch(uip_udp_conn->lport)
	{
#if UIP_CONF_DHCP
	case HTONS(68):  //dhcp
		//dhcpc_appcall();
			break;
#endif
	}

	if(uip_newdata())
	{
		//uip_flags &= ~2;
		if((uip_udp_conn->rport == HTONS(sys_cfg.rport)) && (uip_udp_conn->lport == HTONS(1024)))
		{
			if(server_handler.recv_callback != NULL)
			{
				server_handler.recv_callback(uip_appdata, uip_len);
			}
		}
	}


	if(uip_poll())  //����ʱ��ѯ����
	{
		if(udp_send_len)  //��Ҫ��������
		{
			memcpy(uip_appdata, udp_send_buf, udp_send_len);  //����
			uip_udp_send(udp_send_len);  //����
			udp_send_len = 0;
		}
	}
}



/*
 * @brief  UDP��������
 * @param  buf ����
 * @param  len �����ֽ���
 * @retval 0�ɹ�  1ʧ��
 */
int udp_send(u8 *buf, u16 len)
{
	if(!udp_ready)
	{
		return 1;
	}

	if(len <= UDP_SEND_BUF_LEN)
	{
		memcpy(udp_send_buf, buf, len);
		udp_send_len = len;
		return 0;
	}
	else
	{
		return 1;
	}
}


//#if 1
//
////���ַ�����������һ�µ�  STM32��Ӳ���㷨   ����㷨��������ƽ̨У��ʹ��
//
///*
// * @brief  STM32�ڲ�Ӳ��CRC32����   ע���CRCʱ�ӣ�
// * @param  buf ����
// * @param  len �����ֽ���
// * @retval У����
// */
//u32 crc32(u8 *buf, u16 len)
//{
//	u16 index;
//	CRC_ResetDR();        //��λCRC
//	for(index = 0; index < len; index++)
//	{
//		CRC->DR = (u32)buf[index];
//	}
//	return (CRC->DR);
//}
//#else
///*
// * @brief  ���CRC32����
// * @param  buf ����
// * @param  len �����ֽ���
// * @retval У����
// */
//u32 crc32(u8 *buf, u16 len)
//{
//
//#define POLY  0x04c11db7
//
//    int i,j;
//    u32 crc = 0xFFFFFFFF;
//    for (j = 0; j < len; j++)
//    {
//        crc = crc ^ (*buf++);
//        for (i = 0; i < 32; i++)
//        {
//            if (crc & 0x80000000)
//                crc = (crc << 1) ^ POLY;
//            else
//                crc <<= 1;
//        }
//    }
//    return(crc);
//}
//#endif
























