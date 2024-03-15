#define _IAP_C
#include "head.h"

//#define _FLASH_PROG
//����һҳ�ĵ�ַ
#define START_ADDR  ((u32)0x0803fc00)
#define END_ADDR    ((u32)0x0803ffff)
//#define PageSize   ((u16)0x400)

u32 *p=(u32 *)0x0803fc00;


FLASH_Status FLASHStatus;

/*

length :˫�ֳ���
*/
u8 WritePara(u32 * p_data,u8 length)
{
	u8 i;

	UINT32 Address;	


	Address=START_ADDR;
	//������ֱ�־
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	FLASH_Unlock();
	FLASHStatus = FLASH_ErasePage(START_ADDR);//����һҳ������

	if(FLASHStatus==FLASH_COMPLETE)//����ȷ����
	{
		for (i=0; i<length; i++)
		{
			FLASHStatus = FLASH_ProgramWord(Address, *p_data);
			p_data++;

			Address += 4;
		
			if (FLASHStatus != FLASH_COMPLETE)	
			{
				//error

			//	send_str(0,"��дʧ��\n");

				break;
			}
		}	
	}

	FLASH_Lock();

	if(FLASHStatus==FLASH_COMPLETE)
	{
		

		//UINT8 *p,i;
		
		//UINT8 pr_buf[10];
		
		//p = (u8*)0x0801fc00;;
		//
	//	send_str(0,"��д�ɹ�\n");
		/*************
		for (i=0; i<71; i++)
		{
			sprintf((char*)pr_buf,"%02X ",*p++);
			send_str(0,pr_buf);
                        OSTimeDly(OS_TICKS_PER_SEC/100);//delay 10ms
		}
		*********/

		return 1;
	}
	else
	{
		return 0;
	}

}

