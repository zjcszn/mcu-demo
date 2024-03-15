#include "mcu_id.h"

u16 ChipFlashSize   = 0x00;		//����MCU��Flash����
u32 ChipUniqueID[3] = {0,};		//����MCU�ĵ���ǩ��(96bit)


/**************************************
	* ������������ȡоƬ��Ϣ
	* ��ڲ�������  
	* ����ֵ��	��
	* ��ע��	��
***************************************/
void GetChipInfo(void)
{
	ChipUniqueID[0] = *(__IO u32 *)(0x1FFFF7F0); 	//���ֽ�
	ChipUniqueID[1] = *(__IO u32 *)(0x1FFFF7EC); 	//�м��ֽ�
	ChipUniqueID[2] = *(__IO u32 *)(0x1FFFF7E8); 	//���ֽ�

	ChipFlashSize   = *(__IO u16 *)(0x1FFFF7E0);	//���������Ĵ���(��λ:KB)  
}




