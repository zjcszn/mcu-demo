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

#include "bsp.h"

/* !#----------> */
void led_init(void)
{
	nrf_gpio_cfg_output(LED_RX); 
	nrf_gpio_pin_clear(LED_RX);
}

/* !#----------> */
void key_init(void)
{
	nrf_gpio_cfg_input(KEY,NRF_GPIO_PIN_PULLUP);	
}

/* !#----------> */
uint8_t read_button(void)
{
	if(nrf_gpio_pin_read(KEY) == 0)
	{
		return true;
	}
	return false;
}

/* !#----------> */
void shutdown_init(void)
{
	nrf_gpio_cfg_output(SHUT);
	nrf_gpio_pin_clear(SHUT);
}


/*parameter
Input high voltage	0.7VDD
Input low voltage   0.3VDD
*/
