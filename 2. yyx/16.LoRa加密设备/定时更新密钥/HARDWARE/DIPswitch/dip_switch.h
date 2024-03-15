#ifndef __DIP_SWITCH_H
#define __DIP_SWITCH_H

#include "sys.h"
#include "hostcomprotocol.h"

//���뿪�غ궨��
#define		ID1			PCin(13)
#define		ID2			PBin(9)
#define		ID3			PBin(8)
#define		ID4			PBin(4)
#define		ID5			PBin(3)
#define		ID6			PAin(15)
#define		ID7			PAin(12)
#define		ID8			PAin(11)


//��������
extern u8 LoRaDeviceIs;


void	DIP_SwitchInit(void);
u8 		ReadDeviceID(void);
void	ConfirmUartx(void);
void 	SetUartBound(void);			//���ô��ڲ�����
void 	JudgeMasterOrSlave(void);	//�ж��豸����

#endif



