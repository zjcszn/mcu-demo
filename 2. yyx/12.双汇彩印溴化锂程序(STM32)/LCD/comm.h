#ifndef _COMM_H
#define _COMM_H

#ifndef _COMM_C
#define EXT_COMM_C extern
#else
#define EXT_COMM_C
#endif

/*
*******************************************************************************
�� �� ����	UINT8 CheckXor(UINT8 *source,UINT8 length)
��    �ܣ�	���У��
��ڲ�����	UINT16 *source	:Դ����λ��
			UINT8 length	:���ݳ���
���ڲ�����
��д���ڣ�	2005.11.20
��    �ߣ�	����
*******************************************************************************
*/
EXT_COMM_C UINT8 CheckXor(UINT8 *source,UINT8 length);

/*
**************************************************************************
* �� �� ����UINT8 CheckSum(UINT8 *source,UINT16 len)
* ��    �ܣ�����У���
* ˵    ����

* ��ڲ���������Դ(source),���ݳ���(len)
* ���ڲ���������У���
* ��д���ڣ�2006.2
* ��    �ߣ� 
**************************************************************************
*/
EXT_COMM_C UINT8 CheckSum(UINT8 *source,UINT16 len);

/***********************************************************
* ��������static void pop_one_byte(UINT8 scr,UINT8 length)
* ���ܣ�����һ���ֽ�
***********************************************************/


/*
***********************************************************
* �� �� ����UINT16 CheckCrc(UINT8 *source,UINT8 length)
* ��    �ܣ����CRCУ��
* ˵    ����

* ��ڲ�����UINT8 *source	Դ����λ��
			UINT8 length	���ݳ���
* ���ڲ���������У��ֵ
* ��д���ڣ�2006.3
* ��    �ߣ�SUNYUAN
***********************************************************
*/

 
 
EXT_COMM_C void PrintfFormat(INT8 *p_des,INT32 value, UINT8 length);

EXT_COMM_C UINT8 GetListCount(INT8 **scr_list);
#endif