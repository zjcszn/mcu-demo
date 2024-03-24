/************************************************************    
 FileName: esb.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 不想写    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "esb.h"

SemaphoreHandle_t nrfSndFlag;
SemaphoreHandle_t nrfAckFlag;
SemaphoreHandle_t nrfSndMutex;
QueueHandle_t 		nrfEvent;
static volatile uint8_t mState;	
EsbData_t mbox_rx;


// Function to do bytewise bit-swap on a unsigned 32 bit value
static uint32_t bytewise_bit_swap(uint8_t const * p_inp)
{
    uint32_t inp = (*(uint32_t*)p_inp);
#if defined(NRF52)

    return __REV((uint32_t)__RBIT(inp)); //lint -esym(628, __rev) -esym(526, __rev) -esym(628, __rbit) -esym(526, __rbit) */
#else
    inp = (inp & 0xF0F0F0F0) >> 4 | (inp & 0x0F0F0F0F) << 4;
    inp = (inp & 0xCCCCCCCC) >> 2 | (inp & 0x33333333) << 2;
    inp = (inp & 0xAAAAAAAA) >> 1 | (inp & 0x55555555) << 1;
    return inp;
#endif
}

// Internal function to convert base addresses from nRF24L type addressing to nRF51 type addressing
static uint32_t addr_conv(uint8_t const* p_addr)
{
    return __REV(bytewise_bit_swap(p_addr)); //lint -esym(628, __rev) -esym(526, __rev) */
}


void nrfSignalCreate(void)
{
	if(nrfSndFlag == NULL)
	{
		nrfSndFlag = xSemaphoreCreateBinary();
	}
	if(nrfAckFlag == NULL)
	{
		nrfAckFlag = xSemaphoreCreateBinary();
	}
	if(nrfSndMutex == NULL)
	{	
		nrfSndMutex = xSemaphoreCreateMutex();
	}
	if(nrfEvent == NULL)
	{		
		nrfEvent = xQueueCreate(5, sizeof(ESB_RX_t));
	}
}

void esb_payload_format(uint8_t mode)
{
	if(mode==1)	//动态包
	{
		NRF_RADIO->PCNF0 =
		(3 << RADIO_PCNF0_S1LEN_Pos)|		//设置为4为了兼容SDS-TWR 6次时序
		(0 << RADIO_PCNF0_S0LEN_Pos)|		//S0
		(6 << RADIO_PCNF0_LFLEN_Pos);		//动态包长范围size.2^n

		NRF_RADIO->PCNF1 =	
		(RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)|
		(RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)       |
		(4UL << RADIO_PCNF1_BALEN_Pos)|
		(0 << RADIO_PCNF1_STATLEN_Pos)|
		(m_box_size << RADIO_PCNF1_MAXLEN_Pos);	
		
	}
	else				//动态包固定包
	{
		NRF_RADIO->PCNF0 =
		(0 << RADIO_PCNF0_S1LEN_Pos)|		//配置S1的长度[19:16]——将S0配成0
		(0 << RADIO_PCNF0_S0LEN_Pos)|		//配置S0的长度[8]——————S0配成零
		(0 << RADIO_PCNF0_LFLEN_Pos);		//配置发送的数据包的大小[3:0]

		NRF_RADIO->PCNF1 =	
		(RADIO_PCNF1_WHITEEN_Disabled << RADIO_PCNF1_WHITEEN_Pos)|
		(RADIO_PCNF1_ENDIAN_Big << RADIO_PCNF1_ENDIAN_Pos)       |
		(4UL << RADIO_PCNF1_BALEN_Pos)|
		(m_box_size << RADIO_PCNF1_STATLEN_Pos)|						
		(m_box_size << RADIO_PCNF1_MAXLEN_Pos);					//固定包长设置最大为fix
	}
}


void ppi_init(void)
{
	/* 初始化PA和LAN收发脚,由于我用的是单脚,并且低电平为接收 */
	nrf_gpio_cfg_output(PA_RX_Pin);		
	NRF_GPIOTE->CONFIG[0]=
	(GPIOTE_CONFIG_MODE_Task<<GPIOTE_CONFIG_MODE_Pos)
	|	(PA_RX_Pin<<GPIOTE_CONFIG_PSEL_Pos)
	|(GPIOTE_CONFIG_POLARITY_Toggle<<GPIOTE_CONFIG_POLARITY_Pos)
	|(GPIOTE_CONFIG_OUTINIT_Low<<GPIOTE_CONFIG_OUTINIT_Pos);

//	nrf_gpio_cfg_output(PA_TX_Pin);		//
//	NRF_GPIOTE->CONFIG[1]=
//	(GPIOTE_CONFIG_MODE_Task<<GPIOTE_CONFIG_MODE_Pos)
//	|	(PA_TX_Pin<<GPIOTE_CONFIG_PSEL_Pos)
//	|(GPIOTE_CONFIG_POLARITY_Toggle<<GPIOTE_CONFIG_POLARITY_Pos)
//	|(GPIOTE_CONFIG_OUTINIT_High<<GPIOTE_CONFIG_OUTINIT_Pos);
	
	/* PPI绑定无线失能事件 触发GPIOTE任务 */
	NRF_PPI->CH[0].EEP = (uint32_t )&NRF_RADIO->EVENTS_DISABLED;
	NRF_PPI->CH[0].TEP = (uint32_t )&NRF_GPIOTE->TASKS_OUT[0];
	
//	NRF_PPI->FORK[0].TEP = (uint32_t )&NRF_GPIOTE->TASKS_OUT[1];
	/* 使能PPI通道0 */
	NRF_PPI->CHENSET |= 1<<0;
}

//可多次执行本程序,来替换频率
void esb_init(uint8_t Channel)
{	
	uint8_t base_addr_0[4] = {'B', 'C', 'D', 'E'};   //设置pipe0地址
	uint8_t addr_prefix[8] = {'A'};										//设置pipe0地址，其中addr_prefix[0]属于pipe0
	
	nrfSignalCreate();
	
	ppi_init();
	
	/* 启动电源 */
	NRF_RADIO->POWER = 1;		

	/*设置发送功率	*/
	NRF_RADIO->TXPOWER = RADIO_TXPOWER_TXPOWER_Pos4dBm << RADIO_TXPOWER_TXPOWER_Pos;
	
	/*设置空中速率 */
	NRF_RADIO->MODE = (RADIO_MODE_MODE_Nrf_250Kbit << RADIO_MODE_MODE_Pos);

	/* 设置频率 */
	NRF_RADIO->FREQUENCY = Channel;	//default Frequency

	/* 设置 收发地址 和基础地址,基础地址和节点的地址相同*/
	NRF_RADIO->PREFIX0 = bytewise_bit_swap(&addr_prefix[0]);
	NRF_RADIO->BASE0   = addr_conv(&base_addr_0[0]);

	/* 接收逻辑地址 1 的数据 (节点) */
	NRF_RADIO->RXADDRESSES = (1<<0);   // Enable device address 0 to use which receiving

  /* 设置无线数据报格式，长度和静态包或固定长度包 */
	esb_payload_format(1);					   //true:dynamic false:fixed payload
	
	/*				16位CRC设置			*/	
	NRF_RADIO->CRCCNF  = RADIO_CRCCNF_LEN_Two;	//CRC length set
	NRF_RADIO->CRCINIT = 0xFFFFUL;      // Initial value
  NRF_RADIO->CRCPOLY = 0x11021UL;     // CRC poly: x^16+x^12^x^5+1

	/* 接收或发送完成中断 */
	NRF_RADIO->INTENSET = RADIO_INTENSET_END_Msk;		
	NRF_RADIO->SHORTS = RADIO_SHORTS_READY_START_Msk;					
	NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;	
	NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RSSISTOP_Msk;	

	NVIC_SetPriority(RADIO_IRQn, 4);		
	NVIC_ClearPendingIRQ(RADIO_IRQn);		
	NVIC_EnableIRQ(RADIO_IRQn);						
	
	NRF_RADIO->PACKETPTR = (uint32_t)&mbox_rx;	  //配置接收数据包地址
	NRF_RADIO->TASKS_RXEN = 1U;										//开始接收模式
	
	mState = mRx;

}


/*
	使用ppi绑定disable来触发PA和LAN的使能,
	等待DISABLE=1;则才开始收发,避免功放没有
	正常切换完成就发送导致的错误
*/
#define TASK_DISABLE_STATE() {	\
	NRF_RADIO->EVENTS_DISABLED = 0;\
	NRF_RADIO->TASKS_DISABLE = 1;\
	while(NRF_RADIO->EVENTS_DISABLED == 0);\
	NRF_RADIO->EVENTS_DISABLED = 0;\
}


void SetRx(void)
{
	TASK_DISABLE_STATE();
	NRF_RADIO->SHORTS |= RADIO_SHORTS_ADDRESS_RSSISTART_Msk;	
	NRF_RADIO->SHORTS |= RADIO_SHORTS_DISABLED_RSSISTOP_Msk;
	NRF_RADIO->PACKETPTR = (uint32_t)&mbox_rx;	//配置接收数据包地址
	NRF_RADIO->TASKS_RXEN = 1U;									//开始接收模式	
	mState = mRx;
}

/**
	* @brief  发送数据包
  * @retval None
  */
uint8_t esbSendData(uint8_t *data,uint8_t length)
{
	uint8_t ret = 0;
	static uint8_t pid = 0;
	uint8_t timeCnt = 0;
	
	EsbData_t mbox_tx = {0};
	
	memcpy(mbox_tx.up_data,data,length);
	mbox_tx.pid   = pid;
	mbox_tx.size  = length;
	mbox_tx.noack = PKT_NACK ;  
	
_repl:	
	
	TASK_DISABLE_STATE();
	NRF_RADIO->SHORTS &= ~RADIO_SHORTS_ADDRESS_RSSISTART_Msk;	
	NRF_RADIO->SHORTS &= ~RADIO_SHORTS_DISABLED_RSSISTOP_Msk;	
	
	mState = mTx;
	NVIC_DisableIRQ(RADIO_IRQn);
	
	xSemaphoreTake(nrfAckFlag,0);
	NRF_RADIO->PACKETPTR = (uint32_t)&mbox_tx;
	NRF_RADIO->TASKS_TXEN = 1U;	
	NVIC_EnableIRQ(RADIO_IRQn);	
	
	xSemaphoreTake(nrfSndFlag, portMAX_DELAY);
	if(mbox_tx.noack)
	{
		if(xSemaphoreTake(nrfAckFlag,5)!=pdTRUE)
		{
			timeCnt++;
			if(timeCnt < 3)
			{
				goto _repl;
			}
			ret = 1;
		}
	}
	pid = (pid+1)%4;
	return ret;
}


int ReadRxQueue(ESB_RX_t *out,uint32_t timeout)
{
	int error;
	ESB_RX_t Data;	
	error = xQueueReceive(nrfEvent, (void *)&Data , timeout) ;
	if(error == pdTRUE)
	{
		memcpy(out,&Data,sizeof(ESB_RX_t));	
	}
	return error;
}

QueueHandle_t *GetRxQueue(void)
{
	return &nrfEvent;
}

void RADIO_IRQHandler(void)
{
	if(NRF_RADIO->EVENTS_END) 
	{
		NRF_RADIO->EVENTS_END = 0UL;
		switch (mState)
		{
			case mRx:
			{
				if(!NRF_RADIO->CRCSTATUS) 
				{
					NRF_RADIO->TASKS_START = 1;	
					return;
				}
				
				mbox_rx.crc  = NRF_RADIO->RXCRC;
				
				//fa
				ESB_RX_t inQueue;
				inQueue.rssi = NRF_RADIO->RSSISAMPLE;
				inQueue.size = mbox_rx.size;
				memcpy(inQueue.up_data,mbox_rx.up_data,mbox_rx.size);

				BaseType_t yield_req = pdFALSE;
				xQueueSendFromISR(nrfEvent,(void*)&inQueue,&yield_req);		
				portYIELD_FROM_ISR(yield_req);	
				
				NRF_RADIO->TASKS_START = 1;	 //继续接收模式
				break;
			}
			
			case mTx:
			{
				/* 切换到接受模式 */
				SetRx();
				
				/* 发送完成信号量 */
				BaseType_t yield_req = pdFALSE;
				xSemaphoreGiveFromISR(nrfSndFlag, &yield_req);
				portYIELD_FROM_ISR(yield_req);
			}
			break;			
		}
	}
}
