#include "RTL.h"
#include "stm32f10x.h"
#include "dbg.h"
#include "string.h"
#include "sys_cfg.h"

#if 1
#define SF_MSG dbg_msg
#define SF_ERROR dbg_error
#else
#define SF_MSG (void)
#define SF_ERROR (void)
#endif


#define AT45BD_PAGE_SIZE 512

OS_SEM spi1_rx_sem, spi1_tx_sem;

u8 sf_cmd[8];


 
#define CS_H()  GPIOA->BSRR = GPIO_Pin_4
#define CS_L()  GPIOA->BRR = GPIO_Pin_4
#define WP_H()  GPIOA->BSRR = GPIO_Pin_3
#define WP_L()  GPIOA->BRR = GPIO_Pin_3

#define DMA1_CH2_EN()  DMA1_Channel2->CCR |= DMA_CCR1_EN  //��DMA
#define DMA1_CH3_EN()  DMA1_Channel3->CCR |= DMA_CCR1_EN;	//��DMA
#define DMA1_CH2_DIS() DMA1_Channel2->CCR &= (u16)(~DMA_CCR1_EN)  //��DMA
#define DMA1_CH3_DIS() DMA1_Channel3->CCR &= (u16)(~DMA_CCR1_EN);  //��DMA

/*
 * @brief  DMA1_CH2 SPI1 RX��������жϴ������
 */
void DMA1_Channel2_IRQHandler()
{
	DMA1_CH2_DIS();
	DMA1->IFCR = DMA1_IT_TC2;  //���־
	isr_sem_send(&spi1_rx_sem);
}


/*
 * @brief  DMA1_CH3 SPI1 TX��������жϴ������
 */
void DMA1_Channel3_IRQHandler()
{
	DMA1_CH3_DIS();
	DMA1->IFCR = DMA1_IT_TC3;  //���־
	isr_sem_send(&spi1_tx_sem);
}

/*
 * @brief  SPI1 ����
 * @param  addr: ���ݵ�ַ
 * @param  lenth: ���ݳ���
 * @retval 0:�ɹ�    <0  ʧ��
 */
static int spi1_send(u8 *buf, u16 lenth)
{
	DMA1_CH3_DIS();
	DMA1_Channel3->CMAR = (u32)buf;
	DMA1_Channel3->CNDTR = lenth;
	DMA1_CH3_EN();  //����

	if(os_sem_wait(&spi1_tx_sem, 200) != OS_R_TMO)
	{
		return 0;
	}
	else
	{
		DMA1_CH3_DIS();
	}
	SF_MSG("send err\n");
	return -1;
}


/*
 * @brief  SPI1 �л���ֻ����ģʽ
 */
static void spi1_mode_rxonly_en()
{
	//SPI_Cmd(SPI1, DISABLE);
	SPI1->CR1 |= SPI_Direction_2Lines_RxOnly;
	//SPI_Cmd(SPI1, ENABLE);
}

/*
 * @brief  SPI1 �л����շ�ģʽ
 */
static void spi1_mode_rxonly_dis()
{
	SPI_Cmd(SPI1, DISABLE);  //������disable
	SPI1->CR1 &= ~SPI_Direction_2Lines_RxOnly;
	SPI_Cmd(SPI1, ENABLE);
}

/*
 * @brief  SPI1 ����
 * @param  addr: ���ݵ�ַ
 * @param  lenth: ���ݳ���
 * @retval 0:�ɹ�    <0  ʧ��
 */
static int spi1_recv(u8 *buf, u16 lenth)
{
	spi1_mode_rxonly_en();
	SPI1->DR;  //��������ݽ���
	DMA1_CH2_DIS();
	DMA1_Channel2->CMAR = (u32)buf;
	DMA1_Channel2->CNDTR = lenth;
	DMA1_CH2_EN();  //��ʼ����

	if(os_sem_wait(&spi1_rx_sem, 200) != OS_R_TMO)
	{
		spi1_mode_rxonly_dis();
		return 0;
	}
	else
	{
		DMA1_CH2_DIS();
	}
	spi1_mode_rxonly_dis();
	SF_MSG("recv err\n");
	return -1;
}


/*
 * @brief  SPI1 ����һ���ֽ�
 */
static u8 spi1_read_byte(void)
{
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1, 0x00);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SPI1);
}

/*
 * @brief  SPI1 ����һ���ֽ�
 */
static void spi1_send_byte(u8 byte)
{
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1, byte);

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
}



/*
 * @brief  ��ȡID
 * @param  id: ���ݵ�ַ
 * @retval 0:�ɹ�    <0  ʧ��
 */
int sflash_read_id(u8 *id)
{
	CS_L();
#if 0
	sf_cmd[0] = 0x9f;
	spi1_send(sf_cmd, 1);
#else
	spi1_send_byte(0x9f);  //ֻ��һ���ֽڲ���DMA
#endif
	os_dly_wait(2);
	spi1_recv(id, 4);
	CS_H();
	return 0;
}

/*
 * @brief  ��ȡ״̬
 * @retval ״̬
 */
u8 sflash_read_status()
{
	u8 temp;
	CS_L();
#if 0
	sf_cmd[0] = 0xd7;
	spi1_send(sf_cmd, 1);
	//os_dly_wait(2);
	spi1_recv(&temp, 1);
#else
	spi1_send_byte(0xd7);
	temp = spi1_read_byte();
#endif
	CS_H();
	return temp;
}


int sflash_wait_busy()
{
	u8 cnt = 20;
	do
	{
		if(sflash_read_status() & 0x80)
		{
			return 0;
		}
		else
		{
			os_dly_wait(10);
		}
		cnt--;
	}while(cnt);
	return -1;
}

/*
 * @brief  ����ҳ
 * @retval
 */
int sflash_earse_page(u32 page_addr)
{
	if(!sflash_wait_busy())
	{
		CS_L();
		sf_cmd[0] = 0x81;
		sf_cmd[1] = (u8)(page_addr >> 6);
		sf_cmd[2] = (u8)(page_addr << 2);
		sf_cmd[3] = 0xA5;
		spi1_send(sf_cmd, 4);
		CS_H();
		return 0;
	}
	return -1;
}

/*
 * @brief  ��ȡҳ
 * @retval 0:�ɹ�   <0 ʧ��
 */
int sflash_read_page(u32 page_addr, u8 *buf)
{
	if(!sflash_wait_busy())
	{
		CS_L();
		sf_cmd[0] = 0xD2;
		sf_cmd[1] = (u8)(page_addr >> 6);
		sf_cmd[2] = (u8)(page_addr << 2);
		sf_cmd[3] = 0;
		sf_cmd[4] = 0;
		sf_cmd[5] = 0;
		sf_cmd[6] = 0;
		sf_cmd[7] = 0;
		spi1_send(sf_cmd, 8);
		spi1_recv(buf, 512);
		CS_H();
		return 0;
	}
	return -1;
}


/*
 * @brief  дҳ
 * @retval 0:�ɹ�   <0 ʧ��
 */
int sflash_write_page(u32 page_addr, u8 *buf)
{
	if(!sflash_wait_busy())
	{
		CS_L();
		WP_H();
		sf_cmd[0] = 0x82;  //buf1
		sf_cmd[1] = (u8)(page_addr >> 6);
		sf_cmd[2] = (u8)(page_addr << 2);
		sf_cmd[3] = 0;
		spi1_send(sf_cmd, 4);
		spi1_send(buf, AT45BD_PAGE_SIZE);  //��������
		CS_H();
		WP_L();
		return 0;
	}
	return -1;
}



/*
 * @brief  ��ȡ
 * @param  page_addr: ҳ��ַ
 * @param  offset: ҳ��ƫ�Ƶ�ַ
 * @param  buf: ����
 * @param  len: ���ݳ���
 * @retval 0:�ɹ�   <0 ʧ��
 */
int sflash_read(u32 page_addr, u16 offset, u8 *buf, u16 len)
{
	if(!sflash_wait_busy())
	{
		CS_L();
		sf_cmd[0] = 0xD2;
		sf_cmd[1] = (u8)(page_addr >> 6);
		sf_cmd[2] = (u8)((page_addr << 2) | (offset >> 8));
		sf_cmd[3] = (u8)offset;
		sf_cmd[4] = 0;
		sf_cmd[5] = 0;
		sf_cmd[6] = 0;
		sf_cmd[7] = 0;
		spi1_send(sf_cmd, 8);
		spi1_recv(buf, len);
		CS_H();
		return 0;
	}
	return -1;
}


/*
 * @brief  д
 * @retval 0:�ɹ�   <0 ʧ��
 */
int sflash_write(u32 page_addr, u16 offset, u8 *buf, u16 len)
{
	if(!sflash_wait_busy())
	{
		CS_L();
		sf_cmd[0] = 0x55;  //�Ƚ�һҳ���ݶ���BUF2
		sf_cmd[1] = (u8)(page_addr >> 6);
		sf_cmd[2] = (u8)(page_addr << 2);
		sf_cmd[3] = 0;
		spi1_send(sf_cmd, 4);
		CS_H();
		if(!sflash_wait_busy())
		{
			CS_L();
			sf_cmd[0] = 0x85;  //������д��BUF2��  ֮��оƬ�Զ�д��ҳ��
			sf_cmd[1] = (u8)(page_addr >> 6);
			sf_cmd[2] = (u8)((page_addr << 2) | (offset >> 8));
			sf_cmd[3] = (u8)offset;
			spi1_send(sf_cmd, 4);
			spi1_send(buf, len);
			CS_H();
		}

		return 0;
	}
	return -1;
}

void sflash_init()
{
	CS_H();  //ȡ��Ƭѡ
	WP_H();  //д����

	os_sem_init(&spi1_rx_sem, 0);
	os_sem_init(&spi1_tx_sem, 0);
}




















