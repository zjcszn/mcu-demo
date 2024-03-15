#include "core.h"
#include "nrf_delay.h"
#include "nrf_twi.h"
#include "i2c.h"

#define I2C0_PPI_CHANNEL 1

#define I2C0_SCL_pin 14
#define I2C0_SDA_pin 13


bool twi0_master_init(void);
bool twi0_master_transfer(uint8_t   address,
                         uint8_t * data,
                         uint8_t   data_length,
                         bool      issue_stop_condition);






