
/************************************************************    
 FileName: esb.c 
 Author:   WangXh . 1220420938@qq.com
 Version : 0.1         
 Date:     2020-8-14
 Description: 不想写    // 模块描述         
 Version:         			        // 版本信息   
 Function List:                 // 主要函数及其功能     
	1. -------   
	History:         // 历史修改记录       
	<author>  <time>   <version >   <desc>       
	WangXh    20/08/14  1.0     build this moudle   
***********************************************************/ 

#include "consert.h"

//pdata:char * pdata(原始双声道音频数据)
//dlen:pdata数据长度
//单字节1280长度的数据pdata 转换为640双字节数据g_sSrcAudioBuf

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




