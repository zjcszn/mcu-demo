#include "i2c0.h"



#define TWI0_SCL_HIGH()   do { NRF_GPIO->OUTSET = (1UL << I2C0_SCL_pin); } while(0)   /*!< Pulls SCL line high */
#define TWI0_SCL_LOW()    do { NRF_GPIO->OUTCLR = (1UL << I2C0_SCL_pin); } while(0)   /*!< Pulls SCL line low  */
#define TWI0_SDA_HIGH()   do { NRF_GPIO->OUTSET = (1UL << I2C0_SDA_pin);  } while(0)   /*!< Pulls SDA line high */
#define TWI0_SDA_LOW()    do { NRF_GPIO->OUTCLR = (1UL << I2C0_SDA_pin);  } while(0)   /*!< Pulls SDA line low  */
#define TWI0_SDA_INPUT()  do { NRF_GPIO->DIRCLR = (1UL << I2C0_SDA_pin);  } while(0)   /*!< Configures SDA pin as input  */
#define TWI0_SDA_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << I2C0_SDA_pin);  } while(0)   /*!< Configures SDA pin as output */
#define TWI0_SCL_OUTPUT() do { NRF_GPIO->DIRSET = (1UL << I2C0_SCL_pin); } while(0)   /*!< Configures SCL pin as output */

#define TWI0_SDA_READ() ((NRF_GPIO->IN >> I2C0_SDA_pin) & 0x1UL)                     /*!< Reads current state of SDA */
#define TWI0_SCL_READ() ((NRF_GPIO->IN >> I2C0_SCL_pin) & 0x1UL)                    /*!< Reads current state of SCL */


static bool twi0_master_write(uint8_t * data, uint8_t data_length, bool issue_stop_condition)
{
    uint32_t timeout = MAX_TIMEOUT_LOOPS;
    if (data_length == 0)
        return false;
    NRF_TWI0->TXD           = *data++;
    NRF_TWI0->TASKS_STARTTX = 1;
    while (true)
    {
        while (NRF_TWI0->EVENTS_TXDSENT == 0 && NRF_TWI0->EVENTS_ERROR == 0 && (--timeout));

        if (timeout == 0 || NRF_TWI0->EVENTS_ERROR != 0)
        {
            NRF_TWI0->EVENTS_ERROR = 0;
            NRF_TWI0->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos; 
//            NRF_TWI0->POWER        = 0;
            delay_us(5);
            //NRF_TWI0->POWER        = 1;
            NRF_TWI0->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
            (void)twi0_master_init();
            return false;
        }
        NRF_TWI0->EVENTS_TXDSENT = 0;
        if (--data_length == 0)
            break;
        NRF_TWI0->TXD = *data++;
    }
    if (issue_stop_condition)
    {
        NRF_TWI0->EVENTS_STOPPED = 0;
        NRF_TWI0->TASKS_STOP     = 1;
        while(NRF_TWI0->EVENTS_STOPPED == 0);
    }
    return true;
}


static bool twi0_master_read(uint8_t * data, uint8_t data_length, bool issue_stop_condition)
{
    uint32_t timeout = MAX_TIMEOUT_LOOPS; 

    if (data_length == 0)
        return false;
    else if (data_length == 1)
        NRF_PPI->CH[I2C0_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI0->TASKS_STOP;
    else
        NRF_PPI->CH[I2C0_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI0->TASKS_SUSPEND;
    NRF_PPI->CHENSET |= 1 << I2C0_PPI_CHANNEL;
    NRF_TWI0->EVENTS_RXDREADY = 0;
    NRF_TWI0->TASKS_STARTRX   = 1;
    while (true)
    {
        while (NRF_TWI0->EVENTS_RXDREADY == 0 && NRF_TWI0->EVENTS_ERROR == 0 && (--timeout));
        NRF_TWI0->EVENTS_RXDREADY = 0;
        if (timeout == 0 || NRF_TWI0->EVENTS_ERROR != 0)
        {
            NRF_TWI0->EVENTS_ERROR = 0;
            NRF_TWI0->ENABLE       = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;
            //NRF_TWI0->POWER        = 0;
            delay_us(5);
            //NRF_TWI0->POWER        = 1;
            NRF_TWI0->ENABLE       = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
            (void)twi0_master_init();
            return false;
        }
        *data++ = NRF_TWI0->RXD;
        if (--data_length == 1)
            NRF_PPI->CH[I2C0_PPI_CHANNEL].TEP = (uint32_t)&NRF_TWI0->TASKS_STOP;
        if (data_length == 0)
            break;
        delay_us(20);
        NRF_TWI0->TASKS_RESUME = 1;
    }
    while(NRF_TWI0->EVENTS_STOPPED == 0);
    NRF_TWI0->EVENTS_STOPPED = 0;
    NRF_PPI->CHENCLR = 1 << I2C0_PPI_CHANNEL;
    return true;
}

static bool twi0_master_clear_bus(void)
{
    uint32_t twi_state;
    bool     bus_clear;
    uint32_t clk_pin_config;
    uint32_t data_pin_config;

    twi_state        = NRF_TWI0->ENABLE;
    NRF_TWI0->ENABLE = TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos;

    clk_pin_config = \
        NRF_GPIO->PIN_CNF[I2C0_SCL_pin];
				NRF_GPIO->PIN_CNF[I2C0_SCL_pin] =      \
        (GPIO_PIN_CNF_SENSE_Disabled  << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos)   \
      | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)    \
      | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)   \
      | (GPIO_PIN_CNF_DIR_Output    << GPIO_PIN_CNF_DIR_Pos);

    data_pin_config = \
        NRF_GPIO->PIN_CNF[I2C0_SDA_pin];
				NRF_GPIO->PIN_CNF[I2C0_SDA_pin] =       \
        (GPIO_PIN_CNF_SENSE_Disabled  << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1    << GPIO_PIN_CNF_DRIVE_Pos)   \
      | (GPIO_PIN_CNF_PULL_Pullup   << GPIO_PIN_CNF_PULL_Pos)    \
      | (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos)   \
      | (GPIO_PIN_CNF_DIR_Output    << GPIO_PIN_CNF_DIR_Pos);

    TWI0_SDA_HIGH();
    TWI0_SCL_HIGH();
    TWI_DELAY();
		
    if ((TWI0_SDA_READ() == 1) && (TWI0_SCL_READ() == 1))
        bus_clear = true;
    else
    {
        uint_fast8_t i;
        bus_clear = false;
        for (i=18; i--;)
        {
            TWI0_SCL_LOW();
            TWI_DELAY();
            TWI0_SCL_HIGH();
            TWI_DELAY();
            if (TWI0_SDA_READ() == 1)
            {
                bus_clear = true;
                break;
            }
        }
    }
    NRF_GPIO->PIN_CNF[I2C0_SCL_pin] = clk_pin_config;
    NRF_GPIO->PIN_CNF[I2C0_SDA_pin] = data_pin_config;
    NRF_TWI0->ENABLE = twi_state;
    return bus_clear;
}


bool twi0_master_init(void)
{
    NRF_GPIO->PIN_CNF[I2C0_SCL_pin] =     \
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
      | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  \
      | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
      | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);

    NRF_GPIO->PIN_CNF[I2C0_SDA_pin] =      \
        (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos) \
      | (GPIO_PIN_CNF_DRIVE_S0D1     << GPIO_PIN_CNF_DRIVE_Pos) \
      | (GPIO_PIN_CNF_PULL_Pullup    << GPIO_PIN_CNF_PULL_Pos)  \
      | (GPIO_PIN_CNF_INPUT_Connect  << GPIO_PIN_CNF_INPUT_Pos) \
      | (GPIO_PIN_CNF_DIR_Input      << GPIO_PIN_CNF_DIR_Pos);

    NRF_TWI0->EVENTS_RXDREADY = 0;
    NRF_TWI0->EVENTS_TXDSENT  = 0;
	
    NRF_TWI0->PSELSCL         = I2C0_SCL_pin;
    NRF_TWI0->PSELSDA         = I2C0_SDA_pin;
    NRF_TWI0->FREQUENCY       = TWI_FREQUENCY_FREQUENCY_K100 << TWI_FREQUENCY_FREQUENCY_Pos;
	
	  nrf_twi_event_clear(NRF_TWI0, NRF_TWI_EVENT_STOPPED);
    nrf_twi_event_clear(NRF_TWI0, NRF_TWI_EVENT_RXDREADY);
    nrf_twi_event_clear(NRF_TWI0, NRF_TWI_EVENT_TXDSENT);
    nrf_twi_event_clear(NRF_TWI0, NRF_TWI_EVENT_ERROR);
		
    NRF_PPI->CH[I2C0_PPI_CHANNEL].EEP        = (uint32_t)&NRF_TWI0->EVENTS_BB;
    NRF_PPI->CH[I2C0_PPI_CHANNEL].TEP        = (uint32_t)&NRF_TWI0->TASKS_SUSPEND;
		
    NRF_PPI->CHENCLR = 1 << I2C0_PPI_CHANNEL;
    NRF_TWI0->ENABLE = TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos;
		
    return twi0_master_clear_bus();
}


bool twi0_master_transfer(uint8_t   address,
                         uint8_t * data,
                         uint8_t   data_length,
                         bool      issue_stop_condition)
{
    bool transfer_succeeded = false;
    if(data_length > 0 && twi0_master_clear_bus())
    {
      NRF_TWI0->ADDRESS = (address >> 1);
      if ((address & TWI_READ_BIT))
          transfer_succeeded = twi0_master_read(data, data_length, issue_stop_condition);
      else
          transfer_succeeded = twi0_master_write(data, data_length, issue_stop_condition);
    }
    return transfer_succeeded;
}


