#include "MasterLoRaHandle.h"


/**********************************************************************
							变量区
***********************************************************************/
	void GetSlaveID(u8 *SlaveID_Buf);
	void LoRaCtrlRelayCmd(u8 *data,u16 datalen);
	void LoRaGetIDCmd(u8 *data,u16 datalen);
	void LoRaHandleEncryptedData(void);
	void SplitComDataToEncryptedData(void);
	void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen);
	void LoRaSendEncryptedData(u8 *data,u16 datalen);
	//----------------------------------------------------------
	LoRaStruct	LoRaPact;			//定义lora解析协议结构体
	u8	LoRaRxBuf[LORA_RX_SIZE];	//定义接收缓冲区
	u8	LoRaTxBuf[LORA_TX_SIZE];	//定义发送缓冲区
	_ST st = Tx_ST;					//定义主机的状态枚举
	u16	LoRaTransCounter = 0x00;	//传输计时器(收/发总时长)
	u16 WaitIDCounter = 0x00;		//等待对方ID计时器
	//------------------------------------------------
	u8 	SelfID_Buf[16]  = {0,};		//保存自身标识符
	u8 	OtherID_Buf[16] = {0,};		//保存对方标识符
	u8 	SM4_KEY[16] = {0,};			//SM4密钥
	u8 	GetBidStatus = DISABLE;		//得到对方标识符
	u8 	AgreeStatus  = DISABLE;		//sm2密钥协商成功
	//------------------------------------------------
	
	//---------------------------------------------------
	u8 	EncryptedData[1100] = {0,};	//保存加密后的数据
	u8  DecryptedData[1100] = {0,};	//保存解密后的数据
	u8 	PackSum = 0x00;				//总的包数
	u8 	CountPackNum = 0x00;		//数据包计数
	u16 AfterEncryptSzie = 0x00;	//加密后的数据长度
	u16 PackSumSize = 0x00;			//总的数据包长度
	//---------------------------------------------------
/**********************************************************************/


/***************************************************
	* 功能描述：	设置LoRa结构体参数
	* 入口参数：	无
	* 返回值：		无
****************************************************/
void SetLoRaStruPara(void)
{
	LoRaPact.RxCnt 	    	= 0x00;
	LoRaPact.TxCnt 	    	= 0x00;
	LoRaPact.RxBuf 	    	= LoRaRxBuf;	
	LoRaPact.TxBuf 	    	= LoRaTxBuf;
	LoRaPact.DeviceAddr 	= ReadDeviceID();	//读取本机设备号
	LoRaPact.DstDeviceAddr 	= 0x09;
	
	//****保存系统日志(设置设备地址)
	RestoreWhichLog(SystemLogFlag,SystemLogBuf,SET_DEVICE_ADDR_CODE,0,Log_Successful);
}


/****************************************
	* 功能描述：SM2密钥协商
	* 入口参数：
	* 返回值：
	* 备注：	
*********************************************/
void MasterHandleSm2KeyAgreement(void)
{
	//密钥协商成功则退出函数
	if(AgreeStatus != DISABLE)
		return ;
	
	GetSlaveID(OtherID_Buf);	//请求获取从机的ID
	if( GetBidStatus )			//得到了从机的ID
	{
		GetBidStatus = DISABLE;
		SetTheBID(OtherID_Buf,16);
		AgreeStatus = ENABLE;
		TimerDelay_Ms(500);
		HSC32I1_ReadLenByte(SM4_KEYADDR,SM4_KEY,16);
		TimerDelay_Ms(500);
	}
}


/********************************************************
	* 功能描述：获取从机标识符
	* 入口参数：SlaveID_Buf -- 存储从机标识符缓冲区
	* 返回值：	无
	* 备注：	无
*********************************************************/
void GetSlaveID(u8 *SlaveID_Buf)
{
	u8 size;
	
	LoRaGetIDCmd(LoRaPact.TxBuf,0);
	SX1276RxStateEnter();
	
	//如果超时无响应，则重新等待时延
	if(WaitIDCounter > 5000)
		WaitIDCounter = 0x00;
	while( (WaitIDCounter < 5000) && (!DIO0) );
	
	if( DIO0==1 )
	{
		SX1276RxDataReadCRC32( LoRaPact.RxBuf, &size );
		if( (LoRaPact.RxBuf[6]=='O')&&
			(LoRaPact.RxBuf[7]=='K') )
		{
			memcpy(SlaveID_Buf,&LoRaPact.RxBuf[8],16);
			memset(LoRaPact.RxBuf,0,size);
			GetBidStatus = ENABLE;
		}
	}
	//进入待机模式
	SetLoRaStandBy();
}


/*************************************************
	* 功能描述：LoRa传输主处理函数
	* 入口参数：
	* 返回值：
	* 备注：
**************************************************/
void LoRaMasterMainProcess(void)
{
	//秘钥还未匹配成功
	if(AgreeStatus !=ENABLE)
		return ;
	
	//未切换到透传模式
	if(com_work_mode != ComMode_Trans)
		return;
	
	IsComChangeToAT_WorkMode();
	
	//串口接收到数据且工作在传透模式
	if( (hostcom_struct.Comx_RevCompleted == ENABLE) && (com_work_mode == ComMode_Trans) )
	{
		hostcom_struct.Comx_RevCompleted = DISABLE;		//使能下一次数据处理
		LoRaPact.TxCnt = hostcom_struct.Comx_RxCnt;		//得到接收到的数据长度
		
		//状态处理
		switch(st)
		{
			//发送数据
			case Tx_ST:
//				LoRaTransCounter = 0x00;
				LoRaHandleEncryptedData();	//发送加密数据
				st = Rx_ST;					//进入接收状态
				SX1276RxStateEnter();		//进入接收状态
//				printf("开始发送-->接收状态的时间为 --  t3:%dms\r\n", LoRaTransCounter);
//				printf("\r\n");
			break;

			default:
			break;
		}
		
		PackSum 	 = 0;
		PackSumSize  = 0;
		CountPackNum = 0;
		AfterEncryptSzie = 0;
		
		//接收完数据后，待处理DMA1相关... ...
		hostcom_struct.Comx_RxCnt = 0;							//将接收计数器清零
		memset(hostcom_struct.Comx_RxBuf,0,HOST_RX_SIZE);		//将接收缓冲区清空
		if(hostcom_struct.WhichUart == UART1)
		{
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);	//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始						
			DMA_Cmd(DMA1_Channel5,ENABLE);  					//打开DMA接收
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);	//设置数据接收长度，重新装填，并让数据接收地址偏移地址从0开始						
			DMA_Cmd(DMA1_Channel3,ENABLE);  					//打开DMA接收
		}
	}
	
	if(st == Rx_ST)
	{
		//等待超时(0.5s)接收到来，否则退出循环
		while( (LoRaTransCounter < 4500) && (!DIO0) );
//		if(LoRaTransCounter >=4500)
//		{
//			printf("*************超时等待*************！\r\n");
//		}

		ReceiveData(LoRaPact.RxBuf,&LoRaPact.RxCnt);

		//间隔约1s发送一次请求
//		LoRaTransCounter = 0;
		st = Tx_ST;
	}
	
}


/*********************************************
	* 功能描述
**********************************************/
void LoRaHandleEncryptedData(void)
{
	u8 size = 0x00;
	
	SplitComDataToEncryptedData();					//获取加密数据
	LoRaBeforeSendEncryptedData(LoRaPact.TxBuf,0);	//在发送加密数据前的数据包
	TimerDelay_Ms(5);
	
	//不足1包
	if(PackSum <= 0)
	{
		LoRaSendEncryptedData(LoRaPact.TxBuf,PackSumSize);			//LoRa发送加密数据
		
		//存储LoRa发送的内容
		RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
	}
	
	//至少为1包
	else
	{
		while( CountPackNum<PackSum+1 )
		{
			if(CountPackNum >= PackSum )
			{	
				size = PackSumSize - PackSum*LORA_PACK_SIZE;				//发送最后一包
				LoRaSendEncryptedData(LoRaPact.TxBuf,size);					//LoRa发送最后1包加密数据
				//存储LoRa发送的数据
				RestoreWhichLog(CommunicateLogFlag,CommunicateLogBuf,0,0,Log_Successful);
				break;
			}
			else
			{
				//正常发送
				LoRaSendEncryptedData(LoRaPact.TxBuf,LORA_PACK_SIZE);
				CountPackNum ++;
				TimerDelay_Ms(20);
			}
		}
	}
}


//将串口数据拆解成加密后的数据
void SplitComDataToEncryptedData(void)
{
	u8 	i = 0x00;
	u8 	RemainSize = 0x00;
	
	RemainSize = LoRaPact.TxCnt % LORA_PACK_SIZE;		//计算剩余数据大小
	if( LoRaPact.TxCnt>LORA_PACK_SIZE )
	{
		PackSum = LoRaPact.TxCnt / LORA_PACK_SIZE;		//得到总的包数
		
		for(i=0; i<PackSum; i++)
		{
			EncryptTheText(&hostcom_struct.Comx_RxBuf[i*224],LORA_PACK_SIZE);
			TimerDelay_Ms(50);
			GetEncryptedText( &EncryptedData[i*224],LORA_PACK_SIZE );
		}
		
		EncryptTheText(&hostcom_struct.Comx_RxBuf[PackSum*224],RemainSize);
		TimerDelay_Ms(50);
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( &EncryptedData[PackSum*224],AfterEncryptSzie );
		}
		PackSumSize = PackSum*LORA_PACK_SIZE + AfterEncryptSzie;	//总的数据长度
	}
	
	else
	{
		EncryptTheText(hostcom_struct.Comx_RxBuf,LoRaPact.TxCnt);
		TimerDelay_Ms(50);
		
		if( RemainSize%16 == 0 )
		{
			AfterEncryptSzie = RemainSize;
			GetEncryptedText( EncryptedData,RemainSize );
		}
		else
		{
			AfterEncryptSzie = ((RemainSize/16)+1)*16;
			GetEncryptedText( EncryptedData,((RemainSize/16)+1)*16 );
		}
		PackSumSize = AfterEncryptSzie;
	}
	
	//存储加密后的数据(待写)
	RestoreWhichLog(EncryptFlag,EncryptLogBuf,0,0,Log_Successful);
}


/******************************************
	* 功能描述：
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaGetIDCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x51;							//帧头
	//增加长度？
	data[1] = 26;							//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 'I';
	data[7] = 'D';
	
	memcpy(&data[8],SelfID_Buf,16);
	crc 	 = CalCrc16(data,24,0xffff);	//获取CRC校验值
	data[24] = crc>>8;						//低字节在前
	data[25] = crc;							//高字节在后
	
	SX1276TxDataCRC32(data,26);				//LoRa发送数据
}


/******************************************
	* 功能描述：控制继电器指令
	* 入口参数：data    -- 数据地址
				datalen -- 数据长度
	* 返回值：	无
	* 备注：	协议参数需要改变
*******************************************/
void LoRaCtrlRelayCmd(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x50;							//帧头
	data[1] = 0x09;							//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = 1;							//动作指令0/1	
	crc 	= CalCrc16(data,7,0xffff);		//获取CRC校验值
	data[7] = crc>>8;						//低字节在前
	data[8] = crc;							//高字节在后
	
	SX1276TxDataCRC32(data,16);				//LoRa发送数据
}


//在发送加密数据之前
void LoRaBeforeSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5E;							//帧头
	data[1] = 0x0b;							//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 	//LoRa目标设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;						//总的包数量
	data[7] = PackSumSize>>8;				//数据包长度
	data[8] = PackSumSize;
	
	crc 	 = CalCrc16(data,9,0xffff);		//获取CRC校验值
	data[9]  = crc>>8;						//低字节在前
	data[10] = crc;							//高字节在后
	
	SX1276TxDataCRC32(data,11);				//LoRa发送数据
}


void LoRaSendEncryptedData(u8 *data,u16 datalen)
{
	u16 crc = 0;
	
	data[0] = 0x5F;								//帧头
	data[1] = datalen+10;						//记录整个数据包的长度(所有字节数)
	
	data[2] = LoRaPact.DstDeviceAddr>>24; 		//LoRa设备地址
	data[3] = LoRaPact.DstDeviceAddr>>16;
	data[4] = LoRaPact.DstDeviceAddr>>8;
	data[5] = LoRaPact.DstDeviceAddr>>0;
	
	data[6] = PackSum;		//总的包数量
	data[7] = CountPackNum;	//此刻的包号
	
	memcpy(&data[8],&EncryptedData[CountPackNum*224],datalen);
	
	crc 	= CalCrc16(data,datalen+8,0xffff);	//获取CRC校验值
	data[datalen+8] = crc>>8;					//高字节在前
	data[datalen+9] = crc;						//低字节在后
	
	SX1276TxDataCRC32(data,datalen+10);			//LoRa发送数据
}






