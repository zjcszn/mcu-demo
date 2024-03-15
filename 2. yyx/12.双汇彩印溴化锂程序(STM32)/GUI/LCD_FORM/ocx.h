#ifndef _OCX_H
#define _OCX_H

#define K_TEXT  0x00
#define K_MENU  0x01
#define K_TIME  0x02
#define K_DATE  0x03
#define K_CAP   0x04
#define K_LIST  0x05
#define K_IP    0x06

typedef struct
{
  UINT8 val;
  INT8 *str;
}name_list_t;


//�ı��򣨽������ı����ؼ��ṹ
typedef struct _text_t
{
  UINT8 Type;       //�ؼ�����
  UINT8 Focus;      //����  
  UINT8 Width;      //���
  UINT8 	ReadOnly;//�Ƿ�ֻ��
  
  INT8 *Caption1;  //����1
  INT8 *Caption2;  //����2
  
  
  INT8 x,y;        //�������
  INT8       Px,Py;      //��������
  
  INT32      Value;      //ֵ
  
  UINT8	     location;		//����ڱ���1������λ��,���λ�����Ƿ�������ʾ
  UINT8      dot;			//С����λ��
  int8u       Cursor;     //���λ��
  int8u       MaskChar;   //�����ַ�
  
  int32s      Max;        //���ֵ
  int32s      Min;        //��Сֵ
  
  void       (*GetFocus)(struct _text_t *);             //�õ�����
  void       (*LossFocus)(struct _text_t *);            //ʧȥ����
  void       (*KeyProcess)(struct _text_t *, int16u );  //��������
  void       (*qbch)(struct _text_t *);                 //ȫ���ػ�
  void       (*SetVal)(struct _text_t *,int32s);        //�趨ֵ
  void       (*GetVal)(struct _text_t *,int32s*);       //��ȡֵ
       
  UINT16 *p_t_data;		//Valueֵ������Դ �����ݳ�����ReadOnly�ĵͰ��ֽ�                                 
}k_text_t;

typedef struct _menu_t
{
  int8u Type;       //const 
  int8u Focus;      //const
  int8s *Text;      //����const
  int8s x,y;        //const
  int8s       Px,Py;      //
  //const void *Token;      //ȷ��ʱִ�е�����
  void *Token;      //ȷ��ʱִ�е�����
  void       (*GetFocus)(struct _menu_t *);             //�õ�����
  void       (*LossFocus)(struct _menu_t *);            //ʧȥ����
  void       (*KeyProcess)(struct _menu_t *, int16u );  //��������
  void       (*qbch)(struct _menu_t *);                 //ȫ���ػ�
}k_menu_t;


typedef struct _rq_t
{
  int8u       df_line;       //����
  int8u       cur_line;      //��ʾ�ĵ�ǰ��
  int8u       Focus;         //����ֵ
  int8s       x,y;           //������ʼ����
  int8u       ThisCont;      //������ʾ�ĸ���
  int8s       Width,Height;  //�������
  int8u       cont;          //�ؼ�����  	
 	
//  list_t      *kj;
  void        *ct;           //����������
  void        *enter;        //ȷ�������������
  void        *cancel;       //ȡ�������������
  void        *cur_ocx;      //��ǰ�ؼ�
  void        (*SetFocus)(struct _rq_t * );      //�������ϵĶ������ͽ���
  void        (*Initsize)(struct _rq_t * );      //��ʼ��
  void        (*Execute)(struct _rq_t * );
  void        **kj;
}k_rq_t;

//�б����ṹ
typedef struct _list_t
{
  int8u Type;       //�ؼ�����
  int8u Focus;      //����
  UINT8 	ReadOnly;                              //�Ƿ�ֻ��
  int8u       Cursor;     //���λ��
  
  int8s *Caption1;  //����1
  int8s *Caption2;  //����2
  int8s **Value;    //ֵ
  
  UINT8	location;		//����ڱ���1������λ��,���λ�����Ƿ�������ʾ
  int8u Width;      //���
    
  int8s x,y;        //�������
  int8s       Px,Py;      //��������  
  
  int32s      Max;        //���ֵ
  void       (*GetFocus)(struct _list_t *);             //�õ�����
  void       (*LossFocus)(struct _list_t *);            //ʧȥ����
  void       (*KeyProcess)(struct _list_t *, int16u );  //��������
  void       (*qbch)(struct _list_t *);                 //ȫ���ػ�
  void       (*SetVal)(struct _list_t *, int8u);        //�趨ֵ
  void       (*GetVal)(struct _list_t *, int8u *);      //��ȡֵ
  //void       *ReadOnly;   
 
  
  UINT16 *p_l_data;		//Valueֵ������Դ �����ݳ�����ReadOnly�ĵͰ��ֽ�  
}k_list_t;

typedef struct _ipad_t
{
  const int8u Type;       //����
  const int8u Focus;      //����
  const int8s *Caption1;  //����1
  const int8s x,y;        //�������
  int8s       Px,Py;      //��������
  int8u       Cursor;     //���λ��
  int8u       wz;         //���λ��
  int16s      Val[4];     //IP��ַ��4���ֽ�
  void       (*GetFocus)(struct _ipad_t *);             //�õ�����
  void       (*LossFocus)(struct _ipad_t *);            //ʧȥ����
  void       (*KeyProcess)(struct _ipad_t *, int16u );  //��������
  void       (*qbch)(struct _ipad_t *);                 //ȫ���ػ�
  void       (*SetVal)(struct _ipad_t *,int8u, int8u, int8u, int8u);      //�趨ֵ
  void       (*GetVal)(struct _ipad_t *,int8u *,int8u *,int8u *,int8u *); //��ȡֵ
  void       *ReadOnly;                                 //�Ƿ�ֻ��
}k_ipad_t;    

#endif