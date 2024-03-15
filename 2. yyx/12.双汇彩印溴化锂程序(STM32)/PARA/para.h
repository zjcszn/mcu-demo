#ifndef _PARA_H
#define _PARA_H

#ifndef _PARA_C
#define EXT_PARA extern 
#else
#define EXT_PARA
#endif

#define NUM_PARA        80
//������������
typedef union 
{	
  struct
  {
        UINT16 bak;
		UINT16 addr;			//������ַ
        
         u16 a_jiayao_time;      //A��ҩʱ���趨
        u16 b_jiayao_time;      //b��ҩʱ���趨           
        u16 jiayao_time_3;      //3�Ź޼�ҩʱ���趨
		u16 jiayao_time_4;      //4�Ź޼�ҩʱ���趨
        
        
        u16 one_h;               
        u16 one_l;               
        u16 two_h;              
        u16 two_l;
        u16 three_h;      
        u16 three_l;      
        
        u16 jianyao_value_3;    //3#������ÿ��ˮ��ҩ��
        u16 jianyao_value_4;    //4#������ÿ��ˮ��ҩ��
        u16 bak3;
        
        u16 pw_single_times;        //һ������ʱ����S
        u16 pw_loop_min_times;      //��С��������,S
        
        
		UINT16 password;        //����
        u16 jiankong_commpara_buf[4];
        u16 gprs_commpara_buf[4];
        

/***********10 WORK_PARA_OFFSET***************/
       
        
        u16 max_liuliang;//�������//28
        u16 max_ph;//���phֵ
        u16 max_orp;
        
        u16 fulx;               //�����趨����ʼ��ҩʱ������


		/***********18 OFFSET***************/
        u16 paiwu_time;         //����ʱ����趨      
        u16 paiwu_gap_time;  //�೤ʱ����һ����

		u16 orp_h;               
        u16 orp_l;               
        u16 ph_h;              
        u16 ph_l;
        u16 conductance_h;      
        u16 conductance_l; 
        
        /***********28ϵ��ƫ��***************/      
        u16 orp_proportion;
        u16 fulx_proportion;
        u16 by2_proportion;
        u16 by3_proportion;
        u16 by_proportion;
        u16 water_proportion;
        
        /***********34***************/
        u16 auto_max_pulse;
        

        u16 max_temperature;
        u16 max_conductance;
        u16 cond_paiwu_h;//���ڵ絼�߿�ʼ���۵���ֵ
        
        u16 auto_mode;
        
        u16 sensor_commpara_buf[5];
  }value;        
  u16 value_group[NUM_PARA];        
}PARA;

// EXT_PARA u16 temp_local_addr;
// EXT_PARA u16 temp_local_password;

EXT_PARA PARA para;
EXT_PARA PARA para_temp;

EXT_PARA u8 para_enable_flag;  //������Ч��־��������´������󣬲����ᱻ���ǵ���Ҫ��ʾ������Ա�������ò����������ǵ�ַ��

EXT_PARA u8 change_para_type_flag;
EXT_PARA u8 comm_para_change_flag;

//����������Һ��������ֵ������̻�ʱ���ڲ����޸Ĺ����У����Ĺ��Ĳ������浽
//����λ�ã����̻�ʱ����ֵŲ����Ӧ�ı������У���Ҫע����ǣ���ÿ���޸Ĳ���ǰ��
//��Ҫ�����ʼ����������change_para_type_flag��Ӧ�ı�־


EXT_PARA void Load_TmepPara_Buf(void);

EXT_PARA UINT8 para_need_save_flag;
EXT_PARA void InitPara(void);
EXT_PARA void SavePara(void); 
#endif
