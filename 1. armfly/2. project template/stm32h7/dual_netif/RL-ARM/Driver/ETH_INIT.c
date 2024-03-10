/*
*********************************************************************************************************
*
*	ģ������ : ��̫����ʼ��
*	�ļ����� : ETH_INIT.c
*	��    �� : V1.0
*	˵    �� : ��̫����ر�����״̬��Ϣ����̫���������
*
*	�޸ļ�¼ :
*		�汾��   ����         ����        ˵��
*		V1.0    2019-10-12   Eric2013     �װ�
*
*	Copyright (C), 2019-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
   ��ֲע�����
   ���˱��ļ��е����ã����������ط�Ҫ�����Լ�������Ӳ��������
   1��bsp.h�ļ��У�����MPU_Config������̫���շ���������ַ�ռ��MPU���á�
   2��stm32h7xx_hal_conf.h�ļ����ķ���ETH_TX_DESC_CNT�ͽ���ETH_RX_DESC_CNT�������������á�
   3������ʹ�õ���̫��PHYоƬ��ѡ����ʵ�PHY������
*/

/*
*********************************************************************************************************
*	                                  ���ڱ��ļ��ĵ���
*********************************************************************************************************
*/
#if 1
	#define printf_eth printf
#else
	#define printf_debug(...)
#endif

/*
**********************************************************************************************************
											����
**********************************************************************************************************
*/
/* Ethernet Rx DMA ������ */
__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; 

/* Ethernet Tx DMA ������ */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; 

/* Ethernet ���ջ��� */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; 


ETH_HandleTypeDef heth;

/* ��̫������״̬��0��1����ʾ��ʼ��ʱ״̬��2��ʾ�����ϣ�3��ʾ�Ͽ� */
__IO uint8_t  g_ucDM9162LinkStatus = 0;  
__IO uint8_t  g_ucDM9000LinkStatus = 0;  

/*
*********************************************************************************************************
*	�� �� ��: netETH_Notify
*	����˵��: ��̫��״̬��Ϣ
*	��    ��: ---
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void netETH_Notify (uint32_t if_num, netETH_Event event, uint32_t val) 
{
	NET_ETH_LINK_INFO *info;

    /* DM9162 */
    if(if_num == 0)
    {
        switch (event) 
        {
            case netETH_LinkDown:
                if(g_ucDM9162LinkStatus == 2)
                {
                    g_ucDM9162LinkStatus = 3;
                }
                else
                {
                    g_ucDM9162LinkStatus = 1;
                }
                printf_eth ("DM9162 Link is down\r\n");
                break;
            
            case netETH_LinkUp:
                g_ucDM9162LinkStatus = 2;
                printf_eth ("DM9162 Link is up\r\n");
                info = (NET_ETH_LINK_INFO *)&val;
                
                switch (info->speed) 
                {
                    case 0:
                        printf_eth ("DM9162 10 MBit\r\n");
                        break;
                    
                    case 1:
                        printf_eth ("DM9162 100 MBit\r\n");
                        break;
                    
                    case 2:
                        printf_eth ("DM9162 1 GBit\r\n");
                        break;
                }
                
                switch (info->duplex) 
                {
                    case 0:
                        printf_eth ("DM9162 Half duplex\r\n");
                        break;
                    case 1:
                        printf_eth ("DM9162 Full duplex\r\n");
                        break;
                }
                break;
            
            case netETH_Wakeup:
                printf_eth ("DM9162 Wakeup frame received\r\n");
                break;
            
            case netETH_TimerAlarm:
                printf_eth ("DM9162 Timer alarm\r\n");
                break;
        }  
    }
    /* DM9000 */
    else
    {
        switch (event) 
        {
            case netETH_LinkDown:
                if(g_ucDM9000LinkStatus == 2)
                {
                    g_ucDM9000LinkStatus = 3;
                }
                else
                {
                    g_ucDM9000LinkStatus = 1;
                }
                printf_eth ("DM9000 Link is down\r\n");
                break;
            
            case netETH_LinkUp:
                g_ucDM9000LinkStatus = 2;
                printf_eth ("DM9000 Link is up\r\n");
                info = (NET_ETH_LINK_INFO *)&val;
                
                switch (info->speed) 
                {
                    case 0:
                        printf_eth ("DM9000 10 MBit\r\n");
                        break;
                    
                    case 1:
                        printf_eth ("DM9000 100 MBit\r\n");
                        break;
                    
                    case 2:
                        printf_eth ("DM9000 1 GBit\r\n");
                        break;
                }
                
                switch (info->duplex) 
                {
                    case 0:
                        printf_eth ("DM9000 Half duplex\r\n");
                        break;
                    case 1:
                        printf_eth ("DM9000 Full duplex\r\n");
                        break;
                }
                break;
            
            case netETH_Wakeup:
                printf_eth ("DM9000 Wakeup frame received\r\n");
                break;
            
            case netETH_TimerAlarm:
                printf_eth ("DM9000 Timer alarm\r\n");
                break;
        }        
    }

}

/*
*********************************************************************************************************
*	�� �� ��: HAL_ETH_MspInit
*	����˵��: ��̫����ʼ�����õĵײ�ص������ڳ�ʼ��IO��ʱ�Ӻ��ж�
*	��    ��: ---
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_ETH_MspInit(ETH_HandleTypeDef* heth)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/*
		PC1     ------> ETH_MDC
		PA1     ------> ETH_REF_CLK
		PA2     ------> ETH_MDIO
		PA7     ------> ETH_CRS_DV
		PC4     ------> ETH_RXD0
		PC5     ------> ETH_RXD1
		PB13    ------> ETH_TXD1
		PG11    ------> ETH_TX_EN
		PG13    ------> ETH_TXD0 
	*/
	if(heth->Instance==ETH)
	{
		/* ʹ������ʱ�� */
		__HAL_RCC_ETH1MAC_CLK_ENABLE();
		__HAL_RCC_ETH1TX_CLK_ENABLE();
		__HAL_RCC_ETH1RX_CLK_ENABLE();

		/* ʹ��ʱ�� */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();

		
		/* ����PA1, PA2 , PA7 */
		GPIO_InitStruct.Pin =  GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL ; 
		GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* ����PB13 */
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* ����PC1, PC4��PC5 */
		GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5; 
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	

		/* ����PG11, PG12��PG13 */
		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

		/* �����ж����ȼ� */
		HAL_NVIC_SetPriority(ETH_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(ETH_IRQn);	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: HAL_ETH_MspDeInit
*	����˵��: ��̫����λ�Ļص����������ڹر�ʱ�ӣ�GPIO���жϡ�
*	��    ��: ---
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
	if(heth->Instance==ETH)
	{
		/* �ر�ʱ�� */
		__HAL_RCC_ETH1MAC_CLK_DISABLE();
		__HAL_RCC_ETH1TX_CLK_DISABLE();
		__HAL_RCC_ETH1RX_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
		HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13);

		/* �ر��ж� */
		HAL_NVIC_DisableIRQ(ETH_IRQn);
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ETH_IRQHandler
*	����˵��: ��̫���ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ETH_IRQHandler(void)
{
	HAL_ETH_IRQHandler(&heth);
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
