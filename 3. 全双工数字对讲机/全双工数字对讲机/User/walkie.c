#include "walkie.h"

#if APP_TYPE == APP_TYPE_WALKIE
/*
	使用PPI将 按键 PPT和WKEY链接起来。
	
	
*/

#include "nrf_gpiote.h"

void sq_gpiote_init()
{
	nrf_gpio_cfg_input(SQ,NRF_GPIO_PIN_PULLUP);
	nrf_gpiote_event_configure(0,SQ,NRF_GPIOTE_POLARITY_LOTOHI);
	nrf_gpiote_event_enable(0);
	
	nrf_gpiote_event_clear(NRF_GPIOTE_EVENTS_IN_0);
	nrf_gpiote_int_enable(GPIOTE_INTENSET_IN0_Msk);
	
	NVIC_SetPriority(GPIOTE_IRQn, 2);
	NVIC_ClearPendingIRQ(GPIOTE_IRQn);
	NVIC_EnableIRQ(GPIOTE_IRQn);	
	
}


void module_gpio_init(void)
{
	sq_gpiote_init();
	
	nrf_gpio_cfg_output(PTT);
	nrf_gpio_pin_set(PTT);
	
	nrf_gpio_cfg_output(PD);
	nrf_gpio_pin_set(PD);
	
	nrf_gpio_cfg_output(SDP);
	nrf_gpio_pin_set(SDP);
	
	uart_init();

}





/*模块在上电工作后，如没有收到AT指令，其默认工作参数为：
  GBW=12.5KHZHZ，TFV=450.050MHZ, RFV=450.050MHZ，
  接收和发射 CTCSS=01， SQ=3, 扰频关闭
  模块工作在数据通讯模式时，PTT脚不能接低电平。
*/

//声控模式关闭			0 接收状态  1 发射状态
#define ASSERT_  0
#if ASSERT_ == 1
#define MY_ASSERT(x) if(x!=ERR_OK){ do{}while(1); }                                             
#else
#define MY_ASSERT(x) x
#endif

#pragma push
#pragma pack(1)
#define CRLF  "\r\n"
#define MESE  "+DMOMES"
#define VERS  "+DMOVERQ"


const char getDemocon[]=	{"AT+DMOCONNECT"};
const char setGroup[]=		{"AT+DMOSETGROUP"};
const char setAutoPower[]=     {"AT+DMOAUTOPOWCONTR"};
const char getVersion[]=	{"AT+DMOVERQ"};
const char setVolume[]=	{"AT+DMOSETVOLUME"};
const char setVox_Level[]=     {"AT+DMOSETVOX"};
const char setMic_Level[]=     {"AT+DMOSETMIC"};
const char setMss[]    =		{"AT+DMOMES"};
const char getMss[]    =    {"AT+DMOREADMES"};
const char getRssi[]    =   {"AT+DMOREADRSSI"};

#pragma pop


char Frs_Cmd[UART_MAX_RECV];
uint16_t out_count=0;

FRS_MODULE_PARA frs_Set;

err_t FRS_Send_Command(char* cmd,uint8_t len,uint8_t cmd_len)
{
    
  uint8_t repe_times=3;
  uint8_t length=0;
  length=sprintf(cmd+len,CRLF);
  length+=len;
  cmd[length]=0;
  repe:

  FRS_Input_Clr();
  FRS_Outpt(cmd,length);
  out_count=HAL_GetTick();
  while(1)
  {
    if(FRS_Input_Flag == 1)
    {
      if((!memcmp(FRS_Input,MESE,7)) || (!memcmp(FRS_Input,VERS,8)))
      {
        if(!memcmp(FRS_Input,MESE,7))
        {
          memcpy(frs_Set.RX_Mss,FRS_Input+10,FRS_Input_LEN-10);	
          return ERR_OK;
        }
        else if(!memcmp(FRS_Input,VERS,8))
        {
          memcpy(frs_Set.version,FRS_Input+9,FRS_Input_LEN-9);				
          return ERR_OK;
        }
      }
      char *frs_in = 0;
      frs_in = (cmd+2);
      frs_in[cmd_len-2]=0;
      frs_in=strncat(frs_in,":0",2);
      if(!strncmp((const char*)frs_in ,(const char*)FRS_Input, strlen((const char*)frs_in)))
      {
        return ERR_OK;
      }
      repe_times--;
      if(repe_times == 0)
        return ERR_DATA;	
        goto repe;			
      }
    if(HAL_GetTick()-out_count >= 100)
    {
      repe_times--;
      if(repe_times == 0)
      return ERR_TIMEOUT;
      goto repe;
    }
  }
}

void FRS_Connecte(void)
{
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  uint8_t length=sprintf(Frs_Cmd,getDemocon);	//
  MY_ASSERT(FRS_Send_Command(Frs_Cmd,length,strlen(getDemocon)));//-----
}

 err_t FRS_Get_Version()
{
    memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,getVersion);	//
  return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(getDemocon));
}

 err_t FRS_Sleep_Set(FRS_MODULE_PARA *param)
{
    memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,setAutoPower);	//
  if(param->AUTOPOWCONTR)
  {
    sprintf(Frs_Cmd+strlen(Frs_Cmd),"=1");	//关闭省电
  }
  else
  {
    sprintf(Frs_Cmd+strlen(Frs_Cmd),"=0");	//开启省电
  }
  return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setAutoPower));
}


 err_t FRS_Volume_Set(FRS_MODULE_PARA *param)
{
    memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,setVolume);	//
  if((param->SETVOLUME<=9)&&(param->SETVOLUME>=1))
  {
    sprintf(Frs_Cmd+strlen(Frs_Cmd),"=%d",param->SETVOLUME);
    return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setVolume));
  }
  return ERR_PARA_OUT;
}

 err_t FRS_VOX_Set(FRS_MODULE_PARA *param)
{
    memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,setVox_Level);	//
  if(param->SETVOX <= 8)
  {
    sprintf(Frs_Cmd+strlen(Frs_Cmd),"=%d",param->SETVOX);
    return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setVox_Level));		
  }
  return ERR_PARA_OUT;
}

 err_t FRS_Mic_Set(FRS_MODULE_PARA *param)
{
    memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,setMic_Level);	//
  if((param->MICLVL <= 8)&&(param->MICLVL >= 1))
  {
    if(param->SRAMLVL <= 8)
    {
      if(param->TOT <=9)
      {
        sprintf(Frs_Cmd+strlen(Frs_Cmd),"=%d,%d,%d",param->MICLVL,param->SRAMLVL,param->TOT);
        return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setMic_Level));		
      }
    }
  }
  return ERR_PARA_OUT;
}


err_t Walkie_Snd_Mess(uint8_t *data,uint8_t length)
{
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
/*   工作参数的设置信息   */ 	
  sprintf(Frs_Cmd,setMss);	
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"=");	
	
	frs_Set.TX_Mss[0] = length;
	
	for(uint8_t i=1;i<length+1;i++)
	{
		frs_Set.TX_Mss[i] = data[i-1];
	}
	
	uint8_t *cmd=(uint8_t*)(Frs_Cmd+strlen(Frs_Cmd));
	
	memcpy(cmd,frs_Set.TX_Mss,(length+1));

	return FRS_Send_Command(Frs_Cmd,strlen(setMss)+length+2,strlen(setMss));	
}


err_t Walkie_ReadMess()
{
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,getMss);	
	return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(getMss));		
}
	
err_t Walkie_ReadRSSI()
{
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  sprintf(Frs_Cmd,getRssi);	
	return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(getRssi));		
}
	

err_t FRS_GET_MSS()
{
  if(FRS_Input_Flag == 1)
  {
    if(!memcmp(FRS_Input,"+DMOMES",7))
    {
      memcpy(frs_Set.RX_Mss,FRS_Input+10,strlen((const char*)FRS_Input));	
      return ERR_OK;			
    }
  }
  return ERR_EMPTY;
}


err_t FRS_Group_Set(FRS_MODULE_PARA *param)
{
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
	
  if(param->GBW >1)
    return ERR_PARA_OUT;
  if( (param->RX_Ch_Frequcy >= 470 )||(param->RX_Ch_Frequcy <= 400))
    return ERR_PARA_OUT;
  if( (param->TX_Ch_Frequcy >= 470 )||(param->TX_Ch_Frequcy <= 400))
    return ERR_PARA_OUT;
  if( (param->TXCXCSS >121)||(param->RXCXCSS >121))
    return ERR_PARA_OUT;
  if(param->SQ_Level > 8)
    return ERR_PARA_OUT;

  sprintf(Frs_Cmd,setGroup);	
  strncat(Frs_Cmd,"=",1);

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",param->GBW);	

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%3d.",param->RX_Ch_Frequcy);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%04d,",param->RX_Ch_Low);		

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%3d.",param->TX_Ch_Frequcy);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%04d,",param->TX_Ch_Low);	

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",param->RXCXCSS);	
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",param->SQ_Level);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",param->TXCXCSS);	
  
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d",param->FLAG);	
  
  return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setGroup));
}

/**
  *http://www.nicerf.cn/news_86_366.html SQ静躁了解
  */

err_t FRS_Param_Init()
{
	frs_Set.Midx  = 0;
/*   工作参数的设置信息   */  
  frs_Set.GBW  = 0;

  frs_Set.TX_Ch_Frequcy = Default_Fre;
  frs_Set.TX_Ch_Low     = Default_Low;

  frs_Set.RX_Ch_Frequcy = Default_Fre;
  frs_Set.RX_Ch_Low     = Default_Low;

  frs_Set.RXCXCSS       = Default_RXCTCSS;  
  frs_Set.SQ_Level      = 5;
  frs_Set.TXCXCSS       = Default_TXCTCSS;
 
  frs_Set.Busy = 0;
  
  frs_Set.H_L  = 1;
  
  frs_Set.PRC  = 0;
  
/*   自动省电功能设置命令   */  
  frs_Set.AUTOPOWCONTR = 1;
  
/*   设置音量   */  
  frs_Set.SETVOLUME    = 8;
  
/*   设置声控   */  
  frs_Set.SETVOX       = 0;
  
/*咪灵敏度、语音加密（扰频）及发射限时（TOT）设置*/  
  frs_Set.MICLVL  = 3;
  frs_Set.SRAMLVL = 0;
  frs_Set.TOT     = 0;
  
  return 0;
}



err_t FRS_Init(void)
{
  FRS_Param_Init();
  FRS_Sleep_Set(&frs_Set);
  FRS_Mic_Set(&frs_Set);
  FRS_VOX_Set(&frs_Set);
  FRS_Group_Set(&frs_Set);
  return ERR_OK;
}


err_t setModule_Ch(int idx,uint8_t ch_idx)
{
	if(idx < 13)
		frs_Set.Midx  = idx;
	
  frs_Set.TX_Ch_Frequcy = Ch_Frequcy+(idx*ch_Spacing);
  frs_Set.TX_Ch_Low     = Ch_Low*1;

  frs_Set.RX_Ch_Frequcy = Ch_Frequcy+(idx*ch_Spacing);
  frs_Set.RX_Ch_Low     = Ch_Low*1;
	
  frs_Set.RXCXCSS       = RXCTCSS;  
  frs_Set.SQ_Level      = 8;
  frs_Set.TXCXCSS       = TXCTCSS;	
	
  memset(Frs_Cmd,0,sizeof(Frs_Cmd));
  if(frs_Set.GBW >1)
    return ERR_PARA_OUT;
  if( (frs_Set.RX_Ch_Frequcy >= 470 )||(frs_Set.RX_Ch_Frequcy <= 400))
    return ERR_PARA_OUT;
  if( (frs_Set.TX_Ch_Frequcy >= 470 )||(frs_Set.TX_Ch_Frequcy <= 400))
    return ERR_PARA_OUT;
  if( (frs_Set.TXCXCSS >121)||(frs_Set.RXCXCSS >121))
    return ERR_PARA_OUT;
  if(frs_Set.SQ_Level > 8)
    return ERR_PARA_OUT;

  sprintf(Frs_Cmd,setGroup);	
  strncat(Frs_Cmd,"=",1);

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",frs_Set.GBW);	

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%3d.",frs_Set.RX_Ch_Frequcy);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%04d,",frs_Set.RX_Ch_Low);		

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%3d.",frs_Set.TX_Ch_Frequcy);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%04d,",frs_Set.TX_Ch_Low);	

  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",frs_Set.RXCXCSS);	
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",frs_Set.SQ_Level);
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d,",frs_Set.TXCXCSS);	
  
  sprintf(Frs_Cmd+strlen(Frs_Cmd),"%d",frs_Set.FLAG);	
	return FRS_Send_Command(Frs_Cmd,strlen(Frs_Cmd),strlen(setGroup));
}









#endif
