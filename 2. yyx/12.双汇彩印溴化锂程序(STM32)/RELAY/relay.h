#ifndef _RELAY_H
#define _RELAY_H

#ifdef _RELAY_C
#define EXT_RELAY
#else
#define EXT_RELAY	extern
#endif	  

//-----------------------------------------------------------------------------------------
//Ϊ����������



//#define   FOR_NANCHANG
//#define   FOR_JIYUAN
//#define   FOR_TEST   //���ԣ������е�485�ڶ����MODBUS-RTU
//#define   FOR_5500T  //5500�֣������˵�5���̵�������ҩ�ٶȿ��趨
//#define FOR_JIYUAN      //��Դ
//#define   FOR_ROUZHIPIN     //�������Ʒ��˾
#define FOR_CAIYIN_XHL
    

#ifdef FOR_NANCHANG
#define VER       "V2.10"
#define VER_DATA  "2011-09-068"
#define PRO_NAME  "�ϲ� ������"
#endif

#ifdef FOR_JIYUAN
#define VER       "V2.10"
#define VER_DATA  "2011-08-18"
#define PRO_NAME  "��Դ ������"
#endif

#ifdef  FOR_5500T
#define VER       "V2.10"
#define VER_DATA  "2011-08-18"
#define PRO_NAME  "5500T ������"
#endif


#ifdef  FOR_TEST
#define VER       "V2.10"
#define VER_DATA  "2011-08-18"
#define PRO_NAME  "���� ������"
#endif

#ifdef FOR_ROUZHIPIN  //����Ʒ
#define VER       "V2.D"
#define VER_DATA  "2011-12-23"
#define PRO_NAME  "����Ʒ ������"
#endif

#ifdef FOR_CAIYIN_XHL  //����Ʒ
#define VER       "V2.7"
#define VER_DATA  "2012-03-6"
#define PRO_NAME  "˫���ӡ �廯�"
#endif

//-----------------------------------------------------------------------------------------
//YXλͳһ��ַ���£�
#define B_ZJ_YW_L             2

//-------------------------------------------------
// #ifdef FOR_NANCHANG
// #define B_ZJ_JIAYAO1_L			4
// #define B_ZJ_JIAYAO2_L			5
// #define B_ZJ_JIAYAO3_L			6
// #define B_ZJ_JIAYAO4_L			7
// #define B_ZJ_PAIWU_OPENED       9
// #define B_ZJ_PAIWU_CLOSED       10
// #endif

// #ifdef FOR_JIYUAN
// #define B_ZJ_JIAYAO1_L			5
// #define B_ZJ_JIAYAO2_L			6
// #define B_ZJ_JIAYAO3_L			7
// #define B_ZJ_JIAYAO4_L			8
// #define B_ZJ_PAIWU_OPENED       9
// #define B_ZJ_PAIWU_CLOSED       10
// #endif


// #ifdef FOR_TEST
// #define B_ZJ_JIAYAO1_L			5
// #define B_ZJ_JIAYAO2_L			6
// #define B_ZJ_JIAYAO3_L			7
// #define B_ZJ_JIAYAO4_L			8
// #define B_ZJ_PAIWU_OPENED       9
// #define B_ZJ_PAIWU_CLOSED       10
// #endif

// #ifdef FOR_5500T
// #define B_ZJ_JIAYAO1_L			5
// #define B_ZJ_JIAYAO2_L			6
// #define B_ZJ_JIAYAO3_L			7
// #define B_ZJ_JIAYAO4_L			8
// #define B_ZJ_PAIWU_OPENED       9
// #define B_ZJ_PAIWU_CLOSED       10
// #endif

// #ifdef FOR_ROUZHIPIN
// #define B_ZJ_JIAYAO1_L			5
// #define B_ZJ_JIAYAO2_L			6
// #define B_ZJ_JIAYAO3_L			7
// #define B_ZJ_JIAYAO4_L			8
// #define B_ZJ_PAIWU_OPENED       9
// #define B_ZJ_PAIWU_CLOSED       10
// #endif
//ÿһ�������ֳ����Ա�׼��������ƹ���߷�ʽ����
#define B_ZJ_JIAYAO1_L			4
#define B_ZJ_JIAYAO2_L			5
#define B_ZJ_JIAYAO3_L			6
#define B_ZJ_JIAYAO4_L			7
//
#define B_ZJ_PAIWU_OPENED       10
#define B_ZJ_PAIWU_CLOSED       11
//-------------------------------------------------

#define B_ZJ_YALI_L             12

//�ֳ��ļ���ѹ���ź�
#define B_ZJ_YALI_L_LENGSHUI             12
#define B_ZJ_YALI_L_LENGQUESHUI          13

#define B_ZJ_YALI_L_LENGSHUI2             12
#define B_ZJ_YALI_L_LENGQUESHUI2          13

#define B_ZJ_JIAYAO1_REMOTE		27	//Զ�̿���״̬
#define B_ZJ_JIAYAO2_REMOTE		28	//Զ�̿���״̬
#define B_ZJ_JIAYAO3_REMOTE		29	//Զ�̿���״̬
#define B_ZJ_JIAYAO4_REMOTE		30	//Զ�̿���״̬
#define B_ZJ_JIAYAO5_REMOTE		26	//Զ�̿���״̬
#define B_ZJ_JIAYAO6_REMOTE		31	//Զ�̿���״̬

#define B_ZJ_JIAYAO1_CONTROL	32		//������1��������
#define B_ZJ_JIAYAO2_CONTROL	33		//������2��������
#define B_ZJ_JIAYAO3_CONTROL	34		//������3��������
#define B_ZJ_JIAYAO4_CONTROL	35		//������4��������
#define B_ZJ_JIAYAO5_CONTROL	36		//������5��������
#define B_ZJ_JIAYAO6_CONTROL	37		//������6��������
#define B_ZJ_JIAYAO7_CONTROL	38		//������7��������

#define B_ZJ_STOP				43		//��ͣ
#define B_ZJ_PAIWU_OPEN			44		//�ֶ�������
#define B_ZJ_PAIWU_CLOSE		45		//�ֶ�������
#define B_ZJ_PAIWU_OPEN_FAILT   46      //���ۿ�ʧ��
#define B_ZJ_PAIWU_CLOSE_FAILT  47      //���۹�ʧ��

//�̵���ӳ��λ
#define B_ZJ_JDQ_1				48		// ������1ӳ��
#define B_ZJ_JDQ_2				49		// ������2ӳ��
#define B_ZJ_JDQ_3				50		// ������3ӳ��
#define B_ZJ_JDQ_4				51		// ������4ӳ��
#define B_ZJ_JDQ_5				52		// 
#define B_ZJ_JDQ_6				53		// 
#define B_ZJ_JDQ_7				54		// 
#define B_ZJ_JDQ_8				55		// 
#define B_ZJ_JDQ_9				56		// ����1ӳ��
#define B_ZJ_JDQ_10				57		// ����2ӳ��

//-----------------------------------------------------------------------------------------



/************************��־**************************/
#define OBJECT_RUNNING                               2
#define OBJECT_CLOSE                                 0
#define OBJECT_OPEN                                  1


/************************Һ��������������**************************/

#define LCD_OPEN_JIAYAO1                              0
#define LCD_OPEN_JIAYAO2                             1
#define LCD_OPEN_JIAYAO3                             2
#define LCD_OPEN_JIAYAO4                             3
#define LCD_OPEN_PAIWU                              4
#define LCD_CLOSE_PAIWU                             5

/************************�̵�����������**************************/

#define OPEN_JIAYAO1                              0
#define OPEN_JIAYAO2                             1
#define OPEN_JIAYAO3                             2
#define OPEN_JIAYAO4                             3
//#define OPEN_BUSHUI                             6
#define OPEN_WARNING                            7
#define OPEN_PAIWU                              8
#define CLOSE_PAIWU                             9


/*******ң������,��yx��������ʵ��λ��Ҫ����YK_OFFSET
***************/
#define JIAYAO1_CONTROL                       0
#define JIAYAO2_CONTROL                       1
#define JIAYAO3_CONTROL                       2
#define JIAYAO4_CONTROL                       3
#define WARNING_CONTROL                       7
#define OPEN_PAIWU_CONTROL                    8
#define CLOSE_PAIWU_CONTROL                   9
#define AUTO_MANNUAL_CONTROL                  10
#define KO_EM_STOP                            11
#define REMOTE_MANNUAL_KPAIWU                  12
#define REMOTE_MANNUAL_GPAIWU                  13


#define PULSE_MIN_TIME                          8
#define PULSE_FLUX2_TIME                         60
#define PULSE_FLUX3_TIME                         120
#define KO0                                     (1<<0)
#define KO1                                     (1<<1)
#define KO2                                     (1<<2)
#define KO3                                     (1<<3)
#define KO4                                     (1<<4)
#define KO5                                     (1<<5)
#define KO6                                     (1<<6)
#define KO7                                     (1<<7)
#define KO8                                     (1<<8)
#define KO9                                     (1<<9)
//#define ATUO                                     (1<<10)
#define M_KO11                                   (1<<11)//����ͣ����


#define GPAIWU_TIME                              10
#define KPAIWU_TIME                              10
#define KGPAIWU_TIME                              13
#define WARNING_PULSE                              1


// #define GET_KO0()                                     GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15)
// #define GET_KO1()                                     GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_10)
// #define GET_KO2()                                     GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_11)
// #define GET_KO3()                                     GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_12)
// #define GET_KO4()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_0)
// #define GET_KO5()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_1)
// #define GET_KO6()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2)
// #define GET_KO7()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_3)
// #define GET_KO8()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_4)
// #define GET_KO9()                                     GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_5)

//���ڿ��ǵ����ڼ�ҩ�̵���Ϊ�����źţ��ں�̨�ϵĿ��ؿ���ʱҲ��˸���ʼ�4����־
// EXT_RELAY u8 remote_jiayao1_flag;//
// EXT_RELAY u8 remote_jiayao2_flag;//
// EXT_RELAY u8 remote_jiayao3_flag;//
// EXT_RELAY u8 remote_jiayao4_flag;//
//EXT_RELAY u8 paiwu_flag;//���ڿ�ʼ����ʱ�ñ꣬��ʼ��ʱ�ı�־
EXT_RELAY u8 e_stop;//���ڿ�ʼ����ʱ�ñ꣬��ʼ��ʱ�ı�־
EXT_RELAY vu16 temp_paiwu_time;
//EXT_RELAY u16 ko_buf,ko_OldBuf;
//EXT_RELAY u8 ko_change_flag;//����Һ����ʾ�жϱ�־


EXT_RELAY u8 flux_zero_flag;//����Ϊ0��־
EXT_RELAY u8 paiwu_running_flag;



EXT_RELAY vu32 kgpaiwu_fail_count;
EXT_RELAY vu8 kgpaiwu_fail_flag;

EXT_RELAY u8 lcd_bushui_flag;//Һ���ֶ�ֹͣ���۱�־
EXT_RELAY void ProLogic(void);
EXT_RELAY u8 auto_flag;//�������Զ����з�ʽʱ��־
EXT_RELAY u8 remote_auto_flag;//��ͨ��ͨ�Ŵ����Զ����з�ʽʱ��־
EXT_RELAY u8 lcd_auto_flag;//��ͨ��Һ�������Զ����з�ʽʱ��־
EXT_RELAY u8 running_mode;//�Զ����ֶ�
//EXT_RELAY u8 remote_relay_flag;
EXT_RELAY u8 auto_paiwu_flag;
//EXT_RELAY u8 yk_group;
EXT_RELAY u16 relay_pulse[10];


EXT_RELAY u32 gpaiwu_time;
EXT_RELAY u32 kpaiwu_time;


EXT_RELAY vu16 sys_counter;
EXT_RELAY vu32 sys_second;
EXT_RELAY vu32 quarter_sys_second;

EXT_RELAY u8 relay_map_state[10];//
EXT_RELAY u8 lcd_ctrl_flag[10];//
EXT_RELAY u8 control_object_state[10];//

//------------------------------------------------------------------------------
//hyman2011-4
EXT_RELAY vu32 sys_tick;  //hyman.��5MSΪ��λ�ۼ�






//������ʹ�ô�������������YX����λ�޸ġ�
EXT_RELAY void SetBit_Yx(u8 bit);
EXT_RELAY void ResetBit_Yx(u8 bit);
EXT_RELAY u8 GetBit_Yx(u8 bit);

EXT_RELAY void OutJdq(void);
//------------------------------------------------------------------------------

EXT_RELAY void Init_Relay(void);
EXT_RELAY void En_All_Relay(void);
EXT_RELAY void Dis_All_Relay(void);
EXT_RELAY void Control_Relay(void);
EXT_RELAY void auto_manual_flag(void);
EXT_RELAY void change_relay_mode(void);
EXT_RELAY void setall_relay_mode(void);
EXT_RELAY void resetall_relay_mode(void);
EXT_RELAY void JiaYaoLogic(void);
EXT_RELAY void PaiWuLogic(void);
EXT_RELAY void PaiWu_State_Process(void);
EXT_RELAY void Remote_Flag_Process(void);

EXT_RELAY void lcd_control_logic(void);
EXT_RELAY void get_object_state(void);


#endif /* __UART_H */


