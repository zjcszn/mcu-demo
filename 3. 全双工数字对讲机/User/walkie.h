#ifndef WALKIE_H
#define WALKIE_H

#include "core.h"
#include "uart.h"


typedef signed char err_t;
typedef unsigned char       uint8_t ;
typedef unsigned short int  uint16_t  ;


#define ERR_OK 		        0
#define ERR_TIMEOUT	       -1
#define ERR_FALSE              -2
#define ERR_DATA	       -3
#define ERR_MALLOC_FALSE       -4
#define ERR_PARA_OUT           -5
#define ERR_EMPTY              -6


#define FRS_Input	             uart_recv
#define FRS_Input_LEN          uart_in
#define FRS_Input_Flag         userRecv 
#define FRS_Input_Clr          uart_pClear      
#define FRS_Outpt              uart_send_data

#define Ch_StartFrq 70

/* 初始频率 */
#define Default_Fre 431
#define Default_Low 3750

/* 初始亚音频 */
#define Default_RXCTCSS 		9
#define Default_TXCTCSS 		9


/* 转发板起始频率*/
#define Ch_Frequcy  431
#define Ch_Low      3750
/* 起始信道间隔 */
#define ch_Spacing  3
/* 转发板亚音频 */
#define RXCTCSS 		9
#define TXCTCSS 		9

/* 序列调频个数 */
#define Ch_NUM      10


#pragma anon_unions
typedef  struct
{	
 /*  模块工作参数的设置信息  */
								//1 一般专业对讲机选择宽带 
	uint8_t GBW;		//0 一般业余的和普通用途的对讲机均选择窄带
   
	uint8_t RXCXCSS;//00： 无编码

	uint8_t TXCXCSS;//00： 无编码
	
	uint16_t TX_Ch_Frequcy;//发射频率值: 400.0000-470.0000MHZ

	uint16_t TX_Ch_Low;
	
	uint16_t RX_Ch_Frequcy;// （要 6.25K 或 5K 的整数倍）根据带宽

	uint16_t RX_Ch_Low;

	uint16_t RM_Ch_Frequcy;// （要 6.25K 或 5K 的整数倍）根据带宽
	uint16_t RM_Ch_Low;
	uint8_t RMRXCXCSS;//00： 无编码
	uint8_t RMTXCXCSS;//00： 无编码	
	
	
	uint8_t SQ_Level;//Sq等级
        
	union{
		uint8_t FLAG ;
		struct{
			uint8_t H_L:1;
			uint8_t PRC:1;
			uint8_t Busy:1;
		};
	};
        
 /* 自动省电功能设置命令 */		
	uint8_t AUTOPOWCONTR;	// 0低功率 1高功率

 /* 音量级别参数取值为 1-9 级 */		
	uint8_t SETVOLUME;//声音等级

 /* 声控级别参数取值为 0-8 级（0 表示 OFF） */	
	uint8_t SETVOX;//声控等级

 /* 咪灵敏度、语音加密（扰频）及发射限时（TOT）设置应答*/	
	uint8_t MICLVL;
	uint8_t SRAMLVL;
	uint8_t TOT;

 /*             短信功能            */	
	union{	
		char TX_MS_Len;
		char TX_Mss[20];
	};
	
	union{
		char RX_MS_Len ;
		char RX_Mss[20];
	};
	
	uint8_t version[10]; //版本号

	uint8_t State;	 //模块状态
	
	uint8_t Midx;
	
}FRS_MODULE_PARA;

extern FRS_MODULE_PARA frs_Set;

void module_gpio_init(void);

err_t FRS_Init(void);
void frs_PowerUp(void);
void frs_PowerDown(void);
err_t FRS_Group_Set(FRS_MODULE_PARA *param);


err_t Walkie_Snd_Mess(uint8_t *data,uint8_t length);

err_t setModule_Ch(int idx,uint8_t ch_idx);

err_t setRemoto(void);


#define SQ  24
#define PTT 23
#define PD  22
#define SDP 11


#endif

