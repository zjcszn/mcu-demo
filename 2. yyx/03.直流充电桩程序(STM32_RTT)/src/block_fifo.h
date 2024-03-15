#ifndef BLOCK_FIFO_H_
#define BLOCK_FIFO_H_




typedef struct
{
	unsigned short len;      //���ݳ���
	unsigned char  data[1];  //����
}b_fifo_data_typ;


typedef struct
{
	b_fifo_data_typ *read;     //��ǰ��ָ��
	b_fifo_data_typ *write;    //��ǰдָ��
	b_fifo_data_typ *write_end;  //��дָ���ƶ����ӽ�buf_endʱ����û�пռ�Ҫ���»ص�bufʱ��ֵ�������ж�readָ���ʱ����
	unsigned char read_cnt;    //�Ѿ���ȡ�����ݿ�����
	unsigned char write_cnt;   //�Ѿ�д������ݿ�����
	unsigned char *buf_end;    //������������ַ
    unsigned char buf[1];      //��������ʼ��ַ
}b_fifo_typ;

int b_fifo_write(void *b_fifo_buf, unsigned char *data, unsigned short len);
unsigned short b_fifo_read(void *b_fifo_buf, unsigned char *data);
void b_fifo_init(void *b_fifo_buf, unsigned short b_fifo_buf_len);
b_fifo_data_typ* b_fifo_get(void *b_fifo_buf);
void b_fifo_del(void *b_fifo_buf);


#endif
