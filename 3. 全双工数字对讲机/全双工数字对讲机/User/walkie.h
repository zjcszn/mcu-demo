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

/* ��ʼƵ�� */
#define Default_Fre 431
#define Default_Low 3750

/* ��ʼ����Ƶ */
#define Default_RXCTCSS 		9
#define Default_TXCTCSS 		9


/* ת������ʼƵ��*/
#define Ch_Frequcy  431
#define Ch_Low      3750
/* ��ʼ�ŵ���� */
#define ch_Spacing  3
/* ת��������Ƶ */
#define RXCTCSS 		9
#define TXCTCSS 		9

/* ���е�Ƶ���� */
#define Ch_NUM      10


#pragma anon_unions
typedef  struct
{	
 /*  ģ�鹤��������������Ϣ  */
								//1 һ��רҵ�Խ���ѡ���� 
	uint8_t GBW;		//0 һ��ҵ��ĺ���ͨ��;�ĶԽ�����ѡ��խ��
   
	uint8_t RXCXCSS;//00�� �ޱ���

	uint8_t TXCXCSS;//00�� �ޱ���
	
	uint16_t TX_Ch_Frequcy;//����Ƶ��ֵ: 400.0000-470.0000MHZ

	uint16_t TX_Ch_Low;
	
	uint16_t RX_Ch_Frequcy;// ��Ҫ 6.25K �� 5K �������������ݴ���

	uint16_t RX_Ch_Low;

	uint16_t RM_Ch_Frequcy;// ��Ҫ 6.25K �� 5K �������������ݴ���
	uint16_t RM_Ch_Low;
	uint8_t RMRXCXCSS;//00�� �ޱ���
	uint8_t RMTXCXCSS;//00�� �ޱ���	
	
	
	uint8_t SQ_Level;//Sq�ȼ�
        
	union{
		uint8_t FLAG ;
		struct{
			uint8_t H_L:1;
			uint8_t PRC:1;
			uint8_t Busy:1;
		};
	};
        
 /* �Զ�ʡ�繦���������� */		
	uint8_t AUTOPOWCONTR;	// 0�͹��� 1�߹���

 /* �����������ȡֵΪ 1-9 �� */		
	uint8_t SETVOLUME;//�����ȼ�

 /* ���ؼ������ȡֵΪ 0-8 ����0 ��ʾ OFF�� */	
	uint8_t SETVOX;//���صȼ�

 /* �������ȡ��������ܣ���Ƶ����������ʱ��TOT������Ӧ��*/	
	uint8_t MICLVL;
	uint8_t SRAMLVL;
	uint8_t TOT;

 /*             ���Ź���            */	
	union{	
		char TX_MS_Len;
		char TX_Mss[20];
	};
	
	union{
		char RX_MS_Len ;
		char RX_Mss[20];
	};
	
	uint8_t version[10]; //�汾��

	uint8_t State;	 //ģ��״̬
	
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

