#include "RTL.h"
#include "stm32f10x.h"
#include "bms_xgb.h"
#include "bms.h"
#include "string.h"
#include "dbg.h"
#include "evc.h"
#include "shk.h"
#include "em.h"
#include "sys_cfg.h"
#include "delay.h"
#include "io.h"

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

//��¼�յ������ݰ���ʱ�� �����жϳ�ʱ
struct time_info_s
{
	u32 pgn256;
	u16 pgn256_cnt;
	u32 pgn512;
	u32 pgn1792;
	u16 pgn1792_cnt;
	u32 pgn2048;
	u16 pgn2048_cnt;
	u32 pgn1536;
	u32 pgn4096;
	u32 pgn4352;
	u32 pgn4608;
	u32 pgn4608_cnt;
	u32 pgn2560;
	u16 pgn2560_cnt;
	u32 pgn2304_0;
	u32 pgn2304_aa;
	u32 pgn6400;
	u32 pgn6656;
	u16 pgn6656_cnt;
	u32 pgn7168;
	u32 pgn7424;
	u32 pgn7424_cnt;

	u32 pgn7936;
	u16 pgn7936_cnt;

	u32 pgn9728;
	u16 pgn9728_cnt;

	u32 pgn9984;

	u8  timeout_pgn;

	u8  bms_timeout;  //��־�Ƿ���Ҫ���ͳ�ʱ������
	u8  current_pgn;

	u8  bms_err;  //������������  �跢�͹�����ϱ���
}bms_xgb_time_info;

/*
 * @brief ��־��Ҫ���ͳ�ʱ����
 * @param pgn
 */
static void bms_xgb_timeout(u8 pgn)
{
	if(bms_xgb_time_info.timeout_pgn == pgn)
	{
		return;
	}
	bms_xgb_time_info.pgn7936 = 0;
	bms_xgb_time_info.pgn7936_cnt = 0;
	bms_xgb_time_info.timeout_pgn = pgn;
	bms_xgb_time_info.bms_timeout = 1;
	bms_info.stage = BMS_stage_timeout;
	
	if(bms_xgb_time_info.timeout_pgn == PGN_512)
	{
		bms_info.msg_id = 1;
		memcpy(bms_info.msg, "���ձ�ʶ���ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_1536)
	{
		bms_info.msg_id = 2;
		memcpy(bms_info.msg, "���յ�س��������ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_2304)
	{
		bms_info.msg_id = 3;
		memcpy(bms_info.msg, "������ɳ��׼�����ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_4352)
	{
		bms_info.msg_id = 4;
		memcpy(bms_info.msg, "���յ�س����״̬���ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_4096)
	{
		bms_info.msg_id = 5;
		memcpy(bms_info.msg, "���յ�س�������ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_6400)
	{
		bms_info.msg_id = 6;
		memcpy(bms_info.msg, "������ֹ��籨�ĳ�ʱ", 30);
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_7168)
	{
		bms_info.msg_id = 7;
		memcpy(bms_info.msg, "����ͳ�Ʊ��ĳ�ʱ", 30);
	}
}



extern u8 stop_charge;


/*
 * @brief ����CHM����(PGN9728)
 */
void bms_xgb_send_chm(void)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1826F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 3;

	TxMsg.Data[0] = 1;
	TxMsg.Data[1] = 1;
	TxMsg.Data[2] = 0;

	CAN2_send(&TxMsg);
}


/*
 * @brief ����CRM����(PGN256)
 * @param code 0x00 ���ܱ�ʶ   0xAA �ܱ�ʶ
 */
void bms_xgb_send_crm(u8 code)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1801F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = code;  //�����ֽ�
	TxMsg.Data[1] = (u8)sys_cfg.id;  //
	TxMsg.Data[2] = (u8)(sys_cfg.id >> 8);
	TxMsg.Data[3] = (u8)(sys_cfg.id >> 16);
	TxMsg.Data[4] = (u8)(sys_cfg.id >> 24);
	TxMsg.Data[5] = 0xff;
	TxMsg.Data[6] = 0xff;
	TxMsg.Data[7] = 0xff;

	CAN2_send(&TxMsg);
}

/*
 * @brief ��������ʱ��ͬ����Ϣ����(PGN1792)
 */
void bms_xgb_send_cts()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1807F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 7;
	memset(TxMsg.Data, 0xff, 7);  //��Ч����
	CAN2_send(&TxMsg);
}

/*
 * @brief ������������������(PGN2048)
 */
void bms_xgb_send_cml()
{
	CanTxMsg TxMsg;

	u16 max_v = evc_info.max_v;  //��������ѹ  ��λ0.1V ��Χ0~750V
	u16 min_v = 100 * 10;  //��������ѹ  ��λ0.1V ��Χ0~750V
	u16 max_c = (400 - (evc_info.max_i / 100) * evc_info.module_cnt) * 10;  //����������  ��λ0.1A -400Aƫ��  ��Χ-400A ~ 0A
	u16 min_c = (400 - 0) * 10;  //��С����

	max_v = 7500;  //750V   ǿ��Ϊһ�����ֵ    2016.6.28
	max_c = 4000 - 4000;  //400A

	TxMsg.ExtId = 0x1808F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)max_v;
	TxMsg.Data[1] = (u8)(max_v >> 8);

	TxMsg.Data[2] = (u8)min_v;
	TxMsg.Data[3] = (u8)(min_v >> 8);

	TxMsg.Data[4] = (u8)max_c;
	TxMsg.Data[5] = (u8)(max_c >> 8);

	TxMsg.Data[6] = (u8)min_c;
	TxMsg.Data[7] = (u8)(min_c >> 8);

	CAN2_send(&TxMsg);
}


/*
 * @brief �������׼����������(PGN2560)
 */
void bms_xgb_send_cro()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x100AF456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 1;

	TxMsg.Data[0] = 0xAA;

	CAN2_send(&TxMsg);
}


/*
 * @brief �������״̬����(PGN4608)
 * @param en 0 ��ͣ���   1 ������
 */
void bms_xgb_send_ccs(u8 en)
{
	CanTxMsg TxMsg;

	u16 v = evc_info.read.v;  //��ѹ���ֵ  ��λ0.1V ��Χ0~750V
	u16 c = 4000 - (evc_info.read.i / 10);  //�������ֵ  ��λ0.1A -400Aƫ��  ��Χ-400A ~ 0A
	u16 t = bms_info.timer_tick / (100 * 60);  //�ۼƳ��ʱ��  ��λ����  ��Χ0~600min

	TxMsg.ExtId = 0x1812F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)v;
	TxMsg.Data[1] = (u8)(v >> 8);

	TxMsg.Data[2] = (u8)c;
	TxMsg.Data[3] = (u8)(c >> 8);

	TxMsg.Data[4] = (u8)t;
	TxMsg.Data[5] = (u8)(t >> 8);

	TxMsg.Data[6] = 0xfc | en;

	TxMsg.Data[7] = 0xff;

	CAN2_send(&TxMsg);
}


/*
 * @brief ������ֹ��籨��(PGN6656)
 */
void bms_xgb_send_cst()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x101AF456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 4;

	TxMsg.Data[0] = 0x00;
	TxMsg.Data[1] = 0x00;
	TxMsg.Data[2] = 0xf0;
	TxMsg.Data[3] = 0xf0;

	if(bms_info.bms_reuest_stop)  //BMS����ֹͣ
	{
		TxMsg.Data[0] |= 0x40;
	}
	else if(bms_info.stop_reason == 7)  //�������������
	{
		TxMsg.Data[0] |= 0x10;  //
		TxMsg.Data[1] |= 0x04;
	}
	else  //�˹���ֹ
	{
		TxMsg.Data[0] |= 0x04;
	}



	CAN2_send(&TxMsg);
}


/*
 * @brief ����ͳ�����ݱ���(PGN7424)
 */
void bms_xgb_send_csd()
{
	CanTxMsg TxMsg;

	u16 t = (bms_info.timer_tick / 100) / 60; //���ʱ��
	u16 kWh = em_info.kwh / 10;  //�������

	TxMsg.ExtId = 0x181DF456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)t;
	TxMsg.Data[1] = (u8)(t >> 8);

	TxMsg.Data[2] = (u8)kWh;
	TxMsg.Data[3] = (u8)(kWh >> 8);

	TxMsg.Data[4] = (u8)sys_cfg.id;  //
	TxMsg.Data[5] = (u8)(sys_cfg.id >> 8);
	TxMsg.Data[6] = (u8)(sys_cfg.id >> 16);
	TxMsg.Data[7] = (u8)(sys_cfg.id >> 24);

	CAN2_send(&TxMsg);
}



/*
 * @brief ������(PGN7936)
 */
void bms_xgb_send_cem()
{
	CanTxMsg TxMsg;


	TxMsg.ExtId = 0x081FF456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 4;

	TxMsg.Data[0] = 0xfc;
	TxMsg.Data[1] = 0xf0;

	TxMsg.Data[2] = 0xc0;
	TxMsg.Data[3] = 0xfc;

	if(bms_xgb_time_info.timeout_pgn == PGN_512)
	{
		TxMsg.Data[0] |= 0x01;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_1536)
	{
		TxMsg.Data[1] |= 0x01;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_2304)
	{
		TxMsg.Data[1] |= 0x04;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_4352)
	{
		TxMsg.Data[2] |= 0x01;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_4096)
	{
		TxMsg.Data[2] |= 0x04;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_6400)
	{
		TxMsg.Data[2] |= 0x10;
	}
	else if(bms_xgb_time_info.timeout_pgn == PGN_7168)
	{
		TxMsg.Data[3] |= 0x01;
	}
	CAN2_send(&TxMsg);
}


/*
 * @brief ������ϱ��� ��ǰ������(PGN8192)
 */
void bms_xgb_send_DM1()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1820F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 4;

	//3522 = 0x0DC2
	TxMsg.Data[0] = 0xc2;  //SPN3522
	TxMsg.Data[1] = 0x0d;

	TxMsg.Data[2] = (u8)((7) << 3);  //����ģʽ
	TxMsg.Data[3] = 1;  	//��������

	CAN2_send(&TxMsg);
}


/*
 * @brief ������ϱ��� ���׼������(PGN8704)
 */
void bms_xgb_send_DM3()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1822F456;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 2;

	TxMsg.Data[0] = 1;  //��ǰ���������
	TxMsg.Data[1] = 0;  //��ʷ���������

	CAN2_send(&TxMsg);
}




/*
 * ���յ��������ݴ���
 */
void bms_xgb_msg_handler(u8 pgn, u8* msg, u16 len)
{
	switch(pgn)
	{
	/************************** ������ֽ׶� ********************/
	case PGN_9984:
	{
		bms_xgb_time_info.pgn9984 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;
	case PGN_512:  //BRM BMS�ͳ�����ʶ����
	{
		PGN512_Def *temp = (PGN512_Def*)msg;
		BMS_MSG("PGN512 type:%x V:%d Ah:%d\n", temp->battery_type, temp->V / 10, temp->Ah / 10);

		bms_info.battery_type = temp->battery_type;
		bms_info.Ah = temp->Ah;
		bms_info.V  = temp->V;

		if(bms_xgb_time_info.pgn512 == 0)  //��һ���յ�
		{
			bms_xgb_time_info.pgn256_cnt = 0;  //���¼���   CRM aa  ����
			bms_xgb_time_info.pgn256 = 0;
		}

		bms_xgb_time_info.pgn512 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;
	/***********************************************************/



	/************************ ���������ý׶� ******************/
	case PGN_1536:  //BCP �������س���������
	{
		PGN1536_Def *temp = (PGN1536_Def*)msg;

		bms_info.limt_v = temp->limt_v;
		bms_info.limt_i = 4000 - temp->limt_i;
		bms_info.limt_v_single = temp->limt_v_single;
		bms_info.limt_t = temp->limt_t;
		bms_info.SOC = temp->SOC / 10;

		//BMS_MSG("PGN1536 limt_v_single:%d limt_v:%d V:%d kWh:%d SOC:%d\n", temp->limt_v_single / 100, temp->limt_v/10, temp->v / 10, temp->max_kWh / 10, temp->SOC/10);

		bms_xgb_time_info.pgn1536 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;

	case PGN_2304:  //BRO ���׼������������
	{
		//BMS_MSG("PGN_2304 %x\n", msg[0]);
		if(msg[0] == 0xAA)  //׼�����  ���Գ��
		{
			bms_xgb_time_info.pgn2304_aa = bms_info.timer_tick;  //�����յ�����ʱ��
		}
		else  //���ܳ��
		{
			bms_xgb_time_info.pgn2304_0 = bms_info.timer_tick;  //�����յ�����ʱ��
		}
	}
	break;
	/***********************************************************/



	/************************** ���׶� ************************/
	case PGN_4352:  //BCS ��س����״̬����
	{
		PGN4352_Def *temp = (PGN4352_Def*)msg;
		bms_info.out_v = temp->v;
		bms_info.out_i = 4000 - temp->i;
		bms_info.SOC = temp->SOC;
		bms_info.max_v_single = temp->max_v_single & 0x0fff;
		bms_info.remain_t = temp->remain_t;

		bms_xgb_time_info.pgn4352 = bms_info.timer_tick;

		if(bms_info.max_v_single > bms_info.limt_v_single)
		{
			bms_stop_charge(22);
		}
		//BMS_MSG("BCS   %d %d %d %d %d %d %d %d\n", msg->Data[0], msg->Data[1],msg->Data[2],msg->Data[3],msg->Data[4],msg->Data[5],msg->Data[6],msg->Data[7]);
	}
	break;

	case PGN_4096:  //BCL ��س��������
	{
		PGN4096_Def *temp = (PGN4096_Def*)msg;
		//evc_set_v_c(temp->v, (4000 - temp->i) * 10);  //����EVCģ���ѹ����
		//�����ѹ��5V   2016.1.8
		evc_set_v_c((temp->v + 50), (4000 - temp->i) * 10);  //����EVCģ���ѹ����
		bms_xgb_time_info.pgn4096 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;

	case PGN_4864:  //BSM ���״̬��Ϣ����
	{
		u8 wait = (msg[6] >> 4) & 0x03;
		bms_info.max_temp = msg[1];
		bms_info.min_temp = msg[3];

		if(bms_info.max_temp > bms_info.limt_t)  //�������
		{
			bms_stop_charge(21);
		}

		if((msg[5] != 0) || ((msg[6] & 0x0f) != 0))  //�쳣
		{
			bms_stop_charge(9);
		}

		if(wait == 0)
		{
			bms_info.bms_wait = 1;
			evc_out_onoff(0);  //�ر����
		}
		else if((wait == 1) && (bms_info.bms_wait == 1))
		{
			bms_info.bms_wait = 0;
			evc_out_onoff(0xff);  //�����
		}
	}
	break;

	case PGN_6400:  //BST BMS��ֹ��籨��
	{
		//BMS_MSG("PGN_6400---------------- %x\n", msg[0]);
		if(!bms_info.stop_reason)
		{
			if((msg[0] & 0x03) == 0x01)
			{
				bms_info.stop_reason = 1;
			}
			else if(((msg[0] >> 2) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 2;
			}
			else if(((msg[0] >> 4) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 3;
			}
			else if(((msg[1] >> 0) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 4;
			}
			else if(((msg[1] >> 2) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 5;
			}
			else if(((msg[1] >> 4) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 6;
			}
			else if(((msg[1] >> 6) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 7;
			}
			else if(((msg[2] >> 0) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 8;
			}
			else if(((msg[2] >> 2) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 9;
			}
			else if(((msg[3] >> 0) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 10;
			}
			else if(((msg[3] >> 2) & 0x03) == 0x01)
			{
				bms_info.stop_reason = 11;
			}
			else
			{
				bms_info.stop_reason = 15;
			}
		}

		if(bms_xgb_time_info.pgn6400 == 0)  //��һ���յ�
		{
			bms_xgb_time_info.pgn6656_cnt = 0;  //���¼���
		}
		bms_xgb_time_info.pgn6400 = bms_info.timer_tick;  //�����յ�����ʱ��

		if((bms_info.bms_reuest_stop == 0) && (stop_charge == 0))
		{
			bms_info.bms_reuest_stop = 1;
		}

		stop_charge = 1; //��Ҫֹͣ���
		
		evc_out_onoff(0);  //�رճ��ģ�����
	}
	break;

	case PGN_5376:  //BMV ���嶯������״̬��Ϣ����
	break;

	case PGN_5632:  //BMT ���������¶ȱ���
	break;

	case PGN_5888:  //BSP ��������Ԥ������
	break;

	/***********************************************************/




	/************************ �������׶� **********************/
	case PGN_7168:  //BSD BMSͳ�����ݱ���
	{
		PGN7168_Def *temp = (PGN7168_Def*)msg;
		bms_xgb_time_info.pgn7168 = bms_info.timer_tick;  //�����յ�����ʱ��
		bms_info.SOC = temp->SOC;
	}
	break;
	/***********************************************************/


	case PGN_7680:  //BEM BMS������
	{
		BMS_ERROR("PGN_7680 %02x %02x %02x %02x\n", msg[0], msg[1], msg[2] ,msg[3]);
		if(bms_info.err_id == 0)
		{
			bms_info.err_id = (msg[3] << 24) | (msg[2] << 16) | (msg[1] << 8) | msg[0];
		}
	}
	break;


	default:
		BMS_ERROR("bms_gb other msg %x\n", pgn);
		break;
	}
}


u32 bms_dly_1s;

/*
 *
 */
int bms_xgb_poll()
{
	switch(bms_info.stage)
	{
	case BMS_stage_handshake:

		if((bms_xgb_time_info.pgn9984 == 0) && ((bms_info.timer_tick - bms_xgb_time_info.pgn9728) >= 25))  //���յ�BHM֮ǰ ��250msΪ���ڷ���
		{
			bms_xgb_send_chm();
			bms_xgb_time_info.pgn9728_cnt++;
			bms_xgb_time_info.pgn9728 = bms_info.timer_tick;  //���淢��ʱ��
			if(bms_xgb_time_info.pgn9728_cnt == 21)  //5sû�յ�BHM
			{
				bms_xgb_time_info.pgn9984 = 1;  //BHM��ʱҲҪ����CRM 00
				return 2;  //��ʱ
			}
		}
		if((bms_xgb_time_info.pgn9984 != 0) &&(bms_xgb_time_info.pgn512 == 0) && ((bms_info.timer_tick - bms_xgb_time_info.pgn256) >= 25))  //���յ�BRM֮ǰ ��250msΪ���ڷ���CRM 0
		{
			bms_xgb_send_crm(0);
			bms_xgb_time_info.pgn256_cnt++;
			bms_xgb_time_info.pgn256 = bms_info.timer_tick;  //���淢��ʱ��
		}
		else if((bms_xgb_time_info.pgn512 != 0) && ((bms_info.timer_tick - bms_xgb_time_info.pgn256) >= 25))  //�յ�BRM֮��  ��250msΪ���ڷ���CRM aa
		{
			bms_xgb_send_crm(0xaa);
			bms_xgb_time_info.pgn256_cnt++;  //��һ���յ�BRM  ���bms_gb_time_info.pgn256_cnt bms_gb_time_info.pgn256����
			bms_xgb_time_info.pgn256 = bms_info.timer_tick;  //���淢��ʱ��
		}

		if((bms_xgb_time_info.pgn256_cnt >= 21) && (bms_xgb_time_info.pgn512 == 0))  //5sû�յ�BRM
		{
			bms_xgb_timeout(PGN_512);  //��ʱ  ���ͳ�ʱ����
			bms_stop_charge(23);
		}
		else if((bms_xgb_time_info.pgn256_cnt >= 21) && (bms_xgb_time_info.pgn512 != 0) && (bms_xgb_time_info.pgn1536 == 0))  //�յ�BRM֮��5sû�յ�BCP
		{
			bms_xgb_timeout(PGN_1536);  //��ʱ  ���ͳ�ʱ����
			bms_stop_charge(24);
		}

		if(bms_xgb_time_info.pgn1536 != 0)
		{
			bms_info.stage = BMS_stage_config;
			BMS_MSG("BMS_stage_config\n");
		}


		if(stop_charge != 0)
		{
			return 0xff;
		}

		break;

	case BMS_stage_config:  //�յ�BCP��������ý׶�

		if(stop_charge != 0)
		{
			return 0xff;
		}

		if(bms_xgb_time_info.pgn2304_aa == 0)  //û�յ�BRO  ���׼������������
		{
			if(bms_info.timer_tick - bms_xgb_time_info.pgn1792 >= 50)
			{
				bms_xgb_time_info.pgn1792_cnt++;
				bms_xgb_send_cts();  //����ʱ��ͬ����Ϣ
				bms_xgb_time_info.pgn1792 = bms_info.timer_tick;
			}

			if(bms_info.timer_tick - bms_xgb_time_info.pgn2048 >= 25)
			{
				bms_xgb_time_info.pgn2048_cnt++;
				bms_xgb_send_cml();  //��������������
				bms_xgb_time_info.pgn2048 = bms_info.timer_tick;
			}

			if((bms_xgb_time_info.pgn2304_0 == 0) && (bms_xgb_time_info.pgn2048_cnt >= 21))  //5sû�յ�BRO 0
			{
				bms_xgb_timeout(PGN_2304);  //��ʱ  ���ͳ�ʱ����
				bms_stop_charge(14);
			}
			if((bms_xgb_time_info.pgn2304_aa == 0) && (bms_xgb_time_info.pgn2048_cnt >= 240))  //1��û�յ�BRO aa
			{
				bms_xgb_timeout(PGN_2304);  //��ʱ  ���ͳ�ʱ����
				bms_stop_charge(14);
			}
		}
		else  //�յ�BRO aa
		{
			if((bms_xgb_time_info.pgn4352 == 0) && ((bms_info.timer_tick - bms_xgb_time_info.pgn2560) >= 25))  //250ms���ڷ���cro  ֱ���յ�BCS
			{
				bms_xgb_send_cro();  // �������׼������

				if(bms_xgb_time_info.pgn2560 == 0)   //��һ�η���CRO AA
				{
					delay_reset(&bms_dly_1s);
				}

				bms_xgb_time_info.pgn2560 = bms_info.timer_tick;
				bms_xgb_time_info.pgn2560_cnt++;
				if(bms_xgb_time_info.pgn2560_cnt >= 21)  //5sû�յ�BCS
				{
					bms_xgb_timeout(PGN_4352);  //��ʱ  ���ͳ�ʱ����
					bms_stop_charge(18);
				}
			}

			if(bms_xgb_time_info.pgn2560 != 0)
			{
				if(delay_timeout(&bms_dly_1s, 1050) == 0)  //����CRO AA  1���
				{
					if(bms_xgb_time_info.pgn4096 == 0)  //û�յ�BCL
					{
						bms_xgb_timeout(PGN_4096);  //��ʱ  ���ͳ�ʱ����
						bms_stop_charge(19);
					}
				}
			}

			if((bms_xgb_time_info.pgn4352 != 0) && (bms_xgb_time_info.pgn4096 != 0))  //�յ�BCS
			{
				io_bms_dc_out_en();
				evc_set_v_c(0, 0);
				evc_out_onoff(0xff);  //�������
				bms_info.stage = BMS_stage_charging;  //������׶�
				bms_xgb_time_info.pgn4096 = bms_info.timer_tick;  //
				
				BMS_MSG("BMS_stage_charging\n");
			}
		}
		break;

	case BMS_stage_charging:  //�յ�BCS������׶�

		if(stop_charge == 0)
		{
				if((bms_xgb_time_info.pgn4352 != 0) && (bms_info.timer_tick - bms_xgb_time_info.pgn4608) >= 5)  //50ms ����һ��ccs
				{
					//���ͳ������״̬����      bcs��bcl�յ����ٷ�ccs
					if(bms_info.bms_wait == 0)
					{
						bms_xgb_send_ccs(1);
					}
					else
					{
						bms_xgb_send_ccs(0);
					}

					bms_xgb_time_info.pgn4608 = bms_info.timer_tick;
					bms_xgb_time_info.pgn4608_cnt++;
				}

				if((bms_info.timer_tick - bms_xgb_time_info.pgn4096) >= 100)  //BCL ��س��������  1sû�յ���ʱ  ֹͣ���
				{
					//ֹͣ���
					evc_out_onoff(0);  //�ر����

					bms_stop_charge(19);

					bms_xgb_timeout(PGN_4096);  //��ʱ  ���ͳ�ʱ����
				}

				if((bms_info.timer_tick - bms_xgb_time_info.pgn4352) >= 500)  //��� BCS
				{
					bms_xgb_timeout(PGN_4352);  //��ʱ  ���ͳ�ʱ����
					bms_stop_charge(18);
				}
		}
		else  //��Ҫֹͣ���
		{
			if((bms_xgb_time_info.pgn6656 == 0) || ((bms_info.timer_tick - bms_xgb_time_info.pgn6656) >= 1))  //����ֹͣ��籨��  ����10ms
			{
				bms_xgb_send_cst();  //���ͳ�����ֹ��籨��
				bms_xgb_time_info.pgn6656 = bms_info.timer_tick;
				bms_xgb_time_info.pgn6656_cnt++;

				if((bms_xgb_time_info.pgn6656_cnt >= 500) && (bms_xgb_time_info.pgn6400 == 0))  //�ȴ�bms���ֹͣ���ĳ�ʱ
				{
					bms_xgb_timeout(PGN_6400);  //��ʱ  ���ͳ�ʱ����
				}

				if((bms_xgb_time_info.pgn6656_cnt >= 500 * 2) && (bms_xgb_time_info.pgn6400 != 0) && (bms_xgb_time_info.pgn7168 == 0))  //�ȴ�bms���ͳ�Ʊ��ĳ�ʱ
				{
					//��һ���յ�pgn6400ʱ��pgn6656_cnt����
					bms_xgb_timeout(PGN_7168);  //��ʱ  ���ͳ�ʱ����
				}
				else if((bms_xgb_time_info.pgn6400 != 0) && (bms_xgb_time_info.pgn7168 != 0))
				{
					bms_info.stage = BMS_stage_end;  //����������׶�
					BMS_MSG("BMS_stage_end\n");
				}
			}
		}
		break;

	case BMS_stage_end:

		if((bms_xgb_time_info.pgn7424 == 0) || ((bms_info.timer_tick - bms_xgb_time_info.pgn7424) >= 25))
		{
			bms_xgb_time_info.pgn7424 = bms_info.timer_tick;
			bms_xgb_time_info.pgn7424_cnt++;
			bms_xgb_send_csd();
			if(bms_xgb_time_info.pgn7424_cnt == 10)  //��10��
			{
				return 0xff;
			}
		}
		break;


	case BMS_stage_timeout:

		if(bms_xgb_time_info.bms_timeout)  //��Ҫ���ʹ�����
		{
			if((bms_xgb_time_info.pgn7936 == 0) || ((bms_info.timer_tick - bms_xgb_time_info.pgn7936) >= 25))  //������  ����250ms
			{
				bms_xgb_send_cem();
				bms_xgb_time_info.pgn7936 = bms_info.timer_tick;
				bms_xgb_time_info.pgn7936_cnt++;
				BMS_MSG("BMS_stage_timeout %d\n", bms_xgb_time_info.timeout_pgn);
				if(bms_xgb_time_info.pgn7936_cnt >= 21)  //5���ֹͣ����CEM
				{
					//�������Ӵ���������3��
					if((bms_info.reset_cnt < 3) && (bms_xgb_time_info.timeout_pgn != PGN_6400) && (bms_xgb_time_info.timeout_pgn != PGN_7168))  //�ǳ������׶α��ĳ�ʱ    ��������
					{
						memset(&bms_xgb_time_info, 0, sizeof(bms_xgb_time_info));
						bms_info.stage = BMS_stage_handshake;
						bms_xgb_time_info.pgn9984 = 1;  //��һ����0ֵ

						bms_info.reset_cnt++;
					}
					else
					{
						return 0xff;  //ֹͣ
					}

					return 1;  //ֹͣ
				}
			}
		}
		break;
	}



	if((!shk_isok()) && (!bms_xgb_time_info.bms_err))  //���ǹ�Ѱγ�  ����
	{
		if(bms_info.stage == BMS_stage_charging)
		{
			evc_out_onoff(0);  //�ر����
			io_bms_dc_out_dis();
			bms_xgb_time_info.bms_err = 1;
			bms_xgb_send_DM1();  //����PGN8192����
			bms_xgb_send_DM3();  //����PGN8704����
			bms_stop_charge(7);
		}
	}

	return 0;
}

/*
 * CAN���ݽ��ճ�ʱ
 */
void bms_xgb_timeout_handler()
{

}

/*
 * ��ʼ��
 */
void bms_xgb_init()
{
	memset(&bms_xgb_time_info, 0, sizeof(bms_xgb_time_info));
	bms_xgb_send_chm();
	bms_xgb_time_info.pgn9728 = bms_info.timer_tick;  //���淢��ʱ��
	bms_info.stage = BMS_stage_handshake;
	BMS_MSG("BMS_stage_handshake\n");
}

/*
 * ������Ӧ������
 */
bms_ptl_handler_def bms_xgb_event =
{
		bms_xgb_init,
		bms_xgb_msg_handler,
		bms_xgb_timeout_handler,
		bms_xgb_poll,
};
