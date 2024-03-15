#ifndef __SYS_MEM_H
#define __SYS_MEM_H


extern uint8_t SysMainBuffer_0[4096];       //������,����U�̽��̺�FATFS���̹���

extern uint8_t FrameBuffer[LCM_WIDTH][LCM_HIGH/8];         //��ʾ����, 128X32

void fifo_in(uint8_t *in, uint8_t *bf, uint16_t bf_size, uint8_t bf_n);
void fifo_clr(uint8_t *bf,uint16_t bf_size,uint8_t bf_n);

#endif


