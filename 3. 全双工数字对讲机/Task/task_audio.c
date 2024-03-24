/************************************************************    
 FileName: task_audio.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description:  ����¼������   // ģ������         
 Version:         			  // �汾��Ϣ   
 Function List:               // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "task_audio.h"
#include "audioFuction.h"
#include "task_rfIT.h"
#include "SpeexC.h"
#include "esb.h"

static TaskHandle_t audioThread;

/* ��ʱ�޸�,�����ú궨��*/
static const uint16_t rxsize = 160;

static uint32_t *rx_point1;
static uint32_t *rx_point2;	
static uint32_t *tx_point1;
static uint32_t *tx_point2;

static QueueHandle_t Audio_Event;
static QueueHandle_t AudioQueIn;	
static QueueHandle_t AudioQueOut;	

void I2S_IRQHandler(void)  //��Ҫ����
{
	uint8_t Event;
	BaseType_t yield_req = pdFALSE;
	
	if(nrf_i2s_int_enable_check(NRF_I2S, NRF_I2S_INT_RXPTRUPD_MASK))
	{
		if(nrf_i2s_event_check(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD))
		{
			nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_RXPTRUPD);
			Event = AudioEvent_RX;
			xQueueSendFromISR(Audio_Event,(void*)&Event,&yield_req);
		}
	}
	
	if(nrf_i2s_int_enable_check(NRF_I2S, NRF_I2S_INT_TXPTRUPD_MASK))
	{
		if(nrf_i2s_event_check(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD))
		{
			nrf_i2s_event_clear(NRF_I2S, NRF_I2S_EVENT_TXPTRUPD);
			Event = AudioEvent_TX;
			xQueueSendFromISR(Audio_Event,(void*)&Event,&yield_req);		
		}
	}
	portYIELD_FROM_ISR(yield_req);
}


/* ����ͷ����ͨ��WM8978����Ȼ��ͨ�����뺯�����н���Ȼ��ͨ��OUT3��� */
void taskAudioMain(void *arg)
{
	
	uint32_t wavRptr=0;
	extern const unsigned char wavdata[48000];
	
	uint8_t event;
	
	/* ¼����ر��� */
	uint8_t rxflag = 0;
	int16_t deRxConsert[rxsize];
	uint8_t TxSeqNumber = 0;
	uint8_t Enbuf[20];
	
	/* ������ر��� */
	uint8_t txflag = 0;
	uint8_t RecLength;
	uint8_t RxSeqNumber;
	uint8_t DecIn[20];
	uint8_t NextPkt = 0, dummy;
	
	/* ��ʼ�� IIR �˲� */
	dsp_fitter_init();
	
	/* ��Ƶ���Źر� */
	awModeSelecet(0);

	/* ��ʼ��ѹ���� */
	SpeexEncode_Init();
	
	/* ��ʼ����ѹ���� */
	SpeexDecode_Init();
	
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON | DAC_ON , EAR_LEFT_ON | EAR_RIGHT_ON );
	wm8978_SetMicGain(50);
	wm8978_SetOUT1Volume(50);
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16 , SAMPLE_8K);
	
	/* ����I2S¼��.*/
	nrfI2S_Start_FullDuplex(tx_point1,rx_point1,rxsize);	
	
	/* ����Ƶ���� */
	awModeSelecet(3);
	while(1)
	{
		if(xQueueReceive(Audio_Event, (void *)&event,1000) == pdTRUE)
		{		
			switch(event)
			{
				case AudioEvent_RX:
				{
					rxflag = !rxflag;
					if(rxflag)
					{
						StereoToMono((int16_t*)rx_point2,rxsize,(int16_t*)deRxConsert);
						nrf_i2s_rx_buffer_set(NRF_I2S, (uint32_t*)rx_point2);
					}
					else
					{
						StereoToMono((int16_t*)rx_point1,rxsize,(int16_t*)deRxConsert);
						nrf_i2s_rx_buffer_set(NRF_I2S, (uint32_t*)rx_point1);
					}					

					//������Ƶ�����Ҫ��ȡ��ע��
					arm_copy_q15((q15_t*)&wavdata[wavRptr],deRxConsert,160);
					wavRptr = (wavRptr + 320)%48000;
					
					//�ж�д������Ƿ񳬹�,������������
					if(uxQueueSpacesAvailable(*audioGetOutQueue()) == 0)
					{
						/* Not Space In Queue Read Longer Data From Queue */
						uint8_t dLength,dSeqNumber,dData[PayLoadSize];
						ReadOutQueue(&dLength,&dSeqNumber,dData);
					}		
					
					/* 300H~3400HZ��Ƶ����Ƶ�����˲� */
					dsp_fiiter_process(deRxConsert,deRxConsert);
					
					/* ѹ��20ms���ݰ� ʾ������14.5ms */
					SpeexEncodeData((uint16_t*)deRxConsert,Enbuf);

					/* ��ѹ����ɵ�����д�뷢�Ͷ��� */
					if(WriteOutQueue(20,TxSeqNumber,Enbuf) == pdPASS)
					{
						TxSeqNumber++;
					}						
				}
				break;
				
				case AudioEvent_TX:
				{
					//��ȡ�������������Ƿ������� 
					if(ReadInQueue((uint8_t*)&RecLength,&RxSeqNumber,DecIn) == pdTRUE)
					{
#ifdef LOSS_PKT				
						dummy = RxSeqNumber - NextPkt;
						NextPkt = RxSeqNumber + 1;
						if(dummy == 0)
						{
#endif				//��ѹ��
							SpeexDecodeData(DecIn,(uint16_t*)deRxConsert,20,0);
#ifdef LOSS_PKT								
						}
#endif
					}
					else
					{
						//������֡,��Ĭ
						arm_fill_q15(0,deRxConsert,rxsize);
					}				

					
					txflag = !txflag;
					if(txflag)
					{
						MonoToStereo(deRxConsert,rxsize,(int16_t*)tx_point2);
						nrf_i2s_tx_buffer_set(NRF_I2S, tx_point2);
					}
					else
					{
						MonoToStereo(deRxConsert,rxsize,(int16_t*)tx_point1);
						nrf_i2s_tx_buffer_set(NRF_I2S,tx_point1);
					}		
				}
				break;
				
				default:
				{
				
				}
				break;
			} /* Switch(event) */
			
		}	/* End of if(xQueueReceive(Audio_Event, (void *)&event,1000)*/
	}
}

//������������ͽ���
void osAudioOpen(void)
{
	if(audioThread == NULL)
	{
		ResetOutQueue();
		ResetInQueue();
		if( xTaskCreate(taskAudioMain,"osAudio",0x800,NULL,configMAX_PRIORITIES - 4,&audioThread) != pdPASS)
		{
			while(1);
		}
	}
}

//�ر���������ͽ���
void osAudioClose(void)
{
	if(audioThread != NULL)
	{	
		taskENTER_CRITICAL();
		stopAudio();
		taskEXIT_CRITICAL();
		
		vTaskDelete(audioThread);
		audioThread = NULL;
		
		xQueueReset(Audio_Event);
	}
}


void osAudioCreate(void)
{
	tx_point1 = pvPortMalloc(rxsize * 2 * sizeof(uint16_t));	
	tx_point2 = pvPortMalloc(rxsize * 2 * sizeof(uint16_t));

	rx_point1 = pvPortMalloc(rxsize * 2 * sizeof(uint16_t));
	rx_point2 = pvPortMalloc(rxsize * 2 * sizeof(uint16_t));	

	I2S_Init();
	shutdown_init();
	wm8978_Init();
	
	Audio_Event = xQueueCreate(4, sizeof(uint8_t));
	
	if(AudioQueOut == NULL)
	{
		AudioQueOut = xQueueCreate(40,sizeof(AUdata_t));	
	}
	if(AudioQueIn == NULL)
	{
		AudioQueIn = xQueueCreate(40,sizeof(AUdata_t));	
	}	

}


void ResetOutQueue(void)
{
	xQueueReset(AudioQueOut);
}

int WriteOutQueue(uint8_t Length,uint8_t SeqNumber,uint8_t *data)
{
	AUdata_t Data;
	
	Data.Length = Length;
	Data.SeqNumber = SeqNumber;
	arm_copy_q7((q7_t*)data,(q7_t*)Data.Data,Length);
	return xQueueSend(AudioQueOut,(void*)&Data,0);	
}

int	ReadOutQueue(uint8_t *Length,uint8_t *SeqNumber,uint8_t *data)
{
	int error;
	AUdata_t Data;	
	error = xQueueReceive(AudioQueOut, (void *)&Data , 0) ;
	if(error == pdTRUE)
	{
		*Length = Data.Length;
		*SeqNumber = Data.SeqNumber;
		arm_copy_q7((q7_t*)Data.Data,(q7_t*)data,Data.Length);	
	}
	return error;
}

QueueHandle_t *audioGetOutQueue(void)
{
	return &AudioQueOut;
}


void ResetInQueue(void)
{
	xQueueReset(AudioQueIn);
}

int WriteInQueue(uint8_t Length,uint8_t SeqNumber,uint8_t *data)
{
	AUdata_t Data;
	
	Data.Length = Length;
	Data.SeqNumber = SeqNumber;
	arm_copy_q7((q7_t*)data,(q7_t*)Data.Data,Length);
	return xQueueSend(AudioQueIn,(void*)&Data,0);
}

int ReadInQueue(uint8_t *Length,uint8_t *SeqNumber,uint8_t *data)
{
	int error;
	AUdata_t Data;	
	error = xQueueReceive(AudioQueIn, (void *)&Data , 0) ;
	if(error == pdTRUE)
	{
		*Length = Data.Length;
		*SeqNumber = Data.SeqNumber;
		arm_copy_q7((q7_t*)Data.Data,(q7_t*)data,Data.Length);	
	}
	return error;
}

QueueHandle_t *audioGetInQueue(void)
{
	return &AudioQueIn;
}


