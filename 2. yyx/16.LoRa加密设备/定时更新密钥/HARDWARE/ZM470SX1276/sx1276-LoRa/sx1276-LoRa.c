#include <string.h>
#include "platform.h"
#include "usart.h"

#if defined( USE_SX1276_RADIO )

#include "radio.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-LoRa.h"
#include "sx1276-LoRaMisc.h"

//Constant values need to compute the RSSI value
#define 	RSSI_OFFSET_LF								-155.0
#define 	RSSI_OFFSET_HF								-150.0

#define 	NOISE_ABSOLUTE_ZERO							-174.0

#define		NOISE_FIGURE_LF								4.0
#define 	NOISE_FIGURE_HF								6.0 

/*!
 * Precomputed signal bandwidth log values
 * Used to compute the Packet RSSI value.
 */
const double SignalBwLog[] =
{
    3.8927900303521316335038277369285,  // 7.8 kHz
    4.0177301567005500940384239336392,  // 10.4 kHz
    4.193820026016112828717566631653,   // 15.6 kHz
    4.31875866931372901183597627752391, // 20.8 kHz
    4.4948500216800940239313055263775,  // 31.2 kHz
    4.6197891057238405255051280399961,  // 41.6 kHz
    4.795880017344075219145044421102,   // 62.5 kHz
    5.0969100130080564143587833158265,  // 125 kHz
    5.397940008672037609572522210551,   // 250 kHz
    5.6989700043360188047862611052755   // 500 kHz
};

const double RssiOffsetLF[] =
{   
	// These values need to be specify in the Lab
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
    -155.0,
};

const double RssiOffsetHF[] =
{   
	// These values need to be specify in the Lab
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
    -150.0,
};

//Frequency hopping frequencies table
const int32_t HoppingFrequencies[] =
{
    916500000,
    923500000,
    906500000,
    917500000,
    917500000,
    909000000,
    903000000,
    916000000,
    912500000,
    926000000,
    925000000,
    909500000,
    913000000,
    918500000,
    918500000,
    902500000,
    911500000,
    926500000,
    902500000,
    922000000,
    924000000,
    903500000,
    913000000,
    922000000,
    926000000,
    910000000,
    920000000,
    922500000,
    911000000,
    922000000,
    909500000,
    926000000,
    922000000,
    918000000,
    925500000,
    908000000,
    917500000,
    926500000,
    908500000,
    916000000,
    905500000,
    916000000,
    903000000,
    905000000,
    915000000,
    913000000,
    907000000,
    910000000,
    926500000,
    925500000,
    911000000,
};

//LoRa���Ʋ�������
tLoRaSettings LoRaSettings =
{
	470000000,		// ����Ƶ�� RFFrequency	
	20,				// ���书�� Power		
	8,		//250k	// �źŴ���	SignalBw [0: 7.8kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz, 4: 31.2 kHz,
					// 			5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz, 8: 250 kHz, 9: 500 kHz, other: Reserved]
	7,		//128	// ��Ƶ���� [6: 64, 7: 128, 8: 256, 9: 512, 10: 1024, 11: 2048, 12: 4096  chips]
	1,		//4/5	// ���������	ErrorCoding [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
	true,			// CRCУ��	CrcOn [0: OFF, 1: ON]  CRC
	false,			// ��ͷģʽ	ImplicitHeaderOn [0: OFF, 1: ON]
	0,				// ����ģʽ	RxSingleOn [0: Continuous(��������), 1 Single(��һ����)]
	0,				// FreqHopOn [0: OFF, 1: ON]
	4,				// HopPeriod Hops every frequency hopping period symbols
	100,			// TxPacketTimeout
	100,			// RxPacketTimeout
	128,			// ���س���PayloadLength (������ʽ��ͷģʽ)
};

//����ṹ��ָ�룬ָ��LoRa�Ĵ�����
tSX1276LR	*SX1276LR;

//Local���ջ�����
static u8 RFBuffer[RF_BUFFER_SIZE];


//RF state machine variable
static u8 RFLRState = RFLR_STATE_IDLE;

//Rx management support variables
static u8 		RxGain = 1;
static int8_t 	RxPacketSnrEstimate;
static u16 		RxPacketSize = 0;
static u32 		RxTimeoutTimer = 0;
static double 	RxPacketRssiValue;

//PacketTimeout Stores the Rx window time value for packet reception
static u32 PacketTimeout;

//Tx management support variables
static u16 TxPacketSize = 0;


/***************************************************
	*����������	��ʼ��LoRa���ƽ����
	*��ڲ�����	��
	*����ֵ��	��
****************************************************/
void SX1276LoRaInit( void )
{
    SX1276LoRaSetDefaults();										//��ȡоƬ�汾
   
    SX1276ReadBuffer( REG_LR_OPMODE, SX1276Regs+1, 0x70-1 );		//��ȡ�Ĵ�����ֵ
	
    SX1276LR->RegLna = RFLR_LNA_GAIN_G1;							//LAN����Ϊ�������
    SX1276WriteBuffer( REG_LR_OPMODE, SX1276Regs+1, 0x70-1 );		//д������ֵ���Ĵ�����

    //set the RF settings 
    SX1276LoRaSetRFFrequency( LoRaSettings.RFFrequency );			//�����ز�Ƶ��Ϊ470 000 000
    SX1276LoRaSetSpreadingFactor( LoRaSettings.SpreadingFactor ); 	//������Ƶ����Ϊ7
    SX1276LoRaSetErrorCoding( LoRaSettings.ErrorCoding );			//���þ�������� 001->4/5
    SX1276LoRaSetPacketCrcOn( LoRaSettings.CrcOn );					//����CRC 1->CRC����
    SX1276LoRaSetSignalBandwidth( LoRaSettings.SignalBw );			//���ô���Ϊ500kHz 1001->500kHz

    SX1276LoRaSetImplicitHeaderOn( LoRaSettings.ImplicitHeaderOn );	//����Ϊ��ʾ��ͷģʽ 0->��ʾ��ͷģʽ
    SX1276LoRaSetSymbTimeout( 0x3FF );								//����RX��ʱ�����Чλ�������Чλ (0x3ff)
    SX1276LoRaSetPayloadLength( LoRaSettings.PayloadLength );		//���ø����ֽڳ���Ϊ128
    SX1276LoRaSetLowDatarateOptimize( 1 );		//false				//�������������Ż� true

#if( ( MODULE_SX1276RF1IAS == 1 ) || ( MODULE_SX1276RF1KAS == 1 ) )
    if( LoRaSettings.RFFrequency > 860000000 )
    {
        SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_RFO );
        SX1276LoRaSetPa20dBm( false );
        LoRaSettings.Power = 14;
        SX1276LoRaSetRFPower( LoRaSettings.Power );
    }
    else
    {
        SX1276Write( REG_LR_OCP, 0x3f );							//Ĭ�ϲ���PA���ڵ���Ϊ100mA,�����20dBmʱ��Ҫ120mA
		SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );	//ѡ��PA�������(PA_BOOST����) �������<=+20dBm
        SX1276LoRaSetPa20dBm( true );								//��������������Ϊ20dBm(���)
        LoRaSettings.Power = 20;									//�����������Ϊ20dBm
        SX1276LoRaSetRFPower( LoRaSettings.Power );					//д���������ֵ
    } 
#endif
	
//	//����ǰ���볤��	--	6
//	SX1276LR->RegPreambleLsb = 0x06;								//����ǰ���볤��Ϊ6�����ų���
//	SX1276Write(REG_LR_PREAMBLELSB, SX1276LR->RegPreambleLsb);		//���Ĵ���д���趨ֵ(0x06)

//	SX1276LR->RegPreambleLsb = 0x08;								//��Ϊȱʡֵ
//	SX1276Read(REG_LR_PREAMBLELSB, &SX1276LR->RegPreambleLsb);		//��ȡ�Ĵ�����ֵ
//	
//    SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );						//����Ϊ����ģʽ
}


void SX1276LoRaSetDefaults( void )
{
    //REMARK: See SX1276 datasheet for modified default values
    SX1276Read( REG_LR_VERSION, &SX1276LR->RegVersion );
}


void SX1276LoRaReset( void )
{
    SX1276SetReset( RADIO_RESET_ON );	//���͸�λ����ʵ�ָ�λ	 
    delay_ms(1);						//�ȴ�1ms

    SX1276SetReset( RADIO_RESET_OFF );	//���߸�λ����ֹͣ��λ
    delay_ms(6);  						//�ȴ�6ms
}


/*****************************************************************
	* ����������	����LoRa����ģʽ
	* ��ڲ�����	RFLR_OPMODE_SLEEP		--	˯��ģʽ
					RFLR_OPMODE_STANDBY		--	����ģʽ
					RFLR_OPMODE_TRANSMITTER	--	����ģʽ
					RFLR_OPMODE_RECEIVER	--	��������ģʽ
	* ����ֵ��		��
*******************************************************************/
void SX1276LoRaSetOpMode( u8 opMode )
{
    static u8   opModePrev 				= RFLR_OPMODE_STANDBY;
    static bool antennaSwitchTxOnPrev 	= true;
    bool 		antennaSwitchTxOn 		= false;

    opModePrev = SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;

    if( opMode != opModePrev )
    {
        if( opMode == RFLR_OPMODE_TRANSMITTER )
        {
            antennaSwitchTxOn = true;
        }
        else
        {
            antennaSwitchTxOn = false;
        }
        if( antennaSwitchTxOn != antennaSwitchTxOnPrev )
        {
            antennaSwitchTxOnPrev = antennaSwitchTxOn;
            //RXTX( antennaSwitchTxOn ); // Antenna switch control
        }
        SX1276LR->RegOpMode = ( SX1276LR->RegOpMode & RFLR_OPMODE_MASK ) | opMode;

        SX1276Write( REG_LR_OPMODE, SX1276LR->RegOpMode );        
    }
}


u8 SX1276LoRaGetOpMode( void )
{
    SX1276Read( REG_LR_OPMODE, &SX1276LR->RegOpMode );
    
    return SX1276LR->RegOpMode & ~RFLR_OPMODE_MASK;
}


u8 SX1276LoRaReadRxGain( void )
{
    SX1276Read( REG_LR_LNA, &SX1276LR->RegLna );
    return( SX1276LR->RegLna >> 5 ) & 0x07;
}

/*
double SX1276LoRaReadRssi( void )
{
    // Reads the RSSI value
    SX1276Read( REG_LR_RSSIVALUE, &SX1276LR->RegRssiValue );

    if( LoRaSettings.RFFrequency < 860000000 )  // LF
    {
        return RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;
    }
    else
    {
        return RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;
    }
}
*/


u8 SX1276LoRaGetPacketRxGain( void )
{
    return RxGain;
}


int8_t SX1276LoRaGetPacketSnr( void )
{
    return RxPacketSnrEstimate;
}


double SX1276LoRaGetPacketRssi( void )
{
    return RxPacketRssiValue;
}


void SX1276LoRaStartRx( void )
{
    SX1276LoRaSetRFState( RFLR_STATE_RX_INIT );
}


void SX1276LoRaGetRxPacket( void *buffer, uint16_t *size )
{
    *size = RxPacketSize;
    RxPacketSize = 0;
    memcpy( ( void * )buffer, ( void * )RFBuffer, ( size_t )*size );
}


void SX1276LoRaSetTxPacket( const void *buffer, u16 size )
{
    if( LoRaSettings.FreqHopOn == false )
    {
        TxPacketSize = size;
    }
    else
    {
        TxPacketSize = 255;
    }
    memcpy( ( void * )RFBuffer, buffer, ( size_t )TxPacketSize ); 

    RFLRState = RFLR_STATE_TX_INIT;
}


u8 SX1276LoRaGetRFState( void )
{
    return RFLRState;
}


void SX1276LoRaSetRFState( u8 state )
{
    RFLRState = state;
}


/*!
 * \brief Process the LoRa modem Rx and Tx state machines depending on the
 *        SX1276 operating mode.
 *
 * \retval rfState Current RF state [RF_IDLE, RF_BUSY, 
 *                                   RF_RX_DONE, RF_RX_TIMEOUT,
 *                                   RF_TX_DONE, RF_TX_TIMEOUT]
 */
u32 SX1276LoRaProcess( void )
{
    u32 result = RF_BUSY;
    
    switch( RFLRState )
    {
		case RFLR_STATE_IDLE:
			break;
		
		case RFLR_STATE_RX_INIT:
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );							//����Ϊ����ģʽ

			SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT 			|
										//RFLR_IRQFLAGS_RXDONE 				|
										//RFLR_IRQFLAGS_PAYLOADCRCERROR 	|
										RFLR_IRQFLAGS_VALIDHEADER 			|
										RFLR_IRQFLAGS_TXDONE 				|
										RFLR_IRQFLAGS_CADDONE 				|
										//RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL 	|
										RFLR_IRQFLAGS_CADDETECTED;
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );		//����Ϊ���ա�CRC����
																				//FHSS�ı��ŵ��ж�
			if( LoRaSettings.FreqHopOn == true )
			{
				SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

				SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
				SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
			}
			else
			{
				SX1276LR->RegHopPeriod = 255;
			}
			
			SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
					
										// RxDone                    RxTimeout                   FhssChangeChannel           CadDone
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
										// CadDetected               ModeReady
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
			//���ν���ģʽ
			if( LoRaSettings.RxSingleOn == true ) 
			{

				SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
			}
			//��������ģʽ
			else 
			{
				SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;			//
				SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );	//
				SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );					//����Ϊ��������ģʽ
			}
			
			memset( RFBuffer, 0, (size_t)RF_BUFFER_SIZE );						//��BUF����
			PacketTimeout	= LoRaSettings.RxPacketTimeout;
			RxTimeoutTimer = GET_TICK_COUNT();									//���ó�ʱʱ��
			RFLRState 		= RFLR_STATE_RX_RUNNING;							//����Ϊ�������״̬
			break;
			
		case RFLR_STATE_RX_RUNNING:
			//�������
			if( DIO0 == 1 ) 
			{
				RxTimeoutTimer = GET_TICK_COUNT();
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				//�����־λ
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );
				RFLRState = RFLR_STATE_RX_DONE;
			}
			//FHSS�ı��ŵ�
			if( DIO2 == 1 ) 
			{
				RxTimeoutTimer = GET_TICK_COUNT();
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
				// Debug
				RxGain = SX1276LoRaReadRxGain( );
			}
			//Rx single mode
			if( LoRaSettings.RxSingleOn == true ) 
			{
				if( ( GET_TICK_COUNT() - RxTimeoutTimer ) > PacketTimeout )
				{
					RFLRState = RFLR_STATE_RX_TIMEOUT;
				}
			}
			break;
			
		//�������
		case RFLR_STATE_RX_DONE:
			SX1276Read( REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags );
			if( ( SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR ) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );
				
				if( LoRaSettings.RxSingleOn == true ) // Rx single mode
				{
					RFLRState = RFLR_STATE_RX_INIT;
				}
				else
				{
					RFLRState = RFLR_STATE_RX_RUNNING;
				}
				break;
			}
			
			{
				u8 rxSnrEstimate;
				SX1276Read( REG_LR_PKTSNRVALUE, &rxSnrEstimate );
				if( rxSnrEstimate & 0x80 ) // The SNR sign bit is 1
				{
					// Invert and divide by 4
					RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;
					RxPacketSnrEstimate = -RxPacketSnrEstimate;
				}
				else
				{
					// Divide by 4
					RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;
				}
			}
			
			if( LoRaSettings.RFFrequency < 860000000 )  // LF
			{    
				if( RxPacketSnrEstimate < 0 )
				{
					RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE_LF + ( double )RxPacketSnrEstimate;
				}
				else
				{    
					SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
					RxPacketRssiValue = RssiOffsetLF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegPktRssiValue;
				}
			}
			else                                        // HF
			{    
				if( RxPacketSnrEstimate < 0 )
				{
					RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + NOISE_FIGURE_HF + ( double )RxPacketSnrEstimate;
				}
				else
				{    
					SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
					RxPacketRssiValue = RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegPktRssiValue;
				}
			}

			if( LoRaSettings.RxSingleOn == true ) // Rx single mode
			{
				SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;
				SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );

				if( LoRaSettings.ImplicitHeaderOn == true )
				{
					RxPacketSize = SX1276LR->RegPayloadLength;
					SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
				}
				else
				{
					SX1276Read( REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes );
					RxPacketSize = SX1276LR->RegNbRxBytes;
					SX1276ReadFifo( RFBuffer, SX1276LR->RegNbRxBytes );
				}
			}
			else // Rx continuous mode
			{
				SX1276Read( REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr );

				if( LoRaSettings.ImplicitHeaderOn == true )
				{
					RxPacketSize = SX1276LR->RegPayloadLength;
					SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
					SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
					SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
				}
				else
				{
					SX1276Read( REG_LR_NBRXBYTES, &SX1276LR->RegNbRxBytes );
					RxPacketSize = SX1276LR->RegNbRxBytes;
					SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
					SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
					SX1276ReadFifo( RFBuffer, SX1276LR->RegNbRxBytes );
				}
			}
			
			if( LoRaSettings.RxSingleOn == true ) // Rx single mode
			{
				RFLRState = RFLR_STATE_RX_INIT;
			}
			else // Rx continuous mode
			{
				RFLRState = RFLR_STATE_RX_RUNNING;
			}
			result = RF_RX_DONE;
			break;
			
		case RFLR_STATE_RX_TIMEOUT:
			RFLRState = RFLR_STATE_RX_INIT;
			result = RF_RX_TIMEOUT;
			break;
		
		case RFLR_STATE_TX_INIT:

			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

			if( LoRaSettings.FreqHopOn == true )
			{
				SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
											RFLR_IRQFLAGS_RXDONE |
											RFLR_IRQFLAGS_PAYLOADCRCERROR |
											RFLR_IRQFLAGS_VALIDHEADER |
											//RFLR_IRQFLAGS_TXDONE |
											RFLR_IRQFLAGS_CADDONE |
											//RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											RFLR_IRQFLAGS_CADDETECTED;
				SX1276LR->RegHopPeriod = LoRaSettings.HopPeriod;

				SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
				SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
			}
			else
			{
				SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
											RFLR_IRQFLAGS_RXDONE |
											RFLR_IRQFLAGS_PAYLOADCRCERROR |
											RFLR_IRQFLAGS_VALIDHEADER |
											//RFLR_IRQFLAGS_TXDONE |
											RFLR_IRQFLAGS_CADDONE |
											RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
											RFLR_IRQFLAGS_CADDETECTED;
				SX1276LR->RegHopPeriod = 0;
			}
			SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );

			// Initializes the payload size
			SX1276LR->RegPayloadLength = TxPacketSize;
			SX1276Write( REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength );
			
			SX1276LR->RegFifoTxBaseAddr = 0x00; // Full buffer used for Tx
			SX1276Write( REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr );

			SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;
			SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );
			
			// Write payload buffer to LORA modem
			SX1276WriteFifo( RFBuffer, SX1276LR->RegPayloadLength );
											// TxDone               RxTimeout                   FhssChangeChannel          ValidHeader         
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_01;
											// PllLock              Mode Ready
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_01 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );

			SX1276LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );

			RFLRState = RFLR_STATE_TX_RUNNING;
			break;
			
		case RFLR_STATE_TX_RUNNING:
			if( DIO0 == 1 ) // TxDone
			{
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );
				RFLRState = RFLR_STATE_TX_DONE;   
			}
			if( DIO2 == 1 ) // FHSS Changed Channel
			{
				if( LoRaSettings.FreqHopOn == true )
				{
					SX1276Read( REG_LR_HOPCHANNEL, &SX1276LR->RegHopChannel );
					SX1276LoRaSetRFFrequency( HoppingFrequencies[SX1276LR->RegHopChannel & RFLR_HOPCHANNEL_CHANNEL_MASK] );
				}
				// Clear Irq
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL );
			}
			break;
			
		case RFLR_STATE_TX_DONE:
			// optimize the power consumption by switching off the transmitter as soon as the packet has been sent
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );

			RFLRState 	= RFLR_STATE_IDLE;
			result 		= RF_TX_DONE;
			break;
		
		case RFLR_STATE_CAD_INIT:    
			SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
		
			SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT |
										RFLR_IRQFLAGS_RXDONE |
										RFLR_IRQFLAGS_PAYLOADCRCERROR |
										RFLR_IRQFLAGS_VALIDHEADER |
										RFLR_IRQFLAGS_TXDONE |
										//RFLR_IRQFLAGS_CADDONE |
										RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL; // |
										//RFLR_IRQFLAGS_CADDETECTED;
			SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );
			   
										// RxDone                   RxTimeout                   FhssChangeChannel           CadDone
			SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_00 | RFLR_DIOMAPPING1_DIO1_00 | RFLR_DIOMAPPING1_DIO2_00 | RFLR_DIOMAPPING1_DIO3_00;
										// CAD Detected              ModeReady
			SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_00;
			SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
				
			SX1276LoRaSetOpMode( RFLR_OPMODE_CAD );
			RFLRState = RFLR_STATE_CAD_RUNNING;
			break;
			
		//CAD Done interrupt
		case RFLR_STATE_CAD_RUNNING:
			if( DIO3 == 1 ) 
			{ 
				//���CAD��־λ
				SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDONE  );
				// CAD Detected interrupt
				if( DIO4 == 1 ) 
				{
					//�����־λ
					SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_CADDETECTED  );
					// CAD detected, we have a LoRa preamble
					RFLRState = RFLR_STATE_RX_INIT;
					result = RF_CHANNEL_ACTIVITY_DETECTED;
				} 
				else
				{    
					// The device goes in Standby Mode automatically    
					RFLRState = RFLR_STATE_IDLE;
					result = RF_CHANNEL_EMPTY;
				}
			}   
			break;
    
		default:
			break;
    } 
	return result;
}


///////////////////////////////////////////////////////////////////////////////
/********************************************************************
	* ����������	��ȡ���ñ�����ǰ���ӵ��ǰ����ݵ������
	* ��ڲ�����	��
	* ����ֵ��		 ����ȣ������и�
*********************************************************************/
int8_t GetLoRaSNR (void)
{  
	u8 rxSnrEstimate;																
	//ȡ���һ�����ݵ������         									//��¼����ȹ�ֵ
	SX1276Read(REG_LR_PKTSNRVALUE, &rxSnrEstimate );					//�����                     
	//The SNR sign bit is 1												//�����С��0�����������     
	if( rxSnrEstimate & 0x80 ) 
	{ 
		RxPacketSnrEstimate = ( ( ~rxSnrEstimate + 1 ) & 0xFF ) >> 2;	//Invert and divide by 4      
		RxPacketSnrEstimate = -RxPacketSnrEstimate;
	} 
	else
	{																	//����ȴ���0�����������     
		RxPacketSnrEstimate = ( rxSnrEstimate & 0xFF ) >> 2;			//Divide by 4                
	}
	return RxPacketSnrEstimate;
}


/*******************************************************************************
	** Function name:       GetPackLoRaRSSI
	** Descriptions:        ��ȡ���ñ�����ǰ���ӵ��ǰ����ݵ��ź�ǿ��
	** input parameters:    ��
	** output parameters:   ��
	** Returned value:      �ź�ǿ�ȣ������и�
********************************************************************************/
double GetPackLoRaRSSI(void)
{  
	if( GetLoRaSNR() < 0 ) 
	{                                           
		/***************************************************************************************
			*���ʣ�P=-174��dBm�� + BW(dB) + NF(dB) + SNR(dB); 
			*���źű�������û��������ô˹�ʽ����RSSI��ǰ����������������,���һ���������
		****************************************************************************************/
		//�źű�������û             
		RxPacketRssiValue = NOISE_ABSOLUTE_ZERO + 10.0 * SignalBwLog[LoRaSettings.SignalBw] + \
							NOISE_FIGURE_LF     + (double)RxPacketSnrEstimate;	
	} 
	else 
	{   
		//�ź�ǿ������
		SX1276Read( REG_LR_PKTRSSIVALUE, &SX1276LR->RegPktRssiValue );
		RxPacketRssiValue = RssiOffsetLF[LoRaSettings.SignalBw] + (double)SX1276LR->RegPktRssiValue;
	}
	return RxPacketRssiValue;
}


/*********************************************************************************************************
	** Function name:       SX1276LoRaReadRssi
	** Descriptions:        ��ȡ��ǰ�ź�ǿ��,���ص��ǵ������������һ�̵��ź�ǿ��
							��Ҫ�ڽ���״̬�µ��ò����á� 
	** input parameters:    ��
	** output parameters:   ��
*********************************************************************************************************/
double SX1276LoRaReadRssi( void )
{
    SX1276Read( REG_LR_RSSIVALUE, &SX1276LR->RegRssiValue );	//�Ĵ���0x1B����ǰ�ź�ǿ�ȵ�ֵ 
	return RssiOffsetHF[LoRaSettings.SignalBw] + ( double )SX1276LR->RegRssiValue;	
	//�˴�ֻ�ܲ������뵽���߽ӿڴ��Ĺ��ʣ������źź�������
    //����ź�ǿ�Ȳ�һ������Ϊ���ź�ǿ�ȿ�ͨ��SNR����ԭ����֮�µ��ź�ǿ��  
}


/*********************************************************************************************************
	* Function name:       LoRaRxStateEnter
	* Descriptions:        �������״̬
	* input parameters:    ��
	* output parameters:   ��
	* Returned value:      ��
*********************************************************************************************************/
void LoRaRxStateEnter (void)
{
	//1.---->�������ģʽ
	SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );		//�����Ĵ���ֻ������Standby,sleep or FSTX ģʽ       
	
	//2.---->���ճ�ʼ������������
	SX1276LR->RegIrqFlagsMask	= 	RFLR_IRQFLAGS_RXTIMEOUT          |	//��ʱ�ж�����
                                    //RFLR_IRQFLAGS_RXDONE           |	//�����ݰ���������ж�
                                    RFLR_IRQFLAGS_PAYLOADCRCERROR    |	//����CRC�����ж�����	
                                    RFLR_IRQFLAGS_VALIDHEADER        |	//Rxģʽ�½��յ�����Ч��ͷ����
                                    RFLR_IRQFLAGS_TXDONE             |	//FIFO���ط�������ж�����
                                    RFLR_IRQFLAGS_CADDONE            |	//CAD����ж�����
                                    RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |	//FHSS�ı��ŵ��ж�����
                                    RFLR_IRQFLAGS_CADDETECTED;			//��⵽CAD�ж�����
	SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );		//������Ҫ���ε��ж�(��ע�͵����жϼ���)

    SX1276LR->RegHopPeriod = 255;
	SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );			//Ƶ������֮��ķ�������:255
                
    //DIO0:RxDone 
    SX1276LR->RegDioMapping1 = 	RFLR_DIOMAPPING1_DIO0_00 |				//DIO0~DIO3����ӳ��
								RFLR_DIOMAPPING1_DIO1_11 |		 
								RFLR_DIOMAPPING1_DIO2_11 | 
								RFLR_DIOMAPPING1_DIO3_11;
	
	SX1276LR->RegDioMapping2 = 	RFLR_DIOMAPPING2_DIO4_10 | 				//DIO4~DIO5����ӳ��
								RFLR_DIOMAPPING2_DIO5_11;
								
	/*****************************************************************************
		*��ZM470SX-M��,DIO4�ӵ���Ƶ���ص�6��,�������״̬���������Ҫ����,
		*������LoRaģʽ������Ϊ:RFLR_DIOMAPPING2_DIO4_10
	******************************************************************************/				
    SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );	//����5��IO�Ĺ���  
    
	//3.---->���ý���ģʽ(����/����ģʽ)
	if( LoRaSettings.RxSingleOn == true ) 
	{ 
		//����Ϊ���ν���
		SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER_SINGLE );
	}
	else
	{ 
		//����Ϊ��������ģʽ
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxBaseAddr;		  	//����:0x00 
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );	//SPI����FIFO�ĵ�ַ
		SX1276LoRaSetOpMode( RFLR_OPMODE_RECEIVER );           			//�������״̬
	}
	
//	PacketTimeout  = LoRaSettings.RxPacketTimeout;						//��ʱʱ��Ϊ 100
//	RxTimeoutTimer = GET_TICK_COUNT( );									//���ճ�ʱʱ��Ϊ 10		
}


/**************************************************************
	* ����������	��ȡ���յ�����
	* ��ڲ�����	pbuf	--	���ݻ�����ָ�� 
					size	--	�����ֽڳ���ָ��  
	* ����ֵ��		��
***************************************************************/
void LoRaRxDataRead (u8 *pbuf, u8 *size )
{
	//4.---->�ȴ����ݽ������
	if( DIO0 == 1 ) 
	{                                                  				//RxDone                      
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_RXDONE  );		//ִ��д��������������жϱ�־
	}	
	
    SX1276Read( REG_LR_IRQFLAGS, &SX1276LR->RegIrqFlags );			//��ȡ�ж�״̬��־λ    
	//5.---->���CRC��־������֤���ݰ��������� 	
	if( (SX1276LR->RegIrqFlags & RFLR_IRQFLAGS_PAYLOADCRCERROR) == RFLR_IRQFLAGS_PAYLOADCRCERROR )
	{
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_PAYLOADCRCERROR  );	//ִ��д��������������жϱ�־
		*size = 0;
		return;
	}     
	
//	GetLoRaSNR();		//��ȡ���һ�����������(�����������SNR�����ɰѴ˺���ɾ��)     	     
//	GetPackLoRaRSSI();	//��ȡ���һ�������ź�ǿ��(�����������RSSI�����ɰѴ˺���ɾ��)

	//6.---->��ȡ���յ������� 
	SX1276Read( REG_LR_FIFORXCURRENTADDR, &SX1276LR->RegFifoRxCurrentAddr );	//��ȡ������һ��������ָ��
	
	//��ʽ��ͷģʽ
	if( LoRaSettings.ImplicitHeaderOn == true ) 
	{
		RxPacketSize = SX1276LR->RegPayloadLength;
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );			//д��FIFO�ķ��ʵ�ַ
		SX1276ReadFifo( RFBuffer, SX1276LR->RegPayloadLength );
	} 
	//��ʽ��ͷģʽ
	else 
	{
		SX1276Read( REG_LR_NBRXBYTES, size );	 						//��ȡ���ݰ�������(size��¼���ݰ��ĳ���)
		SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoRxCurrentAddr;		//�������յ����ݰ����׵�ַ����FIFO���ݻ������ķ��ʵ�ַ(SPI)
		SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );  	//д��FIFO�ķ��ʵ�ַ
		SX1276ReadFifo(pbuf, *size);									//��ȡFIFO�е����ݣ���������pbuf��
	}
}	


//u16 Tx_Counter = 0;
/****************************************************************
	* ����������	��������
	* ��ڲ�����	pbuf		--	���ݻ�����ָ�� 
					size		--	�����ֽ���  
	* ����ֵ��		0			--	���ͳɹ�
					����(1,2)	--	����ʧ��
******************************************************************/
u8 LoRaTxData (u8 *pbuf, u8 size ,u8 *pcrcbuf ,u8 crcflag)
{
     u32 i,j; 
	if ( pbuf==0 || size==0 )	//û�����ݿɷ���
	{
		return 1;				//����ʧ�� -- ����1
	}		 
	//1.------>�������״̬
    SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );
	
	//2.------>���ͳ�ʼ��
	SX1276LR->RegIrqFlagsMask = RFLR_IRQFLAGS_RXTIMEOUT          |
								RFLR_IRQFLAGS_RXDONE             |
								RFLR_IRQFLAGS_PAYLOADCRCERROR    |
								RFLR_IRQFLAGS_VALIDHEADER        |
								//RFLR_IRQFLAGS_TXDONE           |		//�򿪷�������ж�(Tx)
								RFLR_IRQFLAGS_CADDONE            |
								RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL |
								RFLR_IRQFLAGS_CADDETECTED;
	SX1276Write( REG_LR_IRQFLAGSMASK, SX1276LR->RegIrqFlagsMask );
	
	SX1276LR->RegHopPeriod = 0;											//Ƶ������֮��ķ���ʱ����Ϊ��0
	SX1276Write( REG_LR_HOPPERIOD, SX1276LR->RegHopPeriod );
	
	//��ʼ��������������ռ�������Ĵ�С
	SX1276LR->RegPayloadLength = size; 									//���ݸ��س���(������CRC����)
	SX1276Write( REG_LR_PAYLOADLENGTH, SX1276LR->RegPayloadLength +4); 	//��implicitģʽ(��ʽ��ͷ),����д��FIFO����

	SX1276LR->RegFifoTxBaseAddr = 0x00;                                 //���ͻ���ַ����Ϊ0x00   
	SX1276Write( REG_LR_FIFOTXBASEADDR, SX1276LR->RegFifoTxBaseAddr );  //д�뷢�͵��׵�ַ 

	SX1276LR->RegFifoAddrPtr = SX1276LR->RegFifoTxBaseAddr;				//FIFO��ַָ��ָ��Tx����ַ
	SX1276Write( REG_LR_FIFOADDRPTR, SX1276LR->RegFifoAddrPtr );        //д��FIFO�ķ��ʵ�ַ
	
	//3.------>�������͵�����д�뵽FIFO��
	SX1276WriteFifo( pbuf, SX1276LR->RegPayloadLength );	            //д��Ҫ���͵�����
	
	//���д��4Byte��CRCУ��ֵ
	if(crcflag == 1)
	{
		if( size<(256-4) )
			SX1276WriteFifo( pcrcbuf, 4 );
		else
			return 1;
	}
	
	/*****************************************************************************
		*��ZM470SX-M��,DIO4�ӵ���Ƶ���ص�6��,���뷢��״̬���������Ҫ����,
		*������LoRaģʽ������Ϊ:RFLR_DIOMAPPING2_DIO4_00
	******************************************************************************/
    ///////////////////////////////////////////////////////////////////////////////
	//DIO0:TxDone  �˿�ӳ������ 
	SX1276LR->RegDioMapping1 = RFLR_DIOMAPPING1_DIO0_01 | RFLR_DIOMAPPING1_DIO1_11| 
	                           RFLR_DIOMAPPING1_DIO2_11 | RFLR_DIOMAPPING1_DIO3_11;
	SX1276LR->RegDioMapping2 = RFLR_DIOMAPPING2_DIO4_00 | RFLR_DIOMAPPING2_DIO5_11;
	SX1276WriteBuffer( REG_LR_DIOMAPPING1, &SX1276LR->RegDioMapping1, 2 );
	///////////////////////////////////////////////////////////////////////////////
	
	//4.------>���뵽����ģʽ����������
    SX1276LoRaSetOpMode( RFLR_OPMODE_TRANSMITTER );		//���뷢��״̬    
	
	//5.------>�ȴ��������(���TxDone�ж�)
	printf("��ʼ����ʱ��Ϊ		--	t1:%d\r\n",LoRaTransCounter);
	i = 298000; 
	while ( DIO0 == 0 && i != 0 )			//�ȴ�������ɻ��߷���ʧ�ܳ�ʱ,��ɺ�:DIO0Ϊ�ߵ�ƽ
	{                 
        i--;	
		for(j = 0; j < 100; j++);			//��ʱ�ȴ�Լ1ms        
	}
	printf("i��ֵΪ	-- %d\r\n",298000-i);
	//�������ʧ��   
	if (i == 0) 
	{                               
		//ʧ�� -- ����2
		printf("����ʧ��	--	2\r\n");
		return 2;	
	} 
	//������ͳɹ�
	else 
	{					               
	//6.------>������ɽ��뵽����ģʽ
		SX1276Write( REG_LR_IRQFLAGS, RFLR_IRQFLAGS_TXDONE  );	//�����������ж�           
		SX1276LoRaSetOpMode( RFLR_OPMODE_STANDBY );				//�������״̬
		
//		printf("�ɹ����͵�ʱ��Ϊ	--	t2:%d\r\n",LoRaTransCounter);
//		printf("���ͳɹ�	--	0\r\n");
//		Tx_Counter++;	//��¼�ɹ����͵Ĵ���		
		return 0;		//�ɹ� -- ����0
	}          
}


/******************************************************
	* Function name:       LoRaTxPower
	* Descriptions:        ���÷��书��
	* input parameters:    pwr: ���ʷ�Χ5 ~ 20
	* output parameters:   ��
	* Returned value:      ��
*******************************************************/
void LoRaTxPower (u8 pwr)
{  
	//Ĭ�ϲ���PA���ڵ���Ϊ100mA,�����20dBmʱ��Ҫ120mA,���Ա�������0x0b����
	SX1276Write( REG_LR_OCP, 0x3f );	
	SX1276LoRaSetPAOutput( RFLR_PACONFIG_PASELECT_PABOOST );
	SX1276LoRaSetPa20dBm( true );
	SX1276LoRaSetRFPower( pwr);
}


/******************************************************
	* ����������	�����ز�Ƶ��
	* input parameters:    420000000~500000000
	* output parameters:   ��
	* Returned value:      ��
*******************************************************/
void LoRaFreqSet (u32 freq )
{  
    SX1276LoRaSetRFFrequency( freq );
}

#endif
