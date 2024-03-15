#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "evc.h"
#include "string.h"
#include "io.h"

#if 1
#define EVC_MSG dbg_msg
#else
#define EVC_MSG (void)
#endif

#if 1
#define EVC_ERROR dbg_error
#else
#define EVC_ERROR (void)
#endif


#define ERR_PS(s) (((s >> 24) & 0xff) == 13)  //�������
#define ERR_OCP(s)  ((s & 0xff) == 1)  //DC ����

#define EVC_ON(s) ((s >> 8) & 0x01)  //����״̬
#define EVC_OFF(s) (!((s >> 8) & 0x01))  //ֹͣ״̬

#define CMD_ONOFF 	  Bit(1)
#define CMD_RESET 	  Bit(2)
#define CMD_MODE  	  Bit(3)
#define CMD_SAVE  	  Bit(4)
#define CMD_SET_LIMT  Bit(5)
#define CMD_SET_V_C	  Bit(6)


evc_info_def evc_info;

extern OS_TID evc_task_id;

CanRxMsg can1_rx_msg;

OS_SEM can1_rx_sem;

/*
 * ʹ��CAN1����
 */
void CAN1_rx_en()
{
	CAN_FIFORelease(CAN1, CAN_FIFO0);
	CAN1->FA1R |= Bit(0);  //���������0
}

/*
 * �ر�CAN1����
 */
void CAN1_rx_dis()
{
	CAN1->FA1R &= ~Bit(0);
}

/*
 * @brief  CAN1�����жϴ������
 */
void CAN1_RX0_IRQHandler()
{
	CAN1_rx_dis();
	CAN_Receive(CAN1, CAN_FIFO0, &can1_rx_msg);

	isr_sem_send(&can1_rx_sem);
}


int can_send(CanTxMsg* tx)
{
	CAN_Transmit(CAN1, tx);
	return 1;
}

/*
 * @brief  ����ģ��   ��Ӧ�����뷢������ͬ��ɹ�
 * @param  tx: CAN��Ϣ
 * @retval 1 ���ͳɹ�   0 ʧ��
 */
int config_send(CanTxMsg* tx)
{
	u8 err_cnt = 3;
	int ret = 0;

	CAN1_rx_en();
	do{
		can_send(tx);
		if(os_sem_wait(&can1_rx_sem, 100) != OS_R_TMO)  //�ȴ���Ӧ
		{
			//δ��ʱ
			u32 temp = can1_rx_msg.ExtId & (~0x0000ffff);
			temp |= HTONS((u16)can1_rx_msg.ExtId);
			if(tx->ExtId == temp)
			{
				if(memcmp(tx->Data, can1_rx_msg.Data, 8) == 0)  //�Ƚ��յ��������뷢�͵��Ƿ���ͬ
				{
					ret = 1;
					break;
				}
			}
		}
		else
		{
			//��ʱ
			//EVC_ERROR("config_send timeout\n");
		}
		os_dly_wait(50);
	}while(--err_cnt);

	CAN1_rx_dis();

	return ret;
}




/*
 * @brief  ��ȡģ������
 * @param  tx: CAN��Ϣ
 * @retval 1 ���ͳɹ��յ�Ӧ����Ϣ   0 ʧ��Ӧ��ʱ
 */
int read_info_send(CanTxMsg* tx)
{
	u8 err_cnt = 3;
	int ret = 0;

	CAN1_rx_en();
	do
	{
		can_send(tx);

		if(os_sem_wait(&can1_rx_sem, 100) != OS_R_TMO)  //�ȴ���Ӧ
		{
			u32 temp = can1_rx_msg.ExtId & (~0x0000ffff);
			temp |= HTONS((u16)can1_rx_msg.ExtId);
			if(tx->ExtId == temp)
			{
				ret = 1;
				break;
			}
		}
		os_dly_wait(50);
	}while(--err_cnt);

	CAN1_rx_dis();

	return ret;
}

/*
 * @brief  ����ģ�������ѹ ����
 * @param  id: ģ��id��
 * @param  v ��λ0.1V
 * @param  c ��λ0.01A
 * @retval 1 �ɹ�   0 ʧ��
 */
int config_v_c(u8 id, u16 v, u16 c)
{
	CanTxMsg TxMsg;

	if(v > evc_info.max_v)
	{
		v = evc_info.max_v;
	}

	if(c > evc_info.max_i)
	{
		c = evc_info.max_i;
	}

	TxMsg.ExtId = 0x020200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)(v >> 8);
	TxMsg.Data[1] = (u8)v;
	TxMsg.Data[2] = (u8)(c >> 8);
	TxMsg.Data[3] = (u8)c;
	memset(&TxMsg.Data[4], 0, 8 - 4);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}

/*
 * @brief  ������ر�ģ�����
 * @param  id: ģ��id��
 * @param  on_off:  0x00 �ر�   oxff ��
 * @retval 1 �ɹ�   0 ʧ��
 */
int config_on_off(u8 id, u8 on_off)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010400ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = on_off;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  �洢���ò���
 * @param  id: ģ��id��
 * @param  save:  0x01 ��Ч   0x00 ȡ��
 * @retval 1 �ɹ�   0 ʧ��
 */
int config_save_data(u8 id, u8 save)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = save;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}



/*
 * @brief  ��������ʱ��
 * @param  id: ģ��id��
 * @param  run_t: ��ʱ����ʱ�� С��6000s
 * @param  delay_t: ����->�����ӳ�ʱ���趨ֵ С��30s
 * @param  timeout_t: ģ��ͨ�ų�ʱͣ������ʱ��  С��300s
 * @retval 1 �ɹ�   0 ʧ��
 */
int config_run_time(u8 id, u32 run_t, u16 delay_t, u16 timeout_t)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x020300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)(run_t >> 24);
	TxMsg.Data[1] = (u8)(run_t >> 16);
	TxMsg.Data[2] = (u8)(run_t >> 8);
	TxMsg.Data[3] = (u8)(run_t);

	TxMsg.Data[4] = (u8)(delay_t >> 8);
	TxMsg.Data[5] = (u8)(delay_t);

	TxMsg.Data[6] = (u8)(timeout_t >> 8);
	TxMsg.Data[7] = (u8)(timeout_t);

	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  ����ͨѶ��ʱͣ������
 * @param  id: ģ��id��
 * @param  s: 0:�ر�   >0 ʹ��
 * @retval 1 �ɹ�   0 ʧ��
 */
int config_com_timeout(u8 id, u8 s)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010100ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 0x02;
	TxMsg.Data[1] = 0x02;
	TxMsg.Data[2] = 0x01;
	if(s)
	{
		TxMsg.Data[3] = 0x01;
	}
	else
	{
		TxMsg.Data[3] = 0x00;
	}

	TxMsg.Data[4] = 0;

	memset(&TxMsg.Data[5], 0, 8 - 5);

	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}


/*
 * @brief  ��λģ��
 * @param  id: ģ��id��
 * @retval 1 �ɹ�   0 ʧ��
 */
int reset_module(u8 id)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x010300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	TxMsg.Data[0] = 1;
	memset(&TxMsg.Data[1], 0, 8 - 1);
	if(id == 0)
	{
		return can_send(&TxMsg);
	}
	else
	{
		return config_send(&TxMsg);
	}
}

/*
 * @brief  ��ȡģ��״̬  ����״̬�����ͨ��Э��
 * @param  id: ģ��id��
 * @retval С��0xffffffff �ɹ�   0xffffffffʧ��
 */
u32 read_status(u8 id)
{
	CanTxMsg TxMsg;
	u32 ret = 0;

	TxMsg.ExtId = 0x030300ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);

	if(read_info_send(&TxMsg))
	{
		ret |= can1_rx_msg.Data[0];
		ret |= can1_rx_msg.Data[1] << 8;
		ret |= can1_rx_msg.Data[2] << 16;
		ret |= can1_rx_msg.Data[3] << 24;
	}
	else
	{
		ret = 0xffffffff;
	}
	return ret;
}

/*
 * @brief  ��ȡ�����ѹ����
 * @param  id: ģ��id��
 * @retval С��0xffffffff �ɹ�   0xffffffffʧ��  �����ֽ�Ϊ����  �����ֽ�Ϊ��ѹ   ��˸�ʽ
 */
u32 read_v_c(u8 id)
{
	CanTxMsg TxMsg;
	u32 cv = 0;

	TxMsg.ExtId = 0x030200ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);
	if(read_info_send(&TxMsg))
	{
		cv = *(u32*)can1_rx_msg.Data;
	}
	else
	{
		cv = 0xffffffff;
	}
	return cv;
}


/*
 * @brief  ��ȡ�������ѹ����
 * @param  id: ģ��id��
 * @retval С��0xffffffff �ɹ�   0xffffffffʧ��  �����ֽ�Ϊ����  �����ֽ�Ϊ��ѹ   ��˸�ʽ
 */
u32 read_limt(u8 id)
{
	CanTxMsg TxMsg;
	u32 cv = 0;

	TxMsg.ExtId = 0x030100ff | (id << 8);
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;
	memset(&TxMsg.Data[0], 0, 8);
	if(read_info_send(&TxMsg))
	{
		cv = *((u32*)(&can1_rx_msg.Data[4]));
	}
	else
	{
		cv = 0xffffffff;
	}
	return cv;
}


/*
 *
 */
u8 evc_is_err_status(u32 status)
{
	u8  dc_err = (u8)status;
	u8  pfc_err = (u8)(status >> 24);

	if((dc_err == 1) ||
	   (dc_err == 2) ||
	   (dc_err == 3))
	{
		return 1;
	}

	if((pfc_err == 1) ||
	   (pfc_err == 2) ||
	   (pfc_err == 3) ||
	   (pfc_err == 4) ||
	   (pfc_err == 5) ||
	   (pfc_err == 6) ||
	   (pfc_err == 7) ||
	   (pfc_err == 8) ||
	   (pfc_err == 9) ||
	   (pfc_err == 10) ||
	   (pfc_err == 11) ||
	   (pfc_err == 12) ||
	   (pfc_err == 13) ||
	   (pfc_err == 14) ||
	   (pfc_err == 15) ||
	   (pfc_err == 16) ||
	   (pfc_err == 33))
	{
		return 1;
	}
	return 0;
}



/******************  �ⲿ��������������  ************************/


void cmd_onoff_handler()
{
	config_on_off(0, evc_info.config.onoff);  //����ÿ��ģ��
	os_dly_wait(50);
	config_on_off(0, evc_info.config.onoff);  //����ÿ��ģ��

	if(evc_info.config.onoff == 0)  //�ر�
	{
		u8 i;
		io_runled_off();  //���е���
		io_fun_off();  //���ȹ�
		evc_info.config.onoff = 0;
		evc_info.config.set_c = 0;
		evc_info.config.set_v = 0;

		for(i = 0; i < MAX_MODULE_CNT; i++)
		{
			evc_info.module_info[i].ocp_reset_cnt = 0;
		}
	}
	else
	{
		io_runled_on();    //���е���
		io_fun_on();  //���ȿ�
	}
}


void cmd_set_v_c_handler()
{
	u16 c = (evc_info.config.set_c) / evc_info.module_cnt_no_err;  //����ƽ��

	config_v_c(0, evc_info.config.set_v, c);  //����ÿ��ģ��
	os_dly_wait(50);
	config_v_c(0, evc_info.config.set_v, c);
}
/******************************************/

/*
 * �������õ�ѹ����
 */
static void re_set_v_c()
{
	os_evt_set(CMD_SET_V_C, evc_task_id);
}


u16 filter_calc(u8 id, u8 vi)
{
	u8 j, i;
	u16 value_buf[READ_SAMP_CNT], temp, count;
	u32  sum = 0;

	for(j = 0; j < READ_SAMP_CNT; j++)
	{
		if(vi == 0)
		{
			value_buf[j] = evc_info.module_info[id].v_temp[j];
		}
		else
		{
			value_buf[j] = evc_info.module_info[id].i_temp[j];
		}
	}

	for (j = 0; j < READ_SAMP_CNT - 1; j++) //����  С����
	{
		for (i = j + 1; i < READ_SAMP_CNT; i++)
		{
			if (value_buf[j] > value_buf[i])
			{
				temp = value_buf[i];
				value_buf[i] = value_buf[j];
				value_buf[j] = temp;
			}
		}
	}

#define N 1

	for (count = N; count < READ_SAMP_CNT - N; count++)
	{
		sum += value_buf[count];
	}

	return (u16)(sum / (READ_SAMP_CNT - N * 2));
}

void remove_old(u8 id)
{
	u8 j;
	for(j = 0; j < READ_SAMP_CNT - 1; j++)
	{
		evc_info.module_info[id].v_temp[j] = evc_info.module_info[id].v_temp[j + 1];
		evc_info.module_info[id].i_temp[j] = evc_info.module_info[id].i_temp[j + 1];
	}
}

/*
 * @brief    ��ȡ�޹���ģ������
 * @retval   ����
 */
static u8 get_cnt()
{
	u8 i;
	u8 cnt = 0;
	for(i = 0; i < MAX_MODULE_CNT; i++)
	{
		if((evc_info.module_info[i].id != 0) && (evc_info.module_info[i].is_err == 0))
		{
			cnt++;
		}
	}
	return cnt;
}


/*
 * @brief    ��ȡ������ģ������
 * @retval   ����
 */
static u8 get_cnt_online()
{
	u8 i;
	u8 cnt = 0;
	for(i = 0; i < MAX_MODULE_CNT; i++)
	{
		if(evc_info.module_info[i].id != 0)
		{
			cnt++;
		}
	}
	return cnt;
}


OS_MUT evc_mutex;

static U64 stk_evc_scan[200];

/*
 * ѭ��ɨ������ģ��
 */
void task_evc_scan()
{
	u8 t = 0;

	os_dly_wait(2000);

	while(1)
	{
		if((t++ % 10) == 0)  //ɨ��ģ��
		{
			u8 i;
			u8 set_vc_again = 0;
			u32 ret;

			for(i = 1; i < (MAX_MODULE_CNT + 1); i++)  //ģ���Ŵ�1�ſ�ʼ
			{
				os_mut_wait (&evc_mutex, 0xffff);
				ret = read_status(i);  //��ȡģ��״̬
				os_mut_release (&evc_mutex);

				//EVC_MSG("status i %d %x\n", i, ret);

				if(ret != 0xffffffff)  //��ȡ��״̬
				{
					evc_info.module_info[i - 1].id = i;
					evc_info.module_info[i - 1].status = ret;
					evc_info.module_info[i - 1].link_off = 0;

					if(!evc_is_err_status(ret))  //û����
					{
						evc_info.module_info[i - 1].is_err = 0;  //ģ������

						if(evc_info.config.onoff && EVC_OFF(ret))  //ģ��û�����
						{
							os_mut_wait (&evc_mutex, 0xffff);
							config_v_c(i, 0, 0);  //��ѹ����������Ϊ0
							os_dly_wait(50);
							config_on_off(i, 0xff);  //����ģ��
							set_vc_again = 1;
							os_mut_release (&evc_mutex);
						}
						else if((!evc_info.config.onoff) && EVC_ON(ret))  //ģ��û�йر�
						{
							os_mut_wait (&evc_mutex, 0xffff);
							config_on_off(i, 0);  //�ر�ģ��
							os_mut_release (&evc_mutex);
						}

						if(evc_info.max_v == 0)  //��ȡ����������
						{
							u32 temp;
							os_mut_wait (&evc_mutex, 0xffff);
							temp = read_limt(i);  //����������ѹ����
							os_mut_release (&evc_mutex);
							evc_info.max_v = HTONS((u16)temp);
							evc_info.max_i = HTONS((u16)(temp >> 16));
							
							evc_info.max_v /= 100;
							evc_info.max_v *= 100;  //ȥ����ͷ
							evc_info.max_i -= 100;  //����1A

							if(evc_info.max_v == 5000)  //500Vģ��
							{
								evc_info.max_i = 3000;  //������30A
							}
						}

						if(evc_info.module_info[i - 1].cfg_end == 0)  //��û���ò���
						{
							os_mut_wait (&evc_mutex, 0xffff);
							if(config_com_timeout(i, 1))  //����ͨѶ��ʱͣ������
							{
								if(config_run_time(i, 6000, 8, 30))  //��ʱʱ��30s
								{
									evc_info.module_info[i - 1].cfg_end = 1;
								}
							}
							os_mut_release (&evc_mutex);
						}
					}
					else  //ģ���й���
					{
						evc_info.module_info[i - 1].is_err = 1;

						if(ERR_OCP(ret))  //DC ����
						{
							os_mut_wait (&evc_mutex, 0xffff);
							reset_module(i);  //��λģ��
							os_mut_release (&evc_mutex);
							set_vc_again = 1;
//							if(evc_info.config.onoff)  //���״̬
//							{
//								if(evc_info.module_info[i - 1].ocp_reset_cnt == 0)  //��һ�θ�λ
//								{
//									delay_reset(&evc_info.module_info[i - 1].ocp_reset_t);  //��ʱ����λ
//									evc_info.module_info[i - 1].ocp_reset_cnt++;
//								}
//								else if(delay_timeout_reload(&evc_info.module_info[i - 1].ocp_reset_t, 1000 * 30) == 0)  //��λ֮�󲻵�30S�ٴα���
//								{
//									evc_info.module_info[i - 1].ocp_reset_cnt++;
//								}
//							}
						}

						if(ERR_PS(ret))  //�������
						{
							evc_info.err_ps = 1;
						}
					}
				}
				else
				{
					if(evc_info.module_info[i - 1].id != 0)  //ԭ����������
					{
						evc_info.module_info[i - 1].link_off++;  //��⵽һ������
						if(evc_info.module_info[i - 1].link_off == 3)  //ģ������
						{
							set_vc_again = 1;
							memset(&evc_info.module_info[i - 1], 0, sizeof(module_info_def));
						}
					}
					else
					{
						//��ǰid�ŵ�ģ�鲻���ڻ�ͨѶ�쳣
						evc_info.module_info[i - 1].id = 0;
						evc_info.module_info[i - 1].status = 0xffffffff;  //ͨѶ�쳣
					}
				}
			}

			evc_info.module_cnt = get_cnt_online();  //����ģ����

			evc_info.module_cnt_no_err = get_cnt();  //�޹���ģ������

			if((t % 6) == 60)  //30S ����һ�ε�ѹ����
			{
				set_vc_again = 1;
			}

			if(set_vc_again)  //�������õ�ѹ����
			{
				set_vc_again = 0;
				if(evc_info.config.onoff)  //���״̬
				{
					re_set_v_c();
				}
			}
		}



		if ((evc_info.config.onoff == 0xff) || //�������״̬
		   ((evc_info.config.onoff == 0) && ((evc_info.read.v != 0) || (evc_info.read.i != 0)))) //���Ѿ��ر�   �����в����ѹ
		{
			u32 read_v = 0, read_i = 0;
			u16 temp_v, temp_i;
			u8 i, read_cnt = 0;

			//��ȡÿ��ģ���ѹ����
			for (i = 1; i < (MAX_MODULE_CNT + 1); i++) //ģ���Ŵ�1�ſ�ʼ
			{
				if ((evc_info.module_info[i - 1].id != 0) && (evc_info.module_info[i - 1].is_err == 0)) //ģ������
				{
					u32 temp;
					os_mut_wait(&evc_mutex, 0xffff);
					temp = read_v_c(i); //����ѹ����
					os_mut_release(&evc_mutex);

					if (temp < 0xffffffff) //��ȡ�ɹ�
					{
						temp_v = HTONS((u16)temp);
						temp_i = temp >> 16;
						temp_i = HTONS(temp_i);
						if (temp_v < 500) //����50V
						{
							temp_v = 0;
						}
						if (temp_v == 0)
						{
							temp_i = 0;
						}

						{
							remove_old(i - 1);
							evc_info.module_info[i - 1].v_temp[READ_SAMP_CNT - 1] =	temp_v;
							evc_info.module_info[i - 1].i_temp[READ_SAMP_CNT - 1] =	temp_i;
							evc_info.module_info[i - 1].v = filter_calc(i - 1, 0); //��ѹ������ֵ�˲�
							evc_info.module_info[i - 1].i = filter_calc(i - 1, 1); //����������ֵ�˲�
						}
					}
					else
					{
						//EVC_MSG("ERR %d\n", i);
					}
				}
			}

			for(i = 1; i < (MAX_MODULE_CNT + 1); i++) //ģ���Ŵ�1�ſ�ʼ
			{
				if((evc_info.module_info[i - 1].id != 0) && (evc_info.module_info[i - 1].is_err == 0)) //ģ������
				{
					if(read_v < evc_info.module_info[i - 1].v)  //��ѹȡ���ֵ
					{
						read_v = evc_info.module_info[i - 1].v;
					}

					read_i += evc_info.module_info[i - 1].i;
					read_cnt++;
				}
			}

			evc_info.read.v = read_v; //��ѹ
			evc_info.read.i = read_i; //�ܵ���
		}
		os_dly_wait(500);
	}
}


/*
 * EVCģ���������
 */
void task_evc()
{
	u32 evt;

	memset(&evc_info, 0, sizeof(evc_info));

	os_sem_init(&can1_rx_sem, 0);
	os_mut_init (&evc_mutex);

	os_tsk_create_user(task_evc_scan, 3, stk_evc_scan, sizeof(stk_evc_scan));  //���ģ��


	while(1)
	{
		evt = os_evt_wait_or(0xffff, 1000 * 10);  //�ȴ��¼�
		if(evt == OS_R_EVT)  //ִ���ⲿ����
		{
			evt = os_evt_get();  //��ȡ�¼�

			os_mut_wait (&evc_mutex, 1000 * 10);  //��task_evc_scan������Ҳ����ģ�鷢���ݣ�Ϊ��ֹ���������ͻ��ʹ�û����ź���

			if(evt & CMD_SET_V_C)  //���õ�ѹ����
			{
				cmd_set_v_c_handler();
			}

			if(evt & CMD_ONOFF)  //������ر����
			{
				cmd_onoff_handler();
			}

			os_mut_release (&evc_mutex);
		}
	}
}












/**************************************   �ⲿ����     ****************************/

/*
 * @brief  ������ر����
 * @param  id: ģ��id��
 * @param  on_off:  0x00 �ر�   0xff ��
 */
void evc_out_onoff(u8 onoff)
{
	evc_info.config.onoff = onoff;
	os_evt_set(CMD_ONOFF, evc_task_id);
}

/*
 * @brief  ���������ѹ ����
 * @param  v ��λ0.1V ��ߵ�ѹ ������1200 V  ��Ϊ0xffffʱ������Ч
 * @param  c ��λ0.01A ��ߵ��� ������55 A   ��Ϊ0xffffʱ������Ч
 * @param  mode  ģʽ  0x01��ѹ  0x02����
 */
void evc_set_v_c(u16 v, u16 c)
{
	if((v != 0) && (c != 0))
	{
		if((evc_info.config.set_v == v) && (evc_info.config.set_c == c))
		{
			return;
		}
	}

	if(v != 0xffff)
	{
		evc_info.config.set_v = v;
	}

	if(c != 0xffff)
	{
		evc_info.config.set_c = c;
	}

	os_evt_set(CMD_SET_V_C, evc_task_id);
}



