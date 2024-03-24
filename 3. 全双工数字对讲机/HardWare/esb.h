#ifndef __ESB_H
#define __ESB_H

#include "core.h"

#define PKT_NACK 0
#define PKT_ACK  1
#define PA_RX_Pin	23

#define m_box_size  64

/* 	data packet define 	*/
#pragma pack(push,1)

#pragma anon_unions
typedef struct 	//底层数据结构
{
	uint8_t size;
	union
	{
		uint8_t s1;
		struct 
		{
			uint8_t noack :1;
			uint8_t pid :2;
		};
	};
	uint8_t up_data[m_box_size];
	uint32_t crc;
	
}EsbData_t;


typedef struct 	//任务接收处理数据结构
{
	uint8_t rssi;
	uint8_t size;
	uint8_t up_data[m_box_size];
}ESB_RX_t;

typedef struct		//音频数据结构
{
	uint8_t Length;
	uint8_t SeqNumber;
	uint8_t Data[20];
}AUdata_t;

typedef union			//无线到音频结构
{
	uint8_t up_data[m_box_size];
	struct
	{
		uint8_t type;
		AUdata_t audio; 
	};
	
}ESB_TO_AUDIO_t;

#pragma pack(pop)

typedef enum {mTx=1, mRx}esbStatus;	

uint8_t esbSendData(uint8_t *,uint8_t );
void esb_init(uint8_t );

int ReadRxQueue(ESB_RX_t *,uint32_t);
QueueHandle_t *GetRxQueue(void);


#endif

