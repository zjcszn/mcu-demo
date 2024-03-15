#ifndef __RADIO_H__
#define __RADIO_H__

#include "sys.h"
#include "sx1276.h"
#include "platform.h"

#define			LORA			1		//ѡ��LoRaģʽ

//RF process function return codes
typedef enum
{
    RF_IDLE,							//����״̬
    RF_BUSY,							//æ			
    RF_RX_DONE,							//�������
    RF_RX_TIMEOUT,						//���ճ�ʱ
    RF_TX_DONE,							//�������
    RF_TX_TIMEOUT,						//���ͳ�ʱ
    RF_LEN_ERROR,						//LEN����
    RF_CHANNEL_EMPTY,					//�ŵ���
    RF_CHANNEL_ACTIVITY_DETECTED,		//���ֻ�Ծ�ŵ�
}tRFProcessReturnCodes;


//����radio����ָ��ṹ��
typedef struct sRadioDriver
{
    void 	( *Init )( void );
    void 	( *Reset )( void );
    void 	( *StartRx )( void );
    void 	( *GetRxPacket )( void *buffer, u16 *size );
    void 	( *SetTxPacket )( const void *buffer, u16 size );
   	u32 	( *Process )( void );
	/*************************���ӵĺ���**************************/
	inline u8 		( *RFDio0State )	(void);
	int8_t  ( *RFGetPacketSnr)	( void );
	double 	( *RFGetPacketRssi)	( void );
	double 	( *RFReadRssi)		( void );
	void 	( *RFRxStateEnter )	(void);
    void 	( *RFRxDataRead )	(u8 *pbuf, u8 *size );
    u8 		( *RFTxData )		(u8 *pbuf, u8 size );
	void 	( *RFTxPower)		(u8  pwr   );
	void 	( *RFFreqSet)		(u32 freq  );
	void 	( *RFOpModeSet)		(u8  opMode);
	/**************************************************************/
}tRadioDriver;


//�����ṹ���ʼ��
tRadioDriver *RadioDriverInit( void );

#endif
