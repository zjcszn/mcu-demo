#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "sflash.h"
#include "recorder.h"
#include "string.h"


#if 1
#define ETH_RECDER DBG_MSG
#else
#define ETH_RECDER (void)
#endif


#pragma pack(push) //�������״̬
#pragma pack(1)//�趨Ϊ1�ֽڶ���

typedef struct
{
	u16 valid;  //��־�Ƿ���Ч  == 0xAAAA ��Ч ���ڵ�һ���ϵ�
	u16 cnt;
	u16 newest_addr;  //����ID  0~16383   0xffff:��Ч
}record_header_def;

#pragma pack(pop) //�ָ�����״̬


record_header_def  recd_h;


///////�����ʷ��¼��Ϣռ��FLASH ��2047ҳ��ʼ��1MB + 512byte�ռ�
#define FLASH_PAGE_SIZE  512
#define RECD_HEADER_PAGEADDR 2047  //��ʷ��¼ͷ��Ϣ��ַ
#define RECD_DATA_PAGEADDR 2048  //��ʷ��¼��Ϣ��ʼ��ַ
#define RECD_DATA_SIZE 64  //һ����¼���ݴ�С
#define RECD_MAX_CNT     16384  //��ʷ��¼����  16384 * 64 = 1048576 (1MB)


/**
  * @brief  ��ȡ��¼ͷ��Ϣ
  * @retval 0 �ɹ�   <0 ʧ��
  */
int recd_header_read()
{
	sflash_read(RECD_HEADER_PAGEADDR, 0, (u8*)&recd_h, sizeof(recd_h));

	if(recd_h.valid != 0xAAAA)  //��Ч
	{
		recd_h.cnt = 0;
		recd_h.newest_addr = 0xffff;
	}
	return 0;
}


/*
 * @brief  ��ʼ��  ����ͷ��Ϣ
 */
void recd_init()
{
	recd_header_read();
	//current_id = recd_h.newest_addr;
}


/**
  * @brief  дͷ��Ϣ
  * @retval 0 �ɹ�   <0 ʧ��
  */
int recd_header_write()
{
	recd_h.valid = 0xAAAA;
	return sflash_write(RECD_HEADER_PAGEADDR, 0, (u8*)&recd_h, sizeof(recd_h));
}



/*
 * @brief �����¼
 */
void recd_clr()
{
	recd_h.cnt = 0;
	recd_h.newest_addr = 0xffff;
	recd_header_write();
}

/*
 * @brief  ��ȡ��¼
 * @param  addr    0~16383
 * @param  recd
 * @retval 0 �ɹ�   <0 ʧ��
 */
int recd_read(u16 addr, record_def* recd)
{
	u16 page_addr;
	u16 offset;
	if((addr == 0xffff) || (recd_h.cnt == 0) || (addr > recd_h.cnt))  //�޼�¼   ����ʵ������
	{
		return -1;
	}

	page_addr = 2048 + (addr / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));  //ҳ��ַ
	offset = (addr % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;  //ҳ��ƫ�Ƶ�ַ
	if(sflash_read(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)  //��FLASH
	{
		return 0;
	}
	return -2;
}


/*
 * @brief  д��¼
 * @param  addr  0~16383
 * @param  recd
 * @retval 0 �ɹ�   <0 ʧ��
 */
int recd_write(u16 addr, record_def* recd)
{
	u16 page_addr;
	u16 offset;

	if(addr >= RECD_MAX_CNT)  //��������¼����
	{
		return -1;
	}

	page_addr = 2048 + (addr / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));  //ҳ��ַ
	offset = (addr % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;  //ҳ��ƫ�Ƶ�ַ
	if(sflash_write(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)  //дFLASH
	{

	}
	return 0;
}

/*
 * @brief  ��ȡ��һ����¼�洢��ID
 * @retval addr  0~16383
 */
u16 recd_get_next_id()
{
	recd_h.newest_addr++;
	recd_h.newest_addr %= RECD_MAX_CNT;  //��ID == RECD_MAX_CNT ʱ���ص�0
	if(recd_h.cnt != RECD_MAX_CNT)
	{
		recd_h.cnt++;
	}
	recd_header_write();  //����ͷ��Ϣ
	return recd_h.newest_addr;  //����ID
}

/*
 * @brief  ����һ����¼  ��Ҫд��һ���¼�¼ʱ����
 * @retval addr  0~16383
 */
u16 recd_add()
{
	record_def temp;
	u16 id = recd_get_next_id();
	memset(&temp, 0, sizeof(temp));  //������������
	recd_write(id, &temp);
	return id;
}

/*
 * @brief  �������¼�¼ID
 */
u16 recd_get_newest_id()
{
	return recd_h.newest_addr;
}


/*
 * @brief  ��ȡ��¼
 * @param  id  0~16383   0�������µļ�¼   16383������ɵļ�¼
 * @param  recd
 * @retval 0 �ɹ�   <0 ʧ��
 */
int recd_read_new(u16 id, record_def* recd)
{
	u16 addr;
	if(id >= recd_h.cnt)
	{
		return -1;
	}

	//����id����addr
	if(recd_h.newest_addr >= id)
	{
		addr = recd_h.newest_addr - id;
	}
	else
	{
		addr = (RECD_MAX_CNT - 1) - (id - (recd_h.newest_addr + 1));
	}

	return recd_read(addr, recd);  //����¼
}


/*
 * @brief  �����ܼ�¼��
 */
u16 recd_get_cnt()
{
	return recd_h.cnt;
}



///*
// * @brief  д
// * @param  recd
// * @retval 0 �ɹ�   <0 ʧ��
// */
//int recd_write0(record_def* recd)
//{
//	u16 page_addr;
//	u16 offset;
//	u16 next_offset;
//
//	//next_offset 0 ~ RECD_MAX_CNT-1
//	next_offset = (recd_h.newest_id) % RECD_MAX_CNT;   //������RECD_MAX_CNT������ʼλ��
//
//	page_addr = 2048 + (next_offset / (FLASH_PAGE_SIZE / RECD_DATA_SIZE));
//	offset = (next_offset % (FLASH_PAGE_SIZE / RECD_DATA_SIZE)) * RECD_DATA_SIZE;
//	if(sflash_write(page_addr, offset, (u8*)recd, sizeof(record_def)) == 0)
//	{
//		if(recd_h.cnt != RECD_MAX_CNT)
//		{
//			recd_h.cnt++;  //����������1
//		}
//
//		recd_h.newest_id = next_offset + 1;
//		current_id = next_offset + 1;
//
//		if(recd_h.cnt == RECD_MAX_CNT)  //�ռ�д��
//		{
//			if(recd_h.newest_id == RECD_MAX_CNT)
//			{
//				recd_h.oldest_id = 1;
//			}
//			else
//			{
//				recd_h.oldest_id = (recd_h.oldest_id + 1) % (RECD_MAX_CNT + 1);  //������RECD_MAX_CNT������ʼλ��
//			}
//		}
//		else
//		{
//			recd_h.oldest_id = 1;
//		}
//		recd_header_write();  //�����¼ͷ��Ϣ
//		return 0;
//	}
//	return -1;
//}


/*
 * @brief  ��ȡ���¼�¼
 * @param  recd
 * @retval 0 �ɹ�   <0 ʧ��
 */
//int recd_read_newest(record_def* recd)
//{
//	if((recd_h.cnt == 0))  //��¼Ϊ��
//	{
//		return -1;
//	}
//
//	if((recd_h.cnt != RECD_MAX_CNT) && (current_id == 0))  //�Ѿ�����ȫ��
//	{
//		return -1;
//	}
//	else if((recd_h.cnt == RECD_MAX_CNT) && (current_id == recd_h.oldest_id))  //bug:����д��RECD_MAX_CNT������ ���������ɵ�һ������
//	{
//		return -1;
//	}
//
//	if(recd_read(current_id, recd) == 0)
//	{
//		current_id--;
//
//		if((current_id == 0) && (recd_h.cnt == RECD_MAX_CNT))
//		{
//			current_id = RECD_MAX_CNT;
//		}
//		return 0;
//	}
//	return -1;
//}

//
//
//
//
//
///*
// * @brief  �ƶ���ָ�뵽ָ����¼ID��
// */
//void recd_lseek(u16 id)
//{
//	if(id != 0)
//	{
//		current_id = id;
//	}
//}
//
//
///*
// * @brief  �ƶ���ָ��  ����µķ����ƶ�
// */
//void recd_read_forward(u16 offset)
//{
//	current_id += offset + 1;
//	if(current_id > recd_h.newest_addr)
//	{
//		current_id = recd_h.newest_addr;
//	}
//}









