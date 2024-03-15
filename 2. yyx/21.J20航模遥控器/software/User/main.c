/*
J20��ģң����ң������
	΢��������STM32F103C8T6
	�汾��������V2.2
	����˵����Doc/ReadMe.txt
	�ֿ⣺https://github.com/J20RC/STM32_RC_Transmitter
*/
#include "main.h"

/*����Ԥ����*/
void keyEventHandle(void);
void menuEventHandle(void);

/*��������*/
extern unsigned char logo[];
extern unsigned char iconClock[];
extern unsigned char iconAlarm[];
u16 loca;//�������
u16 thrNum;//���Ż����Ĵ�С
extern char batVoltStr[8];//��ص�ѹ�ַ���
extern char timeStr[9];//ʱ���ַ���
u16 loopCount = 0;//ѭ����������
u16 clockCount = 0;

/*ֻ�ڳ���ʼʱ����һ�εĴ���*/
void setup(void)
{
	delay_init();//��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2��2λ��ռ���ȼ���2λ�����ȼ�
	usart_init(115200);//��ʼ������1��������Ϊ115200
	TIM2_Init(1999,71);//1MHz��ÿ10ms����ADC����һ��
	TIM3_Init(19999,71);//1MHz��ÿ20ms��ⰴ��һ�Σ�
	DMA1_Init();	//DMA��ʼ��
	ADC_Pin_Init();		//ADC��ʼ��
	SET_Init();	//��ȡ�û�����
	RTC_Init();		//RTC��ʼ��
	BEEPER_Init();	//BEEPER��ʼ��
	KEY_Init();		//KEY��ʼ��
	NRF24L01_Init();//NRF24L01��ʼ��
	PPM_Init();//PPM���ų�ʼ��
	
	beeperOnce(20);//��������һ��
	OLED_Init();	//��ʼ��OLED
	OLED_Clear();
	OLED_DrawPointBMP(0,0,logo,128,64,setData.onImage);//��ʾlogo
	OLED_Refresh_Gram();//ˢ���Դ�
	while(NRF24L01_Check())
	{
 		delay_ms(100);
		Beeper = !Beeper;//������5Hz��������ʾ����ģ�����
	}
	Beeper = 0;//������ֹͣ��
	delay_ms(1000);
	OLED_Clear_GRAM();//���
	
	if(setData.clockCheck == ON)//���ſ����Լ�
	{
		if(PWMvalue[2]>1200)
		{
			u8 hzIndex[4] = {youMen,men,baoJing,jing};
			OLED_ShowChineseWords(30,24,hzIndex,4,1);
			OLED_ShowString(94,24,(u8 *)"!",16,1);
			OLED_Refresh_Gram();//д���Դ�
		}
		while(PWMvalue[2]>1200){}
		OLED_Clear_GRAM();//���
	}
	if(setData.NRF_Mode == ON)  NRF24L01_TX_Mode(setData.NRF_Power);//����ģʽ
	else NRF24L01_LowPower_Mode();//����ģʽ
	homeWindow();//��ʾ������
	OLED_Refresh_Gram();//ˢ���Դ�
}

/*����ѭ��ִ�еĴ���*/
void loop(void)
{
	if(clockTime>setData.clockTime*60*50 && setData.clockMode == ON)
	{
		if(clockCount%5==0)
		{
			beeperOnce(20);
			if(nowMenuIndex==home)
			{
				OLED_DrawPointBMP(68,1,iconClock,15,12,1);//ʱ��ͼ��
				OLED_Refresh_Gram();//д���Դ�
			}
		}
		else{delay_ms(20);}
		if(clockCount%10==0 && nowMenuIndex==home){
			OLED_DrawPointBMP(68,1,iconAlarm,16,12,1);//����ͼ��
			drawClockTime();//��ʾʱ��00:00:00
			OLED_Refresh_Gram();//д���Դ�
		}
		clockCount++;
		if(clockCount>100)//������2s
		{
			clockTime = 0;//����ʱ������
			clockCount = 0;
		}
	}
	if(loopCount%100==0 && nowMenuIndex==home)//�������š����ĸ�ͨ��
	{
		drawClockTime();//��ʾʱ��00:00:00
		showSwState();//��ʾ���ĸ�ͨ��״̬
		thrNum = (int)(PWMvalue[2]-1000)/22;//��������
		if(setData.throttlePreference)//��������
		{
			OLED_Fill(2,62-thrNum,2,62,0);//�²���д1
			OLED_Fill(2,16,2,62-thrNum,1);//�ϲ���д0
		}
		else{//��������
			OLED_Fill(125,62-thrNum,125,62,0);//�²���д1
			OLED_Fill(125,16,125,62-thrNum,1);//�ϲ���д0
		}
		if(loopCount%1000==0)//����ص�ѹ
		{
			if(batVoltSignal==1) beeperOnce(10);//������������У�����
			sprintf((char *)batVoltStr,"%1.2fV",batVolt);
			OLED_ShowString(80,19, (u8 *)batVoltStr,16,1);//��ʾ��ص�ѹ
		}
		OLED_Refresh_Gram();//ˢ���Դ�
	}
	if(keyEvent>0)//΢�������¼�
	{
		keyDownSound();
		keyEventHandle();
	}
	if(nowMenuIndex==xcjz14)//�г�У׼
	{
		menu_xcjz14();
		OLED_Refresh_Gram();//ˢ���Դ�
		for(int i=0;i<4;i++)
		{
			if(chResult[i]>setData.chUpper[i]) setData.chUpper[i]=chResult[i];
			if(chResult[i]<setData.chLower[i]) setData.chLower[i]=chResult[i];
		}
	}
	if(nowMenuIndex==dljs18)//��������
	{
		menu_dljs18();
		OLED_Refresh_Gram();//ˢ���Դ�
	}
	if(menuEvent[0])//�˵��¼�
	{
		keyDownSound();
		menuEventHandle();
	}
	lastMenuIndex = nowMenuIndex;
	loopCount++;
}

/*������*/
int main()
{
	setup();
	while(1)
	{
		loop();
	}
}


//΢���¼�������������������
void keyEventHandle(void)
{
	if(nowMenuIndex==home)
	{
		if(keyEvent==1|keyEvent==2) 
		{
			if(setData.throttlePreference)//��������
			{//��1ͨ��΢��-�Һ���
				OLED_Fill(66,61,119,61,1);//д1�����ԭ���ı�־
				loca = (int)93+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
			else//��������
			{//��4ͨ��΢��-�Һ���
				OLED_Fill(66,61,119,61,1);//д1�����ԭ���ı�־
				loca = (int)93+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
		}
		if(keyEvent==3|keyEvent==4) 
		{
			if(setData.throttlePreference)//��������
			{//��2ͨ��΢��-������
				OLED_Fill(125,16,125,62,1);//д1
				loca = (int)39-setData.PWMadjustValue[1]/14.29;
				OLED_Fill(125,loca-2,125,loca+2,0);//д1
			}
			else//��������
			{//��2ͨ��΢��-������
				OLED_Fill(2,16,2,56,1);//д1
				loca = (int)39-setData.PWMadjustValue[1]/12;
				OLED_Fill(2,loca-2,2,loca+2,0);//д1
			}
		}
		if(keyEvent==5|keyEvent==6) 
		{	
			if(setData.throttlePreference)//��������
			{//��4ͨ��΢��-�����
				OLED_Fill(7,61,61,61,1);//д1�����ԭ���ı�־
				loca = (int)35+setData.PWMadjustValue[3]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
			else//��������
			{//��1ͨ��΢��-�����
				OLED_Fill(7,61,61,61,1);//д1�����ԭ���ı�־
				loca = (int)34+setData.PWMadjustValue[0]/12.5;
				OLED_Fill(loca-2,61,loca+2,61,0);//д0
			}
		}
		OLED_Refresh_Gram();//ˢ���Դ�
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
	}
	keyEvent = 0;
}

//�˵��¼������������²˵����桢���ݴ���
void menuEventHandle(void)
{
	OLED_display();
	if(nowMenuIndex==xcjz14 && lastMenuIndex != xcjz14)//ͨ��������У׼
	{
		for(int i=0;i<4;i++)
		{
			setData.chLower[i] 	= chResult[i];	//ң�˵���Сֵ����
			setData.chMiddle[i] = chResult[i];	//ң�˵���ֵ
			setData.chUpper[i] 	= chResult[i];	//ң�˵����ֵ����
		}
	}
	
	for(int i=0;i<4;i++)//����΢����Χ
	{
		if(setData.PWMadjustValue[i]>300-setData.PWMadjustUnit) setData.PWMadjustValue[i]=300-setData.PWMadjustUnit;
		if(setData.PWMadjustValue[i]<setData.PWMadjustUnit-300) setData.PWMadjustValue[i]=setData.PWMadjustUnit-300;
	}
	if(menuEvent[1]==NUM_up)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] += setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] += setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] += setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] += setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] += setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] += setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] += setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] += setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust += 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt += 0.01;menu_bjdy();}
		if(nowMenuIndex==jsbj) {setData.RecWarnBatVolt += 0.1;menu_jsbj();}
		if(nowMenuIndex==wtdw) 
		{
			setData.PWMadjustUnit += 1;
			if(setData.PWMadjustUnit>9) {setData.PWMadjustUnit = 9;}//����΢����λ��Χ
			menu_wtdw();
		}
		if(nowMenuIndex==xzmx) {setData.modelType += 1;if(setData.modelType>2) {setData.modelType=0;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
		if(nowMenuIndex==ppmsc) {setData.PPM_Out =!setData.PPM_Out;menu_ppmsc();}
		if(nowMenuIndex==ajyx) {setData.keySound =!setData.keySound;menu_ajyx();}
		if(nowMenuIndex==kjhm) {setData.onImage =!setData.onImage;menu_kjhm();}
		if(nowMenuIndex==nzbj) {setData.clockMode =!setData.clockMode;menu_nzbj();}
		if(nowMenuIndex==nzsc) 
		{
			setData.clockTime += 1;
			if(setData.clockTime>60) {setData.clockTime=60;}//��������ʱ��
			menu_nzsc();
		}
		if(nowMenuIndex==kjzj) {setData.clockCheck =!setData.clockCheck;menu_kjzj();}
		if(nowMenuIndex==skbh) 
		{
			setData.throttleProtect += 5;
			if(setData.throttleProtect>100) {setData.throttleProtect = 100;}//�������ű���ֵ
			menu_skbh();
		}
		if(nowMenuIndex==fsgl)
		{
			setData.NRF_Power += 2;
			if(setData.NRF_Power>0x0f) {setData.NRF_Power=0x0f;}//���ƹ���11,13,15
			menu_fsgl();
		}
	}
	if(menuEvent[1]==NUM_down)
	{
		if(nowMenuIndex==tdwt1){setData.PWMadjustValue[0] -= setData.PWMadjustUnit;menu_tdwt1();}
		if(nowMenuIndex==tdwt2){setData.PWMadjustValue[1] -= setData.PWMadjustUnit;menu_tdwt2();}
		if(nowMenuIndex==tdwt3){setData.PWMadjustValue[2] -= setData.PWMadjustUnit;menu_tdwt3();}
		if(nowMenuIndex==tdwt4){setData.PWMadjustValue[3] -= setData.PWMadjustUnit;menu_tdwt4();}
		if(nowMenuIndex==tdwt5){setData.PWMadjustValue[4] -= setData.PWMadjustUnit;menu_tdwt5();}
		if(nowMenuIndex==tdwt6){setData.PWMadjustValue[5] -= setData.PWMadjustUnit;menu_tdwt6();}
		if(nowMenuIndex==tdwt7){setData.PWMadjustValue[6] -= setData.PWMadjustUnit;menu_tdwt7();}
		if(nowMenuIndex==tdwt8){setData.PWMadjustValue[7] -= setData.PWMadjustUnit;menu_tdwt8();}
		if(nowMenuIndex==tdzf1){setData.chReverse[0] = !setData.chReverse[0];menu_tdzf1();}
		if(nowMenuIndex==tdzf2){setData.chReverse[1] = !setData.chReverse[1];menu_tdzf2();}
		if(nowMenuIndex==tdzf3){setData.chReverse[2] = !setData.chReverse[2];menu_tdzf3();}
		if(nowMenuIndex==tdzf4){setData.chReverse[3] = !setData.chReverse[3];menu_tdzf4();}
		if(nowMenuIndex==tdzf5){setData.chReverse[4] = !setData.chReverse[4];menu_tdzf5();}
		if(nowMenuIndex==tdzf6){setData.chReverse[5] = !setData.chReverse[5];menu_tdzf6();}
		if(nowMenuIndex==tdzf7){setData.chReverse[6] = !setData.chReverse[6];menu_tdzf7();}
		if(nowMenuIndex==tdzf8){setData.chReverse[7] = !setData.chReverse[7];menu_tdzf8();}
		if(nowMenuIndex==ymph) {setData.throttlePreference = !setData.throttlePreference;menu_ymph();}
		if(nowMenuIndex==dyjz) {setData.batVoltAdjust -= 1;menu_dyjz();}
		if(nowMenuIndex==bjdy) {setData.warnBatVolt -= 0.01;menu_bjdy();}
		if(nowMenuIndex==jsbj) {setData.RecWarnBatVolt -= 0.1;menu_jsbj();}
		if(nowMenuIndex==wtdw) 
		{
			setData.PWMadjustUnit -= 1;
			if(setData.PWMadjustUnit<1) {setData.PWMadjustUnit = 1;}//����΢����λ��Χ
			menu_wtdw();
		}
		if(nowMenuIndex==xzmx) {if(setData.modelType==0){setData.modelType=2;}else {setData.modelType -= 1;}menu_xzmx();}
		if(nowMenuIndex==wxfs) {setData.NRF_Mode =!setData.NRF_Mode;menu_wxfs();}
		if(nowMenuIndex==ppmsc) {setData.PPM_Out =!setData.PPM_Out;menu_ppmsc();}
		if(nowMenuIndex==ajyx) {setData.keySound =!setData.keySound;menu_ajyx();}
		if(nowMenuIndex==kjhm) {setData.onImage =!setData.onImage;menu_kjhm();}
		if(nowMenuIndex==nzbj) {setData.clockMode =!setData.clockMode;menu_nzbj();}
		if(nowMenuIndex==nzsc) 
		{
			setData.clockTime -= 1;
			if(setData.clockTime<1) {setData.clockTime=1;}//��������ʱ��
			menu_nzsc();
		}
		if(nowMenuIndex==kjzj) {setData.clockCheck =!setData.clockCheck;menu_kjzj();}
		if(nowMenuIndex==skbh) 
		{
			if(setData.throttleProtect<5) {setData.throttleProtect = 5;}//�������ű���ֵ
			setData.throttleProtect -= 5;
			menu_skbh();
		}
		if(nowMenuIndex==fsgl)
		{
			setData.NRF_Power -= 2;
			if(setData.NRF_Power<0x0b) {setData.NRF_Power=0x0b;}//���ƹ���11,13,15
			menu_fsgl();
		}
	}
	if(menuEvent[1]==MENU_enter)//��ת�������̰��󣬸ı�˵���ʾ
	{
		if(nowMenuIndex==tdwt1){menu_tdwt1();}
		if(nowMenuIndex==tdwt2){menu_tdwt2();}
		if(nowMenuIndex==tdwt3){menu_tdwt3();}
		if(nowMenuIndex==tdwt4){menu_tdwt4();}
		if(nowMenuIndex==tdwt5){menu_tdwt5();}
		if(nowMenuIndex==tdwt6){menu_tdwt6();}
		if(nowMenuIndex==tdwt7){menu_tdwt7();}
		if(nowMenuIndex==tdwt8){menu_tdwt8();}
		if(nowMenuIndex==tdzf1){menu_tdzf1();}
		if(nowMenuIndex==tdzf2){menu_tdzf2();}
		if(nowMenuIndex==tdzf3){menu_tdzf3();}
		if(nowMenuIndex==tdzf4){menu_tdzf4();}
		if(nowMenuIndex==tdzf5){menu_tdzf5();}
		if(nowMenuIndex==tdzf6){menu_tdzf6();}
		if(nowMenuIndex==tdzf7){menu_tdzf7();}
		if(nowMenuIndex==tdzf8){menu_tdzf8();}
		if(nowMenuIndex==ymph) {menu_ymph();}
		if(nowMenuIndex==dyjz) {menu_dyjz();}
		if(nowMenuIndex==bjdy) {menu_bjdy();}
		if(nowMenuIndex==wtdw) {menu_wtdw();}
		if(nowMenuIndex==xzmx) {menu_xzmx();}
		if(nowMenuIndex==wxfs) {menu_wxfs();}
		if(nowMenuIndex==ppmsc) {menu_ppmsc();}
		if(nowMenuIndex==ajyx) {menu_ajyx();}
		if(nowMenuIndex==kjhm) {menu_kjhm();}
		if(nowMenuIndex==nzbj) {menu_nzbj();}
		if(nowMenuIndex==nzsc) {menu_nzsc();}
		if(nowMenuIndex==kjzj) {menu_kjzj();}
		if(nowMenuIndex==jsbj) {menu_jsbj();}
		if(nowMenuIndex==skbh) {menu_skbh();}
		if(nowMenuIndex==fsgl) {menu_fsgl();}
	}
	if(nowMenuIndex!=lastMenuIndex)
	{
		STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)&setData,setDataSize);//���û�����д��FLASH
	}
	OLED_Refresh_Gram();//ˢ���Դ�
	menuEvent[0] = 0;
}
