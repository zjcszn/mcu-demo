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

#include "i2s.h"
//SCK frequency can never exceed the MCK frequency, which can be formulated as
//CONFIG.RATIO >= 2 * CONFIG.SWIDTH
//Integer = (CONFIG.RATIO / (2 * CONFIG.SWIDTH))
//SCK = 2 * LRCK * CONFIG.SWIDTH
//LRCK = MCK / CONFIG.RATIO
//LRCK = 8K SCK = 256K 32

void I2S_Init(void)
{
	
	// Enable reception
	NRF_I2S->CONFIG.RXEN = (I2S_CONFIG_RXEN_RXEN_Enabled <<
	I2S_CONFIG_RXEN_RXEN_Pos);

	// Enable transmission
	NRF_I2S->CONFIG.TXEN = (I2S_CONFIG_TXEN_TXEN_Enabled <<
	I2S_CONFIG_TXEN_TXEN_Pos);

	// Enable MCK generator
	NRF_I2S->CONFIG.MCKEN = (I2S_CONFIG_MCKEN_MCKEN_Enabled <<
	I2S_CONFIG_MCKEN_MCKEN_Pos);

	// MCKFREQ = 4 MHz
	NRF_I2S->CONFIG.MCKFREQ = I2S_CONFIG_MCKFREQ_MCKFREQ_32MDIV16 <<
	I2S_CONFIG_MCKFREQ_MCKFREQ_Pos;

	// Ratio = 256
	NRF_I2S->CONFIG.RATIO = I2S_CONFIG_RATIO_RATIO_256X <<
	I2S_CONFIG_RATIO_RATIO_Pos;

	// MCKFREQ = 4 MHz and Ratio = 256 gives sample rate = 15.625 ks/s
	// Sample width = 16 bit
	NRF_I2S->CONFIG.SWIDTH = I2S_CONFIG_SWIDTH_SWIDTH_16Bit <<
	I2S_CONFIG_SWIDTH_SWIDTH_Pos;

	// Alignment = Left
	NRF_I2S->CONFIG.ALIGN = I2S_CONFIG_ALIGN_ALIGN_Left <<
	I2S_CONFIG_ALIGN_ALIGN_Pos;

	// Format = I2S
	NRF_I2S->CONFIG.FORMAT = I2S_CONFIG_FORMAT_FORMAT_I2S <<
	I2S_CONFIG_FORMAT_FORMAT_Pos;

	// Use stereo
	NRF_I2S->CONFIG.CHANNELS = I2S_CONFIG_CHANNELS_CHANNELS_Stereo <<
	I2S_CONFIG_CHANNELS_CHANNELS_Pos;

	NRF_I2S->CONFIG.MODE = I2S_CONFIG_MODE_MODE_Master << I2S_CONFIG_MODE_MODE_Pos;
	
	nrf_gpio_cfg_output(I2S_MCK_PIN);
	nrf_gpio_cfg_output(I2S_SCK_PIN);
	nrf_gpio_cfg_output(I2S_LRCK_PIN);
	nrf_gpio_cfg_output(I2S_SDOUT_PIN);
	nrf_gpio_cfg_input(I2S_SDOUT_PIN, NRF_GPIO_PIN_NOPULL);
	
	
	// MCK routed to pin 0
	NRF_I2S->PSEL.MCK = (I2S_MCK_PIN << I2S_PSEL_MCK_PIN_Pos) |
	(I2S_PSEL_MCK_CONNECT_Connected <<I2S_PSEL_MCK_CONNECT_Pos);

	// SCK routed to pin 1
	NRF_I2S->PSEL.SCK = (I2S_SCK_PIN << I2S_PSEL_SCK_PIN_Pos) |
	(I2S_PSEL_SCK_CONNECT_Connected <<I2S_PSEL_SCK_CONNECT_Pos);

	// LRCK routed to pin 2
	NRF_I2S->PSEL.LRCK = (I2S_LRCK_PIN << I2S_PSEL_LRCK_PIN_Pos) |
	(I2S_PSEL_LRCK_CONNECT_Connected <<I2S_PSEL_LRCK_CONNECT_Pos);
	
	// SDOUT routed to pin 3
	NRF_I2S->PSEL.SDOUT = (I2S_SDOUT_PIN << I2S_PSEL_SDOUT_PIN_Pos) |
	(I2S_PSEL_SDOUT_CONNECT_Connected <<I2S_PSEL_SDOUT_CONNECT_Pos);
	
	// SDIN routed on pin 4
	NRF_I2S->PSEL.SDIN = (I2S_SDIN_PIN << I2S_PSEL_SDIN_PIN_Pos) |
	(I2S_PSEL_SDIN_CONNECT_Connected <<I2S_PSEL_SDIN_CONNECT_Pos);

	NVIC_SetPriority(I2S_IRQn, 2);		
	NVIC_ClearPendingIRQ(I2S_IRQn);		
	NVIC_EnableIRQ(I2S_IRQn);	
}

void nrfI2S_Start_Rec(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size)
{
	nrf_i2s_transfer_set(NRF_I2S,Size,my_rx_buf,my_tx_buf);

	nrf_i2s_enable(NRF_I2S);
	
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_STOPPED);

	NVIC_ClearPendingIRQ(I2S_IRQn);		
	NVIC_EnableIRQ(I2S_IRQn);	
	
	nrf_i2s_int_enable(NRF_I2S, NRF_I2S_INT_RXPTRUPD_MASK);
	nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
	
}

void nrfI2S_Start_play(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size)
{
	nrf_i2s_transfer_set(NRF_I2S,Size,my_rx_buf,my_tx_buf);

	nrf_i2s_enable(NRF_I2S);

	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_STOPPED);

	NVIC_ClearPendingIRQ(I2S_IRQn);		
	NVIC_EnableIRQ(I2S_IRQn);	
	
	nrf_i2s_int_enable(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK);
	nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
}

void nrfI2S_Start_FullDuplex(uint32_t *my_tx_buf,uint32_t *my_rx_buf,uint32_t Size)
{
	nrf_i2s_transfer_set(NRF_I2S,Size,my_rx_buf,my_tx_buf);

	nrf_i2s_enable(NRF_I2S);

	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_STOPPED);

	NVIC_ClearPendingIRQ(I2S_IRQn);		
	NVIC_EnableIRQ(I2S_IRQn);	
	
	nrf_i2s_int_enable(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK|NRF_I2S_INT_RXPTRUPD_MASK);
	nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_START);
}



void nrfI2S_Stop(void)
{
	NVIC_DisableIRQ(I2S_IRQn);	
	NVIC_ClearPendingIRQ(I2S_IRQn);		

	nrf_i2s_task_trigger(NRF_I2S, NRF_I2S_TASK_STOP);
	nrf_i2s_int_disable(NRF_I2S, NRF_I2S_INT_RXPTRUPD_MASK |NRF_I2S_INT_TXPTRUPD_MASK);

	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);
	nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_STOPPED);
	
}



