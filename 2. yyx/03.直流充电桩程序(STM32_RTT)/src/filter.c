#include "stm32f10x.h"
#include "filter.h"



/*
 * @brief  ����  С����
 * @param  buf: ����
 * @param  len: ���ݳ���
 */
static void sort(u16 *buf, u8 len)
{
	u8 i,j;
	u16 temp;

	for (j = 0; j < len - 1; j++) //����  С����
	{
		for (i = j + 1; i < len; i++)
		{
			if (buf[j] > buf[i])
			{
				temp = buf[i];
				buf[i] = buf[j];
				buf[j] = temp;
			}
		}
	}
}


/*
 * @brief  ��λֵƽ���˲�
 * @param  buf: ����
 * @param  len: ���ݳ���
 * @param  offset: ���������ǰ��Ҫȥ����Ԫ�ظ���  offset < len / 2
 */
static u16 filter_median_value(u16 *buf, u8 len, u8 offset)
{
	u8 i;
	u32 temp = 0;
	sort(buf, len);  //������

	for(i = offset; i < len - offset; i++)  //���м�ֵ�ۼ�
	{
		temp += buf[i];
	}
	return (u16)(temp / (len - offset * 2));  //ƽ��
}


/*
 * @brief  �����˲�
 * @param  buf: ����
 * @param  len: ���ݳ���
 * @param  new: �²����ֵ
 * @param  offset: ���������ǰ��Ҫȥ����Ԫ�ظ���  offset < len / 2
 * @retval 0:�ɹ�   <0 ʧ��
 */
static u16 filter_glide(u16 *buf, u8 len, u16 new)
{
	u8 i;
	u32 temp = 0;
	for(i = 0; i < len - 1; i++)  //ȥ�����
	{
		buf[i] = buf[i + 1];
	}
	buf[len - 1] = new;  //����ֵ

	for(i = 0; i < len; i++)  //���м�ֵ�ۼ�
	{
		temp += buf[i];
	}
	return (u16)(temp / len);  //ƽ��
}


/*
 * @brief  ����ƽ���˲�
 * @param  buf: ����
 * @param  len: ���ݳ���
 * @param  new: �²����ֵ
 * @param  offset: ���������ǰ��Ҫȥ����Ԫ�ظ���  offset < len / 2
 * @retval 0:�ɹ�   <0 ʧ��
 */
static u16 filter_glide_ave(u16 *buf, u8 len, u16 new, u8 offset)
{

}

void filter_calc(filter_def *f)
{
//	u8 count,i,j;
//	u32 temp;
//
//	for(j = 0; j < f->flt1_buf_len - 1; j++) //����  С����
//	{
//		for (i = j + 1; i < f->flt1_buf_len; i++)
//		{
//			if (f->flt1_buf[j] > f->flt1_buf[i])
//			{
//				temp = f->flt1_buf[i];
//				f->flt1_buf[i] = f->flt1_buf[j];
//				f->flt1_buf[j] = temp;
//			}
//		}
//	}
//
//	temp = 0;
//
//	for(count = f->flt1_cnt; count < f->flt1_buf_len - f->flt1_cnt; count++)  //��ֵ�˲�  ȥ������ǰ���f->flt1_cnt������  �м�ֵ���ʾƽ��
//	{
//		temp += f->flt1_buf[count];
//	}
//
//	temp = (u16)(temp / (f->flt1_buf_len - f->flt1_cnt * 2));
//
//	f->value1 = (u16)temp;  //�����һ�ν��
//
//	if(f->flt2_buf != ((void*)0))  //����Ҫ�����˲�
//	{
//		return;
//	}
//
//	for(j = 0; j < f->flt2_buf_len - 1; j++) //ȥ����ɵ�����
//	{
//		f->flt2_buf[j] = f->flt2_buf[j + 1];
//	}
//
//	f->flt2_buf[f->flt2_buf_len - 1] = f->value1;  //�����������µ�ֵ
//
//	temp = 0;
//
//	for(j = 0; j < f->flt2_buf_len; j++) //ʾƽ��
//	{
//		temp += f->flt2_buf[j];
//	}
//
//	temp /= f->flt2_buf_len;
//
//	f->value2 = (u16)temp;
}









