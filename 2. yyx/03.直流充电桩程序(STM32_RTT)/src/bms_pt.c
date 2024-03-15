#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "bms.h"
#include "bms_pt.h"
#include "bms_gb.h"
#include "string.h"
#include "evc.h"
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

extern u8 stop_charge;


//��¼�յ������ݰ���ʱ�� �����жϳ�ʱ
struct time_info_s
{
	u32 pgn256;
	u32 pgn512;
	u32 pgn4096;
}bms_pt_time_info;


/*
 * @brief ����CRM����(PGN256)
 * @param code 0x00 ���ܱ�ʶ   0x01 �ܱ�ʶ
 */
void bms_pt_send_crm(u8 code)
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1801F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = 0;  //�������
	TxMsg.Data[1] = 1;  //���ͷ���

	TxMsg.Data[2] = 0;  //����λ��  0:����   1:����

	TxMsg.Data[3] = code;  //

	//���������������,ASCII��
	TxMsg.Data[4] = 0xff;
	TxMsg.Data[5] = 0xff;
	TxMsg.Data[6] = 0xff;
	TxMsg.Data[7] = 0xff;

	CAN2_send(&TxMsg);
}


/*
 * @brief ��������ʱ��ͬ����Ϣ����(PGN1792)
 */
void bms_pt_send_cts()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x1807F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 7;
	memset(TxMsg.Data, 0xff, 7);  //��Ч����
	CAN2_send(&TxMsg);
}

/*
 * @brief ������������������(PGN2048)
 */
void bms_pt_send_cml()
{
	CanTxMsg TxMsg;

	u16 max_v = 750 * 10;  //��������ѹ  ��λ0.1V ��Χ0~800V
	u16 min_v = 100 * 10;  //��������ѹ  ��λ0.1V ��Χ0~800V
	u16 max_i = (800 - 20 * evc_info.module_cnt) * 10;  //����������  ��λ0.1A -800Aƫ��  ��Χ-800A ~ 0A

	TxMsg.ExtId = 0x1808F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 8;

	TxMsg.Data[0] = (u8)max_v;
	TxMsg.Data[1] = (u8)(max_v >> 8);

	TxMsg.Data[2] = (u8)min_v;
	TxMsg.Data[3] = (u8)(min_v >> 8);

	TxMsg.Data[4] = (u8)max_i;
	TxMsg.Data[5] = (u8)(max_i >> 8);

	TxMsg.Data[6] = 0;  //����ŵ������A�� ������������ķŵ���������������߱��ŵ繦��Ӧ ����Ϊ 0 ���ݷֱ��ʣ�2A/λ��0A ƫ���������ݷ�Χ��0~500A��
	TxMsg.Data[7] = 0;  //����ŵ��ܳ���ʱ�䣨s�� �����������߱��ŵ繦��Ӧ����Ϊ 0	���ݷֱ��ʣ�1s/λ��0s ƫ���������ݷ�Χ��0~255s��
	CAN2_send(&TxMsg);
}

/*
 * @brief �������׼����������(PGN2560)
 */
void bms_pt_send_cro()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x140AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 1;

	TxMsg.Data[0] = 0x04;

	CAN2_send(&TxMsg);
}


/*
 * @brief �������״̬����(PGN4608)
 */
void bms_pt_send_ccs()
{
	CanTxMsg TxMsg;

	u16 v = evc_info.read.v;  //��ѹ���ֵ  ��λ0.1V ��Χ0~800V
	u16 c = 8000 - (evc_info.read.i / 10);  //�������ֵ  ��λ0.1A -800Aƫ��  ��Χ-800A ~ 0A
	u16 t = bms_info.timer_tick / (100 * 60);  //�ۼƳ��ʱ��  ��λ����  ��Χ0~6000min

	TxMsg.ExtId = 0x1814F4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 6;

	TxMsg.Data[0] = (u8)v;
	TxMsg.Data[1] = (u8)(v >> 8);

	TxMsg.Data[2] = (u8)c;
	TxMsg.Data[3] = (u8)(c >> 8);

	TxMsg.Data[4] = (u8)t;
	TxMsg.Data[5] = (u8)(t >> 8);

	CAN2_send(&TxMsg);
}


/*
 * @brief ������ֹ��籨��(PGN6656)
 */
void bms_pt_send_cst()
{
	CanTxMsg TxMsg;

	TxMsg.ExtId = 0x081AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 3;

	TxMsg.Data[0] = 0x03;
	TxMsg.Data[1] = 0x00;
	TxMsg.Data[2] = 0x00;

	CAN2_send(&TxMsg);
}



/*
 * @brief ����ͳ�����ݱ���(PGN7424)
 */
void bms_pt_send_csd()
{
	CanTxMsg TxMsg;

	u16 t = 0; //���ʱ��
	u16 kWh = 0;  //�������
	u8  id = 1;  //�������

	TxMsg.ExtId = 0x181AF4E5;
	TxMsg.RTR = CAN_RTR_DATA;
	TxMsg.IDE = CAN_ID_EXT;
	TxMsg.DLC = 5;

	TxMsg.Data[0] = (u8)t;
	TxMsg.Data[1] = (u8)(t >> 8);

	TxMsg.Data[2] = (u8)kWh;
	TxMsg.Data[3] = (u8)(kWh >> 8);

	TxMsg.Data[4] = (u8)id;

	CAN2_send(&TxMsg);
}

/*
 * ���յ��������ݴ���
 */
void bms_pt_msg_handler(u8 pgn, u8* msg, u16 len)
{
	switch(pgn)
	{
	/************************** ������ֽ׶� ********************/
	case PGN_512:  //BRM BMS�ͳ�����ʶ����
	{
		PGN512_Def *temp = (PGN512_Def*)msg;
		BMS_MSG("PGN512 type:%x V:%d Ah:%d\n", temp->battery_type, temp->V / 10, temp->Ah / 10);

		bms_info.battery_type = temp->battery_type;
		bms_info.Ah = temp->Ah;
		bms_info.V  = temp->V;

		bms_pt_send_crm(0x01);  //ȷ�ϱ�ʶ
		bms_pt_time_info.pgn512 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;


	case PT_PGN_12288:  //VBI �����͵�ر�ʶ
	{
		msg[20] = 0;
		BMS_MSG("PT_PGN_12288 %c%c%c VIN:\n", msg[0], msg[1], msg[2]);
		bms_info.stage = BMS_stage_config;
		//for(int i = 3; i < 20; i++)
		//{
			//BMS_MSG("%x", msg->buf[i]);
		//}
	}
	break;
	/***********************************************************/




	/************************** ���������ý׶� ****************/
	case PGN_1536:  //BCP ���س���������
	{
		PT_PGN1536_Def *temp = (PT_PGN1536_Def*)msg;

		bms_info.limt_v = temp->limt_v;
		bms_info.limt_i = 8000 - temp->limt_i;
		BMS_MSG("PT_PGN1536 limt_v_single:%d limt_i:%d limt_v:%d V:%d Ah:%d SOC:%d\n", temp->limt_v_single / 100, (8000 - temp->limt_i)/10,temp->limt_v/10, temp->v / 10, temp->max_Ah, temp->SOH);

		bms_pt_send_cts();  //����ʱ��ͬ����Ϣ  �������ͬ
		os_dly_wait(10);
		bms_pt_send_cml();  //��������������
		bms_info.stage = BMS_stage_config;
	}
	break;


	case PGN_2304:  //BRO ���׼������������
	{
		BMS_MSG("PGN_2304 %x\n", msg[0]);
		if(msg[0] == 0x04)  //׼�����  ���Գ��
		{
			bms_pt_send_cro();  // �������׼������
		}
		else  //���ܳ��
		{

		}
	}
	break;
	/***********************************************************/




	/************************** ���׶� ************************/
	case PGN_4096:  //BCL ��س��������
	{
		PGN4096_Def *temp = (PGN4096_Def*)msg;

		//BMS_MSG("PGN_4096 v:%d  i:%d  mode:%d\n", temp->v / 10, 800 - temp->i/10, temp->cc_cv);
		//evc_set_v_c(temp->v, (8000 - temp->i) * 10);  //����EVCģ���ѹ����
		//�����ѹ��5V   2016.1.13
		evc_set_v_c((temp->v + 50), (8000 - temp->i) * 10);  //����EVCģ���ѹ����

		bms_pt_send_ccs();  //���ͳ������״̬����

		if(bms_info.stage != BMS_stage_end)
		{
			bms_info.stage = BMS_stage_charging;  //������׶�
		}

		bms_pt_time_info.pgn4096 = bms_info.timer_tick;  //�����յ�����ʱ��
	}
	break;

	case PGN_4352:  //BCS ��س����״̬����
	{
		PT_PGN4352_Def *temp = (PT_PGN4352_Def*)msg;
		bms_info.out_v = temp->v;
		bms_info.out_i = 8000 - temp->i;
		bms_info.SOC = temp->SOC;
		bms_info.remain_t = temp->remain_t;
		bms_info.max_temp = temp->max_t;
		//BMS_MSG("PGN_4352 v:%d i:%d mint:%d maxt:%d soc:%d remaintime:%d\n", temp->v/10,800-temp->i/10, temp->min_t - 50, temp->max_t-50,temp->SOC,temp->remain_t);
	}
	break;


	case PGN_4864:  //BSM ���״̬��Ϣ����
	{
		//bms_info.max_v_single = ((msg->Data[1] << 8) | msg->Data[0]) & 0x0fff;  //������أ���С������Ԫ����ѹ�����ݷֱ��ʣ�0.01V
		//bms_info.max_temp = msg->Data[3];  //��ߵ����¶�
		//BMS_MSG("PGN_4864 max_v_single %d 10mv\n", bms_info.max_v_single);
	}
	break;

	case PGN_6400:  //BST BMS��ֹ��籨��
	{
		BMS_MSG("PGN_6400---------------- %x\n", msg[0]);

		bms_pt_send_cst();  //������ֹ��籨��

		bms_info.stage = BMS_stage_end;
	}
	break;

	case PGN_5376:  //BMV ���嶯������״̬��Ϣ����
	{
		BMS_MSG("PGN_5376\n");
	}
	break;

	case PGN_5632:  //BMT ���������¶ȱ���
	{
		BMS_MSG("PGN_5632\n");
	}
	break;

	case PGN_5888:  //BSOC ������ɵ����� SOC ֵ����
	{
		BMS_MSG("PGN_5888\n");
	}
	break;

	/***********************************************************/




	/************************ �������׶� **********************/
	case PGN_7168:  //BSD BMSͳ�����ݱ���
	{
		PGN7168_Def *temp = (PGN7168_Def*)msg;
		bms_info.SOC = temp->SOC;
		bms_info.min_v_single = temp->min_v_single / 10;
		bms_info.max_v_single = temp->max_v_single / 10;
		bms_info.min_temp = temp->min_temp_single;
		bms_info.max_temp = temp->max_temp_single;

		if(bms_info.stage == BMS_stage_end)
		{
			BMS_MSG("PGN_7168  end charging\n");
			bms_pt_send_csd();  //����ͳ�����ݱ���
		}
	}
	break;
	/***********************************************************/

	case PT_PGN_12544:  //�������֡����
		break;


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
		BMS_ERROR("bms_pt other msg %x\n", pgn);
		break;
	}
}

int bms_pt_poll()
{
	switch(bms_info.stage)
	{
	case BMS_stage_handshake:

		if((bms_pt_time_info.pgn512 == 0) && ((bms_info.timer_tick - bms_pt_time_info.pgn256) > 25))  //BRM BMS�ͳ�����ʶ����  250ms
		{
			bms_pt_send_crm(0);
			bms_pt_time_info.pgn256 = bms_info.timer_tick;  //���淢��ʱ��
		}

		break;

	case BMS_stage_config:
		evc_set_v_c(0, 0);
		evc_out_onoff(0xff);
		io_bms_dc_out_en();
		break;

	case BMS_stage_charging:

		if((bms_info.timer_tick - bms_pt_time_info.pgn4096) > 10)  //BCL ��س��������  100msû�յ���ʱ  ֹͣ���
		{
			//ֹͣ���
			evc_out_onoff(0);  //�ر����
			bms_pt_send_cst();  //������ֹ��籨��
			return 1;
		}

		break;

	case BMS_stage_end:  //�յ�BST BMS��ֹ��籨�Ľ�������׶�
		return 1;

//		break;
	}

	
	if(stop_charge)
	{
		return 1;
	}

	return 0;
}

/*
 * CAN���ݽ��ճ�ʱ
 */
void bms_pt_timeout_handler()
{

}


/*
 * ��ʼ��
 */
void bms_pt_init()
{
	memset(&bms_pt_time_info, 0, sizeof(bms_pt_time_info));

	bms_pt_send_crm(0);
	bms_pt_time_info.pgn256 = bms_info.timer_tick;  //���淢��ʱ��
	bms_info.stage = BMS_stage_handshake;
}



/*
 * ������Ӧ������
 */
bms_ptl_handler_def bms_pt_event =
{
		bms_pt_init,
		bms_pt_msg_handler,
		bms_pt_timeout_handler,
		bms_pt_poll,
};
