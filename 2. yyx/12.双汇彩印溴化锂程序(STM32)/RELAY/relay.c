
#define _RELAY_C
#include "head.h"

//#define DBG_RELAY		//debug �����Ϣ����



#ifdef DBG_RELAY
INT8 dbg_relay_buf[128];
u32 dbg_relay_time;
u8 dbg_relay_flag;
#endif

#define OPEN_PAIWU_CONDITION    1
#define CLOSE_PAIWU_CONDITION   2

#define REMOTE_OPEN_PAIWU_FLAG       1
#define REMOTE_CLOSE_PAIWU_FLAG    2

//-------------------------------------------------------------
u8 Jiayao1_start_flag;	//��
u8 Jiayao2_start_flag;	//��
u8 Jiayao1_hand_flag;	//��
u8 Jiayao2_hand_flag;	//��


u8 LL_Jiayao3_start_flag;	//������������ÿ�ν���ʱҪ��һ��λ��
u8 LL_Jiayao4_start_flag;	//������������ÿ�ν���ʱҪ��һ��λ��
u8 Jiayao5_start_flag;	//
u8 Jiayao6_start_flag;	//
u8 Jiayao7_start_flag;	//
u8 Jiayao5_hand_flag;	//��
u8 Jiayao6_hand_flag;	//��

//������1
u32 jiayao1_zhouqi;		//����������ʱ��
u32 jiayao1_cur_time;	//�����õ�ǰ����
//������2
u32 jiayao2_zhouqi;		//����������ʱ��
u32 jiayao2_cur_time;	//�����õ�ǰ����
//������3
u32 jiayao3_zhouqi;		//����������ʱ��
u32 jiayao3_cur_time;	//�����õ�ǰ����
//������4
u32 jiayao4_zhouqi;		//����������ʱ��
u32 jiayao4_cur_time;	//�����õ�ǰ����
//������5
u32 jiayao5_zhouqi;		//����������ʱ��
u32 jiayao5_cur_time;	//�����õ�ǰ����
//������6
u32 jiayao6_zhouqi;		//����������ʱ��
u32 jiayao6_cur_time;	//�����õ�ǰ����
//������7
u32 jiayao7_zhouqi;		//����������ʱ��
u32 jiayao7_cur_time;	//�����õ�ǰ����

//-------------------------------------------------------------





//---------------------------------------------------------
//hyman2011-04-20
void JiaYao1(void);
void JiaYao2(void);
void JiaYao3(void);
void JiaYao4(void);

void JiaYao5(void);
void JiaYao6(void);

void JiaYao7(void);



// "3#������ֵ����1#�����ù�����ҩ����ֵ���ڸ���ʱ��ʼ��ҩ����ֵ���ڵ���ʱֹͣ��ҩ��
// 1#��ҩ��Һλ��ʱ����ҩ��
// ��ѹ����ʱ����ҩ
// �ֶ�����ʱ���������趨��ҩ�ٶ�
// ��ҩ�ٶȵļ��㣺������ڸ��ޣ�������ٶȼ�ҩ��100��/�֣�
// ���ڸ��޺󣬣���ǰֵ-����ֵ��/(����ֵ-����ֵ�� * 100"




 
void Init_Relay(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
	//  paiwu_counter=500;
	Jiayao1_start_flag = 0;
	Jiayao2_start_flag = 0;
	Jiayao1_hand_flag = 0;
	Jiayao2_hand_flag = 0;
	LL_Jiayao3_start_flag = 0;	
	LL_Jiayao4_start_flag = 0;

    //4��������Ĭ�ϳ��Զ���ҩģʽ��Ҫ�����ֶ��ļ�ҩ�ٶȣ�����Ҫ����Ȩ�޻�Զ��GPRS���ġ�
    ResetBit_Yx(B_ZJ_JIAYAO1_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO2_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO3_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO4_REMOTE);
	ResetBit_Yx(B_ZJ_JIAYAO5_REMOTE);
    ResetBit_Yx(B_ZJ_JIAYAO6_REMOTE);
}



void ProLogic(void)
{ 

    JiaYaoLogic();//	 

}



void SetBit_Yx(u8 bit)
{
	u8 yx_num;
	u8 yx_bit;
	
	yx_num = bit/16;
	yx_bit = bit%16;
	
	yx_buf[yx_num] |= (1<<yx_bit);
	
}
void ResetBit_Yx(u8 bit)
{
	u8 yx_num;
	u8 yx_bit;
	yx_num = bit/16;
	yx_bit = bit%16;
	yx_buf[yx_num] &= (~(1<<yx_bit));
}

u8 GetBit_Yx(u8 bit)
{
	if (yx_buf[bit/16] & (1<<(bit%16)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


// �̵������
void OutJdq(void)
{
	//1
	if (GetBit_Yx(B_ZJ_JDQ_1))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_6);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_6);
	}
	//2
	if (GetBit_Yx(B_ZJ_JDQ_2))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_7);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	}
	//3
	if (GetBit_Yx(B_ZJ_JDQ_3))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	//4
	if (GetBit_Yx(B_ZJ_JDQ_4))
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_9);
	}
	//5
	if (GetBit_Yx(B_ZJ_JDQ_5))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_12);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_12);
	}
	//6
	if (GetBit_Yx(B_ZJ_JDQ_6))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_13);
	}
	//7
	if (GetBit_Yx(B_ZJ_JDQ_7))
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_15);
	}
	else
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	}
	//8
	if (GetBit_Yx(B_ZJ_JDQ_8))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_3);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
	}
	//9
	if (GetBit_Yx(B_ZJ_JDQ_9))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_4);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_4);
	}
	//10
	if (GetBit_Yx(B_ZJ_JDQ_10))
	{
		GPIO_SetBits(GPIOD,GPIO_Pin_5);
	}
	else
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_5);
	}
}


void JiaYao1(void)
{
	u32 temp_k;
	u16 temp2;

	


	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//����1#��ҩ��
	if (!(GetBit_Yx(B_ZJ_JIAYAO1_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI)
			|| (yc_buf[ZJ_AI_MST_3_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//��ҩֹͣʱ��Ҫ��Ϊ��LOAD SYS_TICK ��������ҩλ��0.
			Jiayao1_start_flag = 0;
			
			// ����0ʱ������ֹͣ�����ù������˴���Ҫ�ѱ�ֹͣ��
			yc_buf[ZJ_AI_JIAYAO_SUDU_1] = 0;
			
		}
		//------------------------------------------------------------------------
		//��ʼ��ҩ����
		//��ʱ�����������ã����ٸ��ݲ�ֵ���С�
		else if (yc_buf[ZJ_AI_MST_3_VAL] > yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3])
		{
			if (Jiayao1_start_flag == 0)
			{	
				Jiayao1_start_flag = 1;
				
				jiayao1_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_MST_3_VAL];
		}

		Jiayao1_hand_flag = 0;
			
		temp_k = 0;
		//������ڼ�ҩ״̬����ʼ�����ҩ�ٶ�
		if (1 == Jiayao1_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3]) * 100/(yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_3] - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_3]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = temp_k;
		
		
		
	}
	else	// ������ֶ�״̬�£�Ҫʹ�ú�̨�趨�ļ�ҩ�ٶ�
	{	
		if (Jiayao1_hand_flag == 0)
		{
			Jiayao1_hand_flag = 1;
			
			jiayao1_cur_time = sys_tick;
		}

		Jiayao1_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_1];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//���濪ʼ����ÿ������Ҫ�Ӷ��ٴν�����Ӧ����
	// 60�룬60000MS,12000(5MS).ÿ��������6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_1])
	{
		jiayao1_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_1];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao1_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//1#��
	if (jiayao1_zhouqi)
	{
		if ((sys_tick - jiayao1_cur_time ) >= jiayao1_zhouqi)
		{
			jiayao1_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO1_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_1))
				{
					//��� �бù����ˣ��ͼ�¼���ҩҺ��
					float *p;
					u16 temp;
					
					jlb1_need_times += 0.48;
					
					//ͬʱ���µ�YC��ȥ��hyman2011-12-21
					p = (float*)&yc_buf[52];
					*p = jlb1_need_times;
					
					temp = yc_buf[53] ;
					yc_buf[53] = yc_buf[52];
					yc_buf[52] = temp;						

					ResetBit_Yx(B_ZJ_JDQ_1);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_1);

				}
			}			
		}		
	}
	
	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI) 								 
		)		
	{
		ResetBit_Yx(B_ZJ_JDQ_1);
		
		ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_1] = 0;
	}
	else
	{
		if (jiayao1_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);	
		}
	}	
	
}




void JiaYao2(void)
{
	u32 temp_k;
	u16 temp2;

	


	//for debug
	//yc_buf[ZJ_AI_MST_2_VAL] = 1100;
	
	//����1#��ҩ��
	if (!(GetBit_Yx(B_ZJ_JIAYAO2_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI)
			|| (yc_buf[ZJ_AI_MST_2_VAL] < yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//��ҩֹͣʱ��Ҫ��Ϊ��LOAD SYS_TICK ��������ҩλ��0.
			Jiayao2_start_flag = 0;
			
			// ����0ʱ������ֹͣ�����ù������˴���Ҫ�ѱ�ֹͣ��
			yc_buf[ZJ_AI_JIAYAO_SUDU_2] = 0;
			
		}
		//------------------------------------------------------------------------
		//��ʼ��ҩ����
		//��ʱ�����������ã����ٸ��ݲ�ֵ���С�
		else if (yc_buf[ZJ_AI_MST_2_VAL] > yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2])
		{
			if (Jiayao2_start_flag == 0)
			{	
				Jiayao2_start_flag = 1;
				
				jiayao2_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_MST_2_VAL];
		}

		Jiayao2_hand_flag = 0;
			
		temp_k = 0;
		//������ڼ�ҩ״̬����ʼ�����ҩ�ٶ�
		if (1 == Jiayao2_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2]) * 100/(yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_H_2] - yc_buf[ZJ_MODBUS_YC_ZHU_CGQ_L_2]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = temp_k;
		
		
		
	}
	else	// ������ֶ�״̬�£�Ҫʹ�ú�̨�趨�ļ�ҩ�ٶ�
	{	
		if (Jiayao2_hand_flag == 0)
		{
			Jiayao2_hand_flag = 1;
			
			jiayao2_cur_time = sys_tick;
		}

		Jiayao2_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_2];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//���濪ʼ����ÿ������Ҫ�Ӷ��ٴν�����Ӧ����
	// 60�룬60000MS,12000(5MS).ÿ��������6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_2])
	{
		jiayao2_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_2];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao2_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//1#��
	if (jiayao2_zhouqi)
	{
		if ((sys_tick - jiayao2_cur_time ) >= jiayao2_zhouqi)
		{
			jiayao2_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO2_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_2))
				{
					//��� �бù����ˣ��ͼ�¼���ҩҺ��
					float *p;
					u16 temp;
					
					jlb2_need_times += 0.48;
					
					//ͬʱ���µ�YC��ȥ��hyman2011-12-21
					p = (float*)&yc_buf[54];
					*p = jlb2_need_times;
					
					temp = yc_buf[55] ;
					yc_buf[55] = yc_buf[54];
					yc_buf[54] = temp;

					ResetBit_Yx(B_ZJ_JDQ_2);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_2);
				}
			}			
		}		
	}
	
	//��ͣ����
	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI) 								
		)
	{
		ResetBit_Yx(B_ZJ_JDQ_2);
		
		ResetBit_Yx(B_ZJ_JIAYAO2_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_2] = 0;
	}
	else
	{
		if (jiayao2_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO2_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO2_CONTROL);	
		}
	}
	
	
	
}



// ���ݲ�ˮ������3�ż�����ʵʱ��ҩ
// ����ÿ��ˮ��ҩ***������ʵʱ�����ҩ��ͬ������3#�����ã���Ӧ��Һλ��ʱ����ҩ

void JiaYao3(void)
{
	//Զ�̿���ʱ����ӦԶ���趨���ٶ�
	if (GetBit_Yx(B_ZJ_JIAYAO3_REMOTE))
	{
		//----------------------------------------------------------------------
			
		if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3])
		{
			jiayao3_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3];

			SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);

			if (LL_Jiayao3_start_flag == 0)
			{
				LL_Jiayao3_start_flag = 1;
				jiayao3_cur_time = sys_tick;
			}
            
            yc_buf[ZJ_AI_JIAYAO_SUDU_3] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3];
		}
		else
		{
			jiayao3_zhouqi = 0;
			LL_Jiayao3_start_flag = 0;

			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
		}		
		
		//3#��
		if (jiayao3_zhouqi)
		{
			if ((sys_tick - jiayao3_cur_time ) >= jiayao3_zhouqi)
			{
				jiayao3_cur_time = sys_tick;		
				
				//
				if (GetBit_Yx(B_ZJ_JIAYAO3_CONTROL))
				{
					if (GetBit_Yx(B_ZJ_JDQ_3))
					{
						//��� �бù����ˣ��ͼ�¼���ҩҺ��
						float *p;
						u16 temp;
						
						jlb3_need_times += 0.48;
						
						//ͬʱ���µ�YC��ȥ��hyman2011-12-21
						p = (float*)&yc_buf[0];
						*p = jlb3_need_times;
						
						temp = yc_buf[1] ;
						yc_buf[1] = yc_buf[0];
						yc_buf[0] = temp;

						//�ͼ����ֲᱣ��ͬ����Ϊ�˺���ǰ���ݣ����ٷŵ�һ���ط���
						yc_buf[56] = yc_buf[0];
						yc_buf[57] = yc_buf[1];
                        
						ResetBit_Yx(B_ZJ_JDQ_3);
                         
					}
					else
					{
						SetBit_Yx(B_ZJ_JDQ_3);
					}
				}			
			}		
		}
	}
	else	//�Զ�ʱ���������������ӡ�
	{
		if (LL_Jiayao3_start_flag == 1)
		{
			//���Ȱ��ϴεļ�ҩ���̽���
			if ((sys_tick - jiayao3_cur_time ) >= 50)
			{
				jiayao3_cur_time = sys_tick;
				
				if (GetBit_Yx(B_ZJ_JDQ_3))
				{
					ResetBit_Yx(B_ZJ_JDQ_3);	
				}
				else
				{
					LL_Jiayao3_start_flag = 0;
				}
			}	
		}
		else if ((jlb3_all_times - jlb3_need_times) >= 0.48)
		{
			//��ʼ��һ��ҩ
			//��ʼ��λ����״̬
			LL_Jiayao3_start_flag = 1;
			
			jiayao3_cur_time = sys_tick;
			
			// ��Ӧ��Һλ��ʱ����ҩ
			if (!(GetBit_Yx(B_ZJ_JIAYAO3_L)))
			{
				SetBit_Yx(B_ZJ_JDQ_3);	//�ø߼���������
				SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
			}					
			
			//��� �бù����ˣ��ͼ�¼���ҩҺ��
			if (GetBit_Yx(B_ZJ_JDQ_3))
			{ 
                float *p;
                u16 temp;
              
				jlb3_need_times += 0.48;
                
                //ͬʱ���µ�YC��ȥ��hyman2011-12-21
                p = (float*)&yc_buf[0];
                *p = jlb3_need_times;
                
                temp = yc_buf[1] ;
                yc_buf[1] = yc_buf[0];
                yc_buf[0] = temp;

				//�ͼ����ֲᱣ��ͬ����Ϊ�˺���ǰ���ݣ����ٷŵ�һ���ط���
				yc_buf[56] = yc_buf[0];
				yc_buf[57] = yc_buf[1];
			}		
		}
		else
		{
			//��
			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);		
		}
	}	

	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP))
	{
		ResetBit_Yx(B_ZJ_JDQ_3);
		
		ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_3] = 0;
	}
	else
	{
		if (LL_Jiayao3_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO3_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO3_CONTROL);	
		}
	}
}


// ���ݲ�ˮ������4�ż�����ʵʱ��ҩ
// ����ÿ��ˮ��ҩ***������ʵʱ�����ҩ��ͬ������3#��4#�����ã���Ӧ��Һλ��ʱ����ҩ

void JiaYao4(void)
{
    //for test
    //jlb4_need_times += 0.48;
  
	//Զ�̿���ʱ����ӦԶ���趨���ٶ�
	if (GetBit_Yx(B_ZJ_JIAYAO4_REMOTE))
	{
		//----------------------------------------------------------------------
		
		if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4])
		{
			jiayao4_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4];
			
			SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
			
			if (LL_Jiayao4_start_flag == 0)
			{
				LL_Jiayao4_start_flag = 1;
				jiayao4_cur_time = sys_tick;
			}
            
            yc_buf[ZJ_AI_JIAYAO_SUDU_4] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4];
		}
		else
		{
			jiayao4_zhouqi = 0;
			LL_Jiayao4_start_flag = 0;
			
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}		
		
		//4#��
		if (jiayao4_zhouqi)
		{
			if ((sys_tick - jiayao4_cur_time ) >= jiayao4_zhouqi)
			{
				jiayao4_cur_time = sys_tick;		
				
				//
				if (GetBit_Yx(B_ZJ_JIAYAO4_CONTROL))
				{
					if (GetBit_Yx(B_ZJ_JDQ_4))
					{
						//��� �бù����ˣ��ͼ�¼���ҩҺ��						
						float *p;
						u16 temp;
					  
						jlb4_need_times += 0.48;
						
						//ͬʱ���µ�YC��ȥ��hyman2011-12-21
						p = (float*)&yc_buf[2];
						*p = jlb4_need_times;
						
						temp = yc_buf[3] ;
						yc_buf[3] = yc_buf[2];
						yc_buf[2] = temp;

						//�ͼ����ֲᱣ��ͬ����Ϊ�˺���ǰ���ݣ����ٷŵ�һ���ط���
						yc_buf[58] = yc_buf[2];
						yc_buf[59] = yc_buf[3];
						
						ResetBit_Yx(B_ZJ_JDQ_4);
					}
					else
					{
						SetBit_Yx(B_ZJ_JDQ_4);
					}
				}			
			}		
		}
	}
	else	//�Զ�ʱ���������������ӡ�
	{
		
		if (LL_Jiayao4_start_flag == 1)
		{
			//���Ȱ��ϴεļ�ҩ���̽���
			if ((sys_tick - jiayao4_cur_time ) >= 50)
			{
				jiayao4_cur_time = sys_tick;
				
				if (GetBit_Yx(B_ZJ_JDQ_4))
				{
					ResetBit_Yx(B_ZJ_JDQ_4);	
				}
				else
				{
					LL_Jiayao4_start_flag = 0;
				}
			}	
		}
		else if ((jlb4_all_times - jlb4_need_times) >= 0.48)
		{
			//��ʼ��һ��ҩ
			//��ʼ��λ����״̬
			LL_Jiayao4_start_flag = 1;
			
			jiayao4_cur_time = sys_tick;
			
			// ��Ӧ��Һλ��ʱ����ҩ
			if (!(GetBit_Yx(B_ZJ_JIAYAO4_L)))
			{
				SetBit_Yx(B_ZJ_JDQ_4);	//�ø߼���������
				SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
			}				
			
			//��� �бù����ˣ��ͼ�¼���ҩҺ��
			if (GetBit_Yx(B_ZJ_JDQ_4))
			{
				float *p;
                u16 temp;
              
				jlb4_need_times += 0.48;
                
                //ͬʱ���µ�YC��ȥ��hyman2011-12-21
                p = (float*)&yc_buf[2];
                *p = jlb4_need_times;
                
                temp = yc_buf[3] ;
                yc_buf[3] = yc_buf[2];
                yc_buf[2] = temp;
			}		
		}
		else
		{
			//��		
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}
	}
    
    

	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP))
	{
		ResetBit_Yx(B_ZJ_JDQ_4);
		
		ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_4] = 0;
	}
	else
	{
		if (LL_Jiayao4_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO4_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO4_CONTROL);	
		}
	}
	
}

// 5#�������ܿ�����ˮ��3#������������ˮѹ����ʱֹͣ��ҩ
void JiaYao5(void)
{
	u32 temp_k;
	u16 temp2;

	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//����5#��ҩ��
	if (!(GetBit_Yx(B_ZJ_JIAYAO5_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGSHUI)			
			|| (yc_buf[ZJ_AI_SLV2_3_VAL] < yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//��ҩֹͣʱ��Ҫ��Ϊ��LOAD SYS_TICK ��������ҩλ��0.
			Jiayao5_start_flag = 0;
			
			// ����0ʱ������ֹͣ�����ù������˴���Ҫ�ѱ�ֹͣ��
			yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
			
		}
		//------------------------------------------------------------------------
		//��ʼ��ҩ����
		//��ʱ�����������ã����ٸ��ݲ�ֵ���С�
		else if (yc_buf[ZJ_AI_SLV2_3_VAL] > yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3])
		{
			if (Jiayao5_start_flag == 0)
			{	
				Jiayao5_start_flag = 1;
				
				jiayao5_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_SLV2_3_VAL];
		}

		Jiayao5_hand_flag = 0;
			
		temp_k = 0;
		//������ڼ�ҩ״̬����ʼ�����ҩ�ٶ�
		if (1 == Jiayao5_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3]) * 100/(yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_3] - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_3]);
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = temp_k;
		
		
		
	}
	else	// ������ֶ�״̬�£�Ҫʹ�ú�̨�趨�ļ�ҩ�ٶ�
	{	
		if (Jiayao5_hand_flag == 0)
		{
			Jiayao5_hand_flag = 1;
			
			jiayao5_cur_time = sys_tick;
		}

		Jiayao5_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_5];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//���濪ʼ����ÿ������Ҫ�Ӷ��ٴν�����Ӧ����
	// 60�룬60000MS,12000(5MS).ÿ��������6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_5])
	{
		jiayao5_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_5];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao5_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//5#��
	if (jiayao5_zhouqi)
	{
		if ((sys_tick - jiayao5_cur_time ) >= jiayao5_zhouqi)
		{
			jiayao5_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO5_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_5))
				{
					//��� �бù����ˣ��ͼ�¼���ҩҺ��
					float *p;
					u16 temp;
					
					jlb5_need_times += 0.48;
					
					//ͬʱ���µ�YC��ȥ��hyman2011-12-21
					p = (float*)&yc_buf[60];
					*p = jlb5_need_times;
					
					temp = yc_buf[61] ;
					yc_buf[61] = yc_buf[60];
					yc_buf[60] = temp;

					ResetBit_Yx(B_ZJ_JDQ_5);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_5);
				}
			}			
		}		
	}
	

	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) 
		
		)
	
	{
		ResetBit_Yx(B_ZJ_JDQ_5);
		
		ResetBit_Yx(B_ZJ_JIAYAO5_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
	}
	else
	{
		if (jiayao5_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO5_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO5_CONTROL);	
		}
	}	
	
}

// 6#�������ܿ�����ˮ��2#��������
void JiaYao6(void)
{
	u32 temp_k;
	u16 temp2;

	//for debug
	//yc_buf[ZJ_AI_MST_3_VAL] = 1100;
	
	//����6#��ҩ��
	if (!(GetBit_Yx(B_ZJ_JIAYAO6_REMOTE)))
	{
		//------------------------------------------------------------------------
		//
		if (GetBit_Yx(B_ZJ_YALI_L_LENGSHUI)
			|| (yc_buf[ZJ_AI_SLV2_2_VAL] < yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2])
			)
			
		{			
			// ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			//��ҩֹͣʱ��Ҫ��Ϊ��LOAD SYS_TICK ��������ҩλ��0.
			Jiayao6_start_flag = 0;
			
			// ����0ʱ������ֹͣ�����ù������˴���Ҫ�ѱ�ֹͣ��
			yc_buf[ZJ_AI_JIAYAO_SUDU_6] = 0;
			
		}
		//------------------------------------------------------------------------
		//��ʼ��ҩ����
		//��ʱ�����������ã����ٸ��ݲ�ֵ���С�
		else if (yc_buf[ZJ_AI_SLV2_2_VAL] > yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2])
		{
			if (Jiayao6_start_flag == 0)
			{	
				Jiayao6_start_flag = 1;
				
				jiayao6_cur_time = sys_tick;
			}
			
			// SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
			
			temp2 = yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2]; 
		}
		else
		{
			temp2 = yc_buf[ZJ_AI_SLV2_2_VAL];
		}

		Jiayao6_hand_flag = 0;
			
		temp_k = 0;
		//������ڼ�ҩ״̬����ʼ�����ҩ�ٶ�
		if (1 == Jiayao6_start_flag)
		{
			temp_k = (temp2 - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2]) * 100/(yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_H_2] - yc_buf[ZJ_MODBUS_YC_SLV2_CGQ_L_2]);//����д����L_3
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = temp_k;
		
		
		
	}
	else	// ������ֶ�״̬�£�Ҫʹ�ú�̨�趨�ļ�ҩ�ٶ�
	{	
		if (Jiayao6_hand_flag == 0)
		{
			Jiayao6_hand_flag = 1;
			
			jiayao6_cur_time = sys_tick;
		}

		Jiayao6_start_flag = 0;

		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_6];		
	
	}
	
	
	
	//----------------------------------------------------------------------
	//���濪ʼ����ÿ������Ҫ�Ӷ��ٴν�����Ӧ����
	// 60�룬60000MS,12000(5MS).ÿ��������6000TICK
	
	//for debug
	//ad_measure[ZJ_AI_JIAYAO_SUDU_1].ad_value = 60;

	if (yc_buf[ZJ_AI_JIAYAO_SUDU_6])
	{
		jiayao6_zhouqi = 6000 / yc_buf[ZJ_AI_JIAYAO_SUDU_6];

		//SetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	else
	{
		jiayao6_zhouqi = 0;

		//ResetBit_Yx(B_ZJ_JIAYAO1_CONTROL);
	}
	
	

	//6#��
	if (jiayao6_zhouqi)
	{
		if ((sys_tick - jiayao6_cur_time ) >= jiayao6_zhouqi)
		{
			jiayao6_cur_time = sys_tick;		

			//
			if (GetBit_Yx(B_ZJ_JIAYAO6_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_6))
				{
					//��� �бù����ˣ��ͼ�¼���ҩҺ��
					float *p;
					u16 temp;
					
					jlb6_need_times += 0.48;
					
					//ͬʱ���µ�YC��ȥ��hyman2011-12-21
					p = (float*)&yc_buf[62];
					*p = jlb6_need_times;
					
					temp = yc_buf[63] ;
					yc_buf[63] = yc_buf[62];
					yc_buf[62] = temp;

					ResetBit_Yx(B_ZJ_JDQ_6);
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_6);
				}
			}			
		}		
	}
	
	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP) || GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) )
	{
		ResetBit_Yx(B_ZJ_JDQ_6);
		
		ResetBit_Yx(B_ZJ_JIAYAO6_CONTROL);	
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_6] = 0;
	}
	else
	{
		if (jiayao6_zhouqi)
		{
			SetBit_Yx(B_ZJ_JIAYAO6_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO6_CONTROL);	
		}
	}	
	
}

void JiaYao7(void)
{
	
		
	if (yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7])
	{
		jiayao7_zhouqi = 6000 / yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7];
		
		SetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
		
		if (Jiayao7_start_flag == 0)
		{
			Jiayao7_start_flag = 1;
			jiayao7_cur_time = sys_tick;
		}
		
		yc_buf[ZJ_AI_JIAYAO_SUDU_7] = yc_buf[ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7];
	}
	else
	{
		jiayao7_zhouqi = 0;
		Jiayao7_start_flag = 0;
		
		ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
	}		
	
	//7#��
	if (jiayao7_zhouqi)
	{
		if ((sys_tick - jiayao7_cur_time ) >= jiayao7_zhouqi)
		{
			jiayao7_cur_time = sys_tick;		
			
			//
			if (GetBit_Yx(B_ZJ_JIAYAO7_CONTROL))
			{
				if (GetBit_Yx(B_ZJ_JDQ_9))
				{
					//��� �бù����ˣ��ͼ�¼���ҩҺ��
					float *p;
					u16 temp;
					
					jlb7_need_times += 0.48;
					
					//ͬʱ���µ�YC��ȥ��hyman2011-12-21
					p = (float*)&yc_buf[64];
					*p = jlb7_need_times;
					
					temp = yc_buf[65] ;
					yc_buf[65] = yc_buf[64];
					yc_buf[64] = temp;

					ResetBit_Yx(B_ZJ_JDQ_9);					
				}
				else
				{
					SetBit_Yx(B_ZJ_JDQ_9);
				}
			}			
		}		
	}
	
	//��ͣ����
	if (GetBit_Yx(B_ZJ_STOP) || (	GetBit_Yx(B_ZJ_YALI_L_LENGSHUI) 
									&& GetBit_Yx(B_ZJ_YALI_L_LENGQUESHUI2)									
									) 
		)
	{
		ResetBit_Yx(B_ZJ_JDQ_7);
		
		ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);	
		
		//yc_buf[ZJ_AI_JIAYAO_SUDU_5] = 0;
	}
	else
	{
		if (Jiayao7_start_flag)
		{
			SetBit_Yx(B_ZJ_JIAYAO7_CONTROL);
		}
		else
		{
			ResetBit_Yx(B_ZJ_JIAYAO7_CONTROL);	
		}
	}
	
}



void JiaYaoLogic(void)
{    
	JiaYao1();
	JiaYao2();
	JiaYao3();    
    JiaYao4(); 
	
	JiaYao5();    
    JiaYao6(); 
	
	JiaYao7(); 
}

