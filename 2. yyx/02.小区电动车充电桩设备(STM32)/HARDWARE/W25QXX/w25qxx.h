#ifndef __W25QXX_H
#define __W25QXX_H			    
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//W25QXX ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

extern u16 W25QXX_TYPE;					//����W25QXXоƬ�ͺ�		   

//#define	W25QXX_CS 		PBout(14)  		//W25QXX��Ƭѡ�ź�
#define	W25QXX_CS 		PAout(11)  		//W25QXX��Ƭѡ�ź�


#define	FLASH_SIZE 		8388608								//16777216		//16*1024*1024;

#define FLASH_START_ADD		FLASH_SIZE-200		//����ʹ�õ���ʼ��ַ

#define	FLASH_TrickleDetectionThreshold_ADD			FLASH_START_ADD	+	1
#define	FLASH_TricklePowerSupplyTime_ADD				FLASH_START_ADD	+	2
#define	FLASH_MaxPowerSupplyTime_ADD						FLASH_START_ADD	+	3
#define	FLASH_JitterTime_ADD										FLASH_START_ADD	+	4
#define	FLASH_KeyAlarmEnable_ADD								FLASH_START_ADD	+	5
#define	FLASH_ACPowerOutagesAlarmEnable_ADD			FLASH_START_ADD	+	6
#define	FLASH_WatchDogEnable_ADD								FLASH_START_ADD	+	7
#define	FLASH_CurrentDetectionPowerTime_ADD			FLASH_START_ADD	+	8
#define	FLASH_DeviceAddress_ADD									FLASH_START_ADD	+	9
#define	FLASH_CurrentBRT_ADD										FLASH_START_ADD	+	10


#define FLASH_LENGTH		150

typedef union _flash
{
	uint8_t  u8_buf[FLASH_LENGTH];
	
	struct _str
	{
		uint8_t u8_FirstPowerOn;
		
		uint8_t u8_DeviceAddress;
		uint8_t u8_CurrentBRT;
		uint8_t u8_BL8025TInit;
		uint8_t u8_TrickleDetectionThreshold;
		uint8_t u8_TricklePowerSupplyTime;
		uint8_t u8_MaxPowerSupplyTime;
		uint8_t u8_JitterTime;
		uint8_t u8_KeyAlarmEnable;
		uint8_t u8_ACPowerOutagesAlarmEnable;
		uint8_t u8_WatchDogEnable;
		uint8_t u8_CurrentDetectionPowerTime;
		
		uint8_t u8_MaxPowerSupply;
		
		uint8_t u8_RelayStatusBuf[4];
		uint8_t u8_RelayWorkModeBuf[32];
		uint8_t u8_RelayWorkSetTimeBuf[32];
		uint8_t u8_RelayWorkRunTimeBuf[32];
		
	}Str;
}FlashDataBufDef;


extern FlashDataBufDef uni_SysStatus,uni_SysStatusBak;	

////////////////////////////////////////////////////////////////////////////////// 
//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void W25QXX_Init(void);
u16  W25QXX_ReadID(void);  	    		//��ȡFLASH ID
u8	 W25QXX_ReadSR(void);        		//��ȡ״̬�Ĵ��� 
void W25QXX_Write_SR(u8 sr);  			//д״̬�Ĵ���
void W25QXX_Write_Enable(void);  		//дʹ�� 
void W25QXX_Write_Disable(void);		//д����
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void W25QXX_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void W25QXX_Erase_Chip(void);    	  	//��Ƭ����
void W25QXX_Erase_Sector(u32 Dst_Addr);	//��������
void W25QXX_Wait_Busy(void);           	//�ȴ�����
void W25QXX_PowerDown(void);        	//�������ģʽ
void W25QXX_WAKEUP(void);				//����

void W25QXX_Process(void);
#endif
















