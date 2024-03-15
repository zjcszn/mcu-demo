/***********************************************************************
	* �ļ�˵����W25Qxx�������򡢶�/д����
	* ���ߣ�	ZWQ
	* ʱ�䣺	2019-1-21
	* ��ע��	оƬ��Ϣ��	4KBΪһ������(Sector)��
							16������Ϊ1����(Block)��
							W25xx16(16Mbit/8 = 2MB)��
							����Ϊ2M�ֽ�,����32��Block,512��Sector��
							LoRaͨ����־�ĵ�ַ���֣�
							�ӽ�����־�ĵ�ַ���֡�
************************************************************************/
#include "w25qxx_flash.h"

u16 W25QXX_TYPE = 0x00;	
u8	W25QxxWriteTempBuf[1024]  = {1,};;
//u8	W25QxxReadTempBuf[512]   = {0,};

//��ʼ��SPI FLASH��IO��
void W25QxxFlashInit(void)
{
	W25QxxSPI_Init();							//��ʼ��SPI
    SPIX_SetSpeed(SPI_BaudRatePrescaler_4);		//����Ϊ18Mʱ��,����ģʽ
   
	W25QXX_TYPE = W25QXX_ReadID();		//��ȡFLASH ID
	while(W25QXX_TYPE != W25Q32)		//���оƬ�ܷ���������
	{
		W25QXX_TYPE = W25QXX_ReadID();	//��ȡFLASH ID
	#ifdef UART_DUBUG
		printf("EEPROM IS WORONG!\r\n");
	#endif
	}
	#ifdef UART_DUBUG
		printf("EEPROM IS WORKING!\r\n");
	#endif
}


////���оƬ�Ƿ����
void W25QxxCheckIsWrongProcess(void)
{
	u16 i = 0;
	for(i=0;i<1024;i++)
		W25QxxWriteTempBuf[i] = 0x11;
	
	for(i=0; i<4000; i++)
	{
		W25QXX_Write(W25QxxWriteTempBuf,i*PAGE_SIZE,PAGE_SIZE);
		TimerDelay_Ms(10);
	}
	
}


//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
u8 W25QXX_ReadSR(void)
{
    u8 byte = 0;
	
    W25QXX_CS = 0;                          //ʹ������
    SPIX_ReadWriteByte(W25X_ReadStatusReg); //���Ͷ�ȡ״̬�Ĵ�������
    byte = SPIX_ReadWriteByte(0Xff);        //��ȡһ���ֽ�
    W25QXX_CS = 1;                          //ȡ��Ƭѡ
	
    return byte;
}


//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX_Write_SR(u8 sr)
{
    W25QXX_CS = 0;                          //ʹ������
    SPIX_ReadWriteByte(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������
    SPIX_ReadWriteByte(sr);               	//д��һ���ֽ�
    W25QXX_CS = 1;                          //ȡ��Ƭѡ
}


//W25QXXдʹ��
//��WEL��λ
void W25QXX_Write_Enable(void)
{
    W25QXX_CS = 0;                          	//ʹ������
    SPIX_ReadWriteByte(W25X_WriteEnable); 	//����дʹ��
    W25QXX_CS = 1;                           	//ȡ��Ƭѡ
}


//W25QXXд��ֹ
//��WEL����
void W25QXX_Write_Disable(void)
{
    W25QXX_CS = 0;                          //ʹ������
    SPIX_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��
    W25QXX_CS = 1;                          //ȡ��Ƭѡ
}


//��ȡоƬID
//����ֵ����:
//0XEF13,��ʾоƬ�ͺ�ΪW25Q80
//0XEF14,��ʾоƬ�ͺ�ΪW25Q16
//0XEF15,��ʾоƬ�ͺ�ΪW25Q32
//0XEF16,��ʾоƬ�ͺ�ΪW25Q64
//0XEF17,��ʾоƬ�ͺ�ΪW25Q128
u16 W25QXX_ReadID(void)
{
    u16 Temp = 0;

    W25QXX_CS = 0;
	
	//���Ͷ�ȡID����
    SPIX_ReadWriteByte(0x90);			
    SPIX_ReadWriteByte(0x00);
    SPIX_ReadWriteByte(0x00);
    SPIX_ReadWriteByte(0x00);
    Temp |= SPIX_ReadWriteByte(0xFF) << 8;
    Temp |= SPIX_ReadWriteByte(0xFF);

    W25QXX_CS = 1;
    return Temp;
}


//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX_Read(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
    u16 i;
    W25QXX_CS = 0;                            	//ʹ������
    SPIX_ReadWriteByte(W25X_ReadData);         	//���Ͷ�ȡ����
    SPIX_ReadWriteByte((u8)((ReadAddr) >> 16));  	//����24bit��ַ
    SPIX_ReadWriteByte((u8)((ReadAddr) >> 8));
    SPIX_ReadWriteByte((u8)ReadAddr);
    for(i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = SPIX_ReadWriteByte(0XFF);   	//ѭ������
    }
    W25QXX_CS = 1;
}


//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX_Write_Page(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 i;
	
    W25QXX_Write_Enable();                  		//SET WEL
    W25QXX_CS = 0;                            		//ʹ������
    SPIX_ReadWriteByte(W25X_PageProgram);      		//����дҳ����
    SPIX_ReadWriteByte((u8)((WriteAddr) >> 16)); 	//����24bit��ַ
    SPIX_ReadWriteByte((u8)((WriteAddr) >> 8));
    SPIX_ReadWriteByte((u8)WriteAddr);
    for(i = 0; i < NumByteToWrite; i++)
        SPIX_ReadWriteByte(pBuffer[i]);				//ѭ��д��
    W25QXX_CS = 1;                            		//ȡ��Ƭѡ
    W25QXX_Wait_Busy();					   			//�ȴ�д�����
}


//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u16 pageremain;
    pageremain = 256 - WriteAddr % 256; //��ҳʣ����ֽ���
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite; 	//������256���ֽ�
    while(1)
    {
        W25QXX_Write_Page(pBuffer, WriteAddr, pageremain);
        //д�������
		if(NumByteToWrite == pageremain)
            break;
		//NumByteToWrite>pageremain
        else 
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;		//��ȥ�Ѿ�д���˵��ֽ���
            if(NumByteToWrite > 256)
                pageremain = 256; 				//һ�ο���д��256���ֽ�
            else
                pageremain = NumByteToWrite;	//����256���ֽ���
        }
    };
}


//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
u8 W25QXX_BUFFER[4096];		//����2KB������
void W25QXX_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
    u32 secpos;
    u16 secoff;
    u16 secremain;
    u16 i;
    u8 	*W25QXX_BUF;				//�����ַ�ָ��
    W25QXX_BUF = W25QXX_BUFFER;		//ָ�븳ֵ
    secpos    = WriteAddr / 4096;	//������ַ
    secoff    = WriteAddr % 4096;	//�������ڵ�ƫ��
    secremain = 4096 - secoff;		//����ʣ��ռ��С
	
//#ifdef UART_DUBUG
//    printf("WriteAddr:0x%2x,NumByte:0x%2x\r\n",WriteAddr, NumByteToWrite); //������
//#endif
	
	//������4096���ֽ�
    if(NumByteToWrite <= secremain)
        secremain = NumByteToWrite;	
    while(1)
    {
        W25QXX_Read(W25QXX_BUF, secpos * 4096, 4096);	//������������������
        for(i = 0; i < secremain; i++)					//У������
        {
            if(W25QXX_BUF[secoff + i] != 0XFF)
                break;			//��Ҫ����
        }
        if(i < secremain)		//��Ҫ����
        {
            W25QXX_Erase_Sector(secpos);		//�����������
            for(i = 0; i < secremain; i++)	   	//����
            {
                W25QXX_BUF[i + secoff] = pBuffer[i];
            }
            W25QXX_Write_NoCheck(W25QXX_BUF, secpos * 4096, 4096);	//д����������

        }
        else
            W25QXX_Write_NoCheck(pBuffer, WriteAddr, secremain); 	//д�Ѿ������˵�,ֱ��д������ʣ������.
        
		//д�������
		if(NumByteToWrite == secremain)
            break;	
		//д��δ����			
        else			
        {
            secpos++;	//������ַ��1
            secoff = 0;	//ƫ��λ��Ϊ0

            pBuffer += secremain;  			//ָ��ƫ��
            WriteAddr += secremain;			//д��ַƫ��
            NumByteToWrite -= secremain;	//�ֽ����ݼ�
            if(NumByteToWrite > 4096)
                secremain = 4096;			//��һ����������д����
            else
                secremain = NumByteToWrite;	//��һ����������д����
        }
    };
}


//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX_Erase_Chip(void)
{
    W25QXX_Write_Enable();           	//SET WEL
    W25QXX_Wait_Busy();					//�ȴ�оƬ����		
    W25QXX_CS = 0;                      //ʹ������
    SPIX_ReadWriteByte(W25X_ChipErase);	//����Ƭ��������
    W25QXX_CS = 1;                   	//ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				//�ȴ�оƬ��������
}


//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX_Erase_Sector(u32 Dst_Addr)
{
    //����falsh�������,������
#ifdef UART_DUBUG
    printf("FlashEraseAddr:0x%x\r\n",Dst_Addr);
#endif
    Dst_Addr *= 4096;
    W25QXX_Write_Enable();                  	//SET WEL
    W25QXX_Wait_Busy();
    W25QXX_CS = 0;                            	//ʹ������
    SPIX_ReadWriteByte(W25X_SectorErase);      	//������������ָ��
    SPIX_ReadWriteByte((u8)((Dst_Addr) >> 16));	//����24bit��ַ
    SPIX_ReadWriteByte((u8)((Dst_Addr) >> 8));
    SPIX_ReadWriteByte((u8)Dst_Addr);
    W25QXX_CS = 1;                            	//ȡ��Ƭѡ
    W25QXX_Wait_Busy();   				   		//�ȴ��������
}


//�ȴ�����
void W25QXX_Wait_Busy(void)
{
	//�ȴ�BUSYλ���(����������)
    while((W25QXX_ReadSR() & 0x01) == 0x01); 
}


//�������ģʽ
void W25QXX_PowerDown(void)
{
    W25QXX_CS = 0;                      //ʹ������
    SPIX_ReadWriteByte(W25X_PowerDown);	//���͵�������
    W25QXX_CS = 1;             			//ȡ��Ƭѡ
    delay_us(3);                  		//�ȴ�TPD
}


//����W25QxxоƬ
void W25QXX_WAKEUP(void)
{
    W25QXX_CS = 0;                            	//ʹ������
    SPIX_ReadWriteByte(W25X_ReleasePowerDown);	//send W25X_PowerDown command 0xAB
    W25QXX_CS = 1;                            	//ȡ��Ƭѡ
    delay_us(3);                            	//�ȴ�TRES1
}



