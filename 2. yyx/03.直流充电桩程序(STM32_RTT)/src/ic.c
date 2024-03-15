#include "stm32f10x.h"
#include "RTL.h"
#include "dbg.h"
#include <string.h>
#include "sys_cfg.h"
#include "rtc.h"

#if 1
#define IC_MSG dbg_msg
#define IC_ERROR dbg_error
#else
#define IC_MSG (void)
#define IC_ERROR (void)
#endif

#define IC_RX_BUF_LEN 30
u8 ic_rx_buf[IC_RX_BUF_LEN];  //�������ݵ�ַ
u8 ic_rx_len, ic_tx_len;
u8 *ic_tx_buf;

OS_SEM ic_rx_sem, ic_tx_sem;

static void uart5_rx_dis(void);

void UART5_IRQHandler()
{
	u32 temp;

	if(USART_GetITStatus(UART5, USART_IT_RXNE))
	{
		if(ic_rx_len < IC_RX_BUF_LEN)
		{
			ic_rx_buf[ic_rx_len] = UART5->DR;
		}
		ic_rx_len++;
	}
	else if(USART_GetITStatus(UART5, USART_IT_IDLE))
	{
		uart5_rx_dis();
		USART_ITConfig(UART5,USART_IT_RXNE,DISABLE);
		isr_sem_send(&ic_rx_sem);
	}
	else if(USART_GetITStatus(UART5, USART_IT_TXE))
	{
		if(ic_tx_len > 0)
		{
			UART5->DR = *ic_tx_buf++;
			ic_tx_len--;
		}
		else
		{
			USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
			USART_ITConfig(UART5,USART_IT_RXNE,ENABLE);
			//isr_sem_send(&ic_tx_sem);
		}
	}

	//���־
    temp = UART5->SR;
    temp = UART5->DR;
}

/*
 * @brief  ʹ��uart5����
 */
static void uart5_rx_en()
{
	UART5->CR1 |= USART_Mode_Rx;  //ʹ�ܽ���
}

/*
 * @brief  �ر�uart5����
 */
static void uart5_rx_dis()
{
	UART5->CR1 &= ~USART_Mode_Rx;  //�رս���
}

/**
  * @brief  UART5����
  * @param  buf: ���ݵ�ַ
  * @param  lenth: ���ݳ���
  * @retval 1 ���ͳɹ�   0 ʧ��
  */
static int uart5_send(u8* buf, u8 lenth)
{
	ic_rx_len = 0;
	ic_tx_buf = buf + 1;
	ic_tx_len = lenth - 1;

	uart5_rx_en();  //ʹ�ܽ���
	UART5->DR = *buf;  //����һ���ֽ�
	USART_ITConfig(UART5,USART_IT_TXE,ENABLE);
	//os_sem_wait(&ic_tx_sem, 0xffff);
	return 1;
}

/*
 * @brief  UART5���ճ�ʼ��
 */
void ic_rx_init()
{
	ic_rx_len = 0;
}











void ic_usart_send(u8* buf, u8 lenth)
{
	uart5_send(buf, lenth);
}


/*
 * @brief  ����У��
 * @param  buf ����
 * @param  len �����ֽ���
 * @retval У����
 */
u8 verify(u8 *buf, u8 len)
{
    u8 i, BBC = 0;
    for (i = 0; i < len; i++)
    {
        BBC ^= buf[i];
    }
    return (u8)BBC;
}


/*
 * @brief  �ȴ���������������
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_wait_frame()
{
	if(os_sem_wait(&ic_rx_sem, 1000) != OS_R_TMO)
	{
		if((ic_rx_buf[0] == 0x02) && (ic_rx_buf[ic_rx_len - 1] == 0x03) && (verify(ic_rx_buf, ic_rx_len - 1) == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  ���ˢ��������
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_link_isok()
{
	u8 temp[10];
	u8 cnt = 3;

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x05;
	temp[3] = 0xC1;  //����
	temp[4] = 0x40;
	temp[5] = 0x00;  //�ȴ�ʱ��
	temp[6] = 0x00;
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END

	do
	{
		ic_usart_send(temp, 10);

		if(ic_wait_frame())
		{
			//�յ�����
			return 1;
		}
		IC_MSG("--\n");
		cnt--;
	}while(cnt);
	return 0;
}


/*
 * @brief  ����IC��
 * @param  id ������IC����
 * @retval 0 �ɹ�   >0ʧ��
 */
int ic_find(u32 *id)
{
	u8 temp[10];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x05;
	temp[3] = 0xC1;  //����
	temp[4] = 0x40;
	temp[5] = 0x00;  //�ȴ�ʱ��
	temp[6] = 0x00;
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END

	ic_usart_send(temp, 10);

	if(ic_wait_frame())
	{
		//�յ�����  У��ɹ�
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))  //�ҵ�IC��
		{
			*id = *(u32*)&ic_rx_buf[7];  //ID��
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  ��֤��Կ
 * @param  ab  0x00��ԿA  0x04��ԿB
 * @param  sector  ������
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_auth_keyAB(u8 ab, u8 sector)
{
	u8 temp[10];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x05;
	temp[3] = 0xC1;  //����
	temp[4] = 0x41;
	temp[5] = ab;    //��Կģʽ
	temp[6] = sector; //������
	temp[7] = 0x00;
	temp[8] = verify(temp, 8);
	temp[9] = 0x03;  //END


	ic_usart_send(temp, 10);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  ��֤��Կ
 * @param  sector  ������
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_auth_key(u8 sector)
{
	return ic_auth_keyAB(0x04, sector);
}


const u8 key_a[6] = {0x50, 0x61, 0x77, 0x61, 0x72, 0x41};
const u8 key_b[6] = {0x50, 0x61, 0x77, 0x61, 0x72, 0x42};

/*
 * @brief  ������Կ��������
 * @param  ab  0x00��ԿA  0x04��ԿB
 * @param  sector  ������
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_download_key(u8 ab, u8* key, u8 sector)
{
	u8 temp[15];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x0A;
	temp[3] = 0xC0;  //����
	temp[4] = 0x16;
	temp[5] = ab;    //��Կģʽ
	temp[6] = sector; //������
	memcpy(&temp[7], key, 6);
	temp[13] = verify(temp, 13);
	temp[14] = 0x03;  //END


	ic_usart_send(temp, 15);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}





/*
 * @brief  ��һ������  16�ֽ�
 * @param  id IC����
 * @param  block  ���
 * @param  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_read_block(u8 block, u8 *buf)
{
	u8 temp[8];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x03;
	temp[3] = 0xC1;  //����
	temp[4] = 0x43;
	temp[5] = block; //������
	temp[6] = verify(temp, 6);
	temp[7] = 0x03;  //END

	ic_usart_send(temp, 8);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			memcpy(buf, &ic_rx_buf[5], 16);
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  дһ������  16�ֽ�
 * @param  id IC����
 * @param  block  ���
 * @param  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_write_block(u8 block, u8 *buf)
{
	u8 temp[24];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x13;
	temp[3] = 0xC1;  //����
	temp[4] = 0x44;
	temp[5] = block; //������
	memcpy(&temp[6], buf, 16);
	temp[22] = verify(temp, 22);
	temp[23] = 0x03;  //END


	ic_usart_send(temp, 24);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  ��ʼ��ֵ
 * @param  block  ���
 * @param  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_init_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x07;
	temp[3] = 0xC1;  //����
	temp[4] = 0x45;
	temp[5] = block; //������
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END

	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  ��ֵ
 * @param  block  ���
 * @param  value  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_read_value(u8 block, u32* value)
{
	u8 temp[8];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x03;
	temp[3] = 0xC1;  //����
	temp[4] = 0x46;
	temp[5] = block; //������
	temp[6] = verify(temp, 6);
	temp[7] = 0x03;  //END


	ic_usart_send(temp, 8);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			*value = ic_rx_buf[5] << 24;
			*value |= ic_rx_buf[6] << 16;
			*value |= ic_rx_buf[7] << 8;
			*value |= ic_rx_buf[8];
			return 1;
		}
	}
	return 0;
}

/*
 * @brief  ��ֵ
 * @param  block  ���
 * @param  value  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_add_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x07;
	temp[3] = 0xC1;  //����
	temp[4] = 0x47;
	temp[5] = block; //������
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END


	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  ��ֵ
 * @param  block  ���
 * @param  value  ����
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_sub_value(u8 block, u32 value)
{
	u8 temp[12];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x07;
	temp[3] = 0xC1;  //����
	temp[4] = 0x48;
	temp[5] = block; //������
	temp[6] = (u8)(value >> 24);
	temp[7] = (u8)(value >> 16);
	temp[8] = (u8)(value >> 8);
	temp[9] = (u8)value;
	temp[10] = verify(temp, 10);
	temp[11] = 0x03;  //END


	ic_usart_send(temp, 12);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}


/*
 * @brief  ֹͣ��
 * @retval 1 �ɹ�   0 ʧ��
 */
int ic_halt()
{
	u8 temp[7];

	temp[0] = 0x02;  //BEGIN
	temp[1] = 0x00;  //����������
	temp[2] = 0x02;
	temp[3] = 0xC1;  //����
	temp[4] = 0x4A;
	temp[5] = verify(temp, 5);
	temp[6] = 0x03;  //END


	ic_usart_send(temp, 7);
	if(ic_wait_frame())
	{
		if((ic_rx_buf[3] == 0) && (ic_rx_buf[4] == 0))
		{
			return 1;
		}
	}
	return 0;
}





/*
 * @brief  ��ȡ���
 * @retval 0 �ɹ�  <0 ʧ��
 */
int ic_read_sum(u32 *sum)
{
	if(ic_auth_key(0))  //У����Կ
	{
		if(ic_read_value(1, sum))  //��ȡ���
		{
			return 0;
		}
	}
	return -1;
}


/*
 * @brief  �۷�
 * @param  sum  �۷�
 * @retval 0 �ɹ�  <0 ʧ��
 */
int ic_pay(u32 sum)
{
	if(ic_auth_key(0))  //У����Կ
	{
		if(ic_sub_value(1, sum))  //�۷�
		{
			return 0;
		}
	}
	return -1;
}

/*
 * @brief  �ж�IC���Ƿ�����
 * @retval 1 ������   0 δ����    <0ʧ��
 */
int ic_is_locked()
{
	if(ic_auth_key(1))  //У����Կ
	{
		u8 temp[16];
		if(ic_read_block(4, temp) && (temp[0] != 0))  //��4��һ�ֽڲ�Ϊ0��Ϊ����״̬
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return -1;
}


/*
 * @brief  ����IC��
 * @param  t  ��ǰʱ��
 * @retval 0 �ɹ�  <0 ʧ��
 */
int ic_lock_card(time_t *t)
{
	if(ic_auth_key(1))  //У����Կ
	{
		u8 temp[16];
		temp[0] = 0xff;  //�������
		temp[1] = t->year;
		temp[2] = t->year >> 8;
		temp[3] = t->mon;
		temp[4] = t->day;
		temp[5] = t->hour;
		temp[6] = t->min;
		temp[7] = t->sec;
		memset(&temp[8], 0, 8);
		if(ic_write_block(4, temp))
		{
			return 0;
		}
		else
		{
			return -2;
		}
	}
	return -1;
}


/*
 * @brief  ����IC��
 * @param  t  ��ǰʱ��
 * @retval 0 �ɹ�  <0 ʧ��
 */
int ic_unlock_card(time_t *t)
{
	if(ic_auth_key(1))  //У����Կ
	{
		u8 temp[16];
		temp[0] = 0x00;  //����������
		temp[1] = t->year;
		temp[2] = t->year >> 8;
		temp[3] = t->mon;
		temp[4] = t->day;
		temp[5] = t->hour;
		temp[6] = t->min;
		temp[7] = t->sec;
		memset(&temp[8], 0, 8);
		if(ic_write_block(4, temp))
		{
			return 0;
		}
		else
		{
			return -2;
		}
	}
	return -1;
}


/*
 * @brief  ��ʼ��
 */
void ic_init()
{
	//��ʼ���ź���
	os_sem_init(&ic_rx_sem, 0);
	os_sem_init(&ic_tx_sem, 1);
	ic_rx_init();

	if(sys_cfg.ic_down_pswd != 0xAAAAAAAA)
	{
		if(ic_download_key(0x00, (u8*)key_a, 0) &&
		   ic_download_key(0x00, (u8*)key_a, 1) &&
		   ic_download_key(0x04, (u8*)key_b, 0) &&
		   ic_download_key(0x04, (u8*)key_b, 1))
		{
			sys_cfg.ic_down_pswd = 0xAAAAAAAA;  //���������
			sys_cfg_save();
		}
	}
}


void task_ic()
{
	u8 cnt = 3;
	u32 value;
	//��ʼ���ź���
	os_sem_init(&ic_rx_sem, 0);
	os_sem_init(&ic_tx_sem, 1);

	ic_rx_init();


//	if(sys_cfg.ic_down_pswd == 0xffffffff)  //û�����ع���Կ
//	{
//		do
//		{
//			if(ic_download_key(0x00, 0))
//			{
//				sys_cfg.ic_down_pswd = 0;  //����Ѿ����ع�
//				sys_cfg_save();
//				break;  //�ɹ�
//			}
//			cnt--;
//		}while(cnt);
//
//		if(cnt == 0)
//		{
//			//ͨѶʧ��
//			IC_ERROR("ic download key error!\n");
//			os_dly_wait(1000);
//		}
//	}

	while(1)
	{
		os_dly_wait(0xffff);
	}
}
