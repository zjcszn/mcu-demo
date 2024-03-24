
#ifndef __WM8978_H_
#define __WM8978_H_

#include "stdint.h"



//#define WM8978_MONO

#define WM8978_ADDR				0X1A	//WM8978��������ַ,�̶�Ϊ0X1A 

#define I2S_Standard_Phillips           ((uint16_t)0x0000)
#define I2S_Standard_MSB                ((uint16_t)0x0010)
#define I2S_Standard_LSB                ((uint16_t)0x0020)


/* WM8978 ��Ƶ����ͨ������ѡ��, ����ѡ���·������ MIC_LEFT_ON | LINE_ON */
typedef enum
{
	IN_PATH_OFF		= 0x00,	/* ������ */
	MIC_LEFT_ON 	= 0x01,	/* LIN,LIP�ţ�MIC���������Ӱ�����ͷ��  */
	MIC_RIGHT_ON	= 0x02,	/* RIN,RIP�ţ�MIC���������Ӱ�����ͷ��  */
	LINE_ON			  = 0x04, /* L2,R2 ����������(�Ӱ��ض�������) */
	AUX_ON			  = 0x08,	/* AUXL,AUXR ���������루������û�õ��� */
	DAC_ON			  = 0x10,	/* I2S����DAC (CPU������Ƶ�ź�) */
	ADC_ON			  = 0x20,	/* �������Ƶ����WM8978�ڲ�ADC ��I2S¼��) */
	LINE_BOOST_EN = 0x40,
	LINE_L2_EN    = 0x80,
	LINE_R2_EN    = 0x100,
}IN_PATH_E;

/* WM8978 ��Ƶ���ͨ������ѡ��, ����ѡ���· */
typedef enum
{
	OUT_PATH_OFF	= 0x00,	/* ����� */
	EAR_LEFT_ON 	= 0x01,	/* LOUT1 ����������(�Ӱ��ض�������) */
	EAR_RIGHT_ON	= 0x02,	/* ROUT1 ����������(�Ӱ��ض�������) */
	SPK_ON			  = 0x04,	/* LOUT2��ROUT2������������� */
	OUT3_ON		  	= 0x08,	/* OUT3 �����������Ƶ */
	OUT4_ON				= 0x10,	/* OUT4 �����������Ƶ */
	BYPASS_ON     = 0x20, /* �����ֱͨ����� */	
	MIXER_OUT_ON  = 0x40,	/* ��������������������OUT4 */
	DAC_OUT_ON		= 0x80,
	AUR_BYPASS_ON = 0x100,
}OUT_PATH_E;

typedef enum
{
	SAMPLE_8K		= 0x01,
	SAMPLE_16K	= 0x02,
	SAMPLE_24K	=	0x04,
	SAMPLE_32K	= 0x08,
	SAMPLE_4K	= 0x10	
}SAMPLE_RATE_E;


typedef enum
{
	ALC_OFF = 0,
	ALC_RIGHT_ONLY,
	ALC_LEFT_ONLY,
	ALC_BOTH_ONLY
	
}ALC_SET_E;


typedef enum
{
	ALC_MIN_M12dB,
	ALC_MIN_M6dB,
	ALC_MIN_0dB,
	ALC_MIN_6dB,
	ALC_MIN_12dB,
	ALC_MIN_18dB,
	ALC_MIN_24dB,
	ALC_MIN_30dB	
}ALC_MIN_DB_E;

typedef enum
{
	ALC_MAX_M6p75dB,
	ALC_MAX_M0p75dB,
	ALC_MAX_5p25dB,
	ALC_MAX_11p25dB,
	ALC_MAX_17p25dB,
	ALC_MAX_23p25dB,
	ALC_MAX_29p25dB,
	ALC_MAX_35p25dB	
	
}ALC_MAX_DB_E;

/* ����������� */
#define VOLUME_MAX		                      63		/* ������� */
#define VOLUME_STEP		                       1		/* �������ڲ��� */

/* �������MIC���� */
#define GAIN_MAX		                        63		/* ������� */
#define GAIN_STEP		                         1		/* ���沽�� */



/* ���ⲿ���õĺ������� */
uint8_t wm8978_Init(void);
uint8_t wm8978_Reset(void);
void wm8978_CfgAudioIF(uint16_t _usStandard, uint8_t _ucWordLen,uint16_t _ucSample);
void wm8978_OutMute(uint8_t _ucMute);
void wm8978_PowerDown(void);
void wm8978_CfgAudioPath(uint16_t _InPath, uint16_t _OutPath);
void wm8978_SetMicGain(uint8_t _ucGain);
void wm8978_SetLineGain(uint16_t In_path,uint8_t _ucGain);
void wm8978_SetAUXGain(uint8_t _ucGain);

void wm8978_SetOUT2Volume(uint8_t _ucVolume);
void wm8978_SetOUT1Volume(uint8_t _ucVolume);

uint8_t wm8978_ReadOUT1Volume(void);
uint8_t wm8978_ReadOUT2Volume(void);
void wm8978_NotchFilter(uint16_t _NFA0, uint16_t _NFA1);
void wm8978_ALC_Set(void);
void wm8978_SetOUT1Mute(void);

#endif

