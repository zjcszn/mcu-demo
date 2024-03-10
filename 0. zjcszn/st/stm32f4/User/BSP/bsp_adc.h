//
// Created by zjcszn on 2023/8/6.
//

#ifndef _BSP_BSP_ADC_H_
#define _BSP_BSP_ADC_H_

#include <stdint.h>

void bsp_adc_init(void);
bool bsp_adc_read(uint16_t *read_value);

#endif //_BSP_BSP_ADC_H_
