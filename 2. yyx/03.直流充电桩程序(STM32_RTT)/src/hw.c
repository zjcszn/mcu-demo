#include "stm32f10x.h"


/*
 * @brief  NVIC��ʼ��
 */
void nvic_Init()
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	//NVIC  DMA UART4 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA2_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  UART4
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  UART5
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  DMA UART1 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  UART1 RX IDLE
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);


	//NVIC  DMA UART2 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  UART2 RX IDLE
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);


	//�ⲿ�����ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);


	//TIM4  BMS��ʱ
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//CAN1
	NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//CAN2
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  DMA SPI1 TX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC  DMA SPI1 RX
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}


/*
 * @brief  �������жϳ�ʼ��
 */
void hw_init()
{
	u32 temp;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);  //CRCУ��


	/******************************************TIM4���ڻ�����ʱ��BMS��ʱ��*********************************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;    //72MHzʱ�Ӽ���
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);  //10ms���
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);


	/******************************************TIM3���������źż��  (����AD654���Ƶ��)*********************************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV8;
	TIM_ICInitStructure.TIM_ICFilter = 0x04;
	//TIM_ICInit(TIM3, &TIM_ICInitStructure);
	TIM_PWMIConfig(TIM3, &TIM_ICInitStructure);
	/* Select the TIM3 Input Trigger: TI1FP1 */
	TIM_SelectInputTrigger(TIM3, TIM_TS_TI1FP1);
	/* Select the slave Mode: Reset Mode */
	TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Reset);
	/* Enable the Master/Slave Mode */
	TIM_SelectMasterSlaveMode(TIM3, TIM_MasterSlaveMode_Enable);
	//TIM_ITConfig(TIM3, TIM_IT_CC3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);


	/******************************************ADC ��ʼ��  ���ڼ���ѹ ����*********************************************/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_ADC1, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  //ADC12_IN14  ADC12_IN15
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //ʹ��ɨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  //ADCת������������ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  //���������ת��
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  //ת�������Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 2;  //������ͨ��  14 15
	ADC_Init(ADC1, &ADC_InitStructure); //��ʼ��ADC
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_239Cycles5);  //��ѹ
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 2, ADC_SampleTime_239Cycles5);  //����

	ADC_DMACmd(ADC1, ENABLE);//ʹ��ADC1ģ��DMA
	ADC_Cmd(ADC1, ENABLE);//ʹ��ADC1
	ADC_ResetCalibration(ADC1); //����ADC1У׼�Ĵ���
	while(ADC_GetResetCalibrationStatus(ADC1));//�ȴ�ADC1У׼�������
	ADC_StartCalibration(ADC1);//��ʼADC1У׼
	while(ADC_GetCalibrationStatus(ADC1));//�ȴ�ADC1У׼���
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE); //ʹ��ADC1�����ʼת��


	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_ConvertedValue; //����DMAͨ���洢����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//�����������ݿ��16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //����洢�����ݿ��16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMAͨ������ģʽλ���λ���ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);//��ʼ��DMAͨ��1
	//DMA_Cmd(DMA1_Channel1, ENABLE); //ʹ��DMAͨ��1


	/******************************************CAN1��س�ʼ��  ������EVC���ģ��ͨѶ*********************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap2_CAN1, ENABLE);
	CAN_StructInit(&CAN_InitStructure);
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	//CAN_InitStructure.CAN_ABOM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	/* CAN Baudrate = 125KBps*/    //PCLK1/((CAN_SJW + CAN_BS1 + CAN_BS2) * CAN_Prescaler)   PCLK1 = 36MHz
	//�Ƽ�������
	//1Mbps   �����£����õ��λ����6tqλ�ô���BS1=5, BS2=2
	//500kbps �����£����õ��λ����8tqλ�ô���BS1=7, BS2=3
	//250kbps �����£����õ��λ����14tqλ�ô���BS1=13, BS2=2
	//125k, 100k, 50k, 20k, 10k �Ĳ��õ�λ���� 250K ��ͬ��
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 18;
	CAN_Init(CAN1, &CAN_InitStructure);
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);


	/******************************************CAN2��س�ʼ��  ����������BMSͨѶ*********************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);
	//RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_CAN2, ENABLE);
	CAN_StructInit(&CAN_InitStructure);
	/* CAN cell init */
	CAN_InitStructure.CAN_TTCM = DISABLE;
	CAN_InitStructure.CAN_ABOM = ENABLE;
	CAN_InitStructure.CAN_AWUM = DISABLE;
	CAN_InitStructure.CAN_NART = DISABLE;
	CAN_InitStructure.CAN_RFLM = DISABLE;
	CAN_InitStructure.CAN_TXFP = DISABLE;
	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
	/* CAN Baudrate = 250KBps*/    //PCLK1/((CAN_SJW + CAN_BS1 + CAN_BS2) * CAN_Prescaler)   PCLK1 = 36MHz
	//�Ƽ�������
	//1Mbps   �����£����õ��λ����6tqλ�ô���BS1=5, BS2=2
	//500kbps �����£����õ��λ����8tqλ�ô���BS1=7, BS2=3
	//250kbps �����£����õ��λ����14tqλ�ô���BS1=13, BS2=2
	//125k, 100k, 50k, 20k, 10k �Ĳ��õ�λ���� 250K ��ͬ��
	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure.CAN_BS1 = CAN_BS1_13tq;
	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;
	CAN_InitStructure.CAN_Prescaler = 9;
	CAN_Init(CAN2, &CAN_InitStructure);
	CAN_FilterInitStructure.CAN_FilterNumber = 14;
	CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
#if 0  //����ID  ��16λΪOx56F4 E5F4 ��չ֡  ����֡
	CAN_FilterInitStructure.CAN_FilterIdHigh = (u16)((0xE5F4 >> 13) & 0x0007);
	CAN_FilterInitStructure.CAN_FilterIdLow = (u16)((0xE5F4 << 3) |CAN_ID_EXT | CAN_RTR_DATA);    //��ַ�̶�
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0007;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
#else
	CAN_FilterInitStructure.CAN_FilterIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterIdLow = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0;
	CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0;
#endif
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
	CAN_FilterInitStructure.CAN_FilterActivation = DISABLE;  //�Ȳ�ʹ��  ����Ӧ������ʹ��
	CAN_FilterInit(&CAN_FilterInitStructure);
	CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);



	/******************************************UART4��ʼ�� *********************************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	//PC11  RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//PC10  TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//UART4
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;    //�Ȳ�ʹ�ܽ��� ����Ӧtask��ʹ��
	USART_Init(UART4, &USART_InitStructure);
	USART_ClearFlag(UART4, USART_FLAG_TC);
	USART_DMACmd(UART4, USART_DMAReq_Tx,ENABLE);
	USART_DMACmd(UART4, USART_DMAReq_Rx, ENABLE);
	USART_ITConfig(UART4,USART_IT_IDLE,ENABLE);
	//DMA2_CH5  UART4 TX
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&UART4->DR; //Ŀ���ַ
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32) USART1_DMA_Buf1; //Դ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //����
	//DMA_InitStructure.DMA_BufferSize = 0; //DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ���������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�����ֽ�Ϊ��λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ��ֽ�Ϊ��λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //������ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //���ڴ浽�ڴ�
	DMA_Init(DMA2_Channel5, &DMA_InitStructure);
	DMA_ITConfig(DMA2_Channel5, DMA_IT_TC, ENABLE);
	//DMA2_CH3  UART4 RX
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //����
//	DMA_InitStructure.DMA_BufferSize = UART4_RX_BUF_LEN; //DMA����Ĵ�С
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart4_rx_buf; //Ŀ���ַ
	DMA_Init(DMA2_Channel3, &DMA_InitStructure);
	USART_Cmd(UART4, ENABLE);




	/******************************************UART5��ʼ��  �����������ͨѶ*********************************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
	//PD2  RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	//PC12  TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//UART5
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(UART5, &USART_InitStructure);
	USART_ClearFlag(UART5, USART_FLAG_TC);
	USART_ITConfig(UART5,USART_IT_IDLE,ENABLE);
	USART_Cmd(UART5, ENABLE);



	/******************************************SPI1��ʼ��  ����AT45DB321ͨѶ*********************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	//spi_clk  spi_mosi  spi_miso
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//spi_cs
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//AT45DB  WP
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode =  SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_Cmd(SPI1, ENABLE);

	//spi rx
	DMA_DeInit(DMA1_Channel2);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&SPI1->DR;;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart_TX_Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	//DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA�ڴ��ַ�Զ�����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel2, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel2, DMA_IT_TC, ENABLE);

	//spi tx
	DMA_DeInit(DMA1_Channel3);
	DMA_InitStructure.DMA_PeripheralBaseAddr =(u32)&SPI1->DR;
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)Usart_TX_Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	//DMA_InitStructure.DMA_BufferSize = bufsize;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA�ڴ��ַ�Զ�����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TC, ENABLE);




	/******************************************�������IO��ʼ��  *********************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//PD12 ~ PD15 �ⲿ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource12);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource14);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource15);
	//�½��ش���
//	EXTI_InitStructure.EXTI_Line = EXTI_Line12;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);
//
//	EXTI_InitStructure.EXTI_Line = EXTI_Line15;
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

	temp = GPIOE->ODR & 0xffffff00;
	GPIOE->ODR = temp;
	//PE0 ~ PE6 �̵���   PE7  beep
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |
							      GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);



	/******************************************USART1��ʼ��  ��������Ļ ���ͨѶ*********************************************/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	//PA10  RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//PA9  TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//UART1
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;  //�Ȳ�ʹ�ܽ��� ����Ӧtask��ʹ��
	USART_Init(USART1, &USART_InitStructure);
	USART_ClearFlag(USART1, USART_FLAG_TC);
	USART_DMACmd(USART1, USART_DMAReq_Tx,ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
	//DMA1_CH4  USART1 TX
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR; //Ŀ���ַ
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32) USART1_DMA_Buf1; //Դ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //����
	//DMA_InitStructure.DMA_BufferSize = 0; //DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ���������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�����ֽ�Ϊ��λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ��ֽ�Ϊ��λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //���ڴ浽�ڴ�
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	//DMA1_CH5  USART1 RX
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //����
	//DMA_InitStructure.DMA_BufferSize = USART1_RX_BUF_LEN; //DMA����Ĵ�С
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart1_rx_buf; //Ŀ���ַ
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	USART_Cmd(USART1, ENABLE);


	/******************************************USART2��ʼ�� *********************************************/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	//PD6  RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//PD5  TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	//UART2
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =	USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;  //�Ȳ�ʹ�ܽ��� ����Ӧtask��ʹ��
	USART_Init(USART2, &USART_InitStructure);
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_DMACmd(USART2, USART_DMAReq_Tx,ENABLE);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	//USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	//DMA1_CH7  USART2 TX
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART2->DR; //Ŀ���ַ
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32) USART1_DMA_Buf1; //Դ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; //����
	//DMA_InitStructure.DMA_BufferSize = 0; //DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //�����ַ�Ĵ���������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; //�ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //�����ֽ�Ϊ��λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //�ڴ��ֽ�Ϊ��λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; //
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //���ڴ浽�ڴ�
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	//DMA1_CH6  USART2 RX
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //����
	//DMA_InitStructure.DMA_BufferSize = USART1_RX_BUF_LEN; //DMA����Ĵ�С
	//DMA_InitStructure.DMA_MemoryBaseAddr = (u32)usart1_rx_buf; //Ŀ���ַ
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	USART_Cmd(USART2, ENABLE);

	nvic_Init();  //�жϳ�ʼ��
}




void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_Line12)  //PD12
	{
		EXTI->PR = EXTI_Line12;
		//dbg_msg("PD12\n");
	}

	if(EXTI->PR & EXTI_Line13)  //PD13
	{
		EXTI->PR = EXTI_Line13;
		//dbg_msg("PD13\n");
	}

	if(EXTI->PR & EXTI_Line14)  //PD14
	{
		EXTI->PR = EXTI_Line14;
		//dbg_msg("PD14\n");
	}

	if(EXTI->PR & EXTI_Line15)  //PD15
	{
		EXTI->PR = EXTI_Line15;
		//dbg_msg("PD15\n");
	}
}
















