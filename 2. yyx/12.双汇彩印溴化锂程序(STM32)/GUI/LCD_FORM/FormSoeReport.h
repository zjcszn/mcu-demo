/*
�¼�����˵���ǣ��������Ӳ˵�������
*/

#ifndef _FORM_SOE_REPORT_H
#define _FORM_SOE_REPORT_H

#ifdef _FORM_SOE_REPORT_C
#define EXT_FORM_SOE_REPORT
#else
#define EXT_FORM_SOE_REPORT extern
#endif


//--------------------------------------------------------

EXT_FORM_SOE_REPORT void DispSoeReport(struct _rq_t *item);
EXT_FORM_SOE_REPORT void ClrSoeReport(struct _rq_t *item);

//--------------------------------------------------------


#endif