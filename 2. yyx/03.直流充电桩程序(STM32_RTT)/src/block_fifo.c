#include "block_fifo.h"
#include <string.h>


/*
 * block fifo˵������ȡ����һ������Ϊ��λ(���Ȳ���)�������ֽڣ�һ�����ݵĵ�ַ�������ģ���������MDA��ÿһ�����ݿ��ǰ�����ֽڼ�¼�����ݵĳ��Ⱥ����������
 */


#define FIFO_FULL ((p->read == p->write) && (p->read_cnt != p->write_cnt))
#define FIFO_EMPT ((p->read == p->write) && (p->read_cnt == p->write_cnt))


static void write_to_fifo(b_fifo_typ *p, unsigned char *data, unsigned short len)
{
	memcpy(p->write->data, data, len);
	p->write->len = len;
	p->write_cnt++;
	p->write = (b_fifo_data_typ*)((unsigned int)p->write + len + sizeof(p->write->len));
}


/*
 * @brief   дһ�����ݿ鵽b_fifo
 * @param   b_fifo_buf �ڴ��ַ
 * @param   data Ҫд������
 * @param   len  ���ݳ���
 * @retval  1 �ɹ�  0 ʧ��
 */
int b_fifo_write(void *b_fifo_buf, unsigned char *data, unsigned short len)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;
	unsigned int temp;


	if(FIFO_FULL)  //fifo����
	{
		return 0;
	}


	if(p->write >= p->read)  //дָ���ڶ�ָ���
	{
		temp = (unsigned int)p->buf_end - (unsigned int)p->write;
		if(temp >= len + sizeof(p->write->len))  //�������㹻�ռ�
		{
			write_to_fifo(p, data, len);  //д��FIFO

			if((unsigned int)p->write >= (unsigned int)p->buf_end)
			{
				p->write = (b_fifo_data_typ*)p->buf;  //ָ�뷵�ص���ʼ
			}
			return 1;
		}
		else  //����û���㹻�ռ�  ��ǰ����
		{
			temp = (unsigned int)p->read - (unsigned int)p->buf;
			if(temp >= len + sizeof(p->write->len))  //���㹻�ռ�
			{
				p->write_end = p->write;  //��ǿ���p->buf�����һ��block  ��p->write_end == p->read ʱp->read���ص�p->buf
				p->write = (b_fifo_data_typ*)p->buf;
				write_to_fifo(p, data, len);
				return 1;
			}
		}
	}
	else //дָ���ڶ�ָ��ǰ
	{
		temp = (unsigned int)p->read - (unsigned int)p->write;

		if(temp >= len + sizeof(p->write->len))  //���㹻�ռ�
		{
			write_to_fifo(p, data, len);
			return 1;
		}
	}

	return 0;
}



/*
 * @brief   ��b_fifo��һ�����ݿ�
 * @param   b_fifo_buf �ڴ��ַ
 * @param   data ��ȡ���ݵĴ洢��ַ
 * @param   len  ���ݳ���
 * @retval  0 ʧ��  ����0�ɹ�(���ݳ���)
 */
unsigned short b_fifo_read(void *b_fifo_buf, unsigned char *data)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;
	unsigned short temp;
	if(!FIFO_EMPT)
	{
		memcpy(data, p->read, p->read->len);
		temp = p->read->len;
		p->read_cnt++;
		p->read = (b_fifo_data_typ*)((unsigned int)p->read + p->read->len + sizeof(p->read->len));  //ָ�����
		if(((unsigned int)p->read >= (unsigned int)p->buf_end))
		{
			p->read = (b_fifo_data_typ*)p->buf;  //ָ�뷵��
		}

		return temp;
	}
	return 0;
}


/*
 * @brief   ��b_fifo��һ�����ݿ�ĵ�ַ�����ݴ������Ӧ����b_fifo_del����ɾ��Щ���ݣ����ͷſռ�
 * @param   b_fifo_buf �ڴ��ַ
 * @retval  0 ʧ��  ����0�ɹ�
 */
b_fifo_data_typ* b_fifo_get(void *b_fifo_buf)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	if(!FIFO_EMPT)
	{
		if(p->read == p->write_end)
		{
			p->read = (b_fifo_data_typ*)p->buf;  //ָ�뷵��
			p->write_end = 0;
		}

		return p->read;
	}
	return 0;
}

/*
 * @brief    ɾ����һ����Ч���ݿ�    ��b_fifo_get�������ʹ��
 * @param    b_fifo_buf �ڴ��ַ
 */
void b_fifo_del(void *b_fifo_buf)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	if(!FIFO_EMPT)
	{
		//memset(p->read->data, 0, p->read->len);

		p->read = (b_fifo_data_typ*)((unsigned int)p->read + p->read->len + sizeof(p->read->len));  //ָ�����
		if(((unsigned int)p->read >= (unsigned int)p->buf_end))  //ָ�뷵��
		{
			p->read = (b_fifo_data_typ*)p->buf;
		}
		p->read_cnt++;
	}
}


/*
 * @brief   ��ʼ��block fifo
 * @param   b_fifo_buf �ڴ��ַ
 * @param   b_fifo_buf_len ����
 * @param   block_cnt  block����
 */
void b_fifo_init(void *b_fifo_buf, unsigned short b_fifo_buf_len)
{
	b_fifo_typ *p = (b_fifo_typ*)b_fifo_buf;

	//memset(b_fifo_buf, 0, b_fifo_buf_len);

	p->read = (b_fifo_data_typ*)p->buf;
	p->write = (b_fifo_data_typ*)p->buf;
	p->read_cnt = p->write_cnt = 0;
	p->write_end = 0;
	p->buf_end = (unsigned char*)b_fifo_buf + b_fifo_buf_len;
}

