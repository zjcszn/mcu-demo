#ifndef _VIEW_H_
#define _VIEW_H_

#ifndef _VIEW_C
#define EXT_VIEW extern 
#else
#define EXT_VIEW
#endif

//#include "config.h"

/******************EDIT:chang***********************/
typedef struct
{
	u16 strlen;			//�ַ�������
	u16 *str;			//�ַ�������
	u16 NextToken;		//�ַ���ȷ��ʱ����ʾ����һ��Tokenֵ
}MenuStr_t;

typedef struct
{
	u16 FirstItem;		//����ʾ�ĵ�һ��ѡ��
	u16 CurrentItem;		//������ڵ�ѡ��
	u16 Page;			//�˵���ҳ
        u8 index;                      //�����еĲ˵��е�λ��
	u16 PageItem;		//�˵����ڱ�ѡ�е���
	u16 TurnMode;		//�˵��ķ�ҳ��ʽ
	u16 count;			//�˵��в˵���ĸ���
//	uint16 countofpage;
	u32 *Item;			//�˵���
}Menu_t;


typedef struct
{
	u8 number_page;
	u8 number_menu;
	u8 start_line;
	u8 cursor_top;
	u8 cursor_left;
	u8 turn;
}YC_PAGE;

typedef struct
{
	u8 yx_point;
	u8 disp_way;
	u8 state_pos;		//������״̬��ʾ��λ��
	u8 yx_mean;
}YX_PARA;
typedef struct
{
	u8 delaytime;
	u8 soetype;
	u8 reserve;
	u8 yxmenu;
}CORRECT_YX;
typedef struct
{
	u16 yx_cycle;
	u16 next_page;
}YX_PAGE;
typedef struct
{
	u8 fixed_type;
	u8 fixed_name;
	uint8 fixed_start;
	uint8 fixed_unit;
	uint16 fixed_value_min;
	uint16 fixed_value_max;
}FIXED_PARA_STRU;

typedef struct
{
	uint8 para_type;	
	uint8 fixed_num;
	uint8 *fixed_inf;
}PARA_STRUCT;
typedef struct
{
	uint8 type;
	uint8 sign;
	uint8 dot;
	uint8 position;
	uint16 max;
	uint16 min;
}MODUL_PARA;
typedef struct
{
	uint8 port;
	uint8 baud;
	uint8 check;
	uint8 address;
	uint8 rules;
	uint8 reserve;
}PORT_PARA;
typedef struct
{
	uint16 x;
	uint16 y;
	uint16 length;
	uint16 value;
	uint16 value_min;
	uint16 value_max;
}PORT_SHOW;
typedef struct
{
	u16 y;
	u16 x;
	u8 index;//�ڶ�Ӧ��ʾ������Ҫ��������
}PARACURSOR;
typedef struct
{
	uint8 yksign;
	uint8 yktype;
	uint8 number;
	uint8 ykobject;
	uint8 yuanfang;
	uint8 serever;
}YKPARA_STR;
typedef struct
{
	uint16 x;
	uint16 y;
	uint8 logic;
	uint8 value;
	uint8 strlength;
	uint8 type;
	uint16 *str;
}CHANGEYK;
typedef struct
{
	uint8 code;
	uint8 serever;
}DEVICE_STR;



typedef struct
{
	u16 turn;
	u16 left;
	u16 top;
	u16 count;
	u16 *data;
}inf_char;

#define NUM_YC                  14


EXT_VIEW u32 reset_counter;//�����޲�����һ��ʱ�䣬���ص�������


EXT_VIEW void DispMenu( void );
EXT_VIEW void Yc_Disppage (YC_PAGE* yc_information);


EXT_VIEW void MainView(void);

EXT_VIEW void Adj_xiaoshudian(char *buf,u8 dot_num);


#endif

