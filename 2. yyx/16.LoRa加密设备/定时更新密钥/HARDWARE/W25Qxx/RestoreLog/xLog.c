#include "xLog.h"

void 	MixOpreateLogData(u8 *_OperateLogBuf,u16 _RestoreCode,u8 AdminCode,u8 _result);
void 	MixSystemLogData(u8 *_SystemLogBuf,u16 _RestoreCode,u8 _result);
void 	MixCommunicateLogData(u8 *_CommunicateLogBuf,u8 _result);
void 	MixEncryptLogData(u8 *_EncryptLogBuf,u8 _result);

/****************************************************************************************
										������
*****************************************************************************************/
	extern tLoRaSettings LoRaSettings;
	//-------------------------------------------------------------
	u8 	OperateLogBuf[16] = {0,};			//������־������
	u8 	SystemLogBuf[16] = {0,};			//ϵͳ��־������
	u8 	CommunicateLogBuf[1100] = {0,};		//ͨ����־������
	u8  EncryptLogBuf[1100] = {0,};			//������־������
	//-------------------------------------------------------------
	u32 OperateWriteAddr = 0x00;			//д�������־ƫ�Ƶ�ַ
	u32	OperateReadAddr = 0x00;				//��ȡ������־ƫ�Ƶ�ַ
	u32 SystemWriteAddr = 0x00;				//д��ϵͳ��־ƫ�Ƶ�ַ
	u32	SystemReadAddr = 0x00;				//��ȡϵͳ��־ƫ�Ƶ�ַ
	u32 CommunicateWriteAddr = 0x00;		//д��ͨ����־ƫ�Ƶ�ַ
	u32	CommunicateReadAddr = 0x00;			//��ȡͨ����־ƫ�Ƶ�ַ
	u32 EncryptWriteAddr = 0x00;			//д�������־ƫ�Ƶ�ַ
	u32	EncryptReadAddr = 0x00;				//��ȡ������־ƫ�Ƶ�ַ
/****************************************************************************************/

	
/*********************************************************
	* �����������洢��־
	* ��ڲ�����LogType		--	��־����
				SourceBuf	--	Դ���ݻ�������ַ
				RestoreCode	--	�洢���ʹ���
				AdminCode	-- 	����Ա����
				Result		-- 	���
	* ����ֵ��	��
	* ��ע��	
**********************************************************/
void RestoreWhichLog(u8 LogType,u8 *SourceBuf,u16 RestoreCode,u8 AdminCode,u8 Result )
{
	switch(LogType)
	{
		//---->�洢������־(����Ϊ������Ա)
		case OperateLogFlag:
		//�ж��Ƿ񳬹��洢��ַ
		if( (OPERATE_LOG_BASE_ADDR+OperateWriteAddr) < OPERATE_LOG_SZIE )
		{
			//�ϳɲ�����־
			MixOpreateLogData(SourceBuf,RestoreCode,AdminCode,Result);
			//��������־д���оƬ
			W25QXX_Write(SourceBuf,OPERATE_LOG_BASE_ADDR+OperateWriteAddr,MIN_LOG_PACK_SIZE);
			//��ַƫ��
			OperateWriteAddr += MIN_LOG_PACK_SIZE;
			memset(SourceBuf,0,MIN_LOG_PACK_SIZE);		

			//���������־д���ַ
			RestoreWriteAddr(LogWriteAddr_Operate);
		}
		else
			//��ƫ�Ƶ�ַ����
			OperateWriteAddr = 0x00;				
		break;
		
		//---->�洢ϵͳ��־(����Ϊ�豸)
		case SystemLogFlag:
			//�ж��Ƿ񳬹��洢��ַ
			if( (SYSTEM_LOG_BASE_ADDR+SystemWriteAddr) < SYSTEM_LOG_SZIE )
			{
				//�ϳ�ϵͳ��־
				MixSystemLogData(SourceBuf,RestoreCode,Result);
				//��ϵͳ��־д��洢оƬ
				W25QXX_Write(SourceBuf,SYSTEM_LOG_BASE_ADDR+SystemWriteAddr,MIN_LOG_PACK_SIZE);
				//��ַƫ��
				SystemWriteAddr += MIN_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MIN_LOG_PACK_SIZE);

				//�洢ϵͳ��־д���ַ
				RestoreWriteAddr(LogWriteAddr_System);
			}
			else
				//��ƫ�Ƶ�ַ����
				SystemWriteAddr = 0x00;
		break;
		
		//---->�洢ͨ����־
		case CommunicateLogFlag:
			//�ж��Ƿ񳬹��洢��ַ
			if( (COMMUNICATE_LOG_BASE_ADDR+CommunicateWriteAddr) < COMMUNICATE_LOG_SIZE )
			{
				//�ϳ�ͨ����־
				MixCommunicateLogData(SourceBuf,Result);
				//��ͨ����־д��洢оƬ
				W25QXX_Write(SourceBuf,COMMUNICATE_LOG_BASE_ADDR+CommunicateWriteAddr,MAX_LOG_PACK_SIZE);
				//��ַƫ��
				CommunicateWriteAddr += MAX_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MAX_LOG_PACK_SIZE);

				//����ͨ����־д���ַ
				RestoreWriteAddr(LogWriteAddr_Communicate);
			}
			else
				//��ƫ�Ƶ�ַ����
				CommunicateWriteAddr = 0x00;				
		break;
		
		//---->�洢������־
		case EncryptFlag:
			//�ж��Ƿ񳬹��洢��ַ
			if( (ENCRYPT_LOG_BASE_ADDR+EncryptWriteAddr) < ENCRYPT_LOG_SZIE )
			{
				//�ϳɼ�����־
				MixEncryptLogData(SourceBuf,Result);	
				//��������־д��洢оƬ
				W25QXX_Write(SourceBuf,ENCRYPT_LOG_BASE_ADDR+EncryptWriteAddr,MAX_LOG_PACK_SIZE);
				//��ַƫ��
				EncryptWriteAddr += MAX_LOG_PACK_SIZE;	
				memset(SourceBuf,0,MAX_LOG_PACK_SIZE);

				//���������־д���ַ
				RestoreWriteAddr(LogWriteAddr_Encrypt);	
			}
			else
				//��ƫ�Ƶ�ַ����
				EncryptWriteAddr = 0x00;	
		break;
		
		default:
		break;
	}
}


/*********************************************************
	* ����������������־
	* ��ڲ�����LogType -- ��־����
				DestBuf -- ���������ݻ�������ַ
	* ����ֵ��	��
	* ��ע��	
**********************************************************/
void ExportWhichLog(u8 LogType,u8 *DestBuf)
{
	switch(LogType)
	{
		//---->������־
		case OperateLogFlag:
			//��ȡW25Qxx����
			W25QXX_Read(DestBuf,OPERATE_LOG_BASE_ADDR+OperateReadAddr,MIN_LOG_PACK_SIZE);
			//ƫ�Ƶ�ַ		
			OperateReadAddr += MIN_LOG_PACK_SIZE; 											
		break;
		
		//---->ϵͳ��־
		case SystemLogFlag:
			//��ȡW25Qxx����
			W25QXX_Read(DestBuf,SYSTEM_LOG_BASE_ADDR+SystemReadAddr,MIN_LOG_PACK_SIZE);	
			//ƫ�Ƶ�ַ
			SystemReadAddr += MIN_LOG_PACK_SIZE; 										
		break;
		
		//---->ͨ����־
		case CommunicateLogFlag:
			//��ȡW25Qxx����
			W25QXX_Read(DestBuf,COMMUNICATE_LOG_BASE_ADDR+CommunicateReadAddr,MAX_LOG_PACK_SIZE);	
			//��ַƫ��
			CommunicateReadAddr += MAX_LOG_PACK_SIZE;
		break;
		
		//---->������־
		case EncryptFlag:
			//��ȡW25Qxx����
			W25QXX_Read(DestBuf,ENCRYPT_LOG_BASE_ADDR+EncryptReadAddr,MAX_LOG_PACK_SIZE);	
			//��ַƫ��
			EncryptReadAddr += MAX_LOG_PACK_SIZE;
		break;
		
		default:
			
		break;
	}
}


/**********************************************************
	* �����������ϳɲ�����־����
	* ��ڲ�����OperateLogBuf 	-- �洢������־������
				_result 		-- �������
	* ����ֵ��	��
	* ��ע��	
***********************************************************/
void MixOpreateLogData(u8 *_OperateLogBuf,u16 _RestoreCode,u8 AdminCode,u8 _result)
{
	u16 my_year = 0x00;
	
	my_year = calendar.w_year;

	_OperateLogBuf[0]  = my_year>>8;		//ʱ��
	_OperateLogBuf[1]  = my_year;
	_OperateLogBuf[2]  = calendar.w_month;
	_OperateLogBuf[3]  = calendar.w_date;
	_OperateLogBuf[4]  = calendar.hour;
	_OperateLogBuf[5]  = calendar.min;
	_OperateLogBuf[6]  = calendar.sec;
	_OperateLogBuf[7]  = AdminCode;			//����Ա����
	_OperateLogBuf[8]  = _RestoreCode >>8;	//�������8λ
	_OperateLogBuf[9]  = _RestoreCode;		//�������8λ
	_OperateLogBuf[10] = _result;			//�������
}


/**********************************************************
	* �����������ϳ�ϵͳ��־����
	* ��ڲ�����OperateLogBuf 	-- �洢ϵͳ����־������
				_result 		-- �������
	* ����ֵ��	��
	* ��ע��	
***********************************************************/
void MixSystemLogData(u8 *_SystemLogBuf,u16 _RestoreCode,u8 _result)
{
	_SystemLogBuf[0]  = calendar.w_year>>8;
	_SystemLogBuf[1]  = calendar.w_year;
	_SystemLogBuf[2]  = calendar.w_month;
	_SystemLogBuf[3]  = calendar.w_date;
	_SystemLogBuf[4]  = calendar.hour;
	_SystemLogBuf[5]  = calendar.min;
	_SystemLogBuf[6]  = calendar.sec;
	_SystemLogBuf[7]  = _RestoreCode>>8;
	_SystemLogBuf[8]  = _RestoreCode; 
	_SystemLogBuf[9]  = _result;
}


/**********************************************************
	* �����������ϳ�ͨ����־����
	* ��ڲ�����OperateLogBuf 	-- �洢ͨ����־������
				_result 		-- �������
	* ����ֵ��	��
	* ��ע��	
***********************************************************/
void MixCommunicateLogData(u8 *_CommunicateLogBuf,u8 _result)
{
	_CommunicateLogBuf[0]  = calendar.w_year>>8;				//ʱ��
	_CommunicateLogBuf[1]  = calendar.w_year;
	_CommunicateLogBuf[2]  = calendar.w_month;
	_CommunicateLogBuf[3]  = calendar.w_date;
	_CommunicateLogBuf[4]  = calendar.hour;
	_CommunicateLogBuf[5]  = calendar.min;
	_CommunicateLogBuf[6]  = calendar.sec;
	
	_CommunicateLogBuf[7]  = PackSumSize>>8;					//���ݰ�����
	_CommunicateLogBuf[8]  = PackSumSize;
	
	memcpy(&_CommunicateLogBuf[9],EncryptedData,PackSumSize);	//ͨ������
	
	_CommunicateLogBuf[PackSumSize+9]  = _result;				//ͨ�Ž��
}


/**********************************************************
	* �����������ϳɼ�����־����
	* ��ڲ�����OperateLogBuf 	-- �洢������־������
				_result 		-- �������
	* ����ֵ��	��
	* ��ע��	
***********************************************************/
void MixEncryptLogData(u8 *_EncryptLogBuf,u8 _result)
{
	u16 _year = 0x00;
	
	_year = calendar.w_year;								//ʱ��
	_EncryptLogBuf[0]  = _year>>8;
	_EncryptLogBuf[1]  = _year;
	_EncryptLogBuf[2]  = calendar.w_month;
	_EncryptLogBuf[3]  = calendar.w_date;
	_EncryptLogBuf[4]  = calendar.hour;
	_EncryptLogBuf[5]  = calendar.min;
	_EncryptLogBuf[6]  = calendar.sec;
	
	_EncryptLogBuf[7] = PackSumSize>>8;						//���ݰ�����			
	_EncryptLogBuf[8] = PackSumSize;
	
	_EncryptLogBuf[9]  = ENCRYPT_DATA;						//���ܱ�־
	
	memcpy(&_EncryptLogBuf[10],EncryptedData,PackSumSize);	//��������
	
	_EncryptLogBuf[PackSumSize+10]  = _result;				//���ܽ��
	
}

/**************************************************************
	* ���������������û�����
	* ��ڲ�����
***************************************************************/
void RestoreUserPassword(u8 *PwdBuf, u8 PwdSize,u32 where_addr)
{
	u8 temp_buf[12] = {0,};
	
	memcpy(temp_buf,PwdBuf,PwdSize);
	temp_buf[11] = SETED_USER_INFO;
	W25QXX_Write(temp_buf,where_addr,12);
}


/**************************************************
	* ������������̬�洢д����־��ַ
	* ��ڲ�����д����־��ַ����
	* ����ֵ��	��
	* ��ע��	��
***************************************************/
void RestoreWriteAddr(u8 WhichWriteAddr)
{
	switch(WhichWriteAddr)
	{
		case LogWriteAddr_Operate:
			BKP_WriteBackupRegister(BKP_DR3,OperateWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR4,OperateWriteAddr);
		break;
		
		case LogWriteAddr_System:
			BKP_WriteBackupRegister(BKP_DR5,SystemWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR6,SystemWriteAddr);
		break;
		
		case LogWriteAddr_Communicate:
			BKP_WriteBackupRegister(BKP_DR7,CommunicateWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR8,CommunicateWriteAddr);
		break;
		
		case LogWriteAddr_Encrypt:
			BKP_WriteBackupRegister(BKP_DR9, EncryptWriteAddr>>16);
			BKP_WriteBackupRegister(BKP_DR10,EncryptWriteAddr);
		break;
		
		default:
		break;
	}
}


/******************************************************
	* �����������豸�ϵ������һ�ε���־д���ַ
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	
*******************************************************/
void LoadWriteAddrPowerOnDevice(void)
{
	//������һ�ε�ַ
	OperateWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR3) <<16 | BKP_ReadBackupRegister(BKP_DR4);
	SystemWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR5) <<16 | BKP_ReadBackupRegister(BKP_DR6);
	CommunicateWriteAddr = BKP_ReadBackupRegister(BKP_DR7) <<16 | BKP_ReadBackupRegister(BKP_DR8);
	EncryptWriteAddr 	 = BKP_ReadBackupRegister(BKP_DR9) <<16 | BKP_ReadBackupRegister(BKP_DR10);
}




