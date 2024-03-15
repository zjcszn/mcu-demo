#include "hostcomprotocol.h"


//��������
extern void SX1276LoRaSetSpreadingFactor( u8 factor );
extern void SX1276LoRaSetSignalBandwidth( u8 bw );

extern tLoRaSettings LoRaSettings;
extern u8 SM4_KEY[16];
extern u8 SelfID_Buf[16];
extern _calendar_obj calendar;		
extern u8 PrintTimeFlag;

void HostComSetATCmdACK(u8 *buf,u8 datasize);
void HostComConfirmUserCmdInfoACK(u8 *buf,u8 datasize);
void HostComSetUserInfoCmdACK(u8 *buf,u8 datasize);
void HostComConfirmUserCmdInfoERRACK(u8 *buf,u8 datasize);
void HostComSetLoRaParaCmdACK(u8 *buf,u8 datasize);
void HostComInquireSm4KeyCmdACK(u8 *buf,u8 datasize);
void HostComInquireHSC32IDCmdACK(u8 *buf,u8 datasize);
void HostComSetSysTimeCmdACK(u8 *buf,u8 datasize);
void HostComInquireSysTimeCmdACK(u8 *buf,u8 datasize);
void HostComSetDestDeviceIDCmdACK(u8 *buf,u8 datasize);
void HostComSetDestDeviceIDCmdErrorACK(u8 *buf,u8 datasize);
void HostComExportOperateLogCmdACK(u8 *buf,u16 datasize);
void HostComExportSystemLogCmdACK(u8 *buf,u16 datasize);
void HostComExportEncryptLogCmdACK(u8 *buf,u16 datasize);
void HostComExportCommunicateLogCmdACK(u8 *buf,u16 datasize);
void HostComInquireUartParaCmdACK(u8 *buf,u16 datasize);
void HostComInquireLoRaParaCmdACK(u8 *buf,u16 datasize);
void HostComInquireDestIdCmdACK(u8 *buf,u16 datasize);
void HostComLogoutCmdACK(u8 *buf,u16 datasize);
void UserInfoInitPowerOnDevice(void);
u8 	JudgeDataIseFfective(u8 *buf);
void SaveUartParaToMemChip(void);
void HandleLogoutCmd(u8 NowLogType);

_ComWorkMode		com_work_mode = ComMode_AT;	//���ڵĹ���ģʽ
_HostComStructure 	hostcom_struct;				//������λ���ṹ��
_AT_TimerStruct		at_timer_struct;			//at����ģʽ��ʱ��

u16 SysYear = 0;
u8 	SysMonth = 0;
u8 	SysDate = 0;
u8 	SysHour = 0;
u8 	SysMin = 0;
u8 	SysSec = 0;
static u8  ExportLogEnable = DISABLE;
_LogTypes log_type = NoneLogFlag;


/****************************************
	* ��������������1�жϷ������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
*****************************************/
void USART1_IRQHandler(void)
{
	u16 Temp = 0x00;
	
	//�ж��Ƿ���Ŀ���֡����
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{
		Temp = USART1->SR;												//�ȶ�SR,Ȼ��DR�������
		Temp = USART1->DR;												//��USART_IT_IDLE��־

		DMA_Cmd(DMA1_Channel5, DISABLE);								//�ر�DMA���ܣ���ֹ�ڼ仹�����ݹ���
		Temp = HOST_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel5);	//������յ��ֽ���
		hostcom_struct.Comx_RxCnt = Temp;								//�����յ��ֽڳ��ȸ�ֵ
		hostcom_struct.Comx_RevCompleted = ENABLE;						//������ɱ�־λ	
	}	
}


/****************************************
	* ��������������1�жϷ������
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
*****************************************/
void USART3_IRQHandler(void)
{
	u16 Temp = 0x00;

	//�ж��Ƿ���Ŀ���֡����
	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET)  
	{
		Temp = USART3->SR;												//�ȶ�SR,Ȼ��DR�������
		Temp = USART3->DR;												//��USART_IT_IDLE��־
		
		DMA_Cmd(DMA1_Channel3, DISABLE);								//�ر�DMA���ܣ���ֹ�ڼ仹�����ݹ���
		Temp = HOST_RX_SIZE - DMA_GetCurrDataCounter(DMA1_Channel3);	//������յ��ֽ���
		hostcom_struct.Comx_RxCnt = Temp;								//�����յ��ֽڳ��ȸ�ֵ
		hostcom_struct.Comx_RevCompleted = ENABLE;						//������ɱ�־λ	
	}	
}


/***************************************************
	* ������������λ�����������ʼ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	ʹ��printfʱ���׵��³����������
				�������޸ģ�ʹ���ĸ�����Ϊδ֪����
****************************************************/
void HostComSoftWareInit(void)
{
	ConfirmUartx();				//��ȡ���ڱ��
	SetUartBound();				//���ô��ڲ�����
		
	SaveUartParaToMemChip();	//****������־(���ô��ڲ���)

	//�ж��Ƿ��������
	UserInfoInitPowerOnDevice();		
	
	hostcom_struct.host_status = None;
	hostcom_struct.Comx_RevCompleted = DISABLE;
	hostcom_struct.Comx_RxCnt = 0x00;
	hostcom_struct.Comx_TxCnt = 0x00;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		Usart_X_Init(COM1);
		//------------------------------------------------------------------
		DMA_RX_Init(COM1,hostcom_struct.Comx_RxBuf);	//DMA1����1��ʼ��
		DMA_TX_Init(COM1,hostcom_struct.Comx_TxBuf);	//DMA1����1��ʼ��
		
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8*)"UART IS READY!\r\n",strlen("UART IS READY!\r\n"));
		//****����ϵͳ��־(������λ��������)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,START_HOST_MANGE_CODE,0,Log_Successful);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		Usart_X_Init(COM3);
		//------------------------------------------------------------------
		DMA_RX_Init(COM3,hostcom_struct.Comx_RxBuf);	//DMA1����3��ʼ��
		DMA_TX_Init(COM3,hostcom_struct.Comx_TxBuf);	//DMA1����3��ʼ��
		
		RS485Uart1SendData(hostcom_struct.WhichUart,(u8*)"UART IS READY!\r\n",strlen("UART IS READY!\r\n"));
		
		//****����ϵͳ��־(������λ��������)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,START_HOST_MANGE_CODE,0,Log_Successful);
	}
}


//�豸�ϵ�ǰ�û���Ϣ��ʼ��
void UserInfoInitPowerOnDevice(void)
{
	u8 InfoBuf[16] = {0,};
	
	//��1��
	W25QXX_Read(InfoBuf,SYS_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.SysPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.SysPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	memset(InfoBuf,0,16);
	delay_ms(10);
	
	//��2��
	W25QXX_Read(InfoBuf,AUDIT_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.AuditPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.AuditPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	memset(InfoBuf,0,16);
	delay_ms(10);
	
	//��3��
	W25QXX_Read(InfoBuf,SEC_INFO_ADDR,12);
	if( InfoBuf[11] == SETED_USER_INFO)
		memcpy(hostcom_struct.SecPwd,InfoBuf,11);
	else
		memcpy(hostcom_struct.SecPwd,"Bon$on@2018", strlen("Bon$on@2018"));
	
	memcpy( hostcom_struct.AuditUser, "audit",strlen("audit") );
	memcpy( hostcom_struct.SysUser,   "sys",  strlen("sys")   );
	memcpy( hostcom_struct.SecUser,   "sec",  strlen("sec")   );
}


/**************************************************
	* �������������洮�����ò�����Ϣ���洢оƬ
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	
***************************************************/
void SaveUartParaToMemChip(void)
{
	switch(hostcom_struct.WhichUart)
	{
		//485�ӿ�
		case UART1:
			switch(hostcom_struct.Bound)
			{
				case 9600:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,7,0,Log_Successful);
				break;
				
				case 19200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,8,0,Log_Successful);
				break;
				
				case 38400:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,9,0,Log_Successful);
				break;
				
				case 115200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,10,0,Log_Successful);
				break;
				
				default:
				break;
			}
		break;
		
		//232�ӿ�
		case UART3:
			switch(hostcom_struct.Bound)
			{
				case 9600:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,11,0,Log_Successful);
				break;
				
				case 19200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,12,0,Log_Successful);
				break;
				
				case 38400:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,13,0,Log_Successful);
				break;
				
				case 115200:
					RestoreWhichLog(SystemLogFlag,SystemLogBuf,14,0,Log_Successful);
				break;
				
				default:
				break;
			}
		break;
		
		default:
		break;
	}
}


//�ж�������Ч��
u8 JudgeDataIseFfective(u8 *buf)
{
	u16 Temp= (u16)buf[0] <<8 | buf[1];
	
	//�ж��������Ƿ���Ч
	if(  ( (Temp< 2018)||(Temp>2100) ) || ( buf[0]==0xFF&&buf[1] == 0xFF ) )	
	{
		return False;
	}
	else
	{
		return True;
	}
}


//�ж�������Ч��
void JudgeTheHostComWorkMode()
{
	if( com_work_mode == ComMode_Trans )
	{
		NET_LED_ON();
	}
	else if( com_work_mode == ComMode_AT )
	{
		NET_LED_OFF();
	}
		
}



/*************************************************************
	* ������������λ��Э���������
	* ��ڲ�����HostStruct -- ��λ���ṹ���ַ
	* ����ֵ��	��
	* ��ע��
**************************************************************/
void HostComProtocol(_HostComStructure *HostStruct)
{
	u16 CalcCrc = 0x00;
	u16 tempCrc = 0x00;
	
	//����������ģʽ
	if(com_work_mode!=ComMode_AT)
		return;
	
	//���յ�����
	if( hostcom_struct.Comx_RevCompleted == ENABLE )
	{
		//������һ�ε�����
		HostStruct->Comx_RevCompleted = DISABLE;
		
		//�ж�CRCУ���Ƿ�ͨ��
		CalcCrc = CalCrc16(HostStruct->Comx_RxBuf,HostStruct->Comx_RxCnt-2,0xFFFF);	
		tempCrc = (HostStruct->Comx_RxBuf[HostStruct->Comx_RxCnt - 2]<<8) | 
				  (HostStruct->Comx_RxBuf[HostStruct->Comx_RxCnt - 1]);	
		
		if(tempCrc == CalcCrc)
		{
			//��������֡
			switch( HostStruct->Comx_RxBuf[0] )
			{
				//����ATָ��
				case 0x4c:
					//����ʱ��ʱ������
					if(	HostStruct->Comx_RxBuf[3] == 'A'&&
						HostStruct->Comx_RxBuf[4] == 'T')
					{
						at_timer_struct.SetAtTimer_5s = 0;
						HostComSetATCmdACK(HostStruct->Comx_TxBuf,0);	//ָ��Ӧ��
					}
				break;
				
				//��֤�û���������
				case 0x4b:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
									
					//ƥ���ض����ַ���
					if(	( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.AuditUser))&&
						  (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.AuditPwd)) ) )
					{
						HostStruct->host_status = User_Audit;					//�л����û�ģʽ
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//Ӧ��
						
						//****���������־(��֤audit�û�)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,AUDIT_CODE,Log_Successful);
					}
					else if( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SecUser))&&
							 (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.SecPwd)) )
					{
						HostStruct->host_status = User_Sec;						//�л����̼�����ģʽ
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//Ӧ��
						
						//****���������־(��֤sec�û�)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,SEC_CODE,Log_Successful);
					}
					else if( (strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SysUser))&&
							 (strstr(&HostStruct->Comx_RxBuf[8],hostcom_struct.SysPwd)) )
					{
						HostStruct->host_status = User_Sys;						//�л����̼�����ģʽ
						HostComConfirmUserCmdInfoACK(HostStruct->Comx_TxBuf,0);	//Ӧ��
						
						//****���������־(��֤sys�û�)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_CONFIRM_CODE,SYS_CODE,Log_Successful);
					}
					//��֤ʧ��Ӧ��
					else
						HostComConfirmUserCmdInfoERRACK(HostStruct->Comx_TxBuf,0);
				break;
				
				//�޸Ķ�Ӧ�û�������
				case 0x4a:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					
					//�����û�����						
					if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.AuditUser) )
					{
						memcpy(HostStruct->AuditPwd, &HostStruct->Comx_RxBuf[9],11);
						
						//���氲ȫԱ��¼����
						RestoreUserPassword(hostcom_struct.AuditPwd,11,AUDIT_INFO_ADDR);
						
						//****���������־(����audit����)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,AUDIT_CODE,Log_Successful);
					}
					else if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SecUser) )
					{
						memcpy(HostStruct->SecPwd,  &HostStruct->Comx_RxBuf[9],11);
						
						//�������Ա��¼����
						RestoreUserPassword(hostcom_struct.SecPwd,11,SEC_INFO_ADDR);
						
						//****���������־(����sec����)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,SEC_CODE,Log_Successful);
					}	
					else if( strstr(&HostStruct->Comx_RxBuf[3],hostcom_struct.SysUser) )
					{
						memcpy(HostStruct->SysPwd,  &HostStruct->Comx_RxBuf[9],11);
						
						//����ϵͳԱ��¼����
						RestoreUserPassword(hostcom_struct.SysPwd,11,SYS_INFO_ADDR);
						
						//****���������־(����sys����)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,CHANGE_PWD_CODE,SYS_CODE,Log_Successful);
					}
					HostComSetUserInfoCmdACK(HostStruct->Comx_TxBuf,0);
				break;
		
				//����ϵͳʱ��
				case 0x49:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						SysYear  = HostStruct->Comx_RxBuf[3]<<8|	//��
								   HostStruct->Comx_RxBuf[4];	
						SysMonth = HostStruct->Comx_RxBuf[5];		//��		
						SysDate  = HostStruct->Comx_RxBuf[6];		//��
						SysHour  = HostStruct->Comx_RxBuf[7];		//Сʱ
						SysMin   = HostStruct->Comx_RxBuf[8];		//����
						SysSec   = HostStruct->Comx_RxBuf[9];		//��
						
						//��ӻ�����ʱ��ͬ��
						if(LoRaDeviceIs == MASTER)
							LoRaTimeSynchronizationCmd(LoRaPact.TxBuf,0);
						
						RTC_Set(SysYear,SysMonth,SysDate,SysHour,SysMin,SysSec);
						HostComSetSysTimeCmdACK(HostStruct->Comx_TxBuf,0);
						
							
						
						//****���������־(����ϵͳʱ��)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_SYS_TIME_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//��ȡϵͳʱ��
				case 0x48:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						HostComInquireSysTimeCmdACK(HostStruct->Comx_TxBuf,0);
						
						//****���������־(��ȡϵͳʱ��)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SYS_TIME_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//����LoRa����(�źŴ�����Ƶ����)
				case 0x47:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						SX1276LoRaSetSignalBandwidth(HostStruct->Comx_RxBuf[3]);	//�����źŴ���
						SX1276LoRaSetSpreadingFactor(HostStruct->Comx_RxBuf[4]);	//������Ƶ����
						HostComSetLoRaParaCmdACK(HostStruct->Comx_TxBuf,0);			//Ӧ��
						
						//****���������־(����LoRa����)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_LORA_PARA_CODE,SEC_CODE,Log_Successful);
					}
				break;
				
				//��ȡSM4��Կ
				case 0x46:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3]=='S')&&
							(HostStruct->Comx_RxBuf[4]=='M')&&
							(HostStruct->Comx_RxBuf[5]=='4'))
						{
							HostComInquireSm4KeyCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****���������־(��ȡSM4��Կ)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SM4_KEY_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
				
				//��ȡоƬID(����оƬ�����)
				case 0x45:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3]=='I')&&
							(HostStruct->Comx_RxBuf[4]=='D')  )
						{
							HostComInquireHSC32IDCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****���������־(��ȡ�����)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_RANGDOM_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
				//��ȡ������־
				case 0x44:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'O')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = OperateLogFlag;
							}
							
						}
					}
				break;
					
				//��ȡϵͳ��־
				case 0x43:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'S')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = SystemLogFlag;
							}
							
						}
					}
				break;
				
				//��ȡͨ����־
				case 0x42:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'C')&&
							(HostStruct->Comx_RxBuf[4] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = CommunicateLogFlag;
							}
						}
					}
				break;
					
				//��ȡ������־
				case 0x41:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'E')&&
							(HostStruct->Comx_RxBuf[4] == 'D')&&
							(HostStruct->Comx_RxBuf[5] == 'L') )
						{
							if(ExportLogEnable == DISABLE)
							{
								ExportLogEnable = ENABLE;
								log_type = EncryptFlag;
							}
						}
					}
				break;
					
				//����Ŀ���豸��ID
				case 0x40:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						//�ж��豸��ַ�Ƿ�Խ��
						if( HostStruct->Comx_RxBuf[3]<DEST_DEVICE_MAX_ADDR )
						{
							LoRaPact.DstDeviceAddr = HostStruct->Comx_RxBuf[3];
							HostComSetDestDeviceIDCmdACK(HostStruct->Comx_TxBuf,0);
							
							//���Ϊ����������ԿЭ��
							if(LoRaDeviceIs == MASTER)
								GetSlaveID();
							
							//****���������־(����Ŀ���豸��ID��ַ)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,SET_DEST_ID_CODE,SEC_CODE,Log_Successful);
						}
						else
							HostComSetDestDeviceIDCmdErrorACK(HostStruct->Comx_TxBuf,0);
					}
				break;
				
				//�鿴��������
				case 0x3F:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(HostStruct->host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'U')&&
							(HostStruct->Comx_RxBuf[4] == 'T') )
						{
							HostComInquireUartParaCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****���������־(��ȡ���ڲ���)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_UART_PARA_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
				
				//�ָ���������
				case 0x3E:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if( (HostStruct->Comx_RxBuf[3] == 'R')&&
						(HostStruct->Comx_RxBuf[4] == 'E')&&
						(HostStruct->Comx_RxBuf[5] == 'S')&&
						(HostStruct->Comx_RxBuf[6] == 'T') )
					{
						//�����˺���Ϣ
						memcpy(hostcom_struct.SysUser,SYS_USER,4);
						memcpy(hostcom_struct.SysPwd,PASSWORD,12);	
						memcpy(hostcom_struct.AuditUser,AUDIT_USER,6);
						memcpy(hostcom_struct.AuditPwd,PASSWORD,12);
						memcpy(hostcom_struct.SecUser,SEC_USER,4);
						memcpy(hostcom_struct.SecPwd,PASSWORD,12);
						
						//****���������־(�ָ���������)
						RestoreWhichLog(OperateLogFlag,OperateLogBuf,USER_REST_CODE,SEC_CODE,Log_Successful);
						
						//****����ϵͳ��־(�ָ���������)
						RestoreWhichLog(SystemLogFlag,SystemLogBuf,DEVICE_RESET_CODE,0,Log_Successful);
					}
				break;
				
				//��ѯLoRa����
				case 0X3D:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'L')&&
							(HostStruct->Comx_RxBuf[4] == 'o')&&
							(HostStruct->Comx_RxBuf[5] == 'R')&&
							(HostStruct->Comx_RxBuf[6] == 'a') )
						{
							HostComInquireLoRaParaCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****���������־(��ȡLoRa����)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_LORA_PARA_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
					
				//��ѯĿ���豸ID
				case 0X3C:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Sec)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'D')&&
							(HostStruct->Comx_RxBuf[4] == 'E')&&
							(HostStruct->Comx_RxBuf[5] == 'I')&&
							(HostStruct->Comx_RxBuf[6] == 'D') )
						{
							HostComInquireDestIdCmdACK(HostStruct->Comx_TxBuf,0);
							
							//****���������־(��ȡĿ���豸��ID)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_DEST_ID_CODE,SEC_CODE,Log_Successful);
						}
					}
				break;
					
					
				//��ֹ��ȡ��־����
				case 0X3B:
					//����ʱ��ʱ������
					at_timer_struct.SetAtTimer_5s = 0;
					if(hostcom_struct.host_status == User_Audit)
					{
						if( (HostStruct->Comx_RxBuf[3] == 'L')&&
							(HostStruct->Comx_RxBuf[4] == 'g')&&
							(HostStruct->Comx_RxBuf[5] == 'o')  )
						{
							//ֹͣ��־����ָ���
							HandleLogoutCmd(log_type);
							
							//****���������־(��ֹ��ȡ��־)
							RestoreWhichLog(OperateLogFlag,OperateLogBuf,STOP_READ_LOG_CODE,AUDIT_CODE,Log_Successful);
						}
					}
				break;
					
				//�̼�����
				//������־
				
				//����
				default:
				break;
			}
		}
		
		//--------------------------------------------------------------------------------------------------------------------
		HostStruct->Comx_RxCnt = 0;							//�����ռ���������
		memset(HostStruct->Comx_RxBuf,0,HOST_RX_SIZE);		//�����ջ��������
		memset(HostStruct->Comx_TxBuf,0,HOST_RX_SIZE);		//�����ջ��������
		//�жϴ��ڱ��
		if(hostcom_struct.WhichUart == UART1)		
		{
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel5,HOST_RX_SIZE);	
			//��DMA����			
			DMA_Cmd(DMA1_Channel5,ENABLE);  					
		}
		else if(hostcom_struct.WhichUart == UART3)
		{
			//�������ݽ��ճ��ȣ�����װ��������ݽ��յ�ַƫ�Ƶ�ַ��0��ʼ
			DMA_SetCurrDataCounter(DMA1_Channel3,HOST_RX_SIZE);	
			//��DMA����			
			DMA_Cmd(DMA1_Channel3,ENABLE);  					
		}
	}
}


/********************************************
	* ����������������־����
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��
*********************************************/
void ExportLogHandle(void)
{
	//�жϵ�����־��־λ
	if(ExportLogEnable != ENABLE)
		return;
	
	//�ȴ�1s��ʱ����
	if( GetExportLogTimerValue() < 1000 )
		return;
	
	//����ʱ������
	at_timer_struct.SetAtTimer_5s = 0x00;
	
	//ƥ�䵼����־����
	switch(log_type)
	{
		//����������־
		case OperateLogFlag:
			//�ж�ƫ�Ƶ�ַ�Ƿ�Խ��
			if( (OPERATE_LOG_BASE_ADDR+OperateReadAddr) < OPERATE_LOG_SZIE )
			{
				ExportWhichLog(OperateLogFlag,OperateLogBuf);
				
				//�����ж������Ƿ���Ч
				if( JudgeDataIseFfective(OperateLogBuf) )
					HostComExportOperateLogCmdACK(hostcom_struct.Comx_TxBuf,0x0B);
				
				//���������Ч��������־����
				else
				{
					ExportLogEnable = DISABLE;					//��������
					OperateReadAddr = 0x00;						//��ƫ�Ƶ�ַ����
					memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);	//����־����������
					
					//****���������־(�鿴��־)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_OL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//��������
				OperateReadAddr = 0x00;							//��ƫ�Ƶ�ַ����
				memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);		//����־����������
					
				//****���������־(�鿴��־)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_OL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//ϵͳ��־
		case SystemLogFlag:
			//�ж�ƫ�Ƶ�ַ�Ƿ�Խ��
			if( (SYSTEM_LOG_BASE_ADDR+SystemReadAddr) < SYSTEM_LOG_SZIE )
			{
				ExportWhichLog(SystemLogFlag,SystemLogBuf);
				
				//�����ж������Ƿ���Ч
				if( JudgeDataIseFfective(SystemLogBuf) )
					HostComExportSystemLogCmdACK(hostcom_struct.Comx_TxBuf,0x0A);
				
				//���������Ч��������־����
				else
				{
					ExportLogEnable = DISABLE;					//��������
					SystemReadAddr  = 0x00;						//��ƫ�Ƶ�ַ����
					memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);	//����־����������
					
					//****���������־(��ȡϵͳ��־)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//��������
				SystemReadAddr = 0x00;							//��ƫ�Ƶ�ַ����
				memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);		//����־����������
					
				//****1���������־(��ȡϵͳ��־)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_SL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//ͨ����־
		case CommunicateLogFlag:
			//�ж�ƫ�Ƶ�ַ�Ƿ�Խ��
			if( (COMMUNICATE_LOG_BASE_ADDR+CommunicateReadAddr) < COMMUNICATE_LOG_SIZE )
			{
				ExportWhichLog(CommunicateLogFlag,CommunicateLogBuf);
				
				//�����ж������Ƿ���Ч
				if( JudgeDataIseFfective(CommunicateLogBuf) )
					HostComExportCommunicateLogCmdACK(hostcom_struct.Comx_TxBuf,MAX_LOG_PACK_SIZE);
				
				//���������Ч��������־����
				else
				{
					ExportLogEnable = DISABLE;						//��������
					CommunicateReadAddr = 0x00;						//��ƫ�Ƶ�ַ����
					memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
					
					//****1���������־(��ȡͨ����־)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_CL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;						//��������
				CommunicateReadAddr = 0x00;						//��ƫ�Ƶ�ַ����
				memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
					
				//****1���������־(�鿴��־)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_CL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//������־
		case EncryptFlag:
			//�ж�ƫ�Ƶ�ַ�Ƿ�Խ��
			if( (ENCRYPT_LOG_BASE_ADDR+EncryptReadAddr) < ENCRYPT_LOG_SZIE )
			{
				ExportWhichLog(EncryptFlag,EncryptLogBuf);
				
				//�����ж������Ƿ���Ч
				if( JudgeDataIseFfective(EncryptLogBuf) )
					HostComExportEncryptLogCmdACK(hostcom_struct.Comx_TxBuf,MAX_LOG_PACK_SIZE);
				
				//���������Ч��������־����
				else
				{
					ExportLogEnable = DISABLE;					//��������
					EncryptReadAddr = 0x00;						//��ƫ�Ƶ�ַ����
					memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
					
					//****���������־(�鿴������־)
					RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_EL_CODE,AUDIT_CODE,Log_Successful);
				}
			}
			else
			{
				ExportLogEnable = DISABLE;					//��������
				EncryptReadAddr = 0x00;						//��ƫ�Ƶ�ַ����
				memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
					
				//****���������־(�鿴������־)
				RestoreWhichLog(OperateLogFlag,OperateLogBuf,READ_EL_CODE,AUDIT_CODE,Log_Successful);
			}
		break;
		
		//����
		default:	
		break;
	}
	ClearExportLogTimer();
}


//���д���ATģʽ��ʱ��ʱ��
void RunComAtModeTimer(void)
{
	if( at_timer_struct.SetAtTimer_5s<45000 )
	{
		at_timer_struct.SetAtTimer_5s ++;
	}
}


/************************************************
	* ������������λ�����ڳ�ʱ����
	* ��ڲ�������
	* ����ֵ��
	* ��ע��
*************************************************/
void HostComTimeOutHandle(void)
{
	//��ʱ�ȴ����������˳�AT���ù���ģʽ
	if(at_timer_struct.SetAtTimer_5s > WAIT_HOST_MAX_TIME)
	{
		com_work_mode = ComMode_Trans;
		return;
	}
}


//��������ģʽ
void HostComSetATCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4C;						//������
	
	buf[1] = 0x07>>8;					//���ݰ�����
	buf[2] = 0x07;
	
	buf[3] = 'O';						//��Ӧ����
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);	//CRCУ��ֵ
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//ȷ���û���Ϣ(�û���������)
void HostComConfirmUserCmdInfoACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4B;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//ȷ���û���Ϣ(�û���������)
void HostComConfirmUserCmdInfoERRACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4B;
	
	buf[1] = 0x08>>8;
	buf[2] = 0x08;
	
	buf[3] = 'E';
	buf[4] = 'R';
	buf[5] = 'R';
	
	crc_value = CalCrc16(buf,6,0xFFFF);
	buf[6] = crc_value>>8;
	buf[7] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x08);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x08);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//�����û���Ϣ(�û���������)
void HostComSetUserInfoCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x4A;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����LoRaͨ�Ų���
void HostComSetLoRaParaCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x47;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//��ȡSM4��Կ
void HostComInquireSm4KeyCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x46;
	
	buf[1] = 0x15>>8;
	buf[2] = 0x15;
	
	memcpy(&buf[3],SM4_KEY,16);
	
	crc_value = CalCrc16(buf,3+16,0xFFFF);
	buf[19] = crc_value>>8;
	buf[20] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x15);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x15);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//��ȡ����оƬ��ID��ʶ��
void HostComInquireHSC32IDCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x45;
	
	buf[1] = 0x15>>8;
	buf[2] = 0x15;
	
	memcpy(&buf[3],SelfID_Buf,16);
	
	crc_value = CalCrc16(buf,3+16,0xFFFF);
	buf[19] = crc_value>>8;
	buf[20] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x15);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x15);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����ϵͳʱ��Ӧ��
void HostComSetSysTimeCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x49;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����ϵͳʱ��Ӧ��
void HostComInquireSysTimeCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x48;
	
	buf[1] = 0x0C>>8;
	buf[2] = 0x0C;
	
	buf[3] = calendar.w_year>>8;
	buf[4] = calendar.w_year;
	buf[5] = calendar.w_month;
	buf[6] = calendar.w_date;
	buf[7] = calendar.hour;
	buf[8] = calendar.min;
	buf[9] = calendar.sec;

	crc_value = CalCrc16(buf,3+7,0xFFFF);
	buf[10] = crc_value>>8;
	buf[11] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x0C);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x0C);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}



//����Ŀ���豸�ĵ�ַ�ɹ�Ӧ��
void HostComSetDestDeviceIDCmdACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x40;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����Ŀ���豸�ĵ�ַ����Ӧ��
void HostComSetDestDeviceIDCmdErrorACK(u8 *buf,u8 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x40;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	
	buf[3] = 'E';
	buf[4] = 'R';
	
	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;

	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,0x07);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,0x07);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����������־Ӧ��(0X0B)
void HostComExportOperateLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x44;
	
	buf[1] = (datasize+5) >> 8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],OperateLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����ϵͳ��־Ӧ��
void HostComExportSystemLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x43;
	
	buf[1] = (datasize+5)>>8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],SystemLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����ͨ����־Ӧ��
void HostComExportCommunicateLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x42;
	
	buf[1] = (datasize+5)>>8;	//���ֽ�
	buf[2] = (datasize+5);		//���ֽ�
	
	memcpy(&buf[3],CommunicateLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//����������־Ӧ��
void HostComExportEncryptLogCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x41;
	
	buf[1] = (datasize+5)>>8;
	buf[2] = (datasize+5);
	
	memcpy(&buf[3],EncryptLogBuf,datasize);
	
	crc_value = CalCrc16(buf,datasize+3,0xFFFF);
	buf[datasize+3] = crc_value>>8;
	buf[datasize+4] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,datasize+5);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,datasize+5);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//��ѯ�������ò���Ӧ��
void HostComInquireUartParaCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3F;
	
	buf[1] = 0x08>>8;
	buf[2] = 0x08;
	
	buf[3] = ID1;					//�ӿ�����
	buf[4] = LoRaPact.DeviceAddr;	//�豸��ַ
	buf[5] = ID2 | ID3<<1;			//������
	
 	crc_value = CalCrc16(buf,6,0xFFFF);
	buf[6] = crc_value>>8;
	buf[7] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,8);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,8);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//��ѯLoRa���ò���Ӧ��
void HostComInquireLoRaParaCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3D;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = LoRaSettings.SignalBw;			//�źŴ���
	buf[4] = LoRaSettings.SpreadingFactor;	//��Ƶ����
	
 	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,7);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,7);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//��ѯĿ���豸IDӦ��
void HostComInquireDestIdCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3C;
	
	buf[1] = 0x06>>8;
	buf[2] = 0x06;
	
	buf[3] = LoRaPact.DstDeviceAddr;	//�ӿ�����

 	crc_value = CalCrc16(buf,4,0xFFFF);
	buf[4] = crc_value>>8;
	buf[5] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,6);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,6);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


//ע��ָ��Ӧ��
void HostComLogoutCmdACK(u8 *buf,u16 datasize)
{
	u16 crc_value = 0x00;
	
	buf[0] = 0x3B;
	
	buf[1] = 0x07>>8;
	buf[2] = 0x07;
	
	buf[3] = 'O';
	buf[4] = 'K';
	
 	crc_value = CalCrc16(buf,5,0xFFFF);
	buf[5] = crc_value>>8;
	buf[6] = crc_value;
	
	if(hostcom_struct.WhichUart == UART1)
	{
		//����1DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel4,7);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	else if(hostcom_struct.WhichUart == UART3)
	{
		//����3DMA1����
		DMA_SetCurrDataCounter(DMA1_Channel2,7);
		DMA_Cmd(DMA1_Channel2, ENABLE);
	}
}


/***************************************************
	* ����������ע��ָ���
	* ��ڲ�����NowLogType -- ���ڵ�����־����
	* ����ֵ��	��
	* ��ע��	��
****************************************************/
void HandleLogoutCmd(u8 NowLogType)
{
	//����������־
	ExportLogEnable = DISABLE;
	
	switch(NowLogType)
	{
		//��ǰ����������־
		case OperateLogFlag:
			OperateReadAddr = 0x00;
			memset(OperateLogBuf,0,MIN_LOG_PACK_SIZE);	//����־����������
		break;
		
		//��ǰ����ϵͳ��־
		case SystemLogFlag:
			SystemReadAddr = 0x00;
			memset(SystemLogBuf,0,MIN_LOG_PACK_SIZE);	//����־����������
		break;		
		
		//��ǰ����ͨ����־
		case CommunicateLogFlag:
			CommunicateReadAddr = 0x00;
			memset(CommunicateLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
		break;
		
		//��ǰ����������־
		case EncryptFlag:
			EncryptReadAddr = 0x00;
			memset(EncryptLogBuf,0,MAX_LOG_PACK_SIZE);	//����־����������
		break;
		
		default:
		break;
	}
	HostComLogoutCmdACK(hostcom_struct.Comx_TxBuf,0);
}



