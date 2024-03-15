#ifndef __SPI_H
#define __SPI_H

#include "sys.h"

#if 1
	#define		SPIX						SPI2
	#define 	SPIX_RCC					RCC_APB1Periph_SPI2

	#define 	SPIX_PIN_SCK_PORT_CLK		RCC_APB2Periph_GPIOB
	#define 	SPIX_PIN_SCK_PORT			GPIOB
	#define 	SPIX_PIN_SCK				GPIO_Pin_13

	#define 	SPIX_PIN_MISO_PORT_CLK		RCC_APB2Periph_GPIOB
	#define 	SPIX_PIN_MISO_PORT			GPIOB
	#define 	SPIX_PIN_MISO				GPIO_Pin_14

	#define 	SPIX_PIN_MOSI_PORT_CLK		RCC_APB2Periph_GPIOB
	#define 	SPIX_PIN_MOSI_PORT			GPIOB
	#define 	SPIX_PIN_MOSI				GPIO_Pin_15

	//Ƭѡ����
	#define 	SPIX_FLASH_CS_CLK       	RCC_APB2Periph_GPIOB  
	#define 	SPIX_FLASH_CS_PORT       	GPIOB
	#define 	SPIX_FLASH_CS_PIN       	GPIO_Pin_12
	
	#define		W25QXX_CS 					PBout(12)  		//W25QXX��Ƭѡ�ź� 

#else
	//-------------------------------------------------------------
	#define		SPIX						SPI1
	#define 	SPIX_RCC					RCC_APB2Periph_SPI1

	#define 	SPIX_PIN_SCK_PORT_CLK		RCC_APB2Periph_GPIOA
	#define 	SPIX_PIN_SCK_PORT			GPIOA
	#define 	SPIX_PIN_SCK				GPIO_Pin_5

	#define 	SPIX_PIN_MISO_PORT_CLK		RCC_APB2Periph_GPIOA
	#define 	SPIX_PIN_MISO_PORT			GPIOA
	#define 	SPIX_PIN_MISO				GPIO_Pin_6

	#define 	SPIX_PIN_MOSI_PORT_CLK		RCC_APB2Periph_GPIOA
	#define 	SPIX_PIN_MOSI_PORT			GPIOA
	#define 	SPIX_PIN_MOSI				GPIO_Pin_7
	
	//Ƭѡ����
	#define 	SPIX_FLASH_CS_CLK       	RCC_APB2Periph_GPIOA  
	#define 	SPIX_FLASH_CS_PORT       	GPIOA
	#define 	SPIX_FLASH_CS_PIN       	GPIO_Pin_2

	#define		W25QXX_CS 					PAout(2)  		//W25QXX��Ƭѡ�ź� 
	//-------------------------------------------------------------
#endif


// SPI�����ٶ����� 
#define 	SPI_SPEED_2   		0
#define 	SPI_SPEED_8   		1
#define 	SPI_SPEED_16  		2
#define 	SPI_SPEED_256 		3


void 	W25QxxSPI_Init(void);			//��ʼ��SPI��
void 	SPIX_SetSpeed(u8 SpeedSet); 	//����SPI�ٶ�   
u8  	SPIX_ReadWriteByte(u8 TxData);	//SPI���߶�дһ���ֽ�
		 
#endif



