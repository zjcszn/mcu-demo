#ifndef __GRAPHICAL_H
#define __GRAPHICAL_H

#include "font.h"
#include "my_ico.h"

typedef struct _WAVE_INFO{
  uint8_t  flg;           //���±�־
  int16_t  *buffer;       //ֻ֧�ָ����������峤����ú�img_widthһ�¡�
  uint16_t buffer_size;   //�����С
  uint16_t buffer_cnt;    //���峤�ȣ�ֻ�������������Сֵ
  int16_t  max_value;     //�������������ֵ
  int16_t  min_value;     //������������Сֵ
  int8_t   wave_type;     //0�㲨�Σ�1��״ͼ
  int16_t  minification;  //��С����
  uint16_t img_width;     //���ɵ�ͼ����
  uint16_t img_high;      //���ɵ�ͼ��߶�
}WAVE_INFO;


typedef struct _GIF_INFO{
  uint8_t load;
  uint8_t loop;
  uint8_t speed;
  uint32_t tick_msk;
  uint32_t frame_num;
  uint32_t play_no;
  uint16_t scr_pos_x;
  uint16_t scr_pos_y;
  uint16_t img_width;
  uint16_t img_high;
  uint8_t *p;
}GIF_INFO;


typedef struct _PROGRESS_BAR_INFO{
  uint8_t  type;
  uint16_t scr_pos_x;
  uint16_t scr_pos_y;
  uint16_t img_width;
  uint16_t img_high;
  uint16_t value;
  uint16_t max_value;
}PROGRESS_BAR_INFO;


typedef enum _BAT_STATE{
  ICO_BAT_0,
  ICO_BAT_1,
  ICO_BAT_2,
  ICO_BAT_3,
  ICO_BAT_4,
  ICO_BAT_CHARGE,
}BAT_STATE;


typedef enum _GIF_TYPE{
  GIF_ROTATE_LINE_R8,
}GIF_TYPE;

//

#define Glph_DrawPoint  DrawPoint
#define Glph_DrawBitmap DrawBitmap
void Glph_ClrArea(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high);
void Glph_Print(uint8_t x, uint8_t y,uint8_t font_t, char *dt);                                   //�ַ�����ӡ
void Glph_DrawLine(int16_t x1,int16_t y1, int16_t x2, int16_t y2, uint8_t t );                    //��ֱ��
void Glph_DrawRectangle(int16_t x1,int16_t y1, int16_t x2, int16_t y2, uint8_t fill, uint8_t c ); //������
void Glph_DrawXLine(uint8_t x1, uint8_t y1, uint8_t r, float angle, uint8_t t);                   //�����껭ֱ��
void Glph_Drawcircle( u16 xc, u16 yc, u16 r, u16 fill, u16 c);                                    //��Բ
void Glph_Wave(uint8_t x, uint8_t y, WAVE_INFO *cfg);                                             //������
void Glph_DrawBattery(uint8_t x, uint8_t y, uint8_t type, BAT_STATE st);                          //����ص���ͼ��
void Glph_DrawGif(uint8_t x, uint8_t y, uint8_t type);                                            //������ͼ��
//
void Glph_DrawBarInit(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high);
void Glph_DrawBar(uint8_t x1,uint8_t y1, uint8_t width, uint8_t high, uint8_t type, int num);
//
void Glph_LoadGif(uint8_t x, uint8_t y, uint8_t loop, GIF_INFO *gif_play, uint8_t *p);
void Glph_PlayGif(GIF_INFO *gif);

#endif






