//
// Created by zjcszn on 2023/5/23.
//

#ifndef USB_DEMO_BSP_ETH_H
#define USB_DEMO_BSP_ETH_H

#include "stdbool.h"

void bsp_netif_init(void);
bool bsp_netif_is_linkup(void);

#endif //USB_DEMO_BSP_ETH_H
