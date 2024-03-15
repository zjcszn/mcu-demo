#ifndef _LCDDRV_H
#define _LCDDRV_H

#ifndef _LCDDRV_C
#define EXT_LCDDRV extern
#else
#define EXT_LCDDRV
#endif


//#define USE_HS19264_TURN	//��ת��װ��ʾ


//-------------------------------------------------------
/* ������ɫ��������(���������ݽṹ) */
#define  TCOLOR				uint8 
#define  NULL				_NULL	
/* ����LCM�������� */
#define  GUI_LCM_XMAX		128							/* ����Һ��x��������� */
#define  GUI_LCM_YMAX		64							/* ����Һ��y��������� */
//-------------------------------------------------------

//����Һ��ģ�������
#define C_STARTLINE             	0xc0	//��ʾ��ʼ��
#define C_LEFT_BASE                 0x40	//������ʾ���е�ַ
#define C_TOP_BASE                  0xb8	//������ʾ���е�ַ
#define TURN_PLUS					0X00	//�ַ�����
#define TURN_MINUS					0X01	//�ַ�����
//����Һ��״̬����
#define S_BUSY                      0x80	//Һ��״̬æ
#define S_ONOFF                     0x20	//Һ����ʾ��
#define S_RST                       0x10	//Һ����λ

//----------------------------------------------------------------------------
//
//#define BKG_ON()					GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define BKG_OFF()					GPIO_ResetBits(GPIOB,GPIO_Pin_12)

//��عܽŵĶ���
#define RS_H()						GPIO_SetBits(GPIOC, GPIO_Pin_8)
#define RS_L()						GPIO_ResetBits(GPIOC, GPIO_Pin_8)
#define RW_H()  					GPIO_SetBits(GPIOC, GPIO_Pin_7)
#define RW_L()						GPIO_ResetBits(GPIOC, GPIO_Pin_7)
#define E_H()						GPIO_SetBits(GPIOC, GPIO_Pin_6)
#define E_L()						GPIO_ResetBits(GPIOC, GPIO_Pin_6)

#define C2_EN()						GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define C2_DN()						GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define C1_EN()						GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define C1_DN()						GPIO_SetBits(GPIOB, GPIO_Pin_15)

#define RST_H()						GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define RST_L()						GPIO_ResetBits(GPIOB, GPIO_Pin_13)

#define BUSY_FLAG()                                     GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_8)


//--------------------------------------------------------------------------------

#define DEFAULT_REFURBISH	300


#define  GUI_CopyColor(color1, color2) 	*color1 = color2		//&0x0
#define  GUI_CmpColor(color1, color2)	( (color1&0x01) == (color2&0x01) )

EXT_LCDDRV void gpio_LcdControl(void);

EXT_LCDDRV void LcdWrCommand(UINT8 v_command);
EXT_LCDDRV void LcdWrData(UINT8 v_data);

EXT_LCDDRV void LCD_WriteByte(uint8 x, uint8 y, uint8 wrdata);
EXT_LCDDRV uint8  LCD_ReadByte(uint8 x, uint8 y);
EXT_LCDDRV void LCD_DispFill(uint8 filldata);
EXT_LCDDRV void LCD_DispIni(void);
EXT_LCDDRV void LCD_DispRefurbish(void);
//
EXT_LCDDRV void  GUI_FillSCR(TCOLOR dat);
EXT_LCDDRV void  GUI_Initialize(void);
EXT_LCDDRV uint8 GUI_Point(uint8 x, uint8 y, TCOLOR color);
EXT_LCDDRV uint8 GUI_ReadPoint(uint8 x, uint8 y, TCOLOR *ret);
EXT_LCDDRV void  GUI_HLine(uint8 x0, uint8 y0, uint8 x1, TCOLOR color);
EXT_LCDDRV void  GUI_RLine(uint8 x0, uint8 y0, uint8 y1, TCOLOR color);
EXT_LCDDRV void GUI_ClrScr(void);
void GUI_DispReverse(UINT8 x, UINT8 y, UINT8 width,UINT8 height);

EXT_LCDDRV UINT8 lcd_need_refurbish;

#define CLR_SCREEN()	GUI_FillSCR(0x00)
#endif