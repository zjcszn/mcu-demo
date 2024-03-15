#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "evc.h"
#include "bms.h"
#include "shk.h"
#include "eth.h"
#include "lcd.h"
#include "measure.h"
#include "ic.h"
#include "sys_cfg.h"
#include "gprs.h"
#include "server.h"
#include "recorder.h"

/****************Ӧ�ó�������*********************************************************
1. system_stm32f10x.c �ж����ж��������ַƫ��     #define VECT_TAB_OFFSET  0x8000
2. ��keil�������� Target->IROM1��start����Ϊ0x8008000  size��Ϊ0x3F800
*************************************************************************************/

OS_TID evc_task_id, server_task_id;

static U64 stk_lcd[200];
static U64 stk_bms[200];
static U64 stk_evc[200];
static U64 stk_shk[200];
static U64 stk_eth[400];
static U64 stk_meas[200];
static U64 stk_gprs[200];
static U64 stk_server[200];

__task void task_Start (void)
{
	sflash_init();  //AT45DB��ʼ��

	sys_cfg_read();  //��ȡ���ò���

	meas_adj_read();

	ic_init();  //��������ʼ��

	recd_init();  //�����ʷ��¼

	os_tsk_create_user(task_lcd,  5, stk_lcd, sizeof(stk_lcd));  //lcd
	os_tsk_create_user(task_bms,  6, stk_bms, sizeof(stk_bms));  //bmsͨѶ
	evc_task_id = os_tsk_create_user(task_evc, 7, stk_evc, sizeof(stk_evc));  //���ģ��
	os_tsk_create_user(task_shk,  3, stk_shk, sizeof(stk_shk));  //���ǹ�����ź�
	os_tsk_create_user(task_eth,  1, stk_eth, sizeof(stk_eth));  //��̫��
	os_tsk_create_user(task_meas, 4, stk_meas, sizeof(stk_meas));  //ADC
	//os_tsk_create_user(task_gprs, 5, stk_gprs, sizeof(stk_gprs));  //gprs
	server_task_id = os_tsk_create_user(task_server, 2, stk_server, sizeof(stk_server));  //��̨����
	os_tsk_delete_self();
}

int main()
{
//	RCC_ClocksTypeDef R;
//	RCC_GetClocksFreq(&R);
//	dbg_msg("-------------------------------------------\n");
//	dbg_msg("SYSCLK: %d\n", R.SYSCLK_Frequency);
//	dbg_msg("-------------------------------------------\n");

	hw_init();  //���輰�жϳ�ʼ��
	//os_sys_init_user(task_Start, 10, stk_init, sizeof(stk_init));
	os_sys_init(task_Start);
}



void err_delay()
{
	volatile u32 d = 20000000;
	while(d--);
}



void HardFault_Handler()
{
	io_bms_sng1_dis();
	io_bms_sng2_dis();
	io_bms_dc_out_dis();
	config_on_off(0,0);
	err_delay();
	config_on_off(0,0);
	//err_delay();
	//err_delay();
	//io_evc_poweroff();
	//while(1);
	err_delay();
	err_delay();
	err_delay();
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

