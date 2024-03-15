

#include "head.h"


#define _UART_C

#define RECE_BUF_LEN	256
#define SEND_BUF_LEN	256

typedef struct 
{
	UINT8 send_buffer[SEND_BUF_LEN];	//FIFO
	UINT8 rece_buffer[RECE_BUF_LEN];	//FIFO
	
	UINT8 send_head;				
	UINT8 send_end;
	
	UINT8 rece_head;
	UINT8 rece_end;
	
	UINT8 state;					//OPEN OR CLOSE
	UINT8 transmiting_flag;			//���ڷ��ͱ�־
}PORT;

PORT com[PORT_MAX];						//���崮�ڵ���Դ


//��������
void UART_Configuration (void)  
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;						

	/* Enable USART clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 , ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2|RCC_APB1Periph_USART3 
		                   |RCC_APB1Periph_UART4|RCC_APB1Periph_UART5, ENABLE);

	/* Configure USARTx_Tx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/* Configure USARTx_Rx as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	//USART1_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART1_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	  	//USART2_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//USART2_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//USART3_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//USART3_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	//USART4_TX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//USART4_RX485
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART1, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART1, USART_IT_TC);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART1, ENABLE);       
  
	//UART2------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART2, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART2, USART_IT_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART2, ENABLE); 

	//UART3------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	//USART_InitStructure.USART_Clock = USART_Clock_Disable;
	//USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	//USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	//USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	/* Configure the USARTx */ 
	USART_Init(USART3, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(USART3, USART_IT_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(USART3, ENABLE); 


	//UART4------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(UART4, USART_IT_TC);
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(UART4, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(UART4, ENABLE);

	//UART5------------------------------------------------------------------------------------
	/* USARTx configuration ------------------------------------------------------*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART5, &USART_InitStructure);
	/* Enable the USART Receive interrupt: this interrupt is generated when the 
	USART1 receive data register is not empty */
	USART_ClearITPendingBit(UART5, USART_IT_TC);
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); 
	USART_ITConfig(UART5, USART_IT_TC, ENABLE);
	/* Enable the USARTx */
	USART_Cmd(UART5, ENABLE);
        
    open_port(COM1,9600,1,8,1,1);
    open_port(COM2,9600,1,8,1,1);
    open_port(COM3,9600,1,8,1,1);
	open_port(COM4,9600,1,8,1,1);
    open_port(COM5,9600,1,8,1,1);
}

/****************************************************************************************
** ��������: USART_SendChar
** ��������: UART1 �����ַ�
** ��    ��: USARTx: ���ں�,  Data: ����������
** �� �� ֵ: None       
** ����  ��: �޻���
** ��  ����: 2008��1��7��
**---------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void USART_SendChar(USART_TypeDef* USARTx, u8* Data)
{
  /* Transmit Data */
  USARTx->DR = (Data[0] & (u16)0x01FF);

  while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);  
}



  
//UART DRIVER FOR APP BY GUOHUI 20090708
//---------------------------------------------------------------------------


UINT8 open_port(UINT8 port,UINT32 baud,UINT8 startbit,UINT8 databit,UINT8 stopbit,UINT8 parity)
{	
	//�Ƿ��Լ��	
	if (( port > (PORT_MAX-1) ) || (baud > 230400) || (databit > 8))
	{
		return FALSE;
	}
	if (com[port].state == TRUE)		//�˿��Ѿ�����ֱ�ӷ���
	{
		return FALSE;
	}
	
	
	//
	com[port].send_head = 0;
	com[port].send_end  = 0;
	com[port].rece_head = 0;
	com[port].rece_end  = 0;
	
	com[port].transmiting_flag = FALSE;

	if (port == COM1)					//�򿪴���0
	{		
			 	
	}
	else if (port == COM2)
	{
			
	}
		
	com[port].state = OPEN;
	
	//
	
	//
	return TRUE;
	
}
/*
���ݷ��ͺ�����Ӧ�ò���ô˺��������ݷ��뷢��FIFO��������
���ݵķ��ͣ���UART���жϽ��С�
*/
UINT8 send_data(UINT8 port,UINT8 *pdata, UINT8 length)
{
	UINT8 i;
	if (port > PORT_MAX)				//�Ƿ��Լ��
	{
		return 0;
	}
//	send_str(0,pdata);
	
	if (   (com[port].state == CLOSE)
		|| (length == 0) )
		//|| ((com[port].send_head + 1) == com[port].send_end) )	//���ͻ�������.
	{
		return 0;
	} 
	
	for (i=0; i<length; i++)
	{
		com[port].send_buffer[(com[port].send_head++)] = pdata[i];
		if (com[port].send_head == com[port].send_end)
		{
			com[port].send_end++; 
		}
	}	
	
	if (com[port].transmiting_flag == 0)	
	{
		write_data(port);
		
		com[port].transmiting_flag = 1;
	}
	   	
	
	return i;
	
}

/**********************************************************************************************
��������UINT8 send_str(UINT8 port,UINT8 *pstr)
���ܣ�API����.Ӧ�ò���÷����ַ����ĺ���.
��ڲ��������ں�,�ַ���ָ��
���ڲ�����ʵ�ʷ��͵��ֽ���.
-------------------
��д����:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 send_str(UINT8 port,UINT8 *pstr)
{
	UINT8 i=0;
	
	while(pstr[i++] != '\0');
	
	return (send_data(port,pstr,i-1));	
}

/**********************************************************************************************
��������UINT8 get_uart_state(UINT8 port)
���ܣ�API����.Ӧ�ò���ü�鵱ǰ����״̬�ĺ���.
��ڲ��������ں�
���ڲ��������ڵ�ǰ״̬���رջ����Ǵ�
-------------------
��д����:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 get_uart_state(UINT8 port)
{
	if (port > PORT_MAX)	
	{
		return CLOSE;
	}	
	return (com[port].state);
} 

/**********************************************************************************************
��������UINT8 get_rec_len(UINT8 port)
���ܣ�API����.Ӧ�ò����.�õ��ײ���ջ������Ŀ����ֽ�����
��ڲ��������ں�
���ڲ����������ж��ٸ��ֽڿ���
-------------------
��д����:2005-09-30.GUOHUI
-------------------
***********************************************************************************************/
UINT8 get_rec_len(UINT8 port)
{
	UINT8 temp;
	//
	if (port > PORT_MAX)	
	{
		return 0;
	}
	if (com[port].state == CLOSE)
	{
		return 0;
	}	
	
	temp = com[port].rece_head - com[port].rece_end;
	
	return temp;
	
}

/**********************************************************************************************
��������UINT8 procure_data(UINT8 * Desp,UINT8 port,UINT8 len)
���ܣ�	API������Ӧ�ò���û�õײ����ݵĺ�����
��ڲ�����Ŀ�Ļ�������ַ���˿ںţ����ճ���
���ڲ������ɹ����յ��ֽ���
-------------------
��д����:2005-10-16.GUOHUI
------------------- 
ע�⣺һ������£��˺���Ӧ��UINT8 get_rec_len(UINT8 port)��������ʹ�á�
***********************************************************************************************/
UINT8 procure_data(UINT8 * Desp,UINT8 port,UINT8 len)
{
	UINT8 i;
	//�Ƿ��Լ��
	if ( (port > PORT_MAX) || (len == 0) )
	{
		return FALSE;
	}

	i = get_rec_len(port);

	if (i)
	{
		if (i < len) 
		{
			len = i;
		}
		for (i=0; i<len; i++)
		{
			*Desp++ = com[port].rece_buffer[com[port].rece_end++];
		}
	}
	
	return len;
	
}


/*
�жϵ��õĺ�����дӲ���ķ��ͼĴ���
*/
UINT8 write_data(UINT8 port)
{	  
	
	if (com[port].send_head == com[port].send_end)	//���û�����ݿ��Է���
	{	
		if (port == COM1)
		{
			USART_ITConfig(USART1, USART_IT_TC, DISABLE);
		}
		else if (port == COM2)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE);
		}
		else if(port == COM3)
		{
			USART_ITConfig(USART3, USART_IT_TC, DISABLE);
		}
		else if(port == COM4)
		{
			USART_ITConfig(UART4, USART_IT_TC, DISABLE);
		}
		else if(port == COM5)
		{
			USART_ITConfig(UART5, USART_IT_TC, DISABLE);
		}

		com[port].transmiting_flag = 0;

		return FALSE;		
	}

	if (port == COM1)
	{
		USART_ITConfig(USART1, USART_IT_TC, ENABLE);
	}
	else if (port == COM2)
	{
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
	else if (port == COM3)
	{
		USART_ITConfig(USART3, USART_IT_TC, ENABLE);
	}
	else if (port == COM4)
	{
		USART_ITConfig(UART4, USART_IT_TC, ENABLE);
	}
	else if (port == COM5)
	{
		USART_ITConfig(UART5, USART_IT_TC, ENABLE);
	}

	//
	
	//
	if (port == COM1)
	{
		USART_SendData(USART1, com[port].send_buffer[(com[port].send_end++)]);
	}
	else if (port == COM2)
	{		
		USART_SendData(USART2, com[port].send_buffer[(com[port].send_end++)]);

	}
	else if (port == COM3)
	{
		USART_SendData(USART3, com[port].send_buffer[(com[port].send_end++)]);
	}
	else if (port == COM4)
	{		
		USART_SendData(UART4, com[port].send_buffer[(com[port].send_end++)]);

	}
	else if (port == COM5)
	{
		USART_SendData(UART5, com[port].send_buffer[(com[port].send_end++)]);
	}


	//
	if (com[port].send_end == com[port].send_head)
	{		
		//com[port].transmiting_flag = FALSE;	
		
	}
	//
	
	//
	return TRUE;
	
}

void read_data(UINT8 port)
{  		
	//------------------------------------------------
	if (((com[port].rece_head+1) & 0xff) == com[port].rece_end)	//���ջ���������,�򷵻�
	{
		com[port].rece_end++;			
		//break;		
	}
	
	//------------------------------------------------
	if (port == COM1)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART1);
	}
	else if (port == COM2)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART2);
	}
	//-------------------------------------------------	 
	else if (port == COM3)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(USART3);
	}
	else if (port == COM4)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(UART4);
	}
	//-------------------------------------------------	 
	else if (port == COM5)
	{
		com[port].rece_buffer[com[port].rece_head++] = USART_ReceiveData(UART5);
	}
}

