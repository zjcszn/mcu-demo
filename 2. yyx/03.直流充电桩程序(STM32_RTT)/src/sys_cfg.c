#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "sys_cfg.h"
#include "sflash.h"
#include "string.h"

//У�����
meas_adj_def meas_adj;  //���浽AT45DB ��0ҳ
//ϵͳ���ò���
sys_cfg_def sys_cfg;  //���浽AT45DB ��1ҳ


const meas_adj_def meas_adj_default =
{
		0xAAAA,
		0,
		100000,
		0,
		100000
};

const sys_cfg_def sys_cfg_default =
{
		0xAAAA,
		123456,  //ϵͳ����
		1, //ID
		100,  //e_price

		192,  //����IP
		168,
		1,
		10,

		192,  //����IP
		168,
		1,
		1,

		255,    //��������
		255,
		255,
		0,

		192,  //Զ��IP
		168,
		1,
		1,

		8080,  //Զ�̶˿�
};


/**
  * @brief  ��ȡϵͳ����
  * @retval 0 �ɹ�   <0 ʧ��
  */
int sys_cfg_read()
{
	u16 len = sizeof(sys_cfg_def);
	u8* addr = (u8*)&sys_cfg;
	sflash_read(1, 0, addr, len);
	if(sys_cfg.valid != 0xAAAA)
	{
		memcpy(addr, &sys_cfg_default, len);  //����ΪĬ�ϲ���
	}
	return 0;
}

/**
  * @brief  ����ϵͳ����
  * @retval 0 �ɹ�   <0 ʧ��
  */
int sys_cfg_save()
{
	u16 len = sizeof(sys_cfg_def);
	u8* addr = (u8*)&sys_cfg;
	sys_cfg.valid = 0xAAAA;
	sflash_write(1, 0, addr, len);
	return 0;
}



/**
  * @brief  ��ȡADУ������
  * @retval 0 �ɹ�   <0 ʧ��
  */
int meas_adj_read()
{
	u16 len = sizeof(meas_adj_def);
	u8* addr = (u8*)&meas_adj;
	sflash_read(0, 0, addr, len);
	if(meas_adj.valid != 0xAAAA)
	{
		memcpy(addr, &meas_adj_default, len);  //����ΪĬ�ϲ���
	}
	return 0;
}

/**
  * @brief  ����ADУ������
  * @retval 0 �ɹ�   <0 ʧ��
  */
int meas_adj_save()
{
	u16 len = sizeof(meas_adj_def);
	u8* addr = (u8*)&meas_adj;
	meas_adj.valid = 0xAAAA;
	sflash_write(0, 0, addr, len);
	return 0;
}




#if 0
//���ñ����ַ
//107ϵ��2Kһҳ,��128ҳ   ���ñ��������һҳ
#define CONFIG_ADDR (0x08000000 + 2048 * 127)

void sys_cfg_read()
{
	u16 len = sizeof(sys_cfg_def) / 2;
	u16* addr = (u16*)&sys_cfg;
	volatile u16 *cfg_addr = (volatile u16*)CONFIG_ADDR;

	while(len--)
	{
		*addr++ = *cfg_addr++;
	}
}


void sys_cfg_save()
{
	u16 len = sizeof(sys_cfg_def) / 2;
	u16* addr = (u16*)&sys_cfg;
	volatile u16 *cfg_addr = (volatile u16*)CONFIG_ADDR;

	FLASH_Unlock();

	FLASH_ErasePage(CONFIG_ADDR);  //�Ȳ���
	while(len--)
	{
		FLASH_ProgramHalfWord((u32)cfg_addr, *addr);  //д
		cfg_addr++;
		addr++;
	}

	FLASH_Lock();
}



//��ѹ�������� У������
#define MEAS_ADJ_ADDR (0x08000000 + 2048 * 127)

void meas_adj_read()
{
	u16 len = sizeof(meas_adj_def) / 2;
	u16* addr = (u16*)&meas_adj;
	volatile u16 *adj_addr = (volatile u16*)MEAS_ADJ_ADDR;

	while(len--)
	{
		*addr++ = *adj_addr++;
	}
}


void meas_adj_save()
{
	u16 len = sizeof(meas_adj_def) / 2;
	u16* addr = (u16*)&meas_adj;
	volatile u16 *adj_addr = (volatile u16*)MEAS_ADJ_ADDR;

	FLASH_Unlock();

	FLASH_ErasePage(MEAS_ADJ_ADDR);  //�Ȳ���
	while(len--)
	{
		FLASH_ProgramHalfWord((u32)adj_addr, *addr);  //д
		adj_addr++;
		addr++;
	}

	FLASH_Lock();
}

#endif
