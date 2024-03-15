#include "RTL.h"
#include "bms.h"
#include "stm32f10x.h"
#include "block_fifo.h"
#include "string.h"
#include "evc.h"
#include "dbg.h"
#include "shk.h"
#include "io.h"
#include "measure.h"

#if 1
#define BMS_MSG dbg_msg
#else
#define BMS_MSG (void)
#endif

#if 1
#define BMS_ERROR dbg_error
#else
#define BMS_ERROR (void)
#endif

bms_chg_info_def bms_chg_info;

bms_info_def bms_info;

OS_MUT can2_mutex;


u8 stop_charge = 0xff;  //ֹͣ����־  0xff:ֹͣ 0:����
u8 bms_reset = 0;  //��־

/***********************************************************************/

/*
 * TIM4����жϴ������(10ms)  ����BMS��ʱ
 */
void TIM4_IRQHandler(void)
{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		bms_info.timer_tick++;
}




CanRxMsg CAN2_RxMessage;

OS_SEM CAN2_rx_sem;

u32 CAN2_rx_buf[(sizeof(b_fifo_typ) + sizeof(CanRxMsg) * 10 + 10 * 2 + 3) / 4];

/*
 * @brief  CAN2�����жϴ������
 */
void CAN2_RX0_IRQHandler()
{
	if (CAN_GetITStatus(CAN2, CAN_IT_FMP0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
		CAN_Receive(CAN2, CAN_FIFO0, &CAN2_RxMessage);
		b_fifo_write(CAN2_rx_buf, (u8*)&CAN2_RxMessage, sizeof(CAN2_RxMessage));
		isr_sem_send(&CAN2_rx_sem);
	}
	else if (CAN_GetITStatus(CAN2, CAN_IT_FF0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FF0);
	}
	else if (CAN_GetITStatus(CAN2, CAN_IT_FOV0))
	{
		CAN_ClearITPendingBit(CAN2, CAN_IT_FOV0);
	}
/*
	{
		int i;
		BMS_MSG("CAN2 RX: ");
		BMS_MSG("    %08x  ", CAN2_RxMessage.ExtId);
		for(i = 0; i < 8; i++)
		{
			BMS_MSG("%02x ", CAN2_RxMessage.Data[i]);
		}
		BMS_MSG("\n");
	}
//	*/
}


/*
 * ʹ��CAN2����
 */
void CAN2_rx_en()
{
	CAN_FIFORelease(CAN2, CAN_FIFO0);
	CAN1->FA1R |= Bit(14);  //���������14
}

/*
 * �ر�CAN2����
 */
void CAN2_rx_dis()
{
	CAN1->FA1R &= ~Bit(14);
}

/*
 * CAN2��������
 */
void CAN2_send(CanTxMsg *tx)
{
	CAN_Transmit(CAN2, tx);

/*	
		{
		int i;
		BMS_MSG("CAN2 TX: ");
		BMS_MSG("    %08x  ", tx->ExtId);
		for(i = 0; i < 8; i++)
		{
			BMS_MSG("%02x ", tx->Data[i]);
		}
		BMS_MSG("\n");
	}
	//	*/
}


/*
 * ����CAN2 ���չ�����ID ��16λ
 *
 */
void bms_set_filter_id(u16 id)
{
	//FilterId  BMS ʹ�õ�14��
    CAN1->sFilterRegister[14].FR1 = (((id >> 13) & 0x0007) << 16) | ((id << 3) |CAN_ID_EXT | CAN_RTR_DATA);
    //MaskId
    CAN1->sFilterRegister[14].FR2 = 0x0007FFFF;
}



//�ְ�����  �����ݴ���8�ֽ�ʱ���÷ְ�����Э��
#define MULT_PKG_LEN 1100
u8 mult_pkg_buf[MULT_PKG_LEN];  //�������
u8 mult_pkg_rx_en;  //���յ��ְ���ʾʱ��λ   1:����  0:������

mult_pkg_info_def mult_pkg_info =
{
		0,
		0,
		0,
		0,
		0,
		{0, 0, 0},
		mult_pkg_buf,
};


/*
 * @brief ���յ�BMS�����Ͷ������ʱ���ڻ�ӦBMS ��ʾ��׼���ý�������
 */
void tp_send_cts(u8 pkg_cnt, u8 id, u8 *pgn)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1CECF456;

	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 17;  //�����ֽ�
	TxMsg.Data[1] = pkg_cnt;  //�ɷ��͵����ݰ���Ŀ
	TxMsg.Data[2] = id;  //��һ����Ҫ���͵����ݰ����

	TxMsg.Data[3] = 0xff;
	TxMsg.Data[4] = 0xff;

	TxMsg.Data[5] = pgn[0];
	TxMsg.Data[6] = pgn[1];
	TxMsg.Data[7] = pgn[2];

	CAN2_send(&TxMsg);
}

/*
 * ֪ͨ������ ������ݽ������
 */
void tp_send_EndofMsgAck()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1CECF456;

	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 19;  //�����ֽ�
	TxMsg.Data[1] = mult_pkg_info.len;  //�����ݳ���
	TxMsg.Data[2] = (u8)(mult_pkg_info.len >> 8);

	TxMsg.Data[3] = mult_pkg_info.pkg_cnt;  //������
	TxMsg.Data[4] = 0xff;

	TxMsg.Data[5] = mult_pkg_info.pgn[0];
	TxMsg.Data[6] = mult_pkg_info.pgn[1];
	TxMsg.Data[7] = mult_pkg_info.pgn[2];

	CAN2_send(&TxMsg);
}


u8 get_pgn(u32 id)
{
	return ((id >> 16) & 0x000000ff);
}

u8 get_pri(u32 id)
{
	return ((id >> 24) & 0x000000ff);
}




OS_SEM bms_start_sem;




u8 bms_test;
/*
 * @brief  BMSЭ���Ƿ������
 * @retval 0:���ڼ�� 1:�ɹ� 2:ʧ��
 */
u8 bms_test_isok()
{
	return bms_test;
}



/*
 * @brief           �������
 * @param start_type  ������ʽ   1:��̨     2:ˢ��    3:����Ա
 * @param chg_type  ��緽ʽ         0:�Զ�����   1:������   2:ʱ��    3:�����
 * @param value  ������ʽ����    ����0.01kwh   ʱ��:1����       ���:0.01Ԫ
 * @retval 1 �ɹ�   0 ʧ��
 */
int bms_start_charge(u8 start_type, u8 chg_type, u32 value)
{
	if(stop_charge != 0xff)
	{
		return -1;
	}
//	if(em_calc_init() != 0)
//	{
//		return -2;
//	}

	bms_chg_info.start_type = start_type;
	bms_chg_info.chg_type = chg_type;
	bms_chg_info.value = value;

	bms_test = 0;
	stop_charge = 0;  //
	os_sem_send(&bms_start_sem);
	return 0;
}

/*
 * @brief           ֹͣ���
 * @param r  		ֹͣԭ��  (�����bms.h ��  bms_info_def   �е�    stop_reason)
 */
void bms_stop_charge(u8 r)
{
	if(stop_charge == 0)
	{
		bms_info.stop_reason = r;
		stop_charge = 1;
		evc_out_onoff(0);  //�ر����
	}
}

/*
 * @brief  ������Ƿ����
 * @retval 0 δֹͣ  1:����ֹͣ���    0xff�����ֹͣ (������ͳ�Ʊ���)
 */
u8 bms_charge_complete()
{
	return stop_charge;
}



extern bms_ptl_handler_def bms_gb_event;
extern bms_ptl_handler_def bms_xgb_event;
/*
 * bms����
 */
void task_bms()
{
	u8 pgn, pri;
	CanRxMsg *BMS_rx_msg;
	b_fifo_data_typ *block;
	u8 temp;
	u8 v24;

	os_mut_init(&can2_mutex);

	os_sem_init(&bms_start_sem, 0);

	CAN2_rx_dis();
	bms_set_filter_id(0x56f4);  //CAN ID����


	while(1)
	{
		os_sem_wait(&bms_start_sem, 0xffff);  //�ȴ��ⲿ�����������
		memset(&bms_info, 0, sizeof(bms_info));

		evc_set_v_c(0, 0);

		v24 = 0;  //����12V

restart:

		if(v24 == 0)
		{
			io_bms_12v_en();  //���������Դ
		}
		else
		{
			io_bms_24v_en();  //���������Դ
		}

		/********************************************** BMSЭ���� ********************************************/

		/***************  �¹���Э��  **************/

		if(v24 == 0)  //12V�ż��
		{
			CAN2_rx_dis();
			os_sem_init(&CAN2_rx_sem, 0);
			b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //��ʼ��CAN2���ջ�����
			CAN2_rx_en();  //ʹ��CAN����

			temp = 25;
			while(temp--)
			{
				bms_xgb_send_chm();
				if(os_sem_wait(&CAN2_rx_sem, 250) != OS_R_TMO)  //�յ�����
				{
					while(1)
					{
						block = b_fifo_get(CAN2_rx_buf);  //��ȡ����
						if(block != NULL)
						{
							BMS_rx_msg = (CanRxMsg*)block->data;
							if(get_pgn(BMS_rx_msg->ExtId) == 0x27)  //�յ���Ӧ
							{
								bms_info.protocol = BMS_PROTOCOL_XGB;
								bms_info.event = &bms_xgb_event;
								BMS_MSG("BMS XGB\n");
								b_fifo_del(CAN2_rx_buf);  //ɾ������
								goto start;
							}
							b_fifo_del(CAN2_rx_buf);  //ɾ������
						}
						else
						{
							break;
						}
					}
				}
			}

			io_bms_12v_dis();
			os_dly_wait(5000);
			io_bms_12v_en();
		}

		/***************  ����Э��  **************/

		CAN2_rx_dis();
		os_sem_init(&CAN2_rx_sem, 0);
		b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //��ʼ��CAN2���ջ�����
		CAN2_rx_en();  //ʹ��CAN����

		temp = 25;
		while(temp--)
		{
			bms_gb_send_crm(0);
			if(os_sem_wait(&CAN2_rx_sem, 250) != OS_R_TMO)  //�յ�����
			{
				while(1)
				{
					block = b_fifo_get(CAN2_rx_buf);  //��ȡ����
					if(block != NULL)
					{
						BMS_rx_msg = (CanRxMsg*)block->data;
						if(get_pgn(BMS_rx_msg->ExtId) == PGN_60416)    //�յ���Ӧ
						{
							bms_info.protocol = BMS_PROTOCOL_GB;
							bms_info.event = &bms_gb_event;
							BMS_MSG("BMS GB\n");
							b_fifo_del(CAN2_rx_buf);  //ɾ������
							goto start;
						}
						b_fifo_del(CAN2_rx_buf);  //ɾ������
					}
					else
					{
						break;
					}
				}
			}
		}



		/****************************************/
start:

		if(bms_info.protocol == 0)  //���ʧ��
		{
			io_bms_24v_dis();
			io_bms_12v_dis();
			if(v24 == 0)
			{
				v24 = 1;
				CAN2_rx_dis();
				os_dly_wait(5000);
				goto restart;
			}
			bms_test = 2;
			stop_charge = 0xff;
			continue;  //���ʧ��
		}
		else
		{
			bms_test = 1;  //���ɹ�
		}

		/******************************************************************************************************/

		io_bms_dc_out_en();  //�����ϼ̵���   ����ص�ѹ

		bms_info.v12_v24 = v24;

		TIM_Cmd(TIM4, ENABLE);

reshk:  //��������

		bms_info.event->init();  //��ӦЭ���ʼ��

		while(1)
		{
			if(os_sem_wait(&CAN2_rx_sem, 10) != OS_R_TMO)
			{
				bms_info.timeout_cnt = 0;

				block = b_fifo_get(CAN2_rx_buf);

				if(block == NULL)
				{
					continue;
				}

				BMS_rx_msg = (CanRxMsg*)block->data;
				pgn = get_pgn(BMS_rx_msg->ExtId);
				pri = get_pri(BMS_rx_msg->ExtId);
				//BMS_MSG("PGN   %x\n", pgn);


				if(pgn == PGN_60416)  //can ���(����8�ֽ�)����Э��
				{
					if(pri != 0x1c)
					{
						goto del;
					}
					switch(BMS_rx_msg->Data[0])  //�����ֽ�
					{
					case 0x10:  //������
					{
						TP_CM_RTS_def *rts = (TP_CM_RTS_def*)&BMS_rx_msg->Data[0];
						u32 temp_pgn = 0;
						temp_pgn = rts->pgn[0] << 16;
						temp_pgn |= rts->pgn[1] << 8;
						temp_pgn |= rts->pgn[2];

						//PGN����
						if((temp_pgn == 0x000200) || (temp_pgn == 0x000600) || (temp_pgn == 0x001100) || (temp_pgn == 0x001500) || (temp_pgn == 0x001600) || (temp_pgn == 0x001700))
						{
							mult_pkg_info.len = rts->msg_lenth;
							mult_pkg_info.pkg_cnt = rts->pkg_cnt;
							mult_pkg_info.received_len = 0;
							mult_pkg_info.recv_pkg_id = 1;  //�ְ�id��1��ʼ
							mult_pkg_info.pgn[0] = rts->pgn[0];
							mult_pkg_info.pgn[1] = rts->pgn[1];
							mult_pkg_info.pgn[2] = rts->pgn[2];
							mult_pkg_info.pkg_rx_en = 0;
							if(mult_pkg_info.len <= MULT_PKG_LEN)  //���㹻�Ĵ洢�ռ�
							{
								tp_send_cts(rts->pkg_cnt, 1, rts->pgn);  //׼������BMS�������
								mult_pkg_info.pkg_rx_en = 1;  //ȫ�ܽ���
							}
						}
					}
					break;

					case 0xFF:  //��������
						mult_pkg_info.pkg_rx_en = 0;
						break;

					default:
						break;
					}
				}
				else if((pgn == PGN_60160) && (mult_pkg_info.pkg_rx_en))  //�������֡
				{
					u16 len = mult_pkg_info.len - mult_pkg_info.received_len;  //ʣ�೤��

					if(pri != 0x1c)  //��鱨�����ȼ�
					{
						goto del;
					}

					if(mult_pkg_info.recv_pkg_id != BMS_rx_msg->Data[0])  //���ְ�id   ������������id����
					{
						goto del;
					}

					if(mult_pkg_info.recv_pkg_id > mult_pkg_info.pkg_cnt)  //���ְ�id�Ϸ���
					{
						goto del;
					}

					if(len > 7)
					{
						memcpy(&mult_pkg_info.buf[mult_pkg_info.received_len], &BMS_rx_msg->Data[1], 7);  //��һ���ֽ�ΪID ����7�ֽ�����
						mult_pkg_info.received_len += 7;

						mult_pkg_info.recv_pkg_id++;  //������һ���ְ�
					}
					else  //���һ����
					{
						memcpy(&mult_pkg_info.buf[mult_pkg_info.received_len], &BMS_rx_msg->Data[1], len);
						mult_pkg_info.received_len += len;

						tp_send_EndofMsgAck();  //������ݽ������  ֪ͨ������

						mult_pkg_info.pkg_rx_en = 0;  //�������

						bms_info.event->msg_handler(mult_pkg_info.pgn[1], mult_pkg_info.buf, mult_pkg_info.received_len);  //������ӦЭ�鱨�Ĵ�����
					}
				}
				else  //������������
				{
					switch (pgn)
					//��鱨�����ȼ�
					{
					case PGN_9984: //BHM
					case PGN_4096: //BCL
					case PGN_7168: //BSD
					case PGN_4864: //BSM
						if (pri != 0x18)  //����Ȩ6
						{
							goto del;
							//ɾ������
						}
						break;

					case PGN_4352: //BCS
					case PGN_5376: //BMV
					case PGN_5632: //BMT
					case PGN_5888: //BSP
						if (pri != 0x1C)  //����Ȩ7
						{
							goto del;
							//ɾ������
						}
						break;

					case PGN_2304: //BRO
					case PGN_6400: //BST
						if (pri != 0x10)  //����Ȩ4
						{
							goto del;
							//ɾ������
						}
						break;

					case PGN_7680: //BEM
						if (pri != 0x08)   //����Ȩ2
						{
							goto del;
							//ɾ������
						}
						break;
					}
					bms_info.event->msg_handler(get_pgn(BMS_rx_msg->ExtId), BMS_rx_msg->Data, BMS_rx_msg->DLC);  //������ӦЭ�鱨�Ĵ�����
				}
del:			b_fifo_del(CAN2_rx_buf);  //�Ѵ��������ݰ���FIFO��ɾ��
			}

			{
				u8 ret = bms_info.event->poll();
				if(ret == 0xff)  //
				{
					bms_reset = 0;
					stop_charge = 1;  //������
					goto end;  //�������
				}
				else if(ret == 1)  //��Ҫ�л���24V������Դ
				{
					BMS_MSG("bms 24\n");
					io_bms_12v_dis();  //�Ͽ�������Դ
					CAN2_rx_dis();
					os_sem_init(&CAN2_rx_sem, 0);
					b_fifo_init(CAN2_rx_buf, sizeof(CAN2_rx_buf));  //��ʼ��CAN2���ջ�����
					os_dly_wait(5000);
					io_bms_24v_en();  //��24V������Դ
					bms_info.v12_v24 = 1;
					CAN2_rx_en();  //ʹ��CAN����
					goto reshk;  //��������ͨѶ
				}
			}

			os_dly_wait(5);
		}

end:
		evc_out_onoff(0);  //�رճ��ģ�����
		CAN2_rx_dis();  //ֹͣ���ձ���
		TIM_Cmd(TIM4, DISABLE);  //�ض�ʱ��
		temp = 0;
		while(evc_info.read.i > 100)  //С��1A�ŶϿ�����̵���
		{
			evc_out_onoff(0);  //�رճ��ģ�����
			os_dly_wait(500);
			temp++;
			if(temp == 10)
			{
				break;
			}
		}

		io_bms_dc_out_dis();
		io_bms_24v_dis();  //�رո�����Դ
		io_bms_12v_dis();
		stop_charge = 0xff;  //������
	}
}



