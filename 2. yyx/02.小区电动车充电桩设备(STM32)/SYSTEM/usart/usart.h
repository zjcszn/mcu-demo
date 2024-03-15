#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
	
#define USART_REC_LEN  			300  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define	HEAD_CODE			0x7e
	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	

extern __IO uint16_t Rcv_Length;
extern uint8_t CmdCode;
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void USART_Timer_Ms(void);
void USART_Send_Char(uint8_t ch);
void USART_Send_Buf(uint8_t *buf,uint8_t len);
void USART_Send_String(char *s);
#endif


