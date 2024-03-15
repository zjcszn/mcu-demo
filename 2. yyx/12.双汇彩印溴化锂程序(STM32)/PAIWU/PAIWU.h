#ifndef _PAIWU_H
#define _PAIWU_H

#ifdef _PAIWU_C
#define EXT_PAIWU
#else
#define EXT_PAIWU	extern
#endif	  


//�������۷����� �ĸ���״̬
#define STATE_PAIWU_OPEN_EN			0	//��������۷�
#define STATE_PAIWU_OPEN_ING		1	//���ڴ����۷�
#define STATE_PAIWU_OPENED			2	//���۷��Ѿ���
#define STATE_PAIWU_CLOSE_EN		3	//���۷�����ر�
#define STATE_PAIWU_CLOSE_ING		4	//���۷����ڹر�
#define STATE_PAIWU_CLOSED			5	//���۷��Ѿ��ر�
#define STATE_PAIWU_DELAY			6	//��ʱ�������´�����



EXT_PAIWU u32 paiwu_num;
EXT_PAIWU u32 process_delay;



#define TASK_PAIWU_STK_SIZE	512
EXT_PAIWU OS_STK TaskPAIWUStk[TASK_PAIWU_STK_SIZE];

EXT_PAIWU void TaskPAIWU(void *pdata);




#endif 




