#include "rgbled.h" 

void RGB_Led_Init(void)
{
 
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB,PE�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 //
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		 //IO���ٶ�Ϊ2MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //
    GPIO_ResetBits(GPIOB,GPIO_Pin_7);						 //

}

void RGB_Set_Up(void)   //����1
{
	LED_RGB = 1;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	LED_RGB = 0;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();
}
void RGB_Set_Down(void) //����0
{
	LED_RGB = 1;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	LED_RGB = 0;
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
	__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();__nop();
}
void RGB_SendData(u32 dat)  //ֵ��RGB˳��洢������ʱ����GRB˳����
{
	char i;
	u32 dat1 = dat & 0xff0000;
	u32 dat2 = dat & 0x00ff00;
	dat = (dat2<<8) | (dat1>>8) | (dat&0x0000ff); 
	for(i=0;i<24;i++)
	{
		if(dat & 0x800000)
			RGB_Set_Up();
		else
			RGB_Set_Down();
		dat = dat<<1;
	}
}

void RGB_SendAllData(u32 dat[],u8 size)
{
	u8 tt = 0;
	for(tt=0;tt<size;tt++)
	{
		RGB_SendData(dat[tt]);
	}
	delay_ms(10); 
}

//1�򱣳�ԭ����״̬
static u32 RGB_Data[2];
void RGB_Control(u32 rgb_dev,u32 rgb_warning)   
{
	if(RGB_NOCHANGE != rgb_dev) 
        RGB_Data[0] = rgb_dev;
    if(RGB_NOCHANGE != rgb_warning) 
        RGB_Data[1] = rgb_warning;
    
	RGB_SendAllData(RGB_Data,2);
}


