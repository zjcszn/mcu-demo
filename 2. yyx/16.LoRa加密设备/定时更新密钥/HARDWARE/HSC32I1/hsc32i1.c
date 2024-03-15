#include "hsc32i1.h" 

extern void TimerDelay_Ms(u16 counter);


//��ʼ��IIC�ӿ�
void HSC32I1BaseInit(void)
{
	IIC_Init();
	HSC32RstInit();
}


//��HSC32I1ָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 HSC32I1_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp = 0;	  	 
	
	IIC_Start();  
	
	IIC_Send_Byte(0x60);			//����д����(1010 0000)
	IIC_Wait_Ack(); 

	IIC_Send_Byte(ReadAddr);		//���͸ߵ�ַ
	IIC_Wait_Ack();	
	
	IIC_Start();  	 				//��ʼ��ȡ����	   
	IIC_Send_Byte(0x61);			//�������ģʽ			   
	IIC_Wait_Ack();	 
	
    temp = IIC_Read_Byte(0);		//��ȡһ��Byte������		
	
    IIC_Stop();						//����һ��ֹͣ����	
    
	return temp;
}


//��HSC32I1ָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void HSC32I1_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  					//��ʼ����
	
	IIC_Send_Byte(0x60);	    	//����д����
	IIC_Wait_Ack();					//�ȴ�ACK 	
	
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();					//�ȴ�ACK  	
    IIC_Stop();						//����һ��ֹͣ���� 
	delay_ms(10);	 
}


//��ȡָ�����ȵ�����
u8 HSC32I1_ReadLenByte(u8 ReadAddr,u8 *Buf,u16 Size)
{				  
	u8 i= 0;	  	 
		
	IIC_Start();  					//����һ����ʼ����
	
	IIC_Send_Byte(0x60);			//����д����(1010 0000)
	if( IIC_Wait_Ack() )			//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
//	//�޸�
//	IIC_Wait_Ack();
	
	IIC_Send_Byte(ReadAddr);		//д��ߵ�ַ(�ӻ�������ַ)
//	IIC_Wait_Ack();	
	if( IIC_Wait_Ack() )			//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
	
	IIC_Start();  					//��ʼ��ȡ����(�ӻ���ָ����ַ��������)	
	   
	IIC_Send_Byte(0x61);			//�����ȡ����ģʽ			   
//	IIC_Wait_Ack();	 				//�ȴ�����Ӧ��
	if( IIC_Wait_Ack() )			//�ȴ�Ӧ��
    {
        IIC_Stop();
        return 1;
    }
	while(Size)
    {
        if(Size == 1)
			*Buf = IIC_Read_Byte(0); 	//������,����nACK
        else
			*Buf = IIC_Read_Byte(1);	//������,����ACK
        Size--;
        Buf++;
    }
		
    IIC_Stop();							//����һ��ֹͣ����	
	
	return 0;
}


//��ȡָ�����ȵ�����
void HSC32I1_WriteLenByte(u8 WriteAddr,u8 *Buf,u16 Size)
{				  
	u8 i= 0;	  	 
	
	IIC_Start();  				//��ʼ����
	
	IIC_Send_Byte(0x60);	    //����д����
	IIC_Wait_Ack();				//�ȴ��ӻ���ACK
	
	IIC_Send_Byte(WriteAddr);	//��������д��ĵ�ַ
	IIC_Wait_Ack();	   	
 		
	for(i = 0; i < Size; i++)	
	{
		IIC_Send_Byte(Buf[i]);	//�����ֽ�							   
		IIC_Wait_Ack();			//�ȴ��ӻ���ACK  	
	}
	
    IIC_Stop();					//ֹͣ�������� 
//	delay_ms(10);	 
}




