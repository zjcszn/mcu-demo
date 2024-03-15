#ifndef _MAIN_H
#define _MAIN_H

#include "led.h"
#include "delay.h"
#include "key.h"
#include "motor.h"
#include "sys.h"
#include "usart.h"
#include "weight.h"
#include "adc.h"
#include "usmart.h"	 	 
#include "timer.h"
#include "userdefinefuntion.h"
#include "wifi_esp8266.h"


//-----------����--------------------------
#define CMD_REQUEST     "CMD,0,L3,END"  //����
#define CMD_RESPONSE    "CMD,OK,END"    //Ӧ��

  

//--------------�궨��----------------------
#define OP_CMD_MIN_LENGTH   27 //������С����
#define OP_OFFSET1    0    //ÿ��������һ���������ݴ���׵�ַ
#define OP_VALID    0x27    //�������������Ч��־λ
//------------------------------------------

//-----------������ֵ�趨------------------
//----------------------------------------

//--------------funtion-------------------ENABLE   DISABLE
#define TIM_ENABLE()    TIM_TemporaryClose(0)  //�ָ�ԭ����״̬
#define TIM_DISABLE()   TIM_TemporaryClose(0x11)  //����ԭ����״̬���رն�ʱ��

#define TIM3_ENABLE()   //TIM_TemporaryClose(0)	//�ָ�ԭ����״̬
#define TIM3_DISABLE()  //TIM_TemporaryClose(0x10)	//�رն�ʱ��3
#define TIM2_ENABLE()   //TIM_TemporaryClose(0)	//�ָ�ԭ����״̬
#define TIM2_DISABLE()  //TIM_TemporaryClose(0x01)	//�رն�ʱ��2

#define AT24C_EnableWrite()  AT24C_WP=0;   //����ROMд����
#define AT24C_DisableWrite()  AT24C_WP=1;   //��ֹROMд����
//---------------------------------

//-----------����------------------
#define FLUX_FLAG_H     0x31    //����������
#define FLUX_FLAG_L     0x32    //����������
#define FLUX_FLAG_OK    0x32    //��������
//---------------------------------

//-----Error Code ------------------
#define NOERROR     0x00
#define ERRORCODE_CONNECT   0x01    //WiFiģ��δ����
#define ERRORCODE_CMDFAIL   0x02    //���������ʽ����
#define ERRORCODE_COMM      0x04    //WiFiģ�鷢�����ݴ���
#define ERRORCODE_GETLINIT  0x08    //�����޻�ȡʧ�ܻ�������ֵ����ȷ
#define ERRORCODE_NOCHKDATA 0x10    //��EEPROM��û�л�ȡ�����������ݣ��ȴ��������ݽ�������
#define ERRORCODE_NOWIFI    0x20    //�޷����ӵ�ָ����WIFI������WIFI�Ѿ��رջ����������
#define ERRORCODE_WATCHDOGFAIL  0x40    //���Ź�ʧ�ܣ���ҳ�˻��ж��豸FAIL

#define ERRORCODE_DEVICE1   0x80    //�豸����,��ŷ��ѹر�,�������ƻ��г����涨��ʾ��������ô������
#define ERRORCODE_FLUX_H   0x100    //����������,���������涨��ʾ��������ô������
#define ERRORCODE_FLUX_L   0x200    //����������,���������涨��ʾ��������ô������
//-----------------------------------

extern u16 DeviceErrorCode;  
extern u8 ResetLinitFlag;   //���������ޱ�־�������������ޱ�־,
extern u8 Wifi_ConnetRightFlag;  //wifi������������λ�ñ�־λ
extern u8 Wifi_ConnetSeverFlag;  //wifi���ӷ�������������λ�ñ�־λ
extern u8 ResetRomFlag;  //TIM2���յ���λROM��Ϣ�����ݻ���λ�ñ�־
extern u8 Cmd_Flag_OK;   //�����־,Ӧ��OK
extern u8 Cmd_SendErrorFlag;    //��������ʧ��ʱ��λ�ñ�־,���ܷ��Ͳ������� 
extern u8 SendFluxDoingFlag; //���ڷ������������Ϣʱ����λ�ñ�־,��ֹ�ظ�������������ϢƵ������ROM
extern u8 Timer3_EnableFlag; //��ʱ��3ʹ�ܱ�־,��λ�ñ�־��ʹ�ܶ�ʱ��3����ͨ��ʱ��
extern u8 SolenoidValveOnFlag;   //��ŷ��򿪱�׼,��ŷ��򿪽���λ�ñ�־
extern u8 FluxStartFlag; //��ʼ����������ݱ�־,������ʱ����λ�ñ�־

extern char RecvicePN[32];  //������յ���PN
extern u16 FluxValue_Now;    //ʵʱ����ֵ
extern u16 Flowmeter_H;  //����������
extern u16 Flowmeter_L;  //����������
//----------------------------------

#define GET_VOUT1_VALUE()   ADC_GetAverage(ADC_Channel_1,10)
#define GET_VOUT2_VALUE()   ADC_GetAverage(ADC_Channel_0,10)

void SendWatchdog(void);    //���Ź�ι��
void SendErrorCode(u16* pcode); //���ʹ�����뵽����
u8 SengFluxDataToSever(u8 signal,u8 fluxResult,u16 nowDat,u16 beforeDat); //--�����������ݼ��жϽ������ҳ�����Ƚϻ����Ƿ�һ��-һ�·���0,


#endif




