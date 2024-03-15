#include "w25qxx_init.h"


//������SPIģ��ĳ�ʼ�����룬���ó�����ģʽ������SD Card/W25Q64/NRF24L01						  
//SPI�ڳ�ʼ��
//�������Ƕ�SPI2�ĳ�ʼ��
//void W25QxxBaseInit(void)
//{
//	GPIO_InitTypeDef 	GPIO_InitStructure;
//	SPI_InitTypeDef  	SPI_InitStructure;

////	RCC_APB2PeriphClockCmd( SPIX_RCC,ENABLE);					//ʹ��SPIʼ��
//	RCC_APB1PeriphClockCmd(	SPIX_RCC,  ENABLE );				//SPI2ʱ��ʹ�� 	
//	RCC_APB2PeriphClockCmd(	SPIX_FLASH_CS_CLK      |
//							SPIX_PIN_SCK_PORT_CLK  |
//							SPIX_PIN_MISO_PORT_CLK |
//							SPIX_PIN_MOSI_PORT_CLK , ENABLE );	//ʹ��SPI����ʱ��

//	//SPIx -- SCK���� -- 
//	GPIO_InitStructure.GPIO_Pin   = SPIX_PIN_SCK;
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  			//PB13/14/15����������� 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(SPIX_PIN_SCK_PORT, &GPIO_InitStructure);			//��ʼ��GPIOB

//	//SPIx -- MISO���� -- 
//	GPIO_InitStructure.GPIO_Pin   =  SPIX_PIN_MISO;
//	GPIO_Init(SPIX_PIN_MISO_PORT, &GPIO_InitStructure);			//��ʼ��GPIOB
//	
//	//SPIx -- MOSI���� -- 
//	GPIO_InitStructure.GPIO_Pin   =  SPIX_PIN_MOSI;
//	GPIO_Init(SPIX_PIN_MOSI_PORT, &GPIO_InitStructure);			//��ʼ��GPIOB
//	
//	GPIO_SetBits(GPIOB,	GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	//����PB13/14/15
//	/////////////////////////////////////////////////////////////////////////////
//	GPIO_InitStructure.GPIO_Pin   = SPIX_FLASH_CS_PIN;  		//ѡ��Ƭѡ���� 
//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;			//�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(SPIX_FLASH_CS_PORT, &GPIO_InitStructure);			//��ʼ��Ƭѡ����
//	GPIO_SetBits(SPIX_FLASH_CS_PORT,SPIX_FLASH_CS_PIN);					

//	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
//	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					//����SPI����ģʽ:����Ϊ��SPI
//	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
//	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
//	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
//	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
//	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
//	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								//CRCֵ����Ķ���ʽ
//	SPI_Init(SPIX, &SPI_InitStructure);											//����ָ���Ĳ�����ʼ������SPIx�Ĵ���

//	SPI_Cmd(SPIX, ENABLE); 		//ʹ��SPI����
//	SPIX_ReadWriteByte(0xff);	//��������	 
//}   



//----------------------------
void W25QxxSPI_Init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;
	SPI_InitTypeDef  	SPI_InitStructure;

	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );	//SPI2ʱ��ʹ�� 	
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	//ʹ��SPI����ʱ��

	//SPIx -- SCK���� -- 
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;  	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	//��ʼ��GPIOB

	//SPIx -- MISO���� -- 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��GPIOB
	
	//SPIx -- MOSI���� -- 
	GPIO_InitStructure.GPIO_Pin   =  GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//��ʼ��GPIOB
	
	GPIO_SetBits(GPIOB,	GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);	//����PB13/14/15
	/////////////////////////////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;  		//ѡ��Ƭѡ���� 
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��Ƭѡ����
//	GPIO_SetBits(GPIOB,GPIO_Pin_12);					//
	W25QXX_CS = 1;										//����Ƭѡ����

	SPI_InitStructure.SPI_Direction 		= SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode 				= SPI_Mode_Master;					//����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize 			= SPI_DataSize_8b;					//SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 				= SPI_CPOL_High;					//����״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA 				= SPI_CPHA_2Edge;					//�ڶ��������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS 				= SPI_NSS_Soft;						//�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;		//������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit 			= SPI_FirstBit_MSB;					//���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial 	= 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(SPI2, &SPI_InitStructure);											//����ָ���Ĳ�����ʼ������SPIx�Ĵ���

	SPI_Cmd(SPI2, ENABLE); 		//ʹ��SPI����
	SPIX_ReadWriteByte(0xff);	//��������	 
}   

//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
void SPIX_SetSpeed(u8 SPI_BaudRatePrescaler)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI2,ENABLE); 
} 


//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPIX_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;	
	//���ָ����SPI��־λ�������:���ͻ���ձ�־λ	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) 
	{
		retry++;
		if(retry>200)
			return 0;
	}			  
	SPI_I2S_SendData(SPI2, TxData);		//ͨ������SPIx����һ������
	retry = 0;
	//���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) 
	{
		retry++;
		if(retry>200)
			return 0;
	}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
}



