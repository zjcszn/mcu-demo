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

#include "i2c.h"

/* Max cycles approximately to wait on RXDREADY and TXDREADY event,
 * This is optimized way instead of using timers, this is not power aware. */
#define MAX_TIMEOUT_LOOPS (20000UL) /**< MAX while loops to wait for RXD/TXD event */

#define TWI_READ_BIT                 (0x01)        //!< If this bit is set in the address field, transfer direction is from slave to master.


#define TWI1_SCL_HIGH()   do { NRF_GPIO->OUTSET = (1UL << I2C1_SCL_pin); } while(0)   /*!< Pulls SCL line high */
#define TWI1_SCL_LOW()    do { NRF_GPIO->OUTCLR = (1UL << I2C1_SCL_pin); } while(0)   /*!< Pulls SCL line low  */
#define TWI1_SDA_HIGH()   do { NRF_GPIO->OUTSET = (1UL << I2C1_SDA_pin);  } while(0)   /*!< Pulls SDA line high */
#define TWI1_SDA_LOW()    do { NRF_GPIO->OUTCLR = (1UL << I2C1_SDA_pin);  } while(0)   /*!< Pulls SDA line low  */
#define TWI1_SDA_INPUT()  do { NRF_GPIO->DIRCLR = (1UL << I2C1_SDA_pin);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI1_SDA_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << I2C1_SDA_pin);  } while(0)   /*!< Configures SDA pin as output */
#define TWI1_SCL_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << I2C1_SCL_pin); } while(0)   /*!< Configures SCL pin as output */

#define TWI1_SDA_READ() ((NRF_GPIO->IN >> I2C1_SDA_pin) & 0x1UL)                     /*!< Reads current state of SDA */
#define TWI1_SCL_READ() ((NRF_GPIO->IN >> I2C1_SCL_pin) & 0x1UL)                    /*!< Reads current state of SCL */

static bool twi1_master_write(uint8_t * data, uint8_t data_length, bool issue_stop_condition)
{
    uint32_t timeout = MAX_TIMEOUT_LOOPS;
    if (data_length == 0)
        return false;
    NRF_TWI1->TXD           = *data++;
    NRF_TWI1->TASKS_STARTTX = 1;
    while (true)
    {
        while (NRF_TWI1->EVENTS_TXDSENT == 0 && NRF_TWI1->EVENTS_ERROR == 0 && (--timeout));

        if (timeout == 0 || NRF_TWI1->EVENTS_ERROR != 0)
        {
            NRF_TWI1->EVENTS_ERROR = 0;
            NRF_TWI1->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos; 
//            NRF_TWI1->POWER        = 0;
            nrf_delay_us(5);
            //NRF_TWI1->POWER        = 1;
            NRF_TWI1->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
            (void)twi1_master_init();
            return false;
        }
        NRF_TWI1->EVENTS_TXDSENT = 0;
        if (--data_length == 0)
            break;
        NRF_TWI1->TXD = *data++;
    }
    if (issue_stop_condition)
    {
        NRF_TWI1->EVENTS_STOPPED = 0;
        NRF_TWI1->TASKS_STOP     = 1;
        while(NRF_TWI1->EVENTS_STOPPED == 0);
    }
    return true;
}


static bool twi1_master_read(uint8_t * data, uint8_t data_length, bool issue_stop_condition)
{
	uint32_t timeout = MAX_TIMEOUT_LOOPS; 

	if (data_length == 0)
	{
		return false;
	}
	else if (data_length == 1)
	{
		NRF_PPI->CH[I2C1_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI1->TASKS_STOP;
	}
	else
	{
		NRF_PPI->CH[I2C1_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI1->TASKS_SUSPEND;
	}
	NRF_PPI->CHENSET  |= (1<<I2C1_PPI_CHANNEL);
	NRF_TWI1->EVENTS_RXDREADY = 0;
	NRF_TWI1->TASKS_STARTRX   = 1;
	while (true)
	{
		while (NRF_TWI1->EVENTS_RXDREADY == 0 && NRF_TWI1->EVENTS_ERROR == 0 && (--timeout));
		NRF_TWI1->EVENTS_RXDREADY = 0;
		if (timeout == 0 || NRF_TWI1->EVENTS_ERROR != 0)
		{
				NRF_TWI1->EVENTS_ERROR = 0;
				NRF_TWI1->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
				//NRF_TWI1->POWER        = 0;
				nrf_delay_us(5);
				//NRF_TWI1->POWER        = 1;
				NRF_TWI1->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
				(void)twi1_master_init();
				return false;
		}
		*data++ = NRF_TWI1->RXD;
		if (--data_length == 1)
				NRF_PPI->CH[I2C1_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI1->TASKS_STOP;
		if (data_length == 0)
				break;
		nrf_delay_us(20);
		NRF_TWI1->TASKS_RESUME = 1;
	}
	while(NRF_TWI1->EVENTS_STOPPED == 0);
	NRF_TWI1->EVENTS_STOPPED = 0;
	NRF_PPI->CHENCLR = (1 << I2C1_PPI_CHANNEL);
	return true;
}

static bool twi1_master_clear_bus(void)
{
    uint32_t twi_state;
    bool     bus_clear;
    uint32_t clk_pin_config;
    uint32_t data_pin_config;

    twi_state        = NRF_TWI1->ENABLE;
    NRF_TWI1->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    clk_pin_config = \
        NRF_GPIO->PIN_CNF[I2C1_SCL_pin];
				NRF_GPIO->PIN_CNF[I2C1_SCL_pin] =      \
        (GPIO_PIN_CNF_SENSE_Disabled  << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos)   \
      | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)    \
      | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)   \
      | (GPIO_PIN_CNF_DIR_Output    << GPIO_PIN_CNF_DIR_Pos);

    data_pin_config = \
        NRF_GPIO->PIN_CNF[I2C1_SDA_pin];
				NRF_GPIO->PIN_CNF[I2C1_SDA_pin] =       \
        (GPIO_PIN_CNF_SENSE_Disabled  << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos)   \
      | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)    \
      | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)   \
      | (GPIO_PIN_CNF_DIR_Output    << GPIO_PIN_CNF_DIR_Pos);

    TWI1_SDA_HIGH();
    TWI1_SCL_HIGH();
    TWI_DELAY();
		
    if ((TWI1_SDA_READ() == 1) && (TWI1_SCL_READ() == 1))
        bus_clear = true;
    else
    {
        uint_fast8_t i;
        bus_clear = false;
        for (i=18; i--;)
        {
            TWI1_SCL_LOW();
            TWI_DELAY();
            TWI1_SCL_HIGH();
            TWI_DELAY();
            if (TWI1_SDA_READ() == 1)
            {
                bus_clear = true;
                break;
            }
        }
    }
    NRF_GPIO->PIN_CNF[I2C1_SCL_pin] = clk_pin_config;
    NRF_GPIO->PIN_CNF[I2C1_SDA_pin] = data_pin_config;
    NRF_TWI1->ENABLE = twi_state;
    return bus_clear;
}


bool twi1_master_init(void)
{
    NRF_GPIO->PIN_CNF[I2C1_SCL_pin] =     \
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
      | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  \
      | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
      | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);

    NRF_GPIO->PIN_CNF[I2C1_SDA_pin] =      \
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
      | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  \
      | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
      | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);

    NRF_TWI1->EVENTS_RXDREADY = 0;
    NRF_TWI1->EVENTS_TXDSENT  = 0;
	
    NRF_TWI1->PSELSCL         = I2C1_SCL_pin;
    NRF_TWI1->PSELSDA         = I2C1_SDA_pin;
    NRF_TWI1->FREQUENCY       = TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;
	
	  nrf_twi_event_clear(NRF_TWI1, NRF_TWI_EVENT_STOPPED);
    nrf_twi_event_clear(NRF_TWI1, NRF_TWI_EVENT_RXDREADY);
    nrf_twi_event_clear(NRF_TWI1, NRF_TWI_EVENT_TXDSENT);
    nrf_twi_event_clear(NRF_TWI1, NRF_TWI_EVENT_ERROR);
		
    NRF_PPI->CH[I2C1_PPI_CHANNEL].EEP        = (uint32_t)&NRF_TWI1->EVENTS_BB;
    NRF_PPI->CH[I2C1_PPI_CHANNEL].TEP        = (uint32_t)&NRF_TWI1->TASKS_SUSPEND;
		
    NRF_PPI->CHENCLR  = (1 << I2C1_PPI_CHANNEL);
    NRF_TWI1->ENABLE  = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
		
    return twi1_master_clear_bus();
}


bool twi1_master_transfer(uint8_t   address,
                         uint8_t * data,
                         uint8_t   data_length,
                         bool      issue_stop_condition)
{
    bool transfer_succeeded = false;
    if(data_length > 0 && twi1_master_clear_bus())
    {
      NRF_TWI1->ADDRESS = (address >> 1);
      if ((address & TWI_READ_BIT))
          transfer_succeeded = twi1_master_read(data, data_length, issue_stop_condition);
      else
          transfer_succeeded = twi1_master_write(data, data_length, issue_stop_condition);
    }
    return transfer_succeeded;
}


