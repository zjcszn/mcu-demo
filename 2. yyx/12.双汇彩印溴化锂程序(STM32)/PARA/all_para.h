/**/
#ifndef _ALL_PARA_H
#define _ALL_PARA_H

#ifndef _ALL_PARA_C
#define EXT_ALL_PARA extern
#else
#define EXT_ALL_PARA 
#endif


//----------------------------------------------------------------
//�豸���Ͷ���,�ô�����Ӧ��COMM422.C�ж���һ�¡�ÿ
//�治ͬ�Ĳ�����������һ����ͬ���ͺš�
//�ö���Ϊһ���ֽڣ��߰��ֽڱ�ʾװ�õ����ͣ��Ͱ��ֽڱ�ʾ������
//��ɵĴ��ֽ�ΪΨһֵ����Ӧһ�������

//��·
#define DEV_TYPE_XL_A	0x01	//��·A��
#define DEV_TYPE_XL_B	0x02	//
//��ѹ��������
#define DEV_TYPE_BYQ_CD_A	0x11
#define DEV_TYPE_BYQ_CD_B	0x12
#define DEV_TYPE_BYQ_CD_C	0x13

#define CTRL_PARA_LENGTH                   6*10





//----------------------------------------------------------------
EXT_ALL_PARA const UINT8 ctrl_para_xl_a[];
//����ָ��Ķ���
EXT_ALL_PARA UINT8 *p_fix_para;			//��ֵ����ָ��
EXT_ALL_PARA UINT8 *p_jiankong_para;			//��ֵ����ָ��
EXT_ALL_PARA UINT8 *p_gprs_para;			//��ֵ����ָ��
EXT_ALL_PARA UINT8 *p_sensor_para;			//��ֵ����ָ��
EXT_ALL_PARA UINT8 *p_dispyx_para;		//
EXT_ALL_PARA UINT8 *p_soe_para;
EXT_ALL_PARA UINT8 *p_yc_para;
EXT_ALL_PARA UINT8 *p_kwh_para;
EXT_ALL_PARA UINT8 *p_jiayao_para;
EXT_ALL_PARA UINT8 *p_modulpara_para;
EXT_ALL_PARA UINT8 *p_adj_xs_para;		//����ϵ������
EXT_ALL_PARA UINT8 *p_yx_para;			//ң�Ų���

EXT_ALL_PARA void InitAllPara(void);

#endif

