#ifndef _MEASURE_
#define _MEASURE_

#ifndef _MEASURE_C
#define EXT_MEASURE extern
#else 
#define EXT_MEASURE
#endif
/**********************/


//Ӧ�õ�������ϵͳʱ������ֵ�����YC��������λ�ö���
//����YC���е����ݡ�ʵ�ʸ�MODBUS �йء�
//---------START
#define ZJ_AI_BUSHUI       0   //�ܲ�ˮ��
#define ZJ_AI_JIAYAO	   2   //�ܼ�ҩ��
#define ZJ_AI_MST_1_VAL     4   //��������1#ֵ
#define ZJ_AI_MST_2_VAL     5   //��������2#ֵ
#define ZJ_AI_MST_3_VAL     6   //��������3#ֵ

#define ZJ_AI_SLV_1_VAL     7   //�Ӵ�����1#ֵ
#define ZJ_AI_SLV_2_VAL     8   //�Ӵ�����2#ֵ
#define ZJ_AI_SLV_3_VAL     9   //�Ӵ�����3#ֵ

#define ZJ_AI_LL			10	//����ֵ

#define ZJ_AI_PAIWU_STATE	11	//���۷�����״̬

#define ZJ_AI_JIAYAO_SUDU_1	12	//��ҩƵ��1#
#define ZJ_AI_JIAYAO_SUDU_2	13  //��ҩƵ��2#
#define ZJ_AI_JIAYAO_SUDU_3	14	//��ҩƵ��1#
#define ZJ_AI_JIAYAO_SUDU_4	15  //��ҩƵ��2#
#define ZJ_AI_JIAYAO_SUDU_5	49  //��ҩƵ��2#
#define ZJ_AI_JIAYAO_SUDU_6	50	//��ҩƵ��1#
#define ZJ_AI_JIAYAO_SUDU_7	51  //��ҩƵ��2#


#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_1	16	//��̨�趨�ļ�ҩ�ٶ�
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_2	17	
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_3	18
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_4	19
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_5	28
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_6	43
#define ZJ_MODBUS_YC_SET_JIAYAO_SUDU_7	44

#define ZJ_MODBUS_YC_ZHU_CGQ_H_1 20
#define ZJ_MODBUS_YC_ZHU_CGQ_L_1 21
#define ZJ_MODBUS_YC_ZHU_CGQ_H_2 22
#define ZJ_MODBUS_YC_ZHU_CGQ_L_2 23
#define ZJ_MODBUS_YC_ZHU_CGQ_H_3 24
#define ZJ_MODBUS_YC_ZHU_CGQ_L_3 25
//add by hyman 2012-2-15
#define ZJ_MODBUS_YC_SLV2_CGQ_H_2 45
#define ZJ_MODBUS_YC_SLV2_CGQ_L_2 46
#define ZJ_MODBUS_YC_SLV2_CGQ_H_3 47
#define ZJ_MODBUS_YC_SLV2_CGQ_L_3 48

#define ZJ_MODBUS_YC_JIAYAOLIANG_3 26
#define ZJ_MODBUS_YC_JIAYAOLIANG_4 27

#define ZJ_MODBUS_YC_PAIWU_TIME 29  //ÿ������ʱ��
#define ZJ_MODBUS_YC_PAIWU_LOOP 30  //������������

#define ZJ_MODBUS_YC_AUTO_MODE   31	//���Զ�ת����0�ֶ�1�Զ�

#define ZJ_AI_MST_1_WD     32   //��������1#ֵ
#define ZJ_AI_MST_2_WD     33   //��������2#ֵ

#define ZJ_AI_SLV_1_WD     34   //�Ӵ�����1#ֵ
#define ZJ_AI_SLV_2_WD     35   //�Ӵ�����2#ֵ

#define ZJ_AI_SLV2_1_WD     36   //�ٴӴ�����1#ֵ
#define ZJ_AI_SLV2_2_WD     37   //�ٴӴ�����2#ֵ

#define ZJ_AI_PAIWUED_TIME	38  //���ϴ�����ʱ��
#define ZJ_AI_PAIWUING_TIME	39  //��ǰ������ʱ��

#define ZJ_AI_SLV2_1_VAL     40   //�ٴӴ�����1#ֵ
#define ZJ_AI_SLV2_2_VAL     41   //�ٴӴ�����2#ֵ
#define ZJ_AI_SLV2_3_VAL     42   //�ٴӴ�����3#ֵ

//------------END

#define PERIOD                 10000//ÿ5S����һ������



//ADͨ����ϵ������
#define AD1_xishu               1   //4-20MA��������ֵ	wd
/*#define AD2_xishu               1000     //4-20MA��������ֵ	yl
#define AD3_xishu               1000   //4-20MA��������ֵ	ph
#define AD4_xishu               1000 
*/
#define AD2_xishu               1     //4-20MA��������ֵ	yl
#define AD3_xishu               1   //4-20MA��������ֵ	ph
#define AD4_xishu               1


struct AD_MEASURE
{	
 	UINT32 ad_samp;	//ad����ֵ
 	UINT32 ad_value;//adͨ����Ӧ���ֳ�ֵ	
 //	UINT8  proportion;//�������ٳ�ֵ�ı���
};
typedef union 
{	
  struct
  {    
    u32 orp_samp;               //orp����ֵ
    u32 by1_samp;               //��������ֵ
    u32 by2_samp;        //���ò���ֵ      
    u32 by3_samp;      //���ò���ֵ
//    u32 fulx_samp;               //��������ֵ
//    u32 water_samp;              //��ˮ������ֵ
  }value;        
  u32 ad_group[6];        
}AD_BUF;

EXT_MEASURE AD_BUF ad_buf[6];//��6�κ����һ�Σ���ŵ�ǰ6�εĲ���ֵ
EXT_MEASURE struct AD_MEASURE ad_measure[16];//��ż���������ֵ
EXT_MEASURE struct AD_MEASURE old_ad_measure[16];//��ż���������ֵ
//EXT_MEASURE u16 AD_Value[4];
EXT_MEASURE u16 lcd_disp_measure[16];
EXT_MEASURE u8 measure_flag;//�����ж��Ƿ����6�Σ���Ҫ������
EXT_MEASURE vu8 fulx_measure_flag;//�����ж������Ƿ���Ҫ������
EXT_MEASURE vu8 fulx_measure_flag1;//�����ж������Ƿ���Ҫ������

//EXT_MEASURE u8 update_measure_lcd;
EXT_MEASURE vu16 pulse_num,pulse_minus;
EXT_MEASURE vu16 pulse_num1,pulse_minus1;

//hyman2011-4
EXT_MEASURE float jlb1_need_times;  //������1�Ѿ���ҩ��
EXT_MEASURE float jlb1_all_times;  //������1�ܹ���ҩ��

EXT_MEASURE float jlb2_need_times;  //������2�Ѿ���ҩ��
EXT_MEASURE float jlb2_all_times;  //������2�ܹ���ҩ��

EXT_MEASURE float jlb3_need_times;  //������3�Ѿ���ҩ��
EXT_MEASURE float jlb3_all_times;  //������3�ܹ���ҩ��

EXT_MEASURE float jlb4_need_times;  //������4�Ѿ���ҩ��
EXT_MEASURE float jlb4_all_times;  //������4�ܹ���ҩ��

EXT_MEASURE float jlb5_need_times;  //������5�Ѿ���ҩ��
EXT_MEASURE float jlb5_all_times;  //������5�ܹ���ҩ��

EXT_MEASURE float jlb6_need_times;  //������5�Ѿ���ҩ��
EXT_MEASURE float jlb6_all_times;  //������5�ܹ���ҩ��

EXT_MEASURE float jlb7_need_times;  //������6�Ѿ���ҩ��
EXT_MEASURE float jlb7_all_times;  //������6�ܹ���ҩ��

EXT_MEASURE float ll_single_jiayao3_value;	//����������ҩ��3
EXT_MEASURE float ll_single_jiayao4_value;	//����������ҩ��3

EXT_MEASURE u32 liuliangjiMaster_all_times;  //���������ܽ��մ���
EXT_MEASURE u32 liuliangjiMaster_all_water;  //�����еĲ�ˮ��



EXT_MEASURE void Adc_Init(void);
EXT_MEASURE void time_control(void);
EXT_MEASURE void ad_calucate(void);
EXT_MEASURE void TIM_Configuration(void);

#endif
