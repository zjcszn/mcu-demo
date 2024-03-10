/*
*********************************************************************************************************
*
*	ģ������ : CMSIS-Driver
*	�ļ����� : ETH_DM9000.c
*	��    �� : V1.0
*	˵    �� : CMSIS-Driver��DM9000�ײ�����ʵ�֡�������ԭ����
*
*	�޸ļ�¼ :
*		�汾��    ����        ����     ˵��
*		V1.0    2020-11-21   Eric2013 ��ʽ����
*
*	Copyright (C), 2020-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#ifndef _ETH_DM9000_H_
#define _ETH_DM9000_H_

#include <stdint.h>
#include "Driver_ETH_MAC.h"

/* ETH Driver state flags */
#define ETH_INIT                     (0x01UL)   /* Driver initialized */
#define ETH_POWER                    (0x02UL)   /* Driver powered     */

/* ETH Driver Control Information */
typedef struct {
  uint32_t Us_Cyc;
  uint32_t Tx_Len;
  uint16_t Tx_PacketTag;
  uint8_t  Flags;
  uint8_t  Rsvd;
  ARM_ETH_MAC_SignalEvent_t cb_event;   
} ETH_CTRL;

extern uint32_t SystemCoreClock;

#endif 

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
