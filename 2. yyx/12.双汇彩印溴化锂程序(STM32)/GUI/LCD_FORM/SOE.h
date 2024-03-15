/*

//DATA:2008-03-01
//VER:	V10	
//��ͷ�ļ���ÿ�������л���һ��������ͨ�õĲ��ظĶ���������ش���Ķ�


//�ô�����ּ��˵������������SOE�����ֵĸĶ����õ����ļ���ɾ���˶��伴��

reportsoe����ȥ��INT16U valueh������SOE�ṹ����ȥ�� pevtw->soevalueh = valueh;
LCD_COMMU422�к���void SendFrameSeqSoe(void)������֯��������ʱ������ֵ��Ϊ2���ֽ�
SOESTRU�ṹ��ȥ��INT16U soevalueh;

//ɾ������


//-----------------------------------------------------------------------
//		ͨ �� �� ��(���ظĶ�)
//-----------------------------------------------------------------------
//SOE��������ռ�ö��ֽڣ�����ͨ����Һ����485ͨѶ�ж����ظ���λ����

////D15-D8����������
			D8:
				1:�¹�
				0:���¹�
			D9:	
				1:Ԥ��
				0:��Ԥ��
			D10:	
				1:������
				0:��������
			D11:	
				1:�Ϸ�����
				0:������������
			D12:	(ȡ����D11)
				1:��/����
				0:��/����
			D15:	����ֵ��Ч��־	
				1.��Ч 
				0.��Ч

//D7-D5	����ʽ����
			0:��С����
			1:һλС����
			2:��λС����
			3:��λС����
			4:��λС����
			5:��λС����		
//D0-D4	����λ����
			0:�޵�λ
			1:A
			2:V
			3:��
			4:S
			5:Hz
			6:Hz/s
			7:��
			8:��
			9:k��
			10:%
			11:��
			12:W
			13:Var
		
*/
#ifndef __SOE_H
#define __SOE_H
#include "prodef.h"
#include "typedefine.h"


/***************************************************
SOE_CODE
****************************************************/

#define	BASEKIEVTNUM	0x0

#define EVTKI1					(0x0 + BASEKIEVTNUM)
#define EVTKI2					(0x1 + BASEKIEVTNUM)
#define EVTKI3					(0x2 + BASEKIEVTNUM)
#define EVTKI4					(0x3 + BASEKIEVTNUM)
#define EVTKI5					(0x4 + BASEKIEVTNUM)
#define EVTKI6					(0x5 + BASEKIEVTNUM)
#define EVTKI7					(0x6 + BASEKIEVTNUM)
#define EVTKI8					(0x7 + BASEKIEVTNUM)
#define EVTKI9					(0x8 + BASEKIEVTNUM)
#define EVTKI10					(0x9 + BASEKIEVTNUM)
#define EVTKI11					(0xa + BASEKIEVTNUM)
#define EVTKI12					(0xb + BASEKIEVTNUM)
#define EVTKI13					(0xc + BASEKIEVTNUM)
#define EVTKI14					(0xd + BASEKIEVTNUM)
#define EVTKI15					(0xe + BASEKIEVTNUM)
#define EVTKI16					(0xf + BASEKIEVTNUM)
#define EVTKIHWJ				(0x10 + BASEKIEVTNUM)//��λ��
#define EVTKITWJ				(0x11 + BASEKIEVTNUM)//��λ��
#define EVTKIXHFG				(0x12 + BASEKIEVTNUM)//�źŸ���
#define EVTKIYFJD				(0x13 + BASEKIEVTNUM)//Զ��/�͵�
#define EVTKITHYCN				(0x14 + BASEKIEVTNUM)//�����Ѵ���
#define EVTKIDDHW				(0x15 + BASEKIEVTNUM)//�ӵص�բ��λ
#define EVTKIGDHW				(0x16 + BASEKIEVTNUM)//���뵶բ��λ
#define EVTKIYLJD				(0x17 + BASEKIEVTNUM)//ѹ������
#define EVTKICHZBS				(0x18 + BASEKIEVTNUM)//�غ�բ����
#define EVTKIWTB				(0x19 + BASEKIEVTNUM)//�������������
#define EVTKIUHF				(0x1a + BASEKIEVTNUM)//��ѹ�ิ�ϵ�ѹ����
#define EVTKIUMF				(0x1b + BASEKIEVTNUM)//��ѹ�ิ�ϵ�ѹ����
#define EVTKIULF				(0x1c + BASEKIEVTNUM)//��ѹ�ิ�ϵ�ѹ����
#define EVTKIZWS				(0x1d + BASEKIEVTNUM)//����˹
#define EVTKITYZWS				(0x1e + BASEKIEVTNUM)//��ѹ����˹
#define EVTKIQWS				(0x1f + BASEKIEVTNUM)//����˹
#define EVTKITYQWS				(0x20 + BASEKIEVTNUM)//��ѹ����˹
#define EVTKIYWG				(0x21 + BASEKIEVTNUM)//���¸�
#define EVTKIYWSG				(0x22 + BASEKIEVTNUM)//��������
#define EVTKIYWGG				(0x23 + BASEKIEVTNUM)//���¹���
#define EVTKIYYJD				(0x24 + BASEKIEVTNUM)//��ѹ����
#define EVTKIMCLT				(0x25 + BASEKIEVTNUM)//��ſ�������
#define EVTKISJSGLT				(0x26 + BASEKIEVTNUM)//ˮ���¹�����
#define EVTKILCGZ				(0x27 + BASEKIEVTNUM)//���Ź���
#define EVTKIFJYX				(0x28 + BASEKIEVTNUM)//�������



#define EVTKZD					(0x50 + BASEKIEVTNUM)//�ع���
#define EVTPTDX					(0x51 + BASEKIEVTNUM)//PT����
#define EVTCTDX					(0x52 + BASEKIEVTNUM)//CT����
#define EVTTBLQD				(0x53 + BASEKIEVTNUM)//ͻ��������
#define EVTGFHQD				(0x54 + BASEKIEVTNUM)//����������
#define EVTGFH					(0x55 + BASEKIEVTNUM)//������
#define EVTGFHGJ				(0x56 + BASEKIEVTNUM)//�����ɸ澯
#define EVTFSHGFH				(0x57 + BASEKIEVTNUM)//��ʱ�޹�����
#define EVTFSHGFHGJ				(0x58 + BASEKIEVTNUM)//��ʱ�޹����ɸ澯
#define EVTSD					(0x59 + BASEKIEVTNUM)//�ٶ�
#define EVTI1					(0x5a + BASEKIEVTNUM)//����I��
#define EVTJSI1					(0x5b + BASEKIEVTNUM)//���ٹ���I��
#define EVTI1T1					(0x5c + BASEKIEVTNUM)//����I��Iʱ��
#define EVTI1T2					(0x5d + BASEKIEVTNUM)//����I��IIʱ��
#define EVTI2					(0x5e + BASEKIEVTNUM)//����II��
#define EVTJSI1					(0x5f + BASEKIEVTNUM)//���ٹ���II��
#define EVTI2T1					(0x60 + BASEKIEVTNUM)//����II��Iʱ��
#define EVTI2T2					(0x61 + BASEKIEVTNUM)//����II��IIʱ��
#define EVTFQ					(0x62 + BASEKIEVTNUM)//�������
#define EVTTYBS					(0x63 + BASEKIEVTNUM)//��ѹ����
#define EVTIDL_QD				(0x64 + BASEKIEVTNUM)//�����ж�·����
#define EVTIDL_RUN				(0x65 + BASEKIEVTNUM)//�����ж�·����
#define EVTIFSXGL				(0x66 + BASEKIEVTNUM)//��ʱ�޹���
#define EVTGR					(0x67 + BASEKIEVTNUM)//���ȱ���
#define EVTGR_WARN				(0x68 + BASEKIEVTNUM)//���ȱ����澯
#define EVTQDTL					(0x69 + BASEKIEVTNUM)//����ʱ�����
#define EVTIMEMORY				(0x6a + BASEKIEVTNUM)//�������
#define EVTCDSD					(0x6b + BASEKIEVTNUM)//��ٶ�
#define EVTBLCD					(0x6c + BASEKIEVTNUM)//���ʲ
#define EVTIDIFFWARN			(0x6d + BASEKIEVTNUM)//�����澯
#define EVTNGL_WARN				(0x6e + BASEKIEVTNUM)//�湦�ʸ澯
#define EVTNGL					(0x6f + BASEKIEVTNUM)//�湦����բ
#define EVTIFX					(0x70 + BASEKIEVTNUM)//�������
#define EVTIFX_WARN				(0x71 + BASEKIEVTNUM)//��������澯
#define EVTIFX1					(0x72 + BASEKIEVTNUM)//�������I��
#define EVTIFX2					(0x73 + BASEKIEVTNUM)//�������II��
#define EVTIFXFSX				(0x74 + BASEKIEVTNUM)//��ʱ�޸������
#define EVTIFXFSX_WARN			(0x75 + BASEKIEVTNUM)//��ʱ�޸�������澯
#define EVTI0					(0x76 + BASEKIEVTNUM)//�������
#define EVTI0_WARN				(0x77 + BASEKIEVTNUM)//��������澯
#define EVTI0BAL				(0x78 + BASEKIEVTNUM)//��ƽ���������
#define EVTI0GYC				(0x79 + BASEKIEVTNUM)//��ѹ���������
#define EVTI0GYC_WARN			(0x7a + BASEKIEVTNUM)//��ѹ����������澯
#define EVTI0FSX_DYC			(0x7b + BASEKIEVTNUM)//��ѹ�෴ʱ���������
#define EVTI0DYC_WARN			(0x7c + BASEKIEVTNUM)//��ѹ����������澯
#define EVTI01					(0x7d + BASEKIEVTNUM)//�������I��
#define EVTI01_WARN				(0x7e + BASEKIEVTNUM)//�������I�θ澯
#define EVTJSI01				(0X7f + BASEKIEVTNUM)//�����������I��
#define EVTI01T1				(0x80 + BASEKIEVTNUM)//�������I��Iʱ��
#define EVTI01T2				(0x81 + BASEKIEVTNUM)//�������I��IIʱ��
#define EVTI02					(0x82 + BASEKIEVTNUM)//�������II��
#define EVTI02_WARN				(0x83 + BASEKIEVTNUM)//�������II�θ澯
#define EVTJSI02				(0x84 + BASEKIEVTNUM)//�����������II��
#define EVTI02T1				(0x85 + BASEKIEVTNUM)//�������II��Iʱ��
#define EVTI02T2				(0x86 + BASEKIEVTNUM)//�������II��IIʱ��
#define EVTI03					(0x87 + BASEKIEVTNUM)//�������III��
#define EVTI03_WARN				(0x88 + BASEKIEVTNUM)//�������III�θ澯
#define EVTJSI03				(0x89 + BASEKIEVTNUM)//�����������III��
#define EVTI0JX					(0x8a + BASEKIEVTNUM)//��϶�������
#define EVTWBT					(0x8b + BASEKIEVTNUM)//���������
#define EVTBTW					(0x8c + BASEKIEVTNUM)//���������




#define EVTUCOMPLEX_UL			(0xa0 + BASEKIEVTNUM)//���ϵ�ѹ֮�͵�ѹ
#define EVTUCOMPLEX_UFX			(0xa1 + BASEKIEVTNUM)//���ϵ�ѹ֮�����ѹ
#define EVTUH					(0xa2 + BASEKIEVTNUM)//����ѹ����
#define EVTUL					(0xa3 + BASEKIEVTNUM)//ʧ��ѹ����
#define EVTUL1					(0xa4 + BASEKIEVTNUM)//�͵�ѹI��
#define EVTUL21					(0xa5 + BASEKIEVTNUM)//�͵�ѹII��Iʱ��
#define EVTUL22					(0xa6 + BASEKIEVTNUM)//�͵�ѹII��IIʱ��
#define EVTU0					(0xa7 + BASEKIEVTNUM)//�����ѹ
#define EVTU0_WARN				(0xa8 + BASEKIEVTNUM)//�����ѹ�澯
#define EVTU0_GND_STATOR		(0xa9 + BASEKIEVTNUM)//����95%�ӵر���
#define EVTU0_GND_STATOR_WARN	(0xaa + BASEKIEVTNUM)//����95%�ӵر����澯
#define EVTU3_GND_STATOR		(0xab + BASEKIEVTNUM)//����100%�ӵر���
#define EVTU3_GND_STATOR_WARN 	(0xac + BASEKIEVTNUM)//����100%�ӵر����澯
#define EVTONE_GND_RATOR		(0xad + BASEKIEVTNUM)//ת��һ��ӵ�
#define EVTONE_GND_RATOR_WARN	(0xae + BASEKIEVTNUM)//ת��һ��ӵظ澯
#define	EVTTWO_GND_RATOR		(0xaf + BASEKIEVTNUM)//ת������ӵ�
#define	EVTTWO_GND_RATOR_WARN	(0xb0 + BASEKIEVTNUM)//ת������ӵظ澯
#define EVTLOSE_MAGNETISM		(0Xb1 + BASEKIEVTNUM)//ʧ�ű���



#define EVTREMOTECTRH			(0xd0 + BASEKIEVTNUM)//ң�غ�
#define EVTREMOTECTRF			(0xd1 + BASEKIEVTNUM)//ң�ط�
#define EVTLOCALCTRH			(0xd2 + BASEKIEVTNUM)//�͵غ�
#define EVTLOCALCTRF			(0xd3 + BASEKIEVTNUM)//�͵ط�
#define EVTREMOTECTRQ			(0xd4 + BASEKIEVTNUM)//ң�ط������
#define EVTREMOTECTRT			(0xd5 + BASEKIEVTNUM)//ң�ط��ֹͣ
#define EVTLOCALCTRQ			(0xd6 + BASEKIEVTNUM)//�͵ط������
#define EVTLOCALCTRT			(0xd7 + BASEKIEVTNUM)//�͵ط��ֹͣ






//-----------------------------------------------------------------------
//definition for d15,VAL_VALIABLE
#define EVT_VAL_VALIABLE			(1<<15)

//definition for d8-d14,EVT_DESC
#define EVT_DESC_FAULT				(1<<8)
#define EVT_DESC_WARNIING			(1<<9)
#define EVT_DESC_WITH_FX			(1<<10)
#define EVT_DESC_ATTR_OPEN_CLOSE	(1<<11)
#define EVT_DESC_SET				(1<<12)

//definition for d5-d7,EVT_VAL_FORMAT
#define EVT_VAL_FORMAT_DOT_0		(0)
#define EVT_VAL_FORMAT_DOT_1		(1<<5)
#define EVT_VAL_FORMAT_DOT_2		(2<<5)
#define EVT_VAL_FORMAT_DOT_3		(3<<5)
#define EVT_VAL_FORMAT_DOT_4		(4<<5)
#define EVT_VAL_FORMAT_DOT_5		(5<<5)

//definition for d0-d4,EVT_UNIT
#define EVT_UNIT_NULL				(0)
#define EVT_UNIT_A					(1)
#define EVT_UNIT_V					(2)
#define EVT_UNIT_OU					(3)
#define EVT_UNIT_S					(4)
#define EVT_UNIT_HZ					(5)
#define EVT_UNIT_HZS				(6)
#define EVT_UNIT_XIANG				(7)
#define EVT_UNIT_DU_ASC				(8)
#define EVT_UNIT_K_OU				(9)
#define EVT_UNIT_BFB				(10)
#define EVT_UNIT_DU_CN				(11)
#define EVT_UNIT_W					(12)
#define EVT_UNIT_VAR				(13)
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//			������ز���(��Ҫ�Ķ�)
/*
���ӽ���
//�ٶ�
#define EVT_DESC_SD		(EVT_VAL_VALIABLE											\	//����ֵ�Ƿ���ã��������ã���ʡ�Դ���
						| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\	//��������
						| EVT_VAL_FORMAT_DOT_2										\	//С����λ��
						| EVT_UNIT_A)													//��λ
*/
//-----------------------------------------------------------------------
//�ٶ�
#define EVTSD_DESCRIBE		(EVT_VAL_VALIABLE											\
							| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\
							| EVT_VAL_FORMAT_DOT_2										\
							| EVT_UNIT_A)
//��������
#define EVTI2_DESCRIBE		(EVT_VAL_VALIABLE											\
							| EVT_DESC_FAULT | EVT_DESC_ATTR_OPEN_CLOSE | EVT_DESC_SET	\
							| EVT_VAL_FORMAT_DOT_2										\
							| EVT_UNIT_A)

/*
#define CLEAROLDACTIONL	0xC000
#define CLEAROLDACTIONH	0xf880
#define FRACCSTART	0X10

#define	EVTDZ_DESCRIBE		0x4129
#define EVTSD_DESCRIBE		0X7931
#define EVTI1_DESCRIBE		0X7931
#define EVTI2_DESCRIBE		0X7931
#define EVTJSI1_DESCRIBE	0X7931
#define EVTJSI2_DESCRIBE	0X7931
#define EVTCHZ_DESCRIBE		0xF809
#define EVTI01_DESCRIBE		0X7929	
#define EVTI02_DESCRIBE		0X7929	
#define EVTI03_DESCRIBE		0X7929	
#define	EVTKZD_DESCRIBE		0XF809
#define EVTTBLQD_DESCRIBE	0XF809
#define	EVTGFHQD_DESCRIBE	0X7929
#define EVTPTDX_DESCRIBE	0XFC09
#define	EVTETHERERR_DESCRIBE	0xff09
#define EVTREMOTECTRH_DESCRIBE	0XFC01
#define EVTREMOTECTRF_DESCRIBE	0XFC09
#define EVTLOCALCTRH_DESCRIBE	0XFC01
#define EVTLOCALCTRF_DESCRIBE	0XFC09

#define	EVTI0QD_DESCIRBE	0X6B29
*/

/************************************************************************/
#endif