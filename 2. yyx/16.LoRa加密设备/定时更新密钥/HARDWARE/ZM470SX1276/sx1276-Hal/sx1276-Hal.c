#include <stdint.h>
#include <stdbool.h> 
#include "platform.h"


#if defined( USE_SX1276_RADIO )

#include "lora_spi_init.h"
#include "sx1276-Hal.h"


//sx1276���ų�ʼ��(Ƭѡ����λ)
void SX1276InitIo( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | 			//ʹ��GPIOʱ�ӣ���������ʱ��
							RCC_APB2Periph_GPIOB | 
							RCC_APB2Periph_AFIO,  ENABLE );

    GPIO_InitStructure.GPIO_Mode  	= GPIO_Mode_Out_PP;		//�������
    GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;		//I/O��ʱ��
	
    //����ģ���Ƭѡ���� -- PA4
    GPIO_InitStructure.GPIO_Pin 	= SEL_PIN;				
    GPIO_Init( SEL_IOPORT, &GPIO_InitStructure );
	GPIO_SetBits(SEL_IOPORT,SEL_PIN);						//�ø�
	
	//����ģ��ĸ�λ���� -- PB1
	GPIO_InitStructure.GPIO_Pin 	= RST_PIN;				//��λ����			
	GPIO_Init( RST_IOPORT, &GPIO_InitStructure );
	
	//---------------------------------------------------------------------
	//Configure DIO0 -- PB0
	GPIO_InitStructure.GPIO_Mode 	= 	GPIO_Mode_IN_FLOATING;	//��������
    GPIO_InitStructure.GPIO_Pin 	=  	DIO0_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
	
	// Configure DIO3 as input
    // Configure DIO4 as input
    // Configure DIO5 as input
	//---------------------------------------------------------------------
	
	//��ʼ��SPI1�ӿ�
	LoRaSPI_Init();	
}


/************************************
	*����������	����RST����״̬
	*��ڲ�����	״ֵ̬(0|1)
	*����ֵ��	��
*************************************/
void SX1276SetReset(u8 state)
{
    if( state == RADIO_RESET_ON )
    {
		RST_L();	//����λ�����õ�
    }
    else
    {
		RST_H();	//����λ�����ø�
    }
}


void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    u8 i;

	SEL_L();    //Ƭѡ����(���ݴ��俪ʼ)
    //GPIO_WriteBit( SEL_IOPORT, SEL_PIN, Bit_RESET );

    SPIx_InOut( addr | 0x80 );		//1000 0000(д����+addr)
    for( i = 0; i < size; i++ )
    {
        SPIx_InOut( buffer[i] );	//SPIд��һ��Byte����
    }

    //SEL = 1;
	SEL_H();	//Ƭѡ����(���ݴ������)
    //GPIO_WriteBit( SEL_IOPORT, SEL_PIN, Bit_SET );
}


void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    u8 i;

	SEL_L();	//Ƭѡ����(���ݴ��俪ʼ)
	
    SPIx_InOut( addr & 0x7F );			//0111 1111(������+addr)

    for( i = 0; i < size; i++ )
    {
        buffer[i] = SPIx_InOut( 0 );	//SPI��ȡһ��Byte����
    }

	SEL_H();	//Ƭѡ����(���ݴ������)
}


#if 1
/********************************************
	*����������	��ָ����ַд��1Byte����
	*��ڲ�����	addr:�Ĵ�����ַ
				data:8bit������
	*����ֵ��	��
*********************************************/
void SX1276Write(u8 addr, u8 data)
{
    SX1276WriteBuffer(addr, &data, 1);
}


/********************************************
	*����������	��ָ����ַ��ȡ1Byte����
	*��ڲ�����	addr:�Ĵ�����ַ
				*data:���ݵĵ�ַ
	*����ֵ��	��
*********************************************/
void SX1276Read( u8 addr, u8 *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}
#endif


void SX1276WriteFifo( u8 *buffer, u8 size )
{
    SX1276WriteBuffer( 0, buffer, size );
}


void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}


u8 SX1276ReadDio0( void )
{
	//����DIO0���ŵ�״̬
	return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}


u8 SX1276ReadDio1( void )
{
    //����DIO1���ŵ�״̬
//	return GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}


u8 SX1276ReadDio2( void )
{
	//����DIO2���ŵ�״̬
//	return GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}


uint8_t SX1276ReadDio3( void )
{
    //return IoePinGet( RF_DIO3_PIN );
}


uint8_t SX1276ReadDio4( void )
{
    //return IoePinGet( RF_DIO4_PIN );
}


uint8_t SX1276ReadDio5( void )
{
   //return IoePinGet( RF_DIO5_PIN );
}

#endif




