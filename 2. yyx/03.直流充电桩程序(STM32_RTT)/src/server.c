#include "RTL.h"
#include "stm32f10x.h"
#include "server.h"
#include "dbg.h"
#include "udp.h"
#include "evc.h"
#include "bms.h"
#include "string.h"
#include "sys_cfg.h"
#include "em.h"
#include "delay.h"
#include "sflash.h"
#include "gprs.h"
#include "server.h"

#if 1
#define SERVER_MSG dbg_msg
#define SERVER_ERROR dbg_error
#else
#define SERVER_MSG (void)
#define SERVER_ERROR (void)
#endif


#define SERVER_EVT_RECV 0x01
#define SERVER_EVT_IDLE  0x02
#define SERVER_EVT_CHING  0x04
#define SERVER_EVT_CHEND  0x08

extern OS_TID server_task_id;


server_handler_def server_handler;

server_info_def server_info;


chg_info_def chg_info;  //����������

OS_SEM chgend_sem;

OS_MUT server_buf_mutex;

#define RECV_DATA_LEN 100
u16 recv_data_len;  //�յ����ݵĳ���
u8 recv_data[RECV_DATA_LEN];  //�յ�����buf

#define SEND_DATA_LEN 100
u8 send_data[SEND_DATA_LEN];  //��������buf

u32 crc32(u8 *buf, u16 len);

int udp_send(u8 *buf, u16 len);

/*
 * @brief  ���յ����������ݴ�����  ��UDP���յ�����ʱ����
 * @param  buf ����
 * @param  len �����ֽ���
 */
void server_recv_callback(u8 *buf, u16 len)
{
	if(len <= RECV_DATA_LEN)
	{
		com_header_def *temp = (com_header_def*)buf;
		os_mut_wait (&server_buf_mutex, 0xffff);  //��ֹ������δ�����������±�����
		if((len > 12) && (temp->fh == 0x5A) && (buf[temp->len - 1] == 0xA5) && (temp->id == sys_cfg.id))  //У��֡ͷ֡β  ID
		{
			u32 *temp_crc;
			temp_crc = (u32*)&buf[temp->len - 5];
			if(*temp_crc == crc32(buf, temp->len - 5))
			{
				server_info.status = 1;  //����

				if(temp->cmd == 0)  //��������Ӧ��������
				{
					server_info.rsp_cnt++;
				}
				else if(temp->cmd == 1)  //��������Ӧ���������
				{
					server_info.rsp_cnt++;
				}
				else if(temp->cmd == 2)  //��������Ӧ����������
				{
					os_sem_send(&chgend_sem);
					server_info.rsp_cnt++;
					server_info.status = 1;  //����
				}
				else
				{
					memcpy(recv_data, buf, len);
					recv_data_len = len;
					os_evt_set(SERVER_EVT_RECV, server_task_id);
					return;
				}
			}
		}
		os_mut_release(&server_buf_mutex);  //�ͷ�
	}
	//SERVER_MSG("%s %d\n", buf, len);
}


///*
// * @brief  �ȴ�����������
// * @param  type ��������  0:��������  1:�������  2:��������
// * @retval 0:�ɹ�  1:��ʱ  2������У�����  3:���ݴ��� ˵�����͵������д���
// */
//static int wait_rsp(u8 type)
//{
//	com_header_def *com = (com_header_def*)recv_data;
//	if(os_sem_wait(&recv_sem, 2000) != OS_R_TMO)  //�յ�������Ӧ������
//	{
//		if(recv_data_len < 13)
//		{
//			return 2;  //У�����
//		}
//		if((recv_data[0] == 0x5A) && (recv_data[recv_data_len - 1] == 0xA5) && (com->len == recv_data_len))
//		{
//			u32 *temp_crc;
//			temp_crc = (u32*)&recv_data[recv_data_len - 5];
//			if(*temp_crc == crc32(recv_data, recv_data_len - 5))
//			{
//				if(recv_data[7] == type)
//				{
//					if(recv_data[8] == 0)  //���Ӧ����
//					{
//						return 0;  //��ȷӦ��
//					}
//					else
//					{
//						return 3;  //���ݴ���
//					}
//				}
//			}
//		}
//		return 2;  //����У�����
//	}
//	else
//	{
//		return 1;  //��ʱ
//	}
//}
/*
 * @brief  ��������ʱ���� �������ݱ���
 * @retval 0:�ɹ�  1:��ʱ
 */
static int send_idle()
{
	int ret;
	com_header_def *com = (com_header_def*)send_data;
	u8 *p = com->data;
	com->fh = 0x5A;
	com->len = 12 + 1;
	com->id = sys_cfg.id;
	com->cmd = 0;  //��ʾ��������
	*((u32*)p) = crc32(send_data, com->len - 5);  //���CRC
	p += 4;
	*p = 0xA5;  //֡β

	server_handler.send(send_data, (u16)(p - send_data) + 1);

	server_info.send_cnt++;

	return 0;

//	ret = wait_rsp(0);
//
//	//SERVER_MSG("ret %d\n", ret);
//	return ret;
}



/*
 * @brief  �������ʱ���� ������ݱ���
 * @retval 0:�ɹ�  1:��ʱ
 */
static int send_chging()
{
	int ret;
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com_chging_def *p = (com_chging_def*)com->data;
	com->fh = 0x5A;
	com->len = 12 + 25;
	com->id = sys_cfg.id;
	com->cmd = 1;   //��ʾ�������

	p->id = chg_info.id;  //ic����
	p->v = evc_info.read.v;  //��ѹ
	p->i = evc_info.read.i / 10;  //����
	p->kwh = em_info.kwh;  //���ĵ���
	p->amount = em_info.sum;  //���Ľ��
	p->time = (bms_info.timer_tick / 100) / 60;  //���ʱ��
	p->soc = bms_info.SOC;  //���SOC
	p->start_time.y = chg_info.start_time.y;   //��
	p->start_time.m = chg_info.start_time.m;   //��
	p->start_time.d = chg_info.start_time.d;    //��
	p->start_time.h = chg_info.start_time.h;   //ʱ
	p->start_time.mm = chg_info.start_time.mm;   //��
	p->start_time.s = chg_info.start_time.s;   //��
	p->gun_id = chg_info.gun_id;
	p->max_s_v = bms_info.max_v_single;
	crc = (u32*)&send_data[32];
	*crc = crc32(send_data, 32);
	send_data[36] = 0xA5;
	server_handler.send(send_data, 37);

	server_info.send_cnt++;

	return 0;
//	ret = wait_rsp(1);
//	return ret;
	//SERVER_MSG("ret %d\n", ret);
}


/*
 * @brief  ����������ʱ���� ���������ݱ���
 * @retval 0:�ɹ�  1:��ʱ
 */
static int send_chgend()
{
	int ret;
	u8 cnt = 0 ;
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com_chgend_def *p = (com_chgend_def*)com->data;
	com->fh = 0x5A;
	com->len = 12 + 23;
	com->id = sys_cfg.id;
	com->cmd = 2;  //��ʾ����������

	p->id = chg_info.id;  //ic����
	p->balances = chg_info.balances;  //���
	p->kwh = em_info.kwh;  //���ĵ���
	p->amount = em_info.sum;  //���Ľ��
	p->time = (bms_info.timer_tick / 100) / 60;  //���ʱ��
	p->soc = bms_info.SOC;  //���SOC
	p->start_time.y = chg_info.start_time.y;   //��
	p->start_time.m = chg_info.start_time.m;   //��
	p->start_time.d = chg_info.start_time.d;    //��
	p->start_time.h = chg_info.start_time.h;   //ʱ
	p->start_time.mm = chg_info.start_time.mm;   //��
	p->start_time.s = chg_info.start_time.s;   //��
	crc = (u32*)&send_data[30];
	*crc = crc32(send_data, 30);
	send_data[34] = 0xA5;

re_try:
	os_sem_init(&chgend_sem, 0);  //sem token  ����
	server_handler.send(send_data, 35);

	if(os_sem_wait(&chgend_sem, 3000) != OS_R_TMO)  //�յ�����
	{
		return 0;
	}

	if(cnt < 3)
	{
		cnt++;
		goto re_try;
	}
	return 1;
}


/*
 * @brief  ��Ӧ���������������ݱ���
 * @param  type ��������  0x20:�������  0x40:��ȡ����
 * @param  code 0:�޴���  1:���ݴ���
 */
static int response(u8 type, u8 code)
{
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	com->fh = 0x5A;
	com->len = 12 + 2;
	com->id = sys_cfg.id;

	send_data[7] = type;
	send_data[8] = code;

	crc = (u32*)&send_data[9];
	*crc = crc32(send_data, 9);
	send_data[13] = 0xA5;
	server_handler.send(send_data, 14);
	return 0;
}

#define FW_VER "1.0.0"
/*
 * @brief  ���ͷ�������ȡ������Ϣ
 */
static int send_read_info()
{
	u32 *crc;
	com_header_def *com = (com_header_def*)send_data;
	machine_info_def *info = (machine_info_def*)com->data;
	com->fh = 0x5A;
	com->len = 24;
	com->id = sys_cfg.id;
	com->cmd = 0x40;

	memcpy(&info->ver[0], FW_VER, 5);
	info->price = sys_cfg.e_price;
	info->kwh = em_info.kwh_cnt;

	crc = (u32*)&send_data[19];
	*crc = crc32(send_data, 19);
	send_data[23] = 0xA5;
	server_handler.send(send_data, 24);
	return 0;
}



/*
 * @brief  ���ͳ���������Ϣ
 * @param  err �������
 */
static int send_err_info(u32 err)
{
	u32 *crc, *p;
	com_header_def *com = (com_header_def*)send_data;
	com->fh = 0x5A;
	com->len = 12 + 5;
	com->id = sys_cfg.id;

	send_data[7] = 0x80;

	p = (u32*)&send_data[8];
	*p = err;

	crc = (u32*)&send_data[com->len - 5];
	*crc = crc32(send_data, com->len - 5);
	send_data[com->len - 1] = 0xA5;
	server_handler.send(send_data, com->len);
	return 0;
}



void server_send_idle(void)
{
	chg_info.status = 1;  //��������
	os_evt_set(SERVER_EVT_IDLE, server_task_id);
}


void server_send_chging(void)
{
	os_evt_set(SERVER_EVT_CHING, server_task_id);
}


void server_send_chgend(void)
{
	os_evt_set(SERVER_EVT_CHEND, server_task_id);
}



u32 server_delay_base;
u32 m10_delay_base;  //10���Ӷ�ʱʱ��

extern u8 ui_idle;  //����Ļ����������(page2)ʱΪ0   ����������Ϊ1    ��ֹ���������ڲ���ʱ�������
extern u8 ui_chg_id;   //Զ�̿������ĳ��ǹ���  ��̨���ͳ��ָ��ʱ���ô�ֵ  0:��Ч    1:���ǹ1  2:���ǹ2   ��������0ʱ˵�����ڳ��


void task_server(void)
{
	u32 evt;
	com_header_def *recv_header = (com_header_def*)recv_data;

	os_sem_init(&chgend_sem, 0);  //��־�ڷ��ͳ��������ݺ��յ�����������ȷ��Ӧ

	os_mut_init (&server_buf_mutex);  //server buf ������



	server_handler.send = udp_send;
	//server_handler.send = gprs_send;
	server_handler.recv_callback = server_recv_callback;

	server_info.state = SERVER_SATUS_IDLE;

	delay_reset(&server_delay_base);

	delay_reset(&m10_delay_base);

	while(1)
	{
		evt = os_evt_wait_or(0xffff, 5000);  //�ȴ��¼�
		if(evt == OS_R_EVT)  //�յ�����
		{
			evt = os_evt_get();  //��ȡ�¼�

			if(evt & SERVER_EVT_RECV)  //���յ�����������
			{
				if(recv_header->cmd == 0x20)  //���õ��
				{
					u16 temp = (recv_data[9] << 8) | recv_data[8];  //���
					sys_cfg.e_price = temp;
					if(sys_cfg_save() == 0)
					{
						response(0x20, 0);  //��Ӧ������
					}
				}
				else if(recv_header->cmd == 0x40)  //��ȡ������Ϣ
				{
					send_read_info();
				}
//				else if(recv_header->cmd == 0x21)  //�������
//				{
//					u8 temp = recv_data[8];
//					if((!ui_idle) && (!ui_chg_id) && ((temp < 1) || (temp > 2)))  //���ǹ��ű���Ϊ1 ��  2
//					{
//						ui_chg_id = temp;
//						response(0x21, 0);  //��Ӧ������
//					}
//					else
//					{
//						response(0x21, 1);  //��Ӧ������
//					}
//				}
//				else if(recv_header->cmd == 0x22)  //�رճ��
//				{
//					bms_stop_charge();
//					response(0x21, 0);  //��Ӧ������
//				}
//				else if(recv_header->cmd == 0x23)  //Զ�̿���
//				{
//					GPIO_SetBits(GPIOE, GPIO_Pin_2);
//					response(0x23, 0);  //��Ӧ������
//				}
//				else if(recv_header->cmd == 0x24)  //Զ�̹ػ�
//				{
//					GPIO_ResetBits(GPIOE, GPIO_Pin_2);
//					response(0x24, 0);  //��Ӧ������
//				}

				os_mut_release(&server_buf_mutex);  //�ͷ�
			}
			if(evt & SERVER_EVT_IDLE)  //���Ϳ�������
			{
				server_info.state = SERVER_SATUS_IDLE;
			}
			if(evt & SERVER_EVT_CHING)
			{
				server_info.state = SERVER_SATUS_CHGING;
			}
			if(evt & SERVER_EVT_CHEND)
			{
				server_info.state = SERVER_SATUS_CHGEND;
			}
		}

/********************************************  ��������������״̬  **************************************************/
		if(server_info.send_cnt > server_info.rsp_cnt)
		{
			if(server_info.send_cnt - server_info.rsp_cnt > 10)  //����10��û�յ�������������Ϊ����
			{
				server_info.status = 0;  //��־����
			}
		}

		if(delay_timeout_reload(&m10_delay_base, 1000 * 60 * 10) == 0)  //10���Ӷ�ʱ
		{
			server_info.send_cnt = 0;
			server_info.rsp_cnt = 0;
		}
/********************************************************************************************************************/


		switch(server_info.state)
		{
			case SERVER_SATUS_IDLE:  //���Ϳ�������
			{
				if(delay_timeout_reload(&server_delay_base, 5000) == 0)  //5s��ʱʱ�䵽
				{
					send_idle();

					//SERVER_MSG("server_stauts %d  %d  %d\n", server_info.status, server_info.send_cnt, server_info.rsp_cnt);
				}
			}
			break;

			case SERVER_SATUS_CHGING:  //�������ڳ������
			{
				if(delay_timeout_reload(&server_delay_base, 5000) == 0)  //5s��ʱʱ�䵽
				{
					send_chging();
				}
			}
			break;

			case SERVER_SATUS_CHGEND:  //���ͳ���������
			{
				send_chgend();
				//os_dly_wait(500);
				//send_chgend();
//				if(!send_chgend())
//				{
//
//				}
				server_info.state = SERVER_SATUS_IDLE;
			}
			break;

			case SERVER_SATUS_ERR:  //���ʹ�����Ϣ
			{
				if(delay_timeout_reload(&server_delay_base, 10000) == 0)  //00s��ʱʱ�䵽
				{
					u32 err = 0;
					send_err_info(err);
				}
			}
			break;

			default: break;
		}
	}
}



#if 1

//���ַ�����������һ�µ�  STM32��Ӳ���㷨   ����㷨��������ƽ̨У��ʹ��

/*
 * @brief  STM32�ڲ�Ӳ��CRC32����   ע���CRCʱ�ӣ�
 * @param  buf ����
 * @param  len �����ֽ���
 * @retval У����
 */
u32 crc32(u8 *buf, u16 len)
{
	u16 index;
	CRC_ResetDR();        //��λCRC
	for(index = 0; index < len; index++)
	{
		CRC->DR = (u32)buf[index];
	}
	return (CRC->DR);
}
#else
/*
 * @brief  ���CRC32����
 * @param  buf ����
 * @param  len �����ֽ���
 * @retval У����
 */
u32 crc32(u8 *buf, u16 len)
{

#define POLY  0x04c11db7

    int i,j;
    u32 crc = 0xFFFFFFFF;
    for (j = 0; j < len; j++)
    {
        crc = crc ^ (*buf++);
        for (i = 0; i < 32; i++)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ POLY;
            else
                crc <<= 1;
        }
    }
    return(crc);
}
#endif
