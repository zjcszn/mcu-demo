/*
���������˵���ǣ��������Ӳ˵�������
*/

#ifndef _FORM_CTRL_MANAGE_H
#define _FORM_CTRL_MANAGE_H

#ifdef _FORM_CTRL_MANAGE_C
#define EXT_FORM_CTRL_MANAGE
#else
#define EXT_FORM_CTRL_MANAGE extern
#endif

EXT_FORM_CTRL_MANAGE UINT8 ctrl_code;		//ң�غ�
EXT_FORM_CTRL_MANAGE UINT16 ctrl_opt;		//���Ʋ�������,��ʼ��ʱӦ�뱻�ض���ĵ�ǰ״̬�෴
//--------------------------------------------------------

EXT_FORM_CTRL_MANAGE void R_Ctrl_Manage_Execute(struct _rq_t *item);
//--------------------------------------------------------


#endif