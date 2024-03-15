#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include <string.h>
#include "sys.h"
#include "delay.h"
#include "hostcomprotocol.h"

#define 	COM1			1
#define		COM2			2
#define		COM3			3
#define		BAUDRATE 		115200


void 	Usart_X_Init(u8 WhichUsart);

void 	EnableDmaRec(u8 com);
u16 	GetUartData(u8 *buffer);
void	UsartDmaXSendData(u8 com,u8 *buf,u16 len);
void 	Usart_Init(u8 com,u32 bound,u16 Parity,u16 stopbit);	//���ں�����ʼ��
void 	DMA_TX_Init(u8 com,u8 *Sbuffaddr);						//����DMA�����ʼ��
void 	DMA_RX_Init(u8 com,u8 *Rbuffaddr);						//����DMA���ճ�ʼ��
void 	RTS_gpio_init(void);									//RTS���ų�ʼ��
void 	RS485M_Uart2_SendData(u8 *buf,u8 len);

#endif



