#include "main.h"
#include "tim.h"

volatile u32 nowTime = 0;//��������ʱ�䣬��λ20ms����key.c�и���
volatile u32 clockTime = 0;//���Ӷ�ʱʱ�䣬��λ20ms����key.c�и���
extern char timeStr[9];//ʱ���ַ���

void drawClockTime(void)
{
    u16 hour = 0, min = 0, sec = 0;
    sec = nowTime / 50; //ϵͳ��ʱ����key.c�е�TIM3�жϷ������и���
    hour = sec / 3600;
    min = (sec % 3600) / 60;
    sec = sec % 60;
    OLED_Fill(85, 1, 127, 12, 1); //ð��
    OLED_DrawPoint(99, 5, 0);
    OLED_DrawPoint(99, 8, 0);
    OLED_DrawPoint(114, 5, 0);
    OLED_DrawPoint(114, 8, 0);
    sprintf((char *)timeStr, "%02d", hour); //���ʱ���ַ���
    OLED_ShowString(86, 1, (u8*)timeStr, 12, 0);
    sprintf((char *)timeStr, "%02d", min); //���ʱ���ַ���
    OLED_ShowString(101, 1, (u8*)timeStr, 12, 0);
    sprintf((char *)timeStr, "%02d", sec); //���ʱ���ַ���
    OLED_ShowString(116, 1, (u8*)timeStr, 12, 0);
}

