#ifndef __RELAY_INIT_H
#define __RELAY_INIT_H

#include "sys.h"

#define  	RelayN			PBout(13)	  	//�̵���������
#define  	RelayP			PBout(14)		//�̵���������

#define		RelayOn()		{ RelayP=1; } 	//�����̵���
#define		RelayOff()		{ RelayP=0; } 	//�رռ̵���

extern u16 RelayCounter;


void 	RelayInit(void);
void 	RunRelayCounter(void);

#endif