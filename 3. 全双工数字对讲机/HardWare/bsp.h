#include "core.h"


#define LED_RX 	27
#define KEY	  	31
#define SHUT		28	

void led_init(void);
void key_init(void);
uint8_t read_button(void);
void shutdown_init(void);
