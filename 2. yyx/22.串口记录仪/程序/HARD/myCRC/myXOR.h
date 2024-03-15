/**
 * 2020��5��17��07:09:53
 *     ��λ������Ƭ������Э�飺
*      1�ֽڣ���6λ���δ�С����8λ����֡��1����0����֡����7λ��һ�����źţ���0 �����źţ� + DATA + 1�ֽ�XORУ��  
*      ��λ������DATA��0xaa,0x55 
*      ��λ��Ӧ��DATA: ��Ч���ݵ���� 1�ֽ� + ������
*      ��Ч���ݵ����������������λ�����ָ��Լ����͵Ĵ�����ƥ�䣬�Զ������ط���
*  
*/ 

#ifndef  __MYXOR_H
#define  __MYXOR_H
#include "uhead.h"

/*����*/
#include "usbd_cdc_if.h"

#define		FILESIZE	64				// �ļ������С
#define		MYXOR_SEND(Buf,Len)			CDC_Transmit_FS(Buf, Len)

#define     ENDFILE     0x80            // �����ļ�
#define     CONNECT     0x40            // �����ź�  
#define     BYTE2SEND   62              // һ�η��͵��ֽ�
#define     IS_OK       0            	// ��λ������ɹ�
#define     IS_ERRO     0x01            // ��λ������ʧ��

typedef enum{
	XOR_OK = 0,		// ����
	ERRO_LEN,		// ���ճ��ȳ�����Χ
	ERROR_XOR,		// ���У�����
	
}enumError_XOR;

typedef struct{
	u8 filebuf[FILESIZE];		// ��ȡ���ݻ���
	u8 rxlen;					// ʵ�����ݳ���
	u32 counter;				// �������յ�֡��
	
}fileRXStr;


extern fileRXStr filerx;

/* ���� */
enumError_XOR 	ResolveRxData(fileRXStr* filerx,u8* rxbuf,u32 len);	// ��������
void 			RespectUp(fileRXStr* filerx,u8 isok);// �ظ���λ��

#define			myXOR_Resolve(rxbuf,len)		ResolveRxData(&filerx,rxbuf, len)
#define			myXOR_Respect(isok)				RespectUp(&filerx,isok)


void SynchronizationTime(u8* timebuf,u8 buflen);		// ����ʱ�� 

#endif // !1

