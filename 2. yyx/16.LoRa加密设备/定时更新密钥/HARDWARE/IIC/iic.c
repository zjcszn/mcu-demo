#include "iic.h"
#include "delay.h"


//��ʼ��IIC
void IIC_Init(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	IIC_SCL_RCC, ENABLE );			//ʹ��GPIOBʱ��

	GPIO_InitStructure.GPIO_Pin   = IIC_SCL_PIN ;			//GPIOB.6/7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = IIC_SDA_PIN ;			//GPIOB.6/7
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, IIC_SCL_PIN|IIC_SDA_PIN); 			//PB10,PB11 �����
}


//����IIC��ʼ�ź�(�½���)
void IIC_Start(void)
{
    SDA_OUT();		//sda�����
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(5);
    IIC_SDA = 0;	//START:when CLK is high,DATA change form high to low
    delay_us(5);
    IIC_SCL = 0;	//ǯסI2C���ߣ�׼�����ͻ��������
}


//����IICֹͣ�ź�(������)
void IIC_Stop(void)
{
    SDA_OUT();		//sda�����
    IIC_SCL = 0;
    IIC_SDA = 0; 	//STOP:when CLK is high DATA change form low to high
    delay_us(5);
    IIC_SCL = 1;
    IIC_SDA = 1; 	//����I2C���߽����ź�
    delay_us(5);
}


//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
	
    SDA_IN();      			//SDA����Ϊ����
    IIC_SDA = 1;		
    delay_us(5);
    IIC_SCL = 1 ;
    delay_us(5);			//�ȴ�ACK(SDAΪ0)
    while( READ_SDA() )
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
	IIC_SCL = 0;

    return 0;
}


//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;	//SDA����������
    delay_us(5);	//2us
    IIC_SCL = 1;
    delay_us(5);	//2us
    IIC_SCL = 0;
}


// ������ACKӦ��
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;	//SDA����������
    delay_us(5);
    IIC_SCL = 1;
    delay_us(5);
    IIC_SCL = 0;
}


//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
	
    SDA_OUT();		//SDAΪ���
    IIC_SCL = 0;	//����ʱ�ӿ�ʼ���ݴ���
    
	for(t = 0; t < 8; t++)
    {
        if( (txd & 0x80) >> 7 )
            IIC_SDA = 1;
        else
            IIC_SDA = 0;
		
		txd <<= 1;			//����1bit
		delay_us(5);		
		IIC_SCL = 1;
		delay_us(5);
		IIC_SCL = 0;
		delay_us(5);
    }
	
}


//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    u8 i, receive = 0;
	
    SDA_IN();				//SDA����Ϊ����
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL = 0;
        delay_us(5);
        IIC_SCL = 1;
        receive <<= 1;
        if( READ_SDA() )
            receive++;
        delay_us(5);
    }
	 
    if (!ack)
        IIC_NAck();		//����nACK
    else
        IIC_Ack(); 		//����ACK
    return receive;
}






