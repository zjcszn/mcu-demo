#include "lora.h"


/***********************************************************************************
									������
************************************************************************************/
	extern u8 	LoRaDeviceIs;
	//--------------------------------------------------------
	u16	LoRaTransCounter = 0x00;	//�����ʱ��(��/����ʱ��)
	u16 WaitIDCounter = 0x00;		//�ȴ��Է�ID��ʱ��
	static u8	UPwr = 20;			//���书��	--	5~20dBm
	static u32 	Freq = 0;			//����Ƶ��	
/***********************************************************************************/


	
/**************************************************
	* ����������LoRaģ���ʼ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	����豸�Ƿ�����������LoRa����
***************************************************/
void MyLoRaInit(void)
{
	UPwr 	= 20;			//���书��Ϊ20dBm
	Freq 	= 433000000;	//����Ƶ��470Mhz 433

	LoRaPWR_Init();			//����LoRa��Դ
	SetLoRaStruPara();		//LoRa�ṹ���ʼ��
	SX1276Reset();			//����SX1276ģ��

	if( SX1276CheckSPI() )
	{
//		printf("xxxxxxxxxδ��⵽ZM470SX�豸�������豸...\r\n");
		
		//****����ϵͳ��־(LORAģ���쳣)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,LORA_MOD_ERROR,0,Log_Wrong);
	}
	else
	{
//		printf("*********��⵽ZM470SX�豸��\r\n");
		
		//****����ϵͳ��־(����LoRaͨ�Ź���)
		RestoreWhichLog(SystemLogFlag,SystemLogBuf,STATR_LORA_COMM_CODE,0,Log_Successful);
	}
	SX1276Init();			//��ʼ��SX1276ģ��	
    SX1276FreqSet(Freq);	//����Ƶ��Ϊ470MHz
    SX1276TxPower(UPwr);	//���÷��书��Ϊ20dBM
    SetLoRaStandBy();		//�������״̬
	SX1276RxStateEnter();	//�������״̬
}


//�������״̬,����
void SetLoRaStandBy(void)
{
	SX1276SetOpMode(RFLR_OPMODE_STANDBY);	
}


//��������,����1.2uA
void SetLoRaSleep(void)
{
	SX1276SetOpMode(RFLR_OPMODE_SLEEP);	   
}


/*****************************************
	* ��������������LoRaģ�鴫���ʱ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��(ʱ����ֵΪ500ms)
******************************************/
void RunLoRaTransCounter(void)
{
	if(LoRaTransCounter<6000)
	{
		LoRaTransCounter ++;
	}
}


/*****************************************
	* �������������еȴ�ID��ʱ��
	* ��ڲ�������
	* ����ֵ��	��
	* ��ע��	��(ʱ����ֵΪ500ms)
******************************************/
void RunWaitIDCounter(void)
{
	if( WaitIDCounter<5000 )
	{
		WaitIDCounter ++;
	}
}


/****************************************
	* ����������LoRaģ���������
	* ��ڲ�����
	* ����ֵ��
	* ��ע��	
*****************************************/
void ReceiveData(u8 *LoRaBuf,u8 *RevCnt)
{
    //���DIO0����ߵ�ƽ������յ�����
    if(DIO0 == 1)
    {
		//��ȡRFоƬFIFO�е�����(ʹ��CRC32У��)
        SX1276RxDataReadCRC32(LoRaBuf, RevCnt);
	}
    else
    {
		//... ...
    }
}
//--------->��ǰ�İ汾
//void ReceiveData(u8 *LoRaBuf,u8 *RevCnt)
//{
//	u8 i = 0;
//	u8 rev_lora_data_size = 0x00;
//	
//    //���DIO0����ߵ�ƽ������յ�����
//    if(DIO0 == 1)
//    {
//        printf("��ʼ���յ�ʱ��Ϊ	--	T1:%dms\r\n", LoRaTransCounter);
//		
//		//��ȡRFоƬFIFO�е�����(ʹ��CRC32У��)
//        SX1276RxDataReadCRC32( LoRaBuf, RevCnt );
//		
//        printf("������ɵ�ʱ��Ϊ	--	T2:%dms\r\n", LoRaTransCounter);
//        printf("���յ�������Ϊ��");
//		rev_lora_data_size = *RevCnt;
//		for(i=0;i<rev_lora_data_size;i++)
//			printf(" %2x",LoRaPact.RxBuf[i]);
//    }
//    else
//    {
//        printf("xxxxxxx���ղ��ɹ�xxxxxxx\r\n");
//    }

//    printf("\r\n�ܵĺ�ʱΪ		--	T3��%dms\r\n", LoRaTransCounter);
//    printf("**************************************************\r\n");
//    printf("\r\n");
//}






