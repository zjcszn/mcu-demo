#include "core.h"
#include "nrf_delay.h"
#include "nrf_twi.h"

#define I2C1_PPI_CHANNEL 2

#define I2C1_SCL_pin 21
#define I2C1_SDA_pin 20


#define TWI_ISSUE_STOP               ((bool)true)  
#define TWI_DONT_ISSUE_STOP          ((bool)false) 
	

#define TWI_DELAY() nrf_delay_us(4) 


/* Max cycles approximately to wait on RXDREADY and TXDREADY event,
 * This is optimized way instead of using timers, this is not power aware. */
#define MAX_TIMEOUT_LOOPS (20000UL) /**< MAX while loops to wait for RXD/TXD event */
#define TWI_READ_BIT                 (0x01)        //!< If this bit is set in the address field, transfer direction is from slave to master.

#define TWI_ISSUE_STOP               ((bool)true)  
#define TWI_DONT_ISSUE_STOP          ((bool)false) 
	

bool twi1_master_init(void);
bool twi1_master_transfer(uint8_t   address,
                         uint8_t * data,
                         uint8_t   data_length,
                         bool      issue_stop_condition);






