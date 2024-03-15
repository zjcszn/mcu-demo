#include "RTL.h"
#include "stm32f10x.h"
#include "string.h"
#include "rtc.h"
#include "lcd.h"
#include "shk.h"
#include "sys_cfg.h"
#include "bms.h"
#include "em.h"
#include "evc.h"
#include "dbg.h"
#include "ic.h"
#include "stdio.h"
#include "eth.h"
#include "measure.h"
#include "server.h"
#include "recorder.h"
#include "gprs.h"
#include "delay.h"
#include "io.h"

#if 1
#define LCD_MSG dbg_msg
#else
#define LCD_MSG (void)
#endif


extern int dgus_read_page(u8 page_id);
extern int dgus_write_page(u8 page_id);
extern void dgus_bl_config(u8 bl);
extern void dgus_write_current_page(void);
extern void dgus_rtc_config(u8 YY, u8 MM, u8 DD, u8 h, u8 m);
extern int dgus_rtc_read(time_t *time);
extern int dgus_init_ro_var(u8 page_id);
extern void dgus_trigger_key(u8 key);
extern void dgus_fill_rec(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);
extern u8 dgus_get_prev_id(void);
extern int dgus_show_text(u16 addr, u8* str);



//ȫ�ֱ���
struct lcd_ui
{
	u8 need_card;  //�Ƿ���Ҫˢ��  0:����Ҫ  1:��Ҫ
	u8 r;  //ֹͣԭ��
	u32 ic_id;  //ic��ID
	int balance;  //�������
	u32 sum;  //���ѽ��
	u8  charge_type;  //��緽ʽ  1:���  2:���� 3:ʱ��
	u32 kwh_0;  //���ǰ������
	u32 value;  //����ĵ���(0.01kwh) ʱ��(����)  ���(0.01Ԫ)
	time_t start_t;  //��ʼ���ʱ��
	time_t stop_t;  //�������ʱ��
	u8 test_charge;




	u8  cnt;  //������ ���һ�μ�1     0: ��Ч
	u8  current_id;  //��ǰ���ǹ��� 0:��Ч
	u8  s_shk1;  //���ǹ1״̬  1��⵽  0:û��⵽
	u8  s_shk2;  //���ǹ2״̬
	u8  s_bms1;  //���ǹ1 BMS״̬   1:���ӳɹ�   0:�޷�ʶ��
	u8  s_bms2;  //���ǹ1 BMS״̬

	u16 H1;  //���ǹ1���ʱ��Сʱ
	u16 M1;  //���ǹ1���ʱ�����
	u16 SOC1;    //���ǹ1�����ٷֱ�
	u16 kwh1;
	u16 r1;  //ֹͣԭ��1

	u16 H2;  //���ǹ2���ʱ��Сʱ
	u16 M2;  //���ǹ2���ʱ�����
	u16 SOC2;    //���ǹ2�����ٷֱ�
	u16 kwh2;
	u16 r2;  //ֹͣԭ��2
}ui_info;


//��������
struct page3_wo_var_s
{
	u16 id;  //����������ʾԲ��
}page3_wo_var;


//��緽ʽѡ��
struct page4_wo_var_s
{
	u32 ic_id;  //IC����
	u32 balance;  //���
}page4_wo_var;

//�����
struct page5_ro_var_s
{
	u32 value;
}page5_ro_var;

struct page6_ro_var_s
{
	u32 value;
}page6_ro_var;

struct page7_ro_var_s
{
	u32 value;
}page7_ro_var;

//�����   Ҫ��ʾ������
struct page12_wo_var_s
{
	u16 V;  //�����ѹ 0.1V
	u16 I;  //������� 0.1A
	u16 cccv;  //���ģʽ
	u16 P;  //������� 0.1KW
	u16 M;  //���ʱ�����
	u16 kwh;  //�������  0.1KWH
	u16 type;  //�������
	u16 SOC;    //�����ٷֱ�
	u16 t;  //��ߵ����¶� 0.1��
	u16 Ah;  //�������
	u16 max_v;  //��ߵ����ѹ 0.1V
	u16 min_v;  //��͵����ѹ 0.1V
	u16 price;
	u16 sum;  //�ۼ����ѽ��
	u16 id;  //���ǹ���
}page12_wo_var;

//������  ��ʾ��ͳ������
struct page14_wo_var_s
{
	u16 m;  //���ʱ�����
	u16 kwh;  //�������  0.1KWH
	u16 sum;
	u16 soc;    //�����ٷֱ�
}page14_wo_var;


//������  ��ʾ��ͳ������
struct page15_wo_var_s
{
	u16 H;  //���ʱ��Сʱ
	u16 M;  //���ʱ�����
	u16 kWh;  //�������  0.1KWH
	u16 SOC;    //�����ٷֱ�
	u16 r;  //ֹͣԭ��
}page15_wo_var;

//������ʾҳ��
struct page20_wo_var_s
{
	u16 error_id;  //����ID
}page20_wo_var;


//OK��ʾҳ��
struct page21_wo_var_s
{
	u16 id;  //id
}page21_wo_var;


//ѯ����ʾҳ��
struct page22_wo_var_s
{
	u16 id;  //id
}page22_wo_var;

struct page24_ro_var_s
{
	u16 key_a1;
	u16 key_a2;
	u16 key_a3;
	u16 key_a4;
	u16 key_a5;
	u16 key_a6;

	u16 key_b1;
	u16 key_b2;
	u16 key_b3;
	u16 key_b4;
	u16 key_b5;
	u16 key_b6;
}page24_ro_var;



//ʱ������
struct page25_ro_var_s
{
	u16 YY;  //��
	u16 MM;  //��
	u16 DD;  //��
	u16 h;   //ʱ
	u16 m;   //��
}page25_ro_var;


typedef struct
{
	u16 status;  //״̬
	u16 v;  //��ѹ
	u16 i;  //����
}page26_info_def;


//ģ��״̬
struct page26_wo_var_s
{
	page26_info_def info[MAX_MODULE_CNT];
}page26_wo_var;

#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
//�����Ϣ
struct page27_wo_var_s
{
	u32 id;  //����ID
	u16 em_s;  //���״̬
	u32 kwh;  //������
	u16 eth_s;  //��̫��״̬
	u16 ip1;  //ip��ַ
	u16 ip2;
	u16 ip3;
	u16 ip4;

	u16 ic_s;
	u16 gprs_s;
}page27_wo_var;
#pragma pack(pop) //�ָ�����״̬

struct page28_ro_var_s
{
	u16 v;
	u16 i;
}page28_ro_var;

struct page28_wo_var_s
{
	u16 v;
	u16 i;
	u16 cc_cv;
	u16 p;
	u16 cc_cv_set;
}page28_wo_var;

struct page29_ro_var_s
{
	u16 e_price;  //���  ��λ ��  100��ʾ1Ԫ
}page29_ro_var;

struct page30_wo_var_s
{
	u16 code1;
	u16 code2;
}page30_wo_var;

//IP��ַ����
struct page31_ro_var_s
{
	u32 id;

	u16 rip1;  //Զ��IP
	u16 rip2;
	u16 rip3;
	u16 rip4;

	u16 rport;  //Զ�̶˿�

	u16 lip1;  //����IP
	u16 lip2;
	u16 lip3;
	u16 lip4;

	u16 drip1;  //����IP
	u16 drip2;
	u16 drip3;
	u16 drip4;

	u16 netmask1;  //��������
	u16 netmask2;
	u16 netmask3;
	u16 netmask4;
}page31_ro_var;



#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���
typedef struct
{
	u16 id;
	u32 card_id;
	u32 kwh;
	u32 sum;
	u8  time[8];
	u16 t;
	u16  status;
}page32_recd_def;


struct page32_wo_var_s
{
	page32_recd_def recd[5];
	u16 cnt;
}page32_wo_var;
#pragma pack(pop) //�ָ�����״̬



struct page35_ro_var_s
{
	u32 v1;
	u32 i1;

	u32 v2;
	u32 i2;

	u16 vset;
	u16 iset;
}page35_ro_var;


struct page35_wo_var_s
{
	u32 v;
	u32 i;

	u16 adv1;
	u16 adi1;
	u16 adv2;
	u16 adi2;
}page35_wo_var;



/*
 * @brief           ��ʾ������Ϣ
 * @param err_id  	Ҫ��ʾ�Ĵ���ID
 * @retval          ����ҳ��ID��
 */
static u8 current_err_id;
#define ERROR_VAR_ADDR 0x1401
u8 goto_error_page(u8 err_id)
{
	current_err_id = err_id;

	if(err_id == 1)
	{
		dgus_show_text(ERROR_VAR_ADDR, "       �������!");
	}
	else if(err_id == 2)
	{
		dgus_show_text(ERROR_VAR_ADDR, "   ��������λ������!");
	}
	else if(err_id == 3)
	{
		dgus_show_text(ERROR_VAR_ADDR, "       ��������!");
	}
	else if(err_id == 4)
	{
		dgus_show_text(ERROR_VAR_ADDR, "   �����������벻һ��!");
	}
	else if(err_id == 10)
	{
		dgus_show_text(ERROR_VAR_ADDR, "   �����ϣ��޷����!");
	}
	else if(err_id == 11)
	{
		dgus_show_text(ERROR_VAR_ADDR, "        ģ��ͨѶ����");
	}
	else if(err_id == 12)
	{
		dgus_show_text(ERROR_VAR_ADDR, "        ���뽻���Ӵ����쳣");
	}
	else if(err_id == 15)
	{
		dgus_show_text(ERROR_VAR_ADDR, "        �����������!");
	}
	else if(err_id == 20)
	{
		dgus_show_text(ERROR_VAR_ADDR, "        ��ЧIC��!");
	}
	else if(err_id == 21)
	{
		dgus_show_text(ERROR_VAR_ADDR, "IC��������,����ϵ��Ӫ��!");
	}
	else if(err_id == 30)
	{
		dgus_show_text(ERROR_VAR_ADDR, "      δ������ǹ!");
	}
	else if(err_id == 31)
	{
		dgus_show_text(ERROR_VAR_ADDR, "      BMS����ʧ��,�޷����!");
	}
	else if(err_id == 40)
	{
		dgus_show_text(ERROR_VAR_ADDR, "      �˻�����!");
	}
	return 20;
}


/*
 * @brief           ��ʾ�ɹ���ʾ��Ϣ
 * @param id  	        Ҫ��ʾ�ĳɹ���ʾ��ϢID
 * @retval          �ɹ���ʾҳ��ID��
 */
u8 goto_ok_page(u8 id)
{
	return 21;
}

/*
 * @brief           ȷ����ʾ��Ϣ
 * @param id  	        Ҫ��ʾ����ʾ��ϢID
 * @retval          ȷ����ʾҳ��ID��
 */
static u8 current_ask_id;
#define ASK_VAR_ADDR 0x1601
u8 goto_ask_page(u8 id)
{
	current_ask_id = id;
	if(current_ask_id == 1)
	{
		dgus_show_text(ASK_VAR_ADDR, "      �Ƿ�ָ���ʼ������?");
	}
	else
	{
		return PAGE_ID_INVALID;
	}
	return 22;
}



/*******************************************  LOGO��ʾҳ��  **********************************************/

u8 page0_pool_handler(u32 poll_cnt)  //logoҳ��
{
	u16 delay = 8000;
	u8 i;
	
	io_evc_poweron();  //�����Ӵ����պ�

	for(i = 1; i < 101; i++)
	{
		//��ʾ������
		dgus_fill_rec(0, 590, i * 8, 598, 0xffff);
		os_dly_wait(delay / 100);
	}

	ui_info.need_card = 1;
	

	if(evc_info.err_ps)  //�������
	{
		return goto_error_page(15);
	}

	if(io_ac_status() != 0)
	{
		return goto_error_page(12);  //��������Ӵ����쳣
	}

	if(ui_info.need_card)
	{
		return 2;  //��ʾˢ��
	}
	else
	{
		return 1;  //
	}


	//return PAGE_ID_INVALID;
}

//ҳ��0����  LOGOҳ��
page_t page0 =
{
		page0_pool_handler,  //poll_handler
		NULL,  //vchange_handler
		NULL,  //button_handler
		NULL,  //page_init
		NULL,  //wo
		NULL,  //ro
		100,     //poll_time
		0,     //wo_len
		0,     //ro_len
};


/**************************************** ������ ��ˢ��  ********************************************/
u8 page1_poll_handler(u32 poll_cnt)
{
	if(poll_cnt == 0)
	{
		memset(&ui_info, 0, sizeof(ui_info));
	}
	return PAGE_ID_INVALID;
}

u8 page1_button_handler(u8 offset)
{
	if(offset == 0)
	{
		//����ϵͳ����
		return 23;
	}
	else
	{
		em_calc_init();  //��������ʼ��
		return 10;
	}
}

page_t page1 =
{
		page1_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page1_button_handler,  //button_handler
		NULL,  //page_init
		NULL,  //wo
		NULL,  //ro
		50,    //poll_time
		0,     //wo_len
		0,     //ro_len
};

/*********************************************************************************************/


/**************************************** ������ ��ˢ��  ********************************************/
u8 page2_temp;
void page2_init()
{
	memset(&ui_info, 0, sizeof(ui_info));
	ui_info.need_card = 1;
	page2_temp = 0;
	server_send_idle();
}

u8 page2_poll_handler(u32 poll_cnt)
{
	if(evc_info.module_cnt == 0)
	{
		return goto_error_page(11);
	}


	if(ic_find(&ui_info.ic_id))  //����IC��
	{
		u32 temp;
		if(ic_read_sum(&temp) == 0)  //��ȡ���
		{
			if(ic_is_locked())
			{
				//ic_unlock_card(&ui_info.stop_t);
				return goto_error_page(21); //IC��������
			}

			page4_wo_var.ic_id = HTONL(ui_info.ic_id);
			page4_wo_var.balance = HTONL(temp);
			ui_info.balance = temp;  //���

			ic_halt();

			return 4;  //��ʾ���
		}
		else
		{
			//��Ч��
			return goto_error_page(20);
		}
	}
	return PAGE_ID_INVALID;
}

u8 page2_button_handler(u8 offset)
{
	if(offset == 0)
	{
		//����ϵͳ����ǰ����ϵͳ����
		return 3;
	}
	else if(offset == 0x10)
	{
		page2_temp++;
		if(page2_temp == 10)
		{
			return goto_ask_page(1);  //��ʾ���ǻָ�����
		}
	}
	return PAGE_ID_INVALID;
}

page_t page2 =
{
		page2_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page2_button_handler,  //button_handler
		page2_init,  //page_init
		NULL,  //wo
		NULL,  //ro
		200,     //poll_time
		0,     //wo_len
		0,     //ro_len
};

/*********************************************************************************************/


/**************************************** ��������  ********************************************/
u8 pswd_in_timeout;  //��ʱ
u8 pswd_offset;  //��������λ��ƫ��

void page3_init()
{
	pswd_offset = 0;
	page3_wo_var.id = 0;  //��������
}

u8 page3_poll_handler(u32 poll_cnt)
{
	pswd_in_timeout++;
	if(pswd_in_timeout == 30)  //���볬ʱ
	{
		return 2;  //����������
	}
	return PAGE_ID_INVALID;
}


u8 page3_button_handler(u8 offset)
{
	static u8 pswd[6];

	if(offset < 10)  //���ּ�
	{
		if(pswd_offset < 6)
		{
			pswd[pswd_offset++] = offset;  //����һλ����

			page3_wo_var.id = HTONS(pswd_offset);

			dgus_write_current_page();
		}
	}
	else if(offset == 10)  //���
	{
		pswd_offset = 0;
		page3_wo_var.id = HTONS(pswd_offset);
		dgus_write_current_page();
	}
	else if(offset == 11)  //����
	{
		return 2;  //����������
	}
	else if(offset == 12)  //ȷ��
	{
		if(pswd_offset == 6)
		{
			u32 temp_pswd = 0;
			//�����������ת����32λ��
			temp_pswd += pswd[0] * 100000;
			temp_pswd += pswd[1] * 10000;
			temp_pswd += pswd[2] * 1000;
			temp_pswd += pswd[3] * 100;
			temp_pswd += pswd[4] * 10;
			temp_pswd += pswd[5];

			if(temp_pswd == sys_cfg.sys_pswd)
			{
				return 23;  //����ϵͳ����
			}
			else
			{
				return goto_error_page(1);  //��ʾ�������
			}
		}
		else  //����6λ��
		{
			return goto_error_page(2);  //��ʾ����6λ��
		}
	}

	return PAGE_ID_INVALID;
}

page_t page3 =
{
		page3_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page3_button_handler,  //button_handler
		page3_init,  //page_init
		&page3_wo_var,  //wo
		NULL,  //ro
		1000,     //poll_time
		sizeof(page3_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/


/**************************************** ��緽ʽ  ********************************************/
u8 page4_poll_handler(u32 poll_cnt)
{
	if(poll_cnt == 30)
	{
		return 2;
	}
	return PAGE_ID_INVALID;
}
u8 page4_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return 2;  //���ص�ˢ��ҳ��
	}
	else if(offset == 1)  //�Զ�����
	{
		if(ui_info.balance < 100)  //���С��1Ԫ
		{
			return goto_error_page(40);
		}
		ui_info.charge_type = 0;  //�Զ�����
		return 11;
	}
	else if(offset == 2)  //�����
	{
		if(ui_info.balance < 100)  //���С��1Ԫ
		{
			return goto_error_page(40);
		}
		ui_info.charge_type = 1;
		return 5;
	}
	else if(offset == 3)  //������
	{
		if(ui_info.balance < 100)  //���С��1Ԫ
		{
			return goto_error_page(40);
		}
		ui_info.charge_type = 2;
		return 6;
	}
	else if(offset == 4)  //��ʱ��
	{
		if(ui_info.balance < 100)  //���С��1Ԫ
		{
			return goto_error_page(40);
		}
		ui_info.charge_type = 3;
		return 7;
	}
	return PAGE_ID_INVALID;
}
page_t page4 =
{
		page4_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page4_button_handler,  //button_handler
		NULL,  //page_init
		&page4_wo_var,  //wo
		NULL,  //ro
		1000,     //poll_time
		sizeof(page4_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/**************************************** ������  *******************************************/
void page5_init()
{
	page5_ro_var.value = 0;
	dgus_init_ro_var(5);
}
u8 page5_poll_handler(u32 poll_cnt)
{
	if(poll_cnt == 60)
	{
		return 2;  //��ʱ
	}
	return PAGE_ID_INVALID;
}
u8 page5_button_handler(u8 offset)
{
	if(offset == 1)  //ȷ��
	{
		if(dgus_read_page(5))
		{
			u32 temp = HTONL(page5_ro_var.value);
			ui_info.value = temp;
			if(temp > ui_info.balance)
			{
				return goto_error_page(40);  //����
			}
			else
			{
				return 11;  //�ٴ�ˢ��
			}
		}
	}
	else  //ȡ��
	{
		return 2;
	}
	return PAGE_ID_INVALID;
}
page_t page5 =
{
		page5_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page5_button_handler,  //button_handler
		page5_init,  //page_init
		NULL,  //wo
		&page5_ro_var,  //ro
		1000,     //poll_time
		0,     //wo_len
		sizeof(page5_ro_var),     //ro_len
};
/*********************************************************************************************/



/**************************************** �������   *******************************************/
void page6_init()
{
	page6_ro_var.value = 0;
	dgus_init_ro_var(6);
}
u8 page6_poll_handler(u32 poll_cnt)
{
	if(poll_cnt == 60)
	{
		return 2;  //��ʱ
	}
	return PAGE_ID_INVALID;
}
u8 page6_button_handler(u8 offset)
{
	if(offset == 1)  //ȷ��
	{
		if(dgus_read_page(6))
		{
			u32 temp = HTONL(page6_ro_var.value);
			temp *= 10;
			ui_info.value = temp;
			temp = temp * sys_cfg.e_price / 100;     //������
			if(temp > ui_info.balance)
			{
				return goto_error_page(40);  //����
			}
			else
			{
				return 11;  //�ٴ�ˢ��
			}
		}
	}
	else  //ȡ��
	{
		return 2;
	}

	return PAGE_ID_INVALID;
}
page_t page6 =
{
		page6_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page6_button_handler,  //button_handler
		page6_init,  //page_init
		NULL,  //wo
		&page6_ro_var,  //ro
		1000,     //poll_time
		0,     //wo_len
		sizeof(page6_ro_var),     //ro_len
};
/*********************************************************************************************/




/**************************************** ����ʱ��   *******************************************/
void page7_init()
{
	page7_ro_var.value = 0;
	dgus_init_ro_var(7);
}
u8 page7_poll_handler(u32 poll_cnt)
{
	if(poll_cnt == 60)
	{
		return 2;  //��ʱ
	}
	return PAGE_ID_INVALID;
}
u8 page7_button_handler(u8 offset)
{
	if(offset == 3)  //ȷ��
	{
		if(dgus_read_page(7))
		{
			u32 temp = HTONL(page7_ro_var.value);
			ui_info.value = temp;
			return 11;  //�ٴ�ˢ��
		}
	}
	else  //ȡ��
	{
		return 2;
	}

	return PAGE_ID_INVALID;
}
page_t page7 =
{
		page7_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page7_button_handler,  //button_handler
		page7_init,  //page_init
		NULL,  //wo
		&page7_ro_var,  //ro
		1000,     //poll_time
		0,     //wo_len
		sizeof(page7_ro_var),     //ro_len
};
/*********************************************************************************************/


/******************************************* ����ֹͣ���  *************************************/
u8 page9_poll_handler(u32 poll_cnt)
{
	while(bms_charge_complete() != 0xff)  //�ȴ�BMS�������
	{
		os_dly_wait(100);
	}
	return 14;
}

page_t page9 =
{
		page9_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		NULL,  //button_handler
		NULL,  //page_init
		NULL,  //wo
		NULL,  //ro
		500,     //poll_time
		0,     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/**************************************** �����������  ********************************************/
void page10_init()
{
	
}
u8 page10_poll_handler(u32 poll_cnt)
{
	if(!shk_isok())
	{
		return goto_error_page(30);
	}
	
	bms_start_charge(3, 0, 0);  //����BMS����

	while(1)
	{
		u8 temp = bms_test_isok();
		//dgus_show_text(0x0A00, bms_info.msg);

		if(temp == 1)  //���ӳɹ�
		{
			break;
		}
		else if(temp == 2)
		{
			return goto_error_page(31);  //bms����ʧ��
		}

		if(!shk_isok())
		{
			return goto_error_page(30);
		}
		os_dly_wait(500);
	}

	while(1)
	{
		if(bms_info.stage == BMS_stage_charging)  //������׶�
		{
			return 12;  //������ҳ��
		}

		if(bms_charge_complete())
		{
			return goto_error_page(31);  //bms����ʧ��
		}

		if(!shk_isok())
		{
			return goto_error_page(30);
		}
		os_dly_wait(500);
	}

	return PAGE_ID_INVALID;
}
page_t page10 =
{
		page10_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		NULL,  //button_handler
		page10_init,  //page_init
		NULL,  //wo
		NULL,  //ro
		200,     //poll_time
		0,     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/**************************************** �ٴ�ˢ��(����)  ********************************************/
u8 page11_button_handler(u8 offset)
{
	return 2;  //����ˢ��ҳ��
}
u8 page11_poll_handler(u32 poll_cnt)
{
	u32 temp;

	if(poll_cnt == 0)
	{
		if(!em_calc_init())  //��������
		{
			return goto_error_page(10); //������
		}
	}

	if(ic_find(&temp))
	{
		//ˢ����ʼ���
		if(temp == ui_info.ic_id)  //������ͬһ�ſ�
		{
			if(dgus_rtc_read(&ui_info.start_t))  //��ȡʱ��
			{
				//����IC��
				if(ic_lock_card(&ui_info.start_t) == 0)
				{
					ic_halt();
					return 10;  //�����������ҳ��
				}
			}
		}
	}

	if(poll_cnt == 10 * 30)  //30s��ʱ  ���ؿ�ʼҳ��
	{
		return 2;
	}
	return PAGE_ID_INVALID;
}

page_t page11 =
{
		page11_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page11_button_handler,  //button_handler
		NULL,  //page_init
		NULL,  //wo
		NULL,  //ro
		100,     //poll_time
		0,     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/*******************************************  ����� ��ˢ���Ʒ�   *******************************************/
record_def recd;  //����¼
u16 recd_addr;  //��¼�����ַ
u32 recd_timer;

/*
 * ����б�������
 */
void page12_recd_save()
{
	recd.sum = em_info.sum;  //���
	recd.kwh = em_info.kwh;  //����
	recd.sec = bms_info.timer_tick / 100;  //ʱ��
	recd_write(recd_addr, &recd);  //��������
}

/*
 * ����֮�󱣴�����
 */
void page14_recd_save()
{
	recd.status = 1;  //״̬  �ѽ���
	recd_write(recd_addr, &recd);  //��������
}

void page12_init()
{
	page12_wo_var.id = HTONS(1);  //��ʾ��ǰ���ڳ��ĳ��ǹ���
	page12_wo_var.price = HTONS(sys_cfg.e_price);
	page12_wo_var.kwh = 0;
	page12_wo_var.sum = 0;

	page12_wo_var.type = 0;
	page12_wo_var.t = 0;
	page12_wo_var.max_v = 0;
	page12_wo_var.min_v = 0;
	page12_wo_var.SOC = 0;
	page12_wo_var.Ah = 0;
	page12_wo_var.M = 0;	
	page12_wo_var.V = 0;
	page12_wo_var.I = 0;
	page12_wo_var.P = 0;
	
	recd.time.year = ui_info.start_t.year;  //��ʼʱ��
	recd.time.mon = ui_info.start_t.mon;
	recd.time.day = ui_info.start_t.day;
	recd.time.hour = ui_info.start_t.hour;
	recd.time.min = ui_info.start_t.min;
	recd.time.sec = ui_info.start_t.sec;
	recd.sum = 0;
	recd.kwh = 0;
	recd.sec = 0;
	recd.status = 2;  //δ����״̬
	recd.card_id = ui_info.ic_id;  //ic������
}


u8 page12_button_handler(u8 offset)
{
	if(offset == 0)  //ֹͣ��ť  �ֶ�ȡ�����
	{
		bms_stop_charge(13);  //ֹͣ���
		ui_info.r = 13;  //ֹͣԭ��   �ֶ�ֹͣ
	}
	return PAGE_ID_INVALID;
}

u8 page12_poll_handler(u32 poll_cnt)  //�����
{
	u16 temp;

	if(poll_cnt == 0)
	{
		recd_addr = recd_add();  //�½�һ����¼
		recd_write(recd_addr, &recd);  //��������
		delay_reset(&recd_timer);  //��λ��ʱ��

		chg_info.id = ui_info.ic_id;
		chg_info.start_time.y = ui_info.start_t.year - 2000;
		chg_info.start_time.m = ui_info.start_t.mon;
		chg_info.start_time.d = ui_info.start_t.day;
		chg_info.start_time.h = ui_info.start_t.hour;
		chg_info.start_time.mm = ui_info.start_t.min;
		chg_info.start_time.s = ui_info.start_t.sec;
		chg_info.gun_id = 1;  //��ǹ�̶�Ϊ1
		server_send_chging();
	}
	else if(delay_timeout_reload(&recd_timer, 30 * 1000) == 0)  //30s  ����һ��
	{
		page12_recd_save();  //�����¼
	}


	if(bms_charge_complete())  //ֹͣ���
	{
		page12_recd_save();  //�����¼
		return 9;  //����ֹͣ���
	}


	if((poll_cnt % 3) == 0)
	{
		u32 temp;
		u32 sum_temp;

		em_calc();  //��Ѽ���

		temp = em_info.kwh;  //���Ķ���
		sum_temp = em_info.sum;  //���

		page12_wo_var.kwh = HTONS(temp);
		ui_info.sum = sum_temp;
		page12_wo_var.sum = HTONS(sum_temp);


		if((ui_info.need_card) && (!ui_info.test_charge))  //�ǲ���ģʽ
		{
			if(ui_info.charge_type == 0)  //�Զ�����
			{
				if(sum_temp >= ui_info.balance)
				{
					bms_stop_charge(69);
				}
			}
			else if(ui_info.charge_type == 1)  //������
			{
				if(sum_temp >= ui_info.value)
				{
					//ui_info.sum = ui_info.value;  //���ѽ����Ϊ�趨���
					bms_stop_charge(72);
				}
			}
			else if(ui_info.charge_type == 2)  //��������
			{
				if((temp >= ui_info.value))  //ʵ�����ѽ���������ﵽ�趨����
				{
					page12_wo_var.kwh = HTONS((u16)(ui_info.value));
					//ui_info.sum = (ui_info.value * sys_cfg.e_price) / 100;  //���ѽ����Ϊ����*���
					bms_stop_charge(70);
				}
			}
			else if(ui_info.charge_type == 3)  //��ʱ���
			{
				if((sum_temp >= ui_info.balance) || ((bms_info.timer_tick / 100 / 60) >= ui_info.value))  //ʵ�����ѽ���������ʱ�䵽
				{
					bms_stop_charge(71);
				}
			}
		}
	}



#if 0
	temp = meas_info.v / 100;
	page12_wo_var.V = HTONS(temp);

	temp = meas_info.i / 100;
	page12_wo_var.I = HTONS(temp);

	temp = (temp * (meas_info.v / 100)) / (100 * 100);
	page12_wo_var.P = HTONS(temp);
#else
	temp = evc_info.read.v;
	page12_wo_var.V = HTONS(temp);

	temp = evc_info.read.i / 10;
	page12_wo_var.I = HTONS(temp);

	temp = evc_info.read.i * evc_info.read.v / 100 / 1000;
	page12_wo_var.P = HTONS(temp);
#endif

	temp = bms_info.battery_type;
	page12_wo_var.type = HTONS(temp);

	if(bms_info.max_temp > 50)
	{
		temp = (bms_info.max_temp - 50) * 10;
	}
	else
	{
		temp = 0;
	}
	page12_wo_var.t = HTONS(temp);

	temp = bms_info.max_v_single / 10;
	page12_wo_var.max_v = HTONS(temp);

	if(bms_info.max_v_single > 10)
	{
		temp = (bms_info.max_v_single - 10) / 10;
	}
	else
	{
		temp = 0;
	}
	page12_wo_var.min_v = HTONS(temp);

	temp = bms_info.SOC;
	page12_wo_var.SOC = HTONS(temp);

	temp = bms_info.Ah / 10;
	page12_wo_var.Ah = HTONS(temp);

	page12_wo_var.M = HTONS(bms_info.timer_tick / 100 / 60);

	dgus_write_page(12);

	return PAGE_ID_INVALID;
}


page_t page12 =
{
		page12_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page12_button_handler,  //button_handler
		page12_init,  //page_init
		(u8*)&page12_wo_var,  //wo
		NULL,  //ro
		2000,     //poll_time
		sizeof(page12_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/



/***************************************** ������ ����ҳ�� **********************************************/
void page9_show_reason()  //��ʾֹͣԭ��
{
	if(ui_info.r != 0)
	{
		bms_info.stop_reason = 13;
	}
	if(bms_info.stop_reason != 0)
	{
		switch(bms_info.stop_reason)
		{
		case 1:
			dgus_show_text(0x0E20, "�ﵽSOCֵ");
			break;
		case 2:
			dgus_show_text(0x0E20, "�ﵽ�ܵ�ѹ");
			break;
		case 3:
			dgus_show_text(0x0E20, "�ﵽ�����ѹ");
			break;
		case 4:
			dgus_show_text(0x0E20, "��Ե����");
			break;
		case 5:
			dgus_show_text(0x0E20, "�������������");
			break;
		case 6:
			dgus_show_text(0x0E20, "BMSԪ���������������");
			break;
		case 7:
			dgus_show_text(0x0E20, "�������������");
			break;
		case 8:
			dgus_show_text(0x0E20, "������¶ȹ���");
			break;
		case 9:
			dgus_show_text(0x0E20, "��������");
			break;
		case 10:
			dgus_show_text(0x0E20, "��������");
			break;
		case 11:
			dgus_show_text(0x0E20, "��ѹ�쳣");
			break;
		case 12:
			dgus_show_text(0x0E20, "���ʹ���");
			break;
		case 13:
			dgus_show_text(0x0E20, "�ֶ�ֹͣ");
			break;
		case 14:
			dgus_show_text(0x0E20, "������ɳ��׼�����ĳ�ʱ");
			break;
		case 15:
			dgus_show_text(0x0E20, "�������ֹͣ");
			break;
		case 19:
			dgus_show_text(0x0E20, "���յ�س�������ĳ�ʱ");
			break;
		case 20:
			dgus_show_text(0x0E20, "��������");
			break;
		case 21:
			dgus_show_text(0x0E20, "������¶ȹ���2");
			break;
		case 22:
			dgus_show_text(0x0E20, "�����ѹ����");
			break;
		case 23:
			dgus_show_text(0x0E20, "���ձ�ʶ���ĳ�ʱ");
			break;
		case 24:
			dgus_show_text(0x0E20, "���յ�س��������ĳ�ʱ");
			break;
		case 50:
			dgus_show_text(0x0E20, "������");
			break;
		case 69:
			dgus_show_text(0x0E20, "����");
			break;
		case 70:
			dgus_show_text(0x0E20, "�ﵽ�趨����");
			break;
		case 71:
			dgus_show_text(0x0E20, "�ﵽ�趨ʱ��");
			break;
		case 72:
			dgus_show_text(0x0E20, "�ﵽ�趨���");
			break;
		}
	}
}
void page9_show_msg()
{
	if(ui_info.need_card)
	{
		dgus_show_text(0x0E60, "���ڸ�Ӧ��ˢ����ɽ���");
	}
}

void page14_init()
{
	page14_wo_var.m = page12_wo_var.M;
	page14_wo_var.soc = HTONS(bms_info.SOC);
	page14_wo_var.kwh = page12_wo_var.kwh;
	page14_wo_var.sum = HTONS(ui_info.sum);

	page9_show_reason();
	page9_show_msg();
}

u8 page14_button_handler(u8 offset)
{
	if(ui_info.test_charge)
	{
		ui_info.test_charge = 0;
		return 2;
	}
	
	if(!ui_info.need_card)
	{
		return 1;
	}

	return PAGE_ID_INVALID;
}


u8 page14_poll_handler(u32 poll_cnt)
{
	u32 temp;

	if(poll_cnt == 0)
	{
		page12_recd_save();  //�����¼
		//server_send_chgend();
	}

    if(ui_info.need_card)
    {
		if(ic_find(&temp))
		{
			//ˢ����ʼ���
			if(temp == ui_info.ic_id)  //������ͬһ�ſ�
			{
				if(ui_info.sum != 0)
				{
					if(ui_info.sum > ui_info.balance)
					{
						ui_info.sum = ui_info.balance;
					}
					ui_info.balance -= ui_info.sum;
					if(ic_pay(ui_info.sum) == 0)
					{
						page14_recd_save();
						ui_info.sum = 0;  //���� ��ֹ���ο۷�
					}
				}
				//����IC��
				if((ui_info.sum == 0) && (ic_unlock_card(&ui_info.stop_t) == 0))
				{
					ic_halt();

					chg_info.balances = ui_info.balance;
					server_send_chgend();

					return 2;
				}
			}
		}
    }

	return PAGE_ID_INVALID;
}


page_t page14 =
{
		page14_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page14_button_handler,  //button_handler
		page14_init,  //page_init
		(u8*)&page14_wo_var,  //wo
		NULL,  //ro
		100,     //poll_time
		sizeof(page14_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/


/************************************* ������ʾ ***********************************************/
u8 page20_poll_handler(u32 poll_cnt)
{
	if(current_err_id == 11)
	{
		io_errled_on();  //���ϵ���
		if(evc_info.module_cnt != 0)
		{
			io_errled_off();  //���ϵ���
			return dgus_get_prev_id();
		}
	}
	return PAGE_ID_INVALID;
}
u8 page20_button_handler(u8 offset)
{
	if(offset == 0)  //ȷ��
	{
		if(current_err_id == 1)  //�������
		{
			return 2;
		}
		else if(current_err_id == 2)  //���벻��6λ
		{
			return dgus_get_prev_id();
		}
		else if((current_err_id == 3) || (current_err_id == 4))  //�޸�����
		{
			return 30;
		}
		else if((current_err_id == 10))  //������
		{
			return 2;
		}
		else if((current_err_id == 20) || (current_err_id == 21))  //��������
		{
			return 2;
		}
		else if(current_err_id == 30)  //δ������ǹ
		{
			return dgus_get_prev_id();
		}
		else if(current_err_id == 31)  //BMS�޷�����
		{
			return dgus_get_prev_id();
		}
		else if(current_err_id == 40)  //����
		{
			return dgus_get_prev_id();
		}
	}
	else
	{
		if(current_err_id == 30)  //δ������ǹ
		{
			if(ui_info.need_card)
			{
				return 14;  //������
			}
			else
			{
				return 1;
			}
		}
		else if(current_err_id == 31)  //BMS�޷�����
		{
			return 14;  //������
		}
		else if(current_err_id == 31)  //BMS�޷�����
		{
			if(ui_info.need_card)
			{
				return 14;  //������
			}
			else
			{
				return 1;
			}
		}
		else if(current_err_id == 11)
		{
			if(ui_info.need_card)
			{
				return 2;
			}
			else
			{
				return 1;
			}
		}
	}

	return PAGE_ID_INVALID;
}

page_t page20 =
{
		page20_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page20_button_handler,  //button_handler
		NULL,  //page_init
		&page20_wo_var,  //wo
		NULL,  //ro
		1000,     //poll_time
		sizeof(page20_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/


/**************************************** OK��ʾҳ��  ********************************************/
u8 page21_button_handler(u8 offset)
{
	if(page21_wo_var.id == HTONS(1))  //�����޸ĳɹ�
	{
		return 23; //����ϵͳ����
	}
	if(page21_wo_var.id == HTONS(2))  //������  ��ʾ�γ����ǹ
	{
		while(shk_isok())  //�ȴ����ǹ�γ�
		{
			os_dly_wait(50);
		}

		if(ui_info.need_card)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	if(page21_wo_var.id == HTONS(3))  //������
	{
		if(ui_info.need_card)
		{
			return 2;
		}
		else
		{
			return 1;
		}
	}
	return PAGE_ID_INVALID;
}
page_t page21 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page21_button_handler,  //button_handler
		NULL,  //page_init
		&page21_wo_var,  //wo
		NULL,  //ro
		0,     //poll_time
		sizeof(page21_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/**************************************** ȷ��ҳ��  ********************************************/
u8 page22_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return dgus_get_prev_id();
	}
	else if(offset == 1)
	{
		if(current_ask_id == 1)  //�ָ�����
		{
			sys_cfg.sys_pswd = 123456;  //��ʼ����
			sys_cfg_save();
			return dgus_get_prev_id();
		}
	}

	return PAGE_ID_INVALID;
}
page_t page22 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page22_button_handler,  //button_handler
		NULL,  //page_init
		&page22_wo_var,  //wo
		NULL,  //ro
		0,     //poll_time
		sizeof(page22_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/******************************************  ϵͳ���� ��ˢ������ ***********************************************/
u8 page23_temp;
u8 page23_temp1;
void page23_init()
{
	page23_temp = 0;
	page23_temp1 = 0;
}
u8 page23_button_handler(u8 offset)
{
	if(offset == 0)
	{
		if(ui_info.need_card)
		{
			return 2;  //����������
		}
		else
		{
			return 1;
		}
	}
	else if(offset == 1)  //ʱ��ҳ��
	{
		return 25;  //����ʱ������ҳ��
	}
	else if(offset == 2)  //ģ��״̬
	{
		return 26;  //����ģ��״̬ҳ��
	}
	else if(offset == 3)
	{
		return 27;
	}
	else if(offset == 4)  //�ֶ����
	{
		return 28;
	}
	else if(offset == 5)  //����
	{
		return 30;
	}
	else if(offset == 6)  //��������
	{
		return 31;
	}
	else if(offset == 7)  //��Կ�޸�
	{
		return 24;
	}
	else if(offset == 8)  //�������
	{
		return 29;
	}
	else if(offset == 9)  //��ʷ��¼
	{
		return 32;
	}
	else if(offset == 0x20)
	{
		page23_temp++;
		if(page23_temp == 5)
		{
			return 35;
		}
	}
	else if(offset == 10)  //�Զ����
	{
		dgus_rtc_read(&ui_info.start_t);
		ui_info.test_charge = 1;
		em_calc_init();  //��������ʼ��
		return 10;
	}
	return PAGE_ID_INVALID;
}


page_t page23 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page23_button_handler,  //button_handler
		page23_init,  //page_init
		NULL,  //wo
		NULL,  //ro
		0,     //poll_time
		0,     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/

/**************************************** ������Կ���� ********************************************/
void page24_init()
{
	memset(&page24_ro_var, 0, sizeof(page24_ro_var));
	dgus_init_ro_var(24);
}
u8 page24_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return 23;  //ϵͳ����
	}
	else if(offset == 1)  //����
	{
		if(dgus_read_page(24))
		{
			if((page24_ro_var.key_a1 == 0) && (page24_ro_var.key_a2 == 0) && (page24_ro_var.key_a3 == 0) && (page24_ro_var.key_a4 == 0) && (page24_ro_var.key_a5 == 0) && (page24_ro_var.key_a6 == 0))
			{
				//ȫ0��Ч
			}
			else
			{
				u8 tempA[6], tempB[6];
				tempA[0] = HTONS(page24_ro_var.key_a1);
				tempA[1] = HTONS(page24_ro_var.key_a2);
				tempA[2] = HTONS(page24_ro_var.key_a3);
				tempA[3] = HTONS(page24_ro_var.key_a4);
				tempA[4] = HTONS(page24_ro_var.key_a5);
				tempA[5] = HTONS(page24_ro_var.key_a6);

				tempB[0] = HTONS(page24_ro_var.key_b1);
				tempB[1] = HTONS(page24_ro_var.key_b2);
				tempB[2] = HTONS(page24_ro_var.key_b3);
				tempB[3] = HTONS(page24_ro_var.key_b4);
				tempB[4] = HTONS(page24_ro_var.key_b5);
				tempB[5] = HTONS(page24_ro_var.key_b6);

				if(ic_download_key(0, tempA, 0) && ic_download_key(0, tempA, 1) && ic_download_key(4, tempB, 0) && ic_download_key(4, tempB, 1))
				{
					return 23;
				}
				//LCD_MSG("key: %02X %02X %02X %02X %02X %02X\n", HTONS(page31_ro_var.key1),HTONS(page31_ro_var.key2),HTONS(page31_ro_var.key3),HTONS(page31_ro_var.key4),HTONS(page31_ro_var.key5),HTONS(page31_ro_var.key6));
			}
		}
	}

	return PAGE_ID_INVALID;
}
page_t page24 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page24_button_handler,  //button_handler
		page24_init,  //page_init
		NULL,  //wo
		&page24_ro_var,  //ro
		0,     //poll_time
		0,     //wo_len
		sizeof(page24_ro_var),     //ro_len
};
/*********************************************************************************************/


/**************************************** ʱ������ ********************************************/
void page25_init()
{
	time_t t;
	dgus_rtc_read(&t);
	page25_ro_var.YY = HTONS(t.year);
	page25_ro_var.MM = HTONS(t.mon);
	page25_ro_var.DD = HTONS(t.day);
	page25_ro_var.h = HTONS(t.hour);
	page25_ro_var.m = HTONS(t.min);
	dgus_init_ro_var(25);
}

u8 page25_button_handler(u8 offset)
{
	if(offset == 1)  //ȷ��
	{
		dgus_read_page(25);

		dgus_rtc_config(HTONS(page25_ro_var.YY) - 2000,
				   	   HTONS(page25_ro_var.MM),
				   	   HTONS(page25_ro_var.DD),
				   	   HTONS(page25_ro_var.h),
				   	   HTONS(page25_ro_var.m));
	}

	return dgus_get_prev_id();  //����ҳ��
}


page_t page25 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page25_button_handler,  //button_handler
		page25_init,  //page_init
		NULL,  //wo
		(u8*)&page25_ro_var,  //ro
		0,     //poll_time
		0,     //wo_len
		sizeof(page25_ro_var),     //ro_len
};

/*********************************************************************************************/


/**************************************** ģ��״̬ ********************************************/
static u16 get_status(module_info_def* s)
{
	if(s->id == 0)
	{
		return 0;  //δ����
	}
	else if(!evc_is_err_status(s->status))
	{
		return HTONS(1);  //����
	}
	else
	{
		return HTONS(2);  //����
	}
}
u8 page26_poll_handler(u32 poll_cnt)
{
	u8 i;
	u16 *p = (u16*)&page26_wo_var;
	for(i = 0; i < MAX_MODULE_CNT; i++)
	{
		*p = get_status(&evc_info.module_info[i]);
		p++;
		*p = HTONS(evc_info.module_info[i].v);
		p++;
		*p = HTONS(evc_info.module_info[i].i);
		p++;
	}

	dgus_write_current_page();
	return PAGE_ID_INVALID;
}

u8 page26_button_handler(u8 offset)
{
	return dgus_get_prev_id();  //����ҳ��
}


page_t page26 =
{
		page26_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page26_button_handler,  //button_handler
		NULL,  //page_init
		&page26_wo_var,  //wo
		NULL,  //ro
		1000,     //poll_time
		sizeof(page26_wo_var),     //wo_len
		0,     //ro_len
};

/*********************************************************************************************/



/**************************************** �����Ϣ  ********************************************/
u8 page27_poll_handler(u32 poll_cnt)
{
	u32 temp;
	if(em_read_kwh(&temp))  //�����
	{
		page27_wo_var.em_s = HTONS(1);  //״̬����
		page27_wo_var.kwh = HTONL(temp);
	}
	else
	{
		page27_wo_var.em_s = 0;  //״̬δ����
		page27_wo_var.kwh = 0;
	}

	if(eth_link_isok() == 0)
	{
		//uip_hostaddr
		page27_wo_var.eth_s = HTONS(1);  //״̬����
		page27_wo_var.ip1 = HTONS(uip_ipaddr1(uip_hostaddr));  //��ʾ��ǰIP
		page27_wo_var.ip2 = HTONS(uip_ipaddr2(uip_hostaddr));
		page27_wo_var.ip3 = HTONS(uip_ipaddr3(uip_hostaddr));
		page27_wo_var.ip4 = HTONS(uip_ipaddr4(uip_hostaddr));
	}
	else
	{
		page27_wo_var.eth_s = 0;  //״̬δ����
		page27_wo_var.ip1 = 0;
		page27_wo_var.ip2 = 0;
		page27_wo_var.ip3 = 0;
		page27_wo_var.ip4 = 0;
	}

	if(ic_link_isok())
	{
		page27_wo_var.ic_s = HTONS(1);  //״̬����
	}
	else
	{
		page27_wo_var.ic_s = 0;
	}

	page27_wo_var.id = HTONL(sys_cfg.id);

	dgus_write_current_page();
	return PAGE_ID_INVALID;
}
u8 page27_button_handler(u8 offset)
{
	return dgus_get_prev_id(); //����ϵͳ����
}
page_t page27 =
{
		page27_poll_handler,  //poll_handler
		NULL,  //vchange_handler
		page27_button_handler,  //button_handler
		NULL,  //page_init
		&page27_wo_var,  //wo
		NULL,  //ro
		2000,     //poll_time
		sizeof(page27_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/


/**************************************** �ֶ����(ֹͣ״̬)  ********************************************/
void page28_init()
{

}
u8 page28_poll_handler(u32 poll_cnt)
{
#if 1
	page28_wo_var.v = HTONS(meas_info.v / 100);  //��ʾ��ѹ
	page28_wo_var.i = HTONS(meas_info.i / 100);  //��ʾ����
	page28_wo_var.p = HTONS(((meas_info.v / 100) * (meas_info.i / 100)) / 10000);  //��ʾ����
#else
	page28_wo_var.v = HTONS(evc_info.read.v);  //��ʾ��ѹ
	page28_wo_var.i = HTONS(evc_info.read.i / 10);  //��ʾ����
	page28_wo_var.p = HTONS((evc_info.read.v * evc_info.read.i) / 100000);  //��ʾ����
#endif

	dgus_write_page(28);
	return PAGE_ID_INVALID;
}

u8 page28_button_handler(u8 offset)
{
	if(offset == 0)  //����
	{
		io_bms_dc_out_dis();
		evc_out_onoff(0x00);  //�ر����
		return 23;
	}
	else if(offset == 1)  //����
	{
		u16 v, i;
		if(dgus_read_page(28))
		{
			v = HTONS(page28_ro_var.v);
			i = HTONS(page28_ro_var.i) * 10;

			io_bms_dc_out_en();
			evc_set_v_c(v, i);  //���õ�ѹ����
			os_dly_wait(10);
			evc_out_onoff(0xff);  //�������
		}
	}
	if(offset == 2)  //OFF
	{
		evc_out_onoff(0x00);  //�ر����
	}
	if(offset == 3)  //ѡ����ǹ1
	{
//		bms_sng2_dis();
//		os_dly_wait(1000);
//		bms_sng1_en();
	}
	if(offset == 4)  //ѡ����ǹ2
	{
//		bms_sng1_dis();
//		os_dly_wait(1000);
//		bms_sng2_en();
	}
	return PAGE_ID_INVALID;
}



void page28_vchange_handler(u8 offset, u32 data)
{
	if(offset == 0)  //��ѹ
	{
		evc_set_v_c(HTONS(((u16)data)), 0xffff);  //���õ�ѹ
		//LCD_MSG("set v %d\n", HTONS(((u16)data)));
	}
	else if(offset == 1)  //����
	{
		evc_set_v_c(0xffff, HTONS(((u16)data)) * 10);  //���õ���
		//LCD_MSG("set i %d\n", HTONS(((u16)data)));
	}
}

page_t page28 =
{
		page28_poll_handler,  //poll_handler
		page28_vchange_handler,  //vchange_handler
		page28_button_handler,  //button_handler
		page28_init,  //page_init
		&page28_wo_var,  //wo
		&page28_ro_var,  //ro
		1000,     //poll_time
		sizeof(page28_wo_var),     //wo_len
		sizeof(page28_ro_var),     //ro_len
};
/*********************************************************************************************/

/**************************************** �������  ********************************************/
void page29_init()
{
	page29_ro_var.e_price = HTONS(sys_cfg.e_price);
	dgus_init_ro_var(29);
}
u8 page29_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return 23;  //ϵͳ����
	}
	else if(offset == 1)  //����
	{
		dgus_read_page(29);
		sys_cfg.e_price = HTONS(page29_ro_var.e_price);
		sys_cfg_save();
		return 23;
	}

	return PAGE_ID_INVALID;
}
page_t page29 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page29_button_handler,  //button_handler
		page29_init,  //page_init
		NULL,  //wo
		&page29_ro_var,  //ro
		0,     //poll_time
		0,     //wo_len
		sizeof(page29_ro_var),     //ro_len
};
/*********************************************************************************************/


/**************************************** ����  ********************************************/
u32 page30_code1, page30_code2;
void page30_init()
{
	page30_code1 = 0xffffffff;
	page30_code1 = 0xffffffff;
	page30_wo_var.code1 = 0;
	page30_wo_var.code2 = 0;
}
u8 page30_button_handler(u8 offset)
{
	if(offset == 1)
	{
		if((page30_code1 == 0xffffffff) || (page30_code2 == 0xffffffff))
		{
			return goto_error_page(3);  // ��������
		}
		else if(page30_code1 != page30_code2)
		{
			return goto_error_page(4);  //�����������벻һ��
		}
		else
		{
			//����ȷ�ϳɹ�  ����������
			sys_cfg.sys_pswd = page30_code1;
			sys_cfg_save();
			return 23;
		}
	}
	else
	{
		return 23;  //����ϵͳ����
	}

	return PAGE_ID_INVALID;
}

void page30_vchange_handler(u8 offset, u32 data)
{
	if(offset == 0)
	{
		page30_code1 = HTONL(data);
		page30_wo_var.code1 = HTONS(6);  //��ʾ6��Բ��
		LCD_MSG("%d\n", page30_code1);
	}
	else if(offset == 1)
	{
		page30_code2 = HTONL(data);
		page30_wo_var.code2 = HTONS(6);  //��ʾ6��Բ��
		LCD_MSG("%d\n", page30_code2);
	}
	dgus_write_current_page();
}


page_t page30 =
{
		NULL,  //poll_handler
		page30_vchange_handler,  //vchange_handler
		page30_button_handler,  //button_handler
		page30_init,  //page_init
		&page30_wo_var,  //wo
		NULL,  //ro
		0,     //poll_time
		sizeof(page30_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/



/*******************************************  IP��ַ����   *******************************************/
void page31_init()
{
	page31_ro_var.id = HTONL(sys_cfg.id);

		page31_ro_var.lip1 = HTONS(sys_cfg.lip1);
		page31_ro_var.lip2 = HTONS(sys_cfg.lip2);
		page31_ro_var.lip3 = HTONS(sys_cfg.lip3);
		page31_ro_var.lip4 = HTONS(sys_cfg.lip4);

		page31_ro_var.drip1 = HTONS(sys_cfg.drip1);
		page31_ro_var.drip2 = HTONS(sys_cfg.drip2);
		page31_ro_var.drip3 = HTONS(sys_cfg.drip3);
		page31_ro_var.drip4 = HTONS(sys_cfg.drip4);

		page31_ro_var.netmask1 = HTONS(sys_cfg.netmask1);
		page31_ro_var.netmask2 = HTONS(sys_cfg.netmask2);
		page31_ro_var.netmask3 = HTONS(sys_cfg.netmask3);
		page31_ro_var.netmask4 = HTONS(sys_cfg.netmask4);

		page31_ro_var.rip1 = HTONS(sys_cfg.rip1);
		page31_ro_var.rip2 = HTONS(sys_cfg.rip2);
		page31_ro_var.rip3 = HTONS(sys_cfg.rip3);
		page31_ro_var.rip4 = HTONS(sys_cfg.rip4);

		page31_ro_var.rport = HTONS(sys_cfg.rport);

	dgus_init_ro_var(31);
}


u8 page31_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return 23;  //����ҳ��
	}
	else if(offset == 1)  //ȷ��
	{
		if(dgus_read_page(31))
		{
			sys_cfg.id = HTONL(page31_ro_var.id);

			sys_cfg.lip1 = HTONS(page31_ro_var.lip1);
			sys_cfg.lip2 = HTONS(page31_ro_var.lip2);
			sys_cfg.lip3 = HTONS(page31_ro_var.lip3);
			sys_cfg.lip4 = HTONS(page31_ro_var.lip4);

			sys_cfg.rport = HTONS(page31_ro_var.rport);

			sys_cfg.drip1 = HTONS(page31_ro_var.drip1);
			sys_cfg.drip2 = HTONS(page31_ro_var.drip2);
			sys_cfg.drip3 = HTONS(page31_ro_var.drip3);
			sys_cfg.drip4 = HTONS(page31_ro_var.drip4);

			sys_cfg.netmask1 = HTONS(page31_ro_var.netmask1);
			sys_cfg.netmask2 = HTONS(page31_ro_var.netmask2);
			sys_cfg.netmask3 = HTONS(page31_ro_var.netmask3);
			sys_cfg.netmask4 = HTONS(page31_ro_var.netmask4);

			sys_cfg.rip1 = HTONS(page31_ro_var.rip1);
			sys_cfg.rip2 = HTONS(page31_ro_var.rip2);
			sys_cfg.rip3 = HTONS(page31_ro_var.rip3);
			sys_cfg.rip4 = HTONS(page31_ro_var.rip4);

			sys_cfg_save();  //�������
			return 23;  //����ҳ��
		}
	}
	return PAGE_ID_INVALID;
}


page_t page31 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page31_button_handler,  //button_handler
		page31_init,  //page_init
		NULL,  //wo
		(u8*)&page31_ro_var,  //ro
		0,     //poll_time
		0,     //wo_len
		sizeof(page31_ro_var),     //ro_len
};
/*********************************************************************************************/




/*******************************************  ��ʷ��¼  *******************************************/
u8 page32_hex(u8 dec)
{
	u8 temp;
	temp = ((dec / 10) << 4) | (dec % 10);
	return temp;
}
void page32_time_conv(u8 *p, record_def* recd)
{
	p[0] = page32_hex(recd->time.year / 100);
	p[1] = page32_hex(recd->time.year % 100);
	p[2] = page32_hex(recd->time.mon);
	p[3] = page32_hex(recd->time.day);
	p[4] = page32_hex(recd->time.hour);
	p[5] = page32_hex(recd->time.min);
	p[6] = page32_hex(recd->time.sec);
}


u8 page_id;
void page32_show_page(u8 page)
{
	record_def temp;
	u8 i;

	for(i = 0; i < 5; i++)  //һҳ5��
	{
		if(!recd_read_new(page * 5 + i, &temp))
		{
			page32_wo_var.recd[i].card_id = HTONL(temp.card_id);
			page32_wo_var.recd[i].kwh = HTONL(temp.kwh);
			page32_wo_var.recd[i].sum = HTONL(temp.sum);
			page32_wo_var.recd[i].t = HTONS(temp.sec / 60);
			page32_wo_var.recd[i].status = HTONS(temp.status);
			page32_time_conv(page32_wo_var.recd[i].time, &temp);
		}
		else
		{
			memset(&page32_wo_var.recd[i], 0, sizeof(page32_recd_def));
		}

		page32_wo_var.recd[i].id = HTONS((page * 5) + i + 1);
	}
	dgus_write_page(32);
}


void page32_init()
{
	page_id = 0;
	page32_show_page(page_id);  //��ʾǰ��5��
	page32_wo_var.cnt = HTONS(recd_get_cnt());
}


u8 page32_button_handler(u8 offset)
{
	if(offset == 0)  //ȡ��
	{
		return 23;  //����ҳ��
	}
	else if(offset == 1)  //��һҳ
	{
		if(page_id == 0)
		{
			return PAGE_ID_INVALID;
		}
		page_id--;
		page32_show_page(page_id);
	}
	else if(offset == 2)  //��һҳ
	{
		u16 cnt = recd_get_cnt();
		if(cnt <= (page_id + 1) * 5)  //�����Ѿ�����
		{
			return PAGE_ID_INVALID;
		}
		page_id++;
		page32_show_page(page_id);
	}
	return PAGE_ID_INVALID;
}

page_t page32 =
{
		NULL,  //poll_handler
		NULL,  //vchange_handler
		page32_button_handler,  //button_handler
		page32_init,  //page_init
		&page32_wo_var,  //wo
		NULL,  //ro
		0,     //poll_time
		sizeof(page32_wo_var),     //wo_len
		0,     //ro_len
};
/*********************************************************************************************/



/**************************************** У�� ********************************************/
u16 adv1, adv2, adi1, adi2;
u32 meas_v1, meas_v2, meas_i1, meas_i2;


void page35_init()
{
	io_bms_dc_out_en();
}

void page35_vchange_handler(u8 offset, u32 data)
{
	if(offset == 8)  //��ѹ
	{
		page35_ro_var.vset = (u16)data;
		evc_set_v_c(HTONS(((u16)data)), 0xffff);  //���õ�ѹ
		LCD_MSG("set v %d\n", HTONS(((u16)data)));
	}
	else if(offset == 9)  //����
	{
		page35_ro_var.iset = (u16)data;
		evc_set_v_c(0xffff, HTONS(((u16)data)) * 10);  //���õ���
		LCD_MSG("set i %d\n", HTONS(((u16)data)));
	}
}


u8 page35_button_handler(u8 offset)
{
	if(offset == 0)  //�˳�
	{		
		evc_out_onoff(0x00);  //�ر����
		//bms_sng1_dis();
		//bms_sng2_dis();
		io_bms_dc_out_dis();
		return 23;  //ϵͳ����
	}
	else if(offset == 1)  //ON
	{
		u16 v, i;
		dgus_read_page(35);
		v = HTONS(page35_ro_var.vset);
		i = HTONS(page35_ro_var.iset) * 10;

		//LCD_MSG("VSET %d   ISET %d\n", v, i);
		io_bms_dc_out_en();
		evc_set_v_c(v, i);  //���õ�ѹ����
		os_dly_wait(10);
		evc_out_onoff(0xff);  //�������
	}
	else if(offset == 2)  //OFF
	{
		evc_out_onoff(0x00);  //�ر����
		io_bms_dc_out_dis();
	}
	else if(offset == 3)  //У��
	{
		meas_adj.v_offset = meas_info.ad_v;
		meas_adj_save();
	}
	else if(offset == 4)  //У��
	{
		if(dgus_read_page(35))
		{
			u32 dv;
			u16 dadv;
			meas_v1 = HTONL(page35_ro_var.v1);  //ʵ��ֵ
			meas_v2 = HTONL(page35_ro_var.v2);

			dv = meas_v2 - meas_v1;
			dadv = adv1 - adv2;

			meas_adj.v_gain = ((float)dv / dadv) / 1000.0;  //���㱶��
			meas_adj_save();
		}
	}
	else if(offset == 7)  //ǹ1
	{
		//bms_sng2_dis();
		//os_dly_wait(500);
		//bms_sng1_en();
	}
	else if(offset == 8)  //ǹ2
	{
		//bms_sng1_dis();
		//os_dly_wait(500);
		//bms_sng2_en();
	}
	else if(offset == 9)  //У����
	{
		if(dgus_read_page(35))
		{
			u32 di;
			u16 dadi;
			meas_i1 = HTONL(page35_ro_var.i1);
			meas_i2 = HTONL(page35_ro_var.i2);

			di = meas_i2 - meas_i1;
			dadi = adi2 - adi1;
			meas_adj.i_gain = ((float)di / dadi) / 1000.0;
			meas_adj_save();
		}
	}
	else if(offset == 5)  //ȡ��1
	{
		adv1 = meas_info.ad_v;
		adi1 = meas_info.ad_i;

		page35_wo_var.adv1 = HTONS(adv1);
		page35_wo_var.adi1 = HTONS(adi1);

		dgus_write_current_page();

		page35_ro_var.v1 = HTONL((u32)(evc_info.read.v * 100));
		page35_ro_var.i1 = HTONL((u32)(evc_info.read.i * 10));

		dgus_init_ro_var(35);
		//LCD_MSG("ad_v1 %d   ad_v1 %d\n", adv1, adi1);
	}
	else if(offset == 6)  //ȡ��2
	{
		adv2 = meas_info.ad_v;
		adi2 = meas_info.ad_i;

		page35_wo_var.adv2 = HTONS(adv2);
		page35_wo_var.adi2 = HTONS(adi2);
		dgus_write_current_page();

		page35_ro_var.v2 = HTONL((u32)(evc_info.read.v * 100));
		page35_ro_var.i2 = HTONL((u32)(evc_info.read.i * 10));

		dgus_init_ro_var(35);
		//LCD_MSG("ad_v2 %d   ad_v2 %d\n", adv2, adi2);
	}

	return PAGE_ID_INVALID;
}

u8 page35_poll_handler(u32 poll_cnt)
{
	page35_wo_var.v = HTONL(meas_info.v);
	page35_wo_var.i = HTONL(meas_info.i);
	dgus_write_page(35);
	return PAGE_ID_INVALID;
}

page_t page35 =
{
		page35_poll_handler,  //poll_handler
		page35_vchange_handler,  //vchange_handler
		page35_button_handler,  //button_handler
		page35_init,  //page_init
		&page35_wo_var,  //wo
		&page35_ro_var,  //ro
		1000,     //poll_time
		sizeof(page35_wo_var),     //wo_len
		sizeof(page35_ro_var),     //ro_len
};
/*********************************************************************************************/



page_t* UI_data[] =
{
	&page0,  //LOGO
	&page1,  //������ ��ˢ��  �����ǹ
	&page2,  //������ ��ˢ��  ���ˢ�����ǹ
	&page3,  //��������
	&page4,  //��緽ʽ
	&page5,  //������
	&page6,  //�������
	&page7,  //����ʱ��
	NULL,
	&page9,  //����ֹͣ���
	&page10,  //�����������
	&page11,  //��ʾ�ٴ�ˢ��
	&page12,  //�����
	NULL,
	&page14,  //������  ��ǹ
	NULL,
	NULL,  //16
	NULL,  //17
	NULL,  //18
	NULL,  //19
	&page20,  //������ʾ
	&page21,  //OK��ʾ
	&page22,  //ѯ����ʾ
	&page23,  //ϵͳ����
	&page24,  //��Կ�޸�
	&page25,  //ʱ������
	&page26,  //ģ��״̬
	&page27,  //ϵͳ��Ϣ
	&page28,  //�ֶ����
	&page29,  //�������
	&page30,  //�����޸�
	&page31,  //��������
	&page32,  //��ʷ��¼
	NULL,
	NULL,
	&page35,  //У��
};











