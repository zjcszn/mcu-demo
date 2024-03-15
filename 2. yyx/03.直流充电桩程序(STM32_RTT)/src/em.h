#ifndef EM_H_
#define EM_H_

#include "stm32f10x.h"

typedef struct
{
	u32 kwh;    //0.01��  ���γ�����ĵ���  ����em_calc ����´�����
	u32 sum;    //0.01Ԫ  ���γ�����  ���� em_calc ����´�����
	u32 kwh_cnt;  //���ǰ����
	u8 err_cnt;   //�������
}em_info_def;

extern int em_read_kwh(u32 *kwh);
extern int em_link_isok(void);
int em_calc(void);
int em_calc_init(void);

extern em_info_def em_info;


#endif
