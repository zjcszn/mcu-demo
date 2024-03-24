
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

#include "consert.h"

//pdata:char * pdata(ԭʼ˫������Ƶ����)
//dlen:pdata���ݳ���
//���ֽ�1280���ȵ�����pdata ת��Ϊ640˫�ֽ�����g_sSrcAudioBuf

void StereoToMono(int16_t* src_audio,int samples_per_channel,int16_t* dst_audio) 
{
    for (int i = 0; i < samples_per_channel; i++) 
    {
        dst_audio[i] = ((src_audio[2 * i]) + src_audio[2 * i + 1]) >> 1;
    }
}


void MonoToStereo(int16_t* src_audio,int samples_per_channel,int16_t* dst_audio) 
{
    for (int i = 0; i < samples_per_channel; i++) 
    {
        dst_audio[2 * i] = src_audio[i];
        dst_audio[2 * i + 1] = src_audio[i];
    }
}




