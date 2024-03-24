/************************************************************    
 FileName: esb.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: ����д    // ģ������         
 Version:         			        // �汾��Ϣ   
 Function List:                 // ��Ҫ�������书��     
	1. -------   
	History:         // ��ʷ�޸ļ�¼       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "audioFuction.h"

void awModeSelecet(uint8_t mode)
{
	switch(mode)
	{
		case 4:
			nrf_gpio_pin_set(SHUT);
			nrf_delay_us(2);
			nrf_gpio_pin_clear(SHUT);
			nrf_delay_us(2);			
		case 3:
			nrf_gpio_pin_set(SHUT);
			nrf_delay_us(2);
			nrf_gpio_pin_clear(SHUT);
			nrf_delay_us(2);			
		case 2:
			nrf_gpio_pin_set(SHUT);
			nrf_delay_us(2);
			nrf_gpio_pin_clear(SHUT);
			nrf_delay_us(2);			
		case 1:
			nrf_gpio_pin_set(SHUT);				
		break;
		default:
			nrf_gpio_pin_clear(SHUT);		
		break;
	}
}


void setPlayVolue(uint8_t volue)
{
	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(volue);
}


void playAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size,uint8_t volue)
{
	/* ����WM8978оƬ������Ϊ�����룬���Ϊ������R2�������ROUT1EN��LOUT1EN�ͻ����?	*/
	wm8978_CfgAudioPath( DAC_ON ,  EAR_LEFT_ON | EAR_RIGHT_ON  );
	
	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(volue);
	
	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16 , SAMPLE_8K);
		
	nrfI2S_Start_play(txbuf,rxbuf,size);
	
	awModeSelecet(3);
	
}


void recoderAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size)
{
	nrf_gpio_pin_clear(SHUT);
	
	/* ����WM8978оƬ������Ϊ�����룬���Ϊ������R2�������ROUT1EN��LOUT1EN�ͻ����?	*/
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON ,OUT_PATH_OFF);
	
	/* ����MIC����֮�������Ŵ��� */
	wm8978_SetMicGain(60);
	
	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16 , SAMPLE_8K);

	nrfI2S_Start_Rec(txbuf,rxbuf,size);
}

void fullduplexAudio(uint32_t *txbuf,uint32_t *rxbuf,uint16_t size,uint8_t volue)
{
	/* ����WM8978оƬ������Ϊ�����룬���Ϊ������R2�������ROUT1EN��LOUT1EN�ͻ����?	*/
	wm8978_CfgAudioPath(MIC_LEFT_ON | MIC_RIGHT_ON | ADC_ON | DAC_ON , EAR_LEFT_ON | EAR_RIGHT_ON  );
	
	/* ����MIC����֮�������Ŵ��� */
	wm8978_SetMicGain(60);
	
	/* ����������������ͬ���� */
	wm8978_SetOUT1Volume(volue);
	
	/* ����WM8978��Ƶ�ӿ�Ϊ�����ֱ�׼I2S�ӿڣ�16bit */
	wm8978_CfgAudioIF(I2S_Standard_Phillips, 16 , SAMPLE_8K);

	nrfI2S_Start_FullDuplex(txbuf,rxbuf,size);
	
	awModeSelecet(4);
	
}


void stopAudio(void)
{
	nrf_gpio_pin_clear(SHUT);
	nrfI2S_Stop();
	wm8978_Reset();
}


#define df2TNumStage 3
float32_t df2T_pCoeffs300_3400[5 * df2TNumStage] = 
{
	1	,	0	,	-1	,	1.559664972369			, -0.6939100921432,
	1	,	0	,	-1	,	-1.46036574919  , -0.6471475574592,
	1	, 0	,	-1	,	0.05917957433294 		, 0.3689194771098,
};

const float df2T_ScaleValue300_3400 = 0.7730059593418 * 0.7730059593418 * 0.6844597385549 ;
arm_biquad_cascade_df2T_instance_f32 df2T;
float32_t df2T_State[2 *df2TNumStage];
float32_t *df2T_pCoeffs = df2T_pCoeffs300_3400;
float32_t df2T_ScaleValue = df2T_ScaleValue300_3400;

/***********************************************************************************************/

void dsp_fitter_init(void)
{
	arm_biquad_cascade_df2T_init_f32(&df2T,df2TNumStage,df2T_pCoeffs,df2T_State);	
}

void dsp_fiiter_process(int16_t *in,int16_t *out)
{
	float32_t pdOutData[160];
	float32_t pdDecData[160];	
	
	arm_q15_to_float((q15_t*)in,pdDecData,160);
	arm_biquad_cascade_df2T_f32(&df2T,pdDecData,pdOutData,160);
	arm_scale_f32(pdOutData,df2T_ScaleValue,pdOutData,160);
	arm_float_to_q15(pdOutData,(q15_t*)out,160);
}
