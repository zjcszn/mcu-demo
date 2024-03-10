/*
*********************************************************************************************************
*
*	模块名称 : 以太网初始化
*	文件名称 : ETH_INIT.c
*	版    本 : V1.0
*	说    明 : 以太网相关变量，状态消息和以太网服务程序。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2019-10-12   Eric2013     首版
*
*	Copyright (C), 2019-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "includes.h"


/*
   移植注意事项：
   除了本文件中的配置，还有三个地方要根据自己制作的硬件做适配
   1、bsp.h文件中，函数MPU_Config做的以太网收发描述符地址空间的MPU配置。
   2、stm32h7xx_hal_conf.h文件做的发送ETH_TX_DESC_CNT和接收ETH_RX_DESC_CNT描述符个数配置。
   3、根据使用的以太网PHY芯片，选择合适的PHY驱动。
*/

/*
*********************************************************************************************************
*	                                  用于本文件的调试
*********************************************************************************************************
*/
#if 1
	#define printf_eth printf
#else
	#define printf_debug(...)
#endif

/*
**********************************************************************************************************
											变量
**********************************************************************************************************
*/
/* Ethernet Rx DMA 描述符 */
__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; 

/* Ethernet Tx DMA 描述符 */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; 

/* Ethernet 接收缓冲 */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; 


ETH_HandleTypeDef heth;

/* 以太网连接状态，0和1都表示初始临时状态，2表示连接上，3表示断开 */
__IO uint8_t  g_ucDM9162LinkStatus = 0;  
__IO uint8_t  g_ucDM9000LinkStatus = 0;  

/*
*********************************************************************************************************
*	函 数 名: netETH_Notify
*	功能说明: 以太网状态消息
*	形    参: ---
*	返 回 值: 无
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
*	函 数 名: HAL_ETH_MspInit
*	功能说明: 以太网初始化调用的底层回调，用于初始化IO，时钟和中断
*	形    参: ---
*	返 回 值: 无
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
		/* 使能外设时钟 */
		__HAL_RCC_ETH1MAC_CLK_ENABLE();
		__HAL_RCC_ETH1TX_CLK_ENABLE();
		__HAL_RCC_ETH1RX_CLK_ENABLE();

		/* 使能时钟 */
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOG_CLK_ENABLE();

		
		/* 配置PA1, PA2 , PA7 */
		GPIO_InitStruct.Pin =  GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_7;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL ; 
		GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* 配置PB13 */
		GPIO_InitStruct.Pin = GPIO_PIN_13;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* 配置PC1, PC4和PC5 */
		GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5; 
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);	

		/* 配置PG11, PG12和PG13 */
		GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_13;
		HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

		/* 设置中断优先级 */
		HAL_NVIC_SetPriority(ETH_IRQn, 3, 0);
		HAL_NVIC_EnableIRQ(ETH_IRQn);	
	}
}

/*
*********************************************************************************************************
*	函 数 名: HAL_ETH_MspDeInit
*	功能说明: 以太网复位的回调函数，用于关闭时钟，GPIO和中断。
*	形    参: ---
*	返 回 值: 无
*********************************************************************************************************
*/
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* heth)
{
	if(heth->Instance==ETH)
	{
		/* 关闭时钟 */
		__HAL_RCC_ETH1MAC_CLK_DISABLE();
		__HAL_RCC_ETH1TX_CLK_DISABLE();
		__HAL_RCC_ETH1RX_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
		HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13);

		/* 关闭中断 */
		HAL_NVIC_DisableIRQ(ETH_IRQn);
	}
}

/*
*********************************************************************************************************
*	函 数 名: ETH_IRQHandler
*	功能说明: 以太网回调函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void ETH_IRQHandler(void)
{
	HAL_ETH_IRQHandler(&heth);
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
