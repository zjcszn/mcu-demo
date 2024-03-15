#include "menu.h"
#include "main.h"
/*https://blog.csdn.net/embedded_guzi/article/details/35835755
https://blog.csdn.net/calmuse/article/details/79346742
*/

u16 homeGoIndex = tdwt;//home�Ŀ�����ת�˵�ҳ
u8 volatile nowMenuIndex = 0;
u8 volatile lastMenuIndex = 0;
extern unsigned char logo[];
extern unsigned char logoR[];
char batVoltStr[8];
char timeStr[9];
u8 iconTop[11] 	= {0X04, 0X0C, 0X18, 0X30, 0X60, 0XC0, 0X60, 0X30, 0X18, 0X0C, 0X04};
u8 iconBottom[11]	= {0X80, 0XC0, 0X60, 0X30, 0X18, 0X0C, 0X18, 0X30, 0X60, 0XC0, 0X80};
u8 iconHand[32] =
{
    0X01, 0X00, 0X01, 0X00, 0X02, 0X80, 0X02, 0XC0, 0X04, 0XBC, 0X05, 0X6C, 0X05, 0XE8, 0X0B, 0X90,
    0X0B, 0X10, 0X0E, 0X08, 0X1C, 0X08, 0X1C, 0X18, 0X18, 0X60, 0X33, 0X80, 0X3C, 0X00, 0X20, 0X00,
};
u8 onSign[28] = {0X7F, 0XF8, 0XFF, 0XFC, 0XC0, 0X0C, 0XC2, 0X0C, 0XC7, 0X0C, 0XC3, 0X8C, 0XC1, 0XCC, 0XC1, 0XCC, 0XC3, 0X8C, 0XC7, 0X0C, 0XCE, 0X0C, 0X9C, 0X0C, 0X39, 0XFC, 0X13, 0XF8,};
u8 offSign[28] = {0X7F, 0XF8, 0XFF, 0XFC, 0XC0, 0X0C, 0XC8, 0X4C, 0XDC, 0XEC, 0XCF, 0XCC, 0XC7, 0X8C, 0XC7, 0X8C, 0XCF, 0XCC, 0XDC, 0XEC, 0XC8, 0X4C, 0XC0, 0X0C, 0XFF, 0XFC, 0X7F, 0XF8,};
unsigned char iconJ20[102] =
{
    0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XE8, 0XFF, 0XE8, 0XFF, 0XE8, 0XFF, 0XE8,
    0XC0, 0X08, 0XC0, 0X08, 0XC0, 0X08, 0XC0, 0X08, 0XFF, 0XF8, 0XDF, 0XC8, 0XDF, 0X88, 0XDF, 0X08,
    0XDE, 0X08, 0XDC, 0X08, 0XD8, 0X68, 0XC0, 0XE8, 0XC1, 0XE8, 0XC3, 0XE8, 0XC7, 0XE8, 0XFF, 0XF8,
    0XC0, 0X08, 0XC0, 0X08, 0XC0, 0X08, 0XDE, 0X68, 0XDE, 0XE8, 0XDD, 0XE8, 0XD9, 0XE8, 0XC0, 0X08,
    0XC0, 0X08, 0XC0, 0X18, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF8, 0XFF, 0XF0,
    0XFF, 0XE0, 0XFF, 0XC0, 0XFF, 0X80, 0XFF, 0X00, 0XFE, 0X00, 0XFC, 0X00, 0XF8, 0X00, 0XF0, 0X00,
    0XE0, 0X00, 0XC0, 0X00, 0X80, 0X00,
};
unsigned char iconSignal100[22] =
{
    0X00, 0X70, 0X00, 0X70, 0X00, 0X00, 0X03, 0XF0, 0X03, 0XF0,
    0X00, 0X00, 0X1F, 0XF0, 0X1F, 0XF0, 0X00, 0X00, 0XFF, 0XF0, 0XFF, 0XF0,
};
unsigned char iconSignal75[22] =
{
    0X00, 0X70, 0X00, 0X70, 0X00, 0X00, 0X03, 0XF0, 0X03, 0XF0, 0X00, 0X00, 0X1F, 0XF0, 0X1F, 0XF0,
    0X00, 0X00, 0XAA, 0XA0, 0X55, 0X50,
};
unsigned char iconSignal50[22] =
{
    0X00, 0X70, 0X00, 0X70, 0X00, 0X00, 0X03, 0XF0, 0X03, 0XF0, 0X00, 0X00, 0X0A, 0XA0, 0X15, 0X50,
    0X00, 0X00, 0XAA, 0XA0, 0X55, 0X50,
};
unsigned char iconSignal25[22] =
{
    0X00, 0X70, 0X00, 0X70, 0X00, 0X00, 0X02, 0XA0, 0X01, 0X50, 0X00, 0X00, 0X0A, 0XA0, 0X15, 0X50,
    0X00, 0X00, 0XAA, 0XA0, 0X55, 0X50,
};
unsigned char iconSignal0[22] =
{
    0X00, 0X20, 0X00, 0X50, 0X00, 0X00, 0X02, 0XA0, 0X01, 0X50, 0X00, 0X00, 0X0A, 0XA0, 0X15, 0X50,
    0X00, 0X00, 0XAA, 0XA0, 0X55, 0X50,
};
unsigned char iconSignalOff[22] =
{
    0XFE, 0X70, 0XBA, 0X70, 0XD6, 0X00, 0XEF, 0XF0, 0XD7, 0XF0, 0XBA, 0X00, 0XFF, 0XF0, 0X1F, 0XF0,
    0X00, 0X00, 0XFF, 0XF0, 0XFF, 0XF0,
};
unsigned char iconClock[22] =
{
    0XEF, 0XB0, 0XDF, 0XF0, 0XBD, 0XE0, 0X7D, 0XF0, 0X7D, 0XF0,
    0X01, 0XF0, 0X7F, 0XF0, 0X7F, 0XF0, 0XBF, 0XE0, 0XDF, 0XF0, 0XEF, 0XB0,
};
unsigned char iconAlarm[24] =
{
    0X00, 0XC0, 0X1F, 0XC0, 0X3F, 0XC0, 0X7F, 0XC0, 0X7F, 0XE0, 0XFF, 0XF0, 0XFF, 0XF0, 0X7F, 0XE0,
    0X7F, 0XC0, 0X3F, 0XC0, 0X1F, 0XC0, 0X00, 0XC0,
};
unsigned char iconAlarmOff[24] =
{
    0X00, 0XD0, 0X1F, 0XA0, 0X3F, 0X40, 0X7E, 0X80, 0X7D, 0X60, 0XFA, 0XF0, 0XF5, 0XF0, 0X6B, 0XE0,
    0X57, 0XC0, 0X2F, 0XC0, 0X5F, 0XC0, 0X80, 0XC0,
};
unsigned char iconSw1000[24] =
{
    0X3F, 0XFF, 0X00, 0X7F, 0XF9, 0X80, 0XFF, 0XF0, 0XC0, 0XFF, 0XE0, 0X40, 0XFF, 0XE0, 0X40, 0XFF,
    0XF0, 0XC0, 0X7F, 0XF9, 0X80, 0X3F, 0XFF, 0X00,
};
unsigned char iconSw1500[24] =
{
    0X3F, 0XFF, 0X00, 0X7F, 0X3F, 0X80, 0XFE, 0X1F, 0XC0, 0XFC, 0X0F, 0XC0, 0XFC, 0X0F, 0XC0, 0XFE,
    0X1F, 0XC0, 0X7F, 0X3F, 0X80, 0X3F, 0XFF, 0X00,
};
unsigned char iconSw2000[24] =
{
    0X3F, 0XFF, 0X00, 0X67, 0XFF, 0X80, 0XC3, 0XFF, 0XC0, 0X81, 0XFF, 0XC0, 0X81, 0XFF, 0XC0, 0XC3,
    0XFF, 0XC0, 0X67, 0XFF, 0X80, 0X3F, 0XFF, 0X00,
};
unsigned char iconPlane[96] =
{
    0X00, 0X10, 0X00, 0X30, 0X00, 0X30, 0X00, 0X38, 0X00, 0X78, 0X00, 0X78, 0X00, 0X78, 0X00, 0XFC,
    0X00, 0XFF, 0X01, 0XFC, 0X01, 0XFC, 0X03, 0X7C, 0X03, 0X7C, 0X02, 0X7C, 0X02, 0X7C, 0X02, 0X7C,
    0X02, 0XFC, 0X02, 0XFC, 0X02, 0XFC, 0X03, 0XFC, 0X03, 0XFE, 0X03, 0XFE, 0X01, 0XFE, 0X01, 0XFE,
    0X01, 0XFE, 0X01, 0XFE, 0X01, 0XFF, 0X00, 0XFF, 0X00, 0XFC, 0X00, 0XFC, 0X00, 0XFC, 0X00, 0XF8,
    0X00, 0XF8, 0X00, 0XF8, 0X01, 0XF8, 0X01, 0XF8, 0X03, 0XF8, 0X07, 0XF0, 0X0F, 0XF0, 0X1F, 0XF0,
    0X3F, 0XF0, 0X7F, 0XE0, 0XFF, 0XE0, 0XFE, 0XE0, 0XFC, 0XC0, 0XF0, 0X00, 0XC0, 0X00, 0X80, 0X00,
};
unsigned char iconCar[96] =
{
    0X00, 0X00, 0X00, 0X08, 0X00, 0XE8, 0X01, 0XF8, 0X01, 0XF8, 0X03, 0XF8, 0X03, 0X80, 0X07, 0X38,
    0X06, 0X7C, 0X0E, 0XFE, 0X0E, 0XFE, 0X0E, 0XFE, 0X0E, 0X7C, 0X0F, 0X38, 0X0F, 0X80, 0X0F, 0XF8,
    0X0F, 0XF8, 0X0F, 0XF8, 0X1F, 0XF8, 0X1F, 0XF8, 0X1F, 0XF8, 0X17, 0XF8, 0X37, 0XF8, 0X27, 0XF8,
    0X67, 0XF8, 0X47, 0XF8, 0X47, 0XF8, 0X47, 0XF8, 0X47, 0XF8, 0X47, 0XF8, 0X4F, 0XF8, 0X4F, 0XF8,
    0X4F, 0XF8, 0X6F, 0X80, 0X6F, 0X38, 0X3E, 0X7C, 0X3E, 0XFE, 0X3E, 0XFE, 0X1E, 0XFE, 0X1E, 0X7C,
    0X3F, 0X38, 0X7F, 0X80, 0X7F, 0XE0, 0X6F, 0XC0, 0X6F, 0X80, 0X40, 0X00, 0X40, 0X00, 0X00, 0X00,
};
unsigned char iconBoat[96] =
{
    0X00, 0X00, 0X01, 0X00, 0X01, 0X80, 0X01, 0XC0, 0X01, 0XC0, 0X01, 0XE0, 0X01, 0XE0, 0X01, 0XF0,
    0X01, 0XF0, 0X01, 0XF0, 0X01, 0XF0, 0X01, 0XF8, 0X01, 0XF8, 0X01, 0XF8, 0X01, 0XF8, 0X01, 0XFC,
    0X01, 0XFC, 0X01, 0XFC, 0X01, 0XFC, 0X01, 0XFC, 0X01, 0XFC, 0X03, 0XFC, 0X03, 0XFC, 0X03, 0XFC,
    0X03, 0XFC, 0X07, 0XFC, 0X07, 0XFC, 0X07, 0XFC, 0X0F, 0XFC, 0X1F, 0XFC, 0X01, 0XFC, 0X01, 0XFC,
    0X03, 0XFC, 0X07, 0XFC, 0X01, 0XFC, 0X07, 0XFC, 0X07, 0XFC, 0X07, 0XFC, 0X07, 0XFC, 0X07, 0XF4,
    0X07, 0XFE, 0X07, 0XF4, 0X07, 0XF0, 0X01, 0XFE, 0X01, 0XFE, 0X01, 0XF0, 0X00, 0X00, 0X00, 0X00,
};

void main2menuList(u8 hz1, u8 hz2, u8 hz3, u8 hz4);
void menuList2tdwt(void);
void menuList2tdzf(void);
void menuList2tdys(void);

Key_index_struct Key_table[MENU_NUM] =
{
    //��ǰ, ��, ��, ȷ��, ����, home
    {home, home, home, tdwt, home, home, (*homeWindow)}, //��ҳ�棺home

    {tdwt, tdzf, gybj, tdwt1, home, home, (*menu_tdwt)}, //һ���˵���ͨ��΢��
    {tdzf, hksz, tdwt, tdzf1, home, home, (*menu_tdzf)}, //һ���˵���ͨ������
    {hksz, jsgl, tdzf, sjyhk, home, home, (*menu_hksz)}, //һ���˵����������
    {jsgl, scsz, hksz, xjjs, home, home, (*menu_jsgl)}, //һ���˵������չ���
    {scsz, tdys, jsgl, ppmsc, home, home, (*menu_scsz)}, //һ���˵����������
    {tdys, dljs, scsz, tdys1, home, home, (*menu_tdys)}, //һ���˵���ͨ��ӳ��
    {dljs, xtsz, tdys, dljs18, home, home, (*menu_dljs)}, //һ���˵�����������
    {xtsz, gybj, dljs, cysz, home, home, (*menu_xtsz)}, //һ���˵���ϵͳ����
    {gybj, tdwt, xtsz, gybjInf, home, home, (*menu_gybj)}, //һ���˵������ڱ���

    {tdwt1, tdwt2, tdwt8, tdwt1, tdwt, home, (*menu_tdwt1)}, //�����˵���ͨ��΢��1
    {tdwt2, tdwt3, tdwt1, tdwt2, tdwt, home, (*menu_tdwt2)}, //�����˵���ͨ��΢��2
    {tdwt3, tdwt4, tdwt2, tdwt3, tdwt, home, (*menu_tdwt3)}, //�����˵���ͨ��΢��3
    {tdwt4, tdwt5, tdwt3, tdwt4, tdwt, home, (*menu_tdwt4)}, //�����˵���ͨ��΢��4
    {tdwt5, tdwt6, tdwt4, tdwt5, tdwt, home, (*menu_tdwt5)}, //�����˵���ͨ��΢��5
    {tdwt6, tdwt7, tdwt5, tdwt6, tdwt, home, (*menu_tdwt6)}, //�����˵���ͨ��΢��6
    {tdwt7, tdwt8, tdwt6, tdwt7, tdwt, home, (*menu_tdwt7)}, //�����˵���ͨ��΢��7
    {tdwt8, tdwt1, tdwt7, tdwt8, tdwt, home, (*menu_tdwt8)}, //�����˵���ͨ��΢��8

    {tdzf1, tdzf2, tdzf8, tdzf1, tdzf, home, (*menu_tdzf1)}, //�����˵���ͨ������1
    {tdzf2, tdzf3, tdzf1, tdzf2, tdzf, home, (*menu_tdzf2)}, //�����˵���ͨ������2
    {tdzf3, tdzf4, tdzf2, tdzf3, tdzf, home, (*menu_tdzf3)}, //�����˵���ͨ������3
    {tdzf4, tdzf5, tdzf3, tdzf4, tdzf, home, (*menu_tdzf4)}, //�����˵���ͨ������4
    {tdzf5, tdzf6, tdzf4, tdzf5, tdzf, home, (*menu_tdzf5)}, //�����˵���ͨ������5
    {tdzf6, tdzf7, tdzf5, tdzf6, tdzf, home, (*menu_tdzf6)}, //�����˵���ͨ������6
    {tdzf7, tdzf8, tdzf6, tdzf7, tdzf, home, (*menu_tdzf7)}, //�����˵���ͨ������7
    {tdzf8, tdzf1, tdzf7, tdzf8, tdzf, home, (*menu_tdzf8)}, //�����˵���ͨ������8
    //��ǰ, ��, ��, ȷ��, ����, home
    {sjyhk, vyhk, jfyhk, sjyhk, hksz, home, (*menu_sjyhk)}, //�����˵�����������
    {vyhk, cshk, sjyhk, vyhk, hksz, home, (*menu_vyhk)}, //�����˵���V����
    {cshk, jfyhk, vyhk, cshk, hksz, home, (*menu_cshk)}, //�����˵������ٻ��
    {jfyhk, sjyhk, cshk, jfyhk, hksz, home, (*menu_jfyhk)}, //�����˵���������

    {xjjs, xzmx, txmm, xjjs, jsgl, home, (*menu_xjjs)}, //�����˵����½�����
    {xzmx, bcmx, xjjs, xzmx, jsgl, home, (*menu_xzmx)}, //�����˵���ѡ��ģ��
    {bcmx, txmm, xzmx, bcmx, jsgl, home, (*menu_bcmx)}, //�����˵�������ģ��
    {txmm, xjjs, bcmx, txmm, jsgl, home, (*menu_txmm)}, //�����˵���ͨ������

    {ppmsc, wxfs, fsgl, ppmsc, scsz, home, (*menu_ppmsc)}, //�����˵���PPM���
    {wxfs, fsgl, ppmsc, wxfs, scsz, home, (*menu_wxfs)}, //�����˵������߷���
    {fsgl, ppmsc, wxfs, fsgl, scsz, home, (*menu_fsgl)}, //�����˵������书��

    {tdys1, tdys2, tdys8, tdys1, tdys, home, (*menu_tdys1)}, //�����˵���ͨ��ӳ��1
    {tdys2, tdys3, tdys1, tdys2, tdys, home, (*menu_tdys2)}, //�����˵���ͨ��ӳ��2
    {tdys3, tdys4, tdys2, tdys3, tdys, home, (*menu_tdys3)}, //�����˵���ͨ��ӳ��3
    {tdys4, tdys5, tdys3, tdys4, tdys, home, (*menu_tdys4)}, //�����˵���ͨ��ӳ��4
    {tdys5, tdys6, tdys4, tdys5, tdys, home, (*menu_tdys5)}, //�����˵���ͨ��ӳ��5
    {tdys6, tdys7, tdys5, tdys6, tdys, home, (*menu_tdys6)}, //�����˵���ͨ��ӳ��6
    {tdys7, tdys8, tdys6, tdys7, tdys, home, (*menu_tdys7)}, //�����˵���ͨ��ӳ��7
    {tdys8, tdys1, tdys7, tdys8, tdys, home, (*menu_tdys8)}, //�����˵���ͨ��ӳ��8

    {dljs18, dljs18, dljs18, dljs, dljs, home, (*menu_dljs18)}, //�����˵�����������1-8

    {cysz, dysz, hfcc, wtdw, xtsz, home, (*menu_cysz)}, //�����˵�����������
    {dysz, nzsz, cysz, dyjz, xtsz, home, (*menu_dysz)}, //�����˵�����ѹ����
    {nzsz, ymsz, dysz, nzbj, xtsz, home, (*menu_nzsz)}, //�����˵�����������
    {ymsz, xcjz, nzsz, ymph, xtsz, home, (*menu_ymsz)}, //�����˵�����������
    {xcjz, hfcc, ymsz, xcjzTip, xtsz, home, (*menu_xcjz)}, //�����˵����г�У׼
    {hfcc, cysz, xcjz, hfccTip, xtsz, home, (*menu_hfcc)}, //�����˵����ָ���������

    {wtdw, ajyx, kjhm, wtdw, cysz, home, (*menu_wtdw)}, //�����˵���΢����λ
    {ajyx, kjhm, wtdw, ajyx, cysz, home, (*menu_ajyx)}, //�����˵���������Ч
    {kjhm, wtdw, ajyx, kjhm, cysz, home, (*menu_kjhm)}, //�����˵�����������

    {dyjz, bjdy, jsbj, dyjz, dysz, home, (*menu_dyjz)}, //�����˵�����ѹУ׼
    {bjdy, jsbj, dyjz, bjdy, dysz, home, (*menu_bjdy)}, //�����˵���������ѹ
    {jsbj, dyjz, bjdy, jsbj, dysz, home, (*menu_jsbj)}, //�����˵������ձ���

    {nzbj, nzsc, kjzj, nzbj, nzsz, home, (*menu_nzbj)}, //�����˵������ӱ���
    {nzsc, kjzj, nzbj, nzsc, nzsz, home, (*menu_nzsc)}, //�����˵�������ʱ��
    {kjzj, nzbj, nzsc, kjzj, nzsz, home, (*menu_kjzj)}, //�����˵��������Լ�
    //��ǰ, ��, ��, ȷ��, ����, home
    {ymph, ymqx, skbh, ymph, ymsz, home, (*menu_ymph)}, //�����˵�������ƫ��
    {ymqx, skbh, ymph, ymqx, ymsz, home, (*menu_ymqx)}, //�����˵�����������
    {skbh, ymph, ymqx, skbh, ymsz, home, (*menu_skbh)}, //�����˵���ʧ�ر���

    {xcjzTip, xcjzTip, xcjzTip, xcjz14, xcjz, home, (*menu_xcjzTip)}, //�����˵����г�У׼��ʾ
    {xcjz14, xcjz14, xcjz14, xcjz, xcjz, home, (*menu_xcjz14)}, //�����˵����г�У׼1-4

    {hfccTip, hfccTip, hfccTip, hfcg, hfcc, home, (*menu_hfccTip)}, //�����˵����ָ�����������ʾ
    {hfcg, hfcg, hfcg, hfcc, hfcc, home, (*menu_hfcg)}, //�����˵����ָ��ɹ�

    {gybjInf, gybjInf, gybjInf, gybj, gybj, home, (*menu_gybjInf)}, //�����˵������ڱ�����ʾ

};
//�˵���ѯ
void OLED_display(void)
{
    u16 temp;

    switch(menuEvent[1])
    {
    case MENU_down: //�˵�����
        nowMenuIndex = Key_table[nowMenuIndex].down_index;
        break;

    case MENU_up: //�˵�����
        nowMenuIndex = Key_table[nowMenuIndex].up_index;
        break;

    case MENU_enter://ȷ��
        temp = nowMenuIndex;//��ȡ��һ�ε�ҳ��
        nowMenuIndex = Key_table[nowMenuIndex].enter_index;

        if(IsInKeyMenu(temp) && IsInKeyMenu(nowMenuIndex)) menuMode = !menuMode; //�̰�������������/�˳��༭

        break;

    case MENU_esc://����
        nowMenuIndex = Key_table[nowMenuIndex].esc_index;
        menuMode = 0;//�л������ɱ༭ģʽ
        break;

    case MENU_home://���� ����������
        nowMenuIndex = Key_table[nowMenuIndex].home_index;
        menuMode = 0;//�л������ɱ༭ģʽ
        break;

    case NUM_down:
        break;

    case NUM_up:
        break;
    }

    if(nowMenuIndex == home && lastMenuIndex != home)
    {
        if(lastMenuIndex != xcjz14) homeGoIndex = lastMenuIndex;
        else homeGoIndex = xcjz;

        OLED_Clear_GRAM();//���
    }

    if(nowMenuIndex != home && lastMenuIndex == home)
    {
        nowMenuIndex = homeGoIndex;    //��գ�ʵ�ֿ�����ת
        OLED_Clear_GRAM();
    }

    if(nowMenuIndex >= tdwt && nowMenuIndex <= gybj && (lastMenuIndex < tdwt | lastMenuIndex > gybj)) main2menuList(sheZhi, zhi, cai, dan); //�л�����

    if(nowMenuIndex >= tdwt1 && nowMenuIndex <= tdwt8 && (lastMenuIndex < tdwt1 | lastMenuIndex > tdwt8))menuList2tdwt(); //�л�����

    if(nowMenuIndex >= tdzf1 && nowMenuIndex <= tdzf8 && (lastMenuIndex < tdzf1 | lastMenuIndex > tdzf8))menuList2tdzf(); //�л�����

    if(nowMenuIndex >= sjyhk && nowMenuIndex <= jfyhk && (lastMenuIndex < sjyhk | lastMenuIndex > jfyhk))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= xjjs && nowMenuIndex <= txmm && (lastMenuIndex < xjjs | lastMenuIndex > txmm))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= ppmsc && nowMenuIndex <= fsgl && (lastMenuIndex < ppmsc | lastMenuIndex > fsgl))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= tdys1 && nowMenuIndex <= tdys8 && (lastMenuIndex < tdys1 | lastMenuIndex > tdys8))menuList2tdys(); //�л�����

    if(nowMenuIndex == dljs18 && lastMenuIndex != dljs18) OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= cysz && nowMenuIndex <= hfcc && (lastMenuIndex < cysz | lastMenuIndex > hfcc)) main2menuList(xi, tongYi, sheZhi, zhi); //�л�����

    if(nowMenuIndex >= wtdw && nowMenuIndex <= kjhm && (lastMenuIndex < wtdw | lastMenuIndex > kjhm))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= dyjz && nowMenuIndex <= jsbj && (lastMenuIndex < dyjz | lastMenuIndex > jsbj))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= nzbj && nowMenuIndex <= kjzj && (lastMenuIndex < nzbj | lastMenuIndex > kjzj))OLED_Clear_GRAM(); //���

    if(nowMenuIndex >= ymph && nowMenuIndex <= skbh && (lastMenuIndex < ymph | lastMenuIndex > skbh))OLED_Clear_GRAM(); //���

    if(nowMenuIndex == xcjz14 && lastMenuIndex != xcjz14) OLED_Clear_GRAM(); //���

//	printf("key:%d, last:%d, now:%d\r\n",menuEvent[1],lastMenuIndex,nowMenuIndex);
    if(nowMenuIndex != lastMenuIndex)
    {
        Key_table[nowMenuIndex].operate();
        OLED_Refresh_Gram();//ˢ���Դ�
    }
}

void showSwState(void)
{
    u8 i = 0;

    for(i = 0; i < 4; i++) //�ĸ�����ͨ����ʾ
    {
        if(PWMvalue[4 + i] < 1250) OLED_DrawPointBMP(14 + 12 * i, 18, iconSw1000, 11, 18, 1);
        else if(PWMvalue[4 + i] < 1750) OLED_DrawPointBMP(14 + 12 * i, 18, iconSw1500, 11, 18, 1);
        else OLED_DrawPointBMP(14 + 12 * i, 18, iconSw2000, 11, 18, 1);
    }
}

void homeWindow(void)
{
    u16 thrNum;
    int loca;

    OLED_DrawPointBMP(0, 0, iconJ20, 54, 15, 1); //J20ͼ��

    if(setData.NRF_Mode == OFF) OLED_DrawPointBMP(51, 1, iconSignalOff, 15, 12, 1); //�ź�ͼ��
    else OLED_DrawPointBMP(51, 1, iconSignal100, 15, 12, 1); //�ź�ͼ��

    if(setData.clockMode == ON)
    {
        OLED_DrawPointBMP(68, 1, iconAlarm, 16, 12, 1);   //����ͼ��
    }
    else
    {
        OLED_DrawPointBMP(68, 1, iconAlarmOff, 16, 12, 1);
    }

    drawClockTime();//ʱ����ʾ00:00:00
    showSwState();//��ʾ��4��ͨ����״̬

    if(setData.modelType == 0) OLED_DrawPointBMP(11, 39, iconPlane, 48, 16, 1); //ģ��ͼ��
    else if(setData.modelType == 1) OLED_DrawPointBMP(11, 39, iconCar, 48, 16, 1); //ģ��ͼ��
    else OLED_DrawPointBMP(11, 39, iconBoat, 48, 16, 1); //ģ��ͼ��

    OLED_ShowString(70, 19, (u8 *)"T", 16, 1); //ң����
    sprintf((char *)batVoltStr, "%1.2fV", batVolt);
    OLED_ShowString(80, 19, (u8 *)batVoltStr, 16, 1); //��ʾ��ص�ѹ

    OLED_ShowString(70, 38, (u8 *)"R", 16, 1); //���ջ�
    OLED_ShowString(80, 38, (u8 *)"12.6V", 16, 1); //��ʾ��ص�ѹ

    OLED_Fill(0, 15, 4, 63, 1); //����
    OLED_DrawPoint(1, 39, 0);
    OLED_DrawPoint(3, 39, 0);
    OLED_Fill(123, 15, 127, 63, 1); //�Ҳ��
    OLED_DrawPoint(124, 39, 0);
    OLED_DrawPoint(126, 39, 0);
    OLED_Fill(7, 59, 62, 63, 1); //�ײ����-4
    OLED_Fill(65, 59, 120, 63, 1); //�ײ��ҿ�-1

    if(setData.throttlePreference)//��������
    {
        OLED_DrawPoint(35, 60, 0); //���ı�ʶ-4
        OLED_DrawPoint(35, 62, 0); //���ı�ʶ-4
        OLED_DrawPoint(93, 60, 0); //���ı�ʶ-1
        OLED_DrawPoint(93, 62, 0); //���ı�ʶ-1
        //��1ͨ��΢����ʶ-���-�Һ���
        OLED_Fill(66, 61, 119, 61, 1); //д1�����ԭ���ı�־
        loca = (int)93 + setData.PWMadjustValue[0] / 12.5;
        OLED_Fill(loca - 2, 61, loca + 2, 61, 0); //д0

        //��2ͨ��΢����ʶ-������-������
        OLED_Fill(125, 16, 125, 62, 1); //д1
        loca = (int)39 - setData.PWMadjustValue[1] / 14.29;
        OLED_Fill(125, loca - 2, 125, loca + 2, 0); //д1

        //��3ͨ�������г���-������
        thrNum = (int)(PWMvalue[2] - 1000) / 22;
        OLED_Fill(2, 62 - thrNum, 2, 62, 0); //�²���д1
        OLED_Fill(2, 16, 2, 62 - thrNum, 1); //�ϲ���д0

        //��4ͨ��΢����ʶ-�����-�����
        OLED_Fill(7, 61, 61, 61, 1); //д1�����ԭ���ı�־
        loca = (int)35 + setData.PWMadjustValue[3] / 12.5;
        OLED_Fill(loca - 2, 61, loca + 2, 61, 0); //д0
    }
    else //��������
    {
        OLED_DrawPoint(34, 60, 0); //���ı�ʶ-4
        OLED_DrawPoint(34, 62, 0); //���ı�ʶ-4
        OLED_DrawPoint(93, 60, 0); //���ı�ʶ-1
        OLED_DrawPoint(93, 62, 0); //���ı�ʶ-1
        //��1ͨ��΢����ʶ-���-�����
        OLED_Fill(7, 61, 61, 61, 1); //д1�����ԭ���ı�־
        loca = (int)34 + setData.PWMadjustValue[0] / 12.5;
        OLED_Fill(loca - 2, 61, loca + 2, 61, 0); //д0

        //��2ͨ��΢����ʶ-������-������
        OLED_Fill(2, 16, 2, 62, 1); //д1
        loca = (int)39 - setData.PWMadjustValue[1] / 14.29;
        OLED_Fill(2, loca - 2, 2, loca + 2, 0); //д1

        //��3ͨ�������г���-������
        thrNum = (int)(PWMvalue[2] - 1000) / 22;
        OLED_Fill(125, 62 - thrNum, 125, 62, 0); //�²���д1
        OLED_Fill(125, 16, 125, 62 - thrNum, 1); //�ϲ���д0

        //��4ͨ��΢����ʶ-�����-�Һ���
        OLED_Fill(66, 61, 119, 61, 1); //д1�����ԭ���ı�־
        loca = (int)93 + setData.PWMadjustValue[3] / 12.5;
        OLED_Fill(loca - 2, 61, loca + 2, 61, 0); //д0
    }
}
//������-һ���˵� �л�
void main2menuList(u8 hz1, u8 hz2, u8 hz3, u8 hz4)
{
    OLED_Fill(0, 0, 127, 15, 1); //�������
    OLED_Fill(0, 16, 127, 63, 0); //����°벿��
    //�����˵��������ò˵�
    u8 szcdIndex[] = {hz1, hz2, hz3, hz4};
    OLED_ShowChineseWords(32, 0, szcdIndex, 4, 0);
    //�����
    OLED_Fill(115, 16, 115, 63, 1); //��������������
    OLED_Fill(127, 16, 127, 63, 1); //��������������
    OLED_DrawPointBMP(116, 16, iconTop, 15, 6, 1); //������ϼ�ͷ
    OLED_DrawPointBMP(116, 58, iconBottom, 15, 6, 1); //������¼�ͷ
}
/*
listIndex����������鶨λ,1~8
mode�����е���ʾģʽ
str1~3�������е��ַ���
hzIndex�������е�12�������������е�����
*/
void menuListWindow(u8 listIndex, u8 mode[], u8 *str1, u8 *str2, u8 *str3, u8 hzIndex[], u8 len)
{
    //��ͷ
    OLED_Fill(0, 16, 16, 63, 0); //��ͷ���
    OLED_DrawPointBMP(0, 16 + 16 * ((listIndex - 1) % 3), iconHand, 16, 16, 1); //ָʾ��ͷ
    //��һ��
    OLED_ShowString(16, 16, str1, 16, mode[0]);
    u8 line1Index[] = {hzIndex[0], hzIndex[1], hzIndex[2], hzIndex[3]};
    OLED_ShowChineseWords(32, 16, line1Index, 4, mode[0]);
    //�ڶ���
    OLED_ShowString(16, 32, str2, 16, mode[1]);
    u8 line2Index[] = {hzIndex[4], hzIndex[5], hzIndex[6], hzIndex[7]};
    OLED_ShowChineseWords(32, 32, line2Index, 4, mode[1]);
    //������
    OLED_ShowString(16, 48, str3, 16, mode[2]);
    u8 line3Index[] = {hzIndex[8], hzIndex[9], hzIndex[10], hzIndex[11]};
    OLED_ShowChineseWords(32, 48, line3Index, 4, mode[2]);
    //�����
    OLED_Fill(116, 22, 126, 57, 0); //��������
    OLED_Fill(116, (u8)19 + 32 * listIndex / len, 126, (u8)24 + 32 * listIndex / len, 1); //������������
}
void menu_tdwt(void)
{
    u8 modes[3] = {0, 1, 1};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {tongDao, dao, weiTiao, tiao, tongDao, dao, zheng, fan, hun, kong, sheZhi, zhi};
    menuListWindow(1, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���1.ͨ��΢��

void menu_tdzf(void)
{
    u8 modes[3] = {1, 0, 1};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {tongDao, dao, weiTiao, tiao, tongDao, dao, zheng, fan, hun, kong, sheZhi, zhi};
    menuListWindow(2, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���2.ͨ������
void menu_hksz(void)
{
    u8 modes[3] = {1, 1, 0};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {tongDao, dao, weiTiao, tiao, tongDao, dao, zheng, fan, hun, kong, sheZhi, zhi};
    menuListWindow(3, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���3.�������
void menu_jsgl(void)
{
    u8 modes[3] = {0, 1, 1};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {jie, shouDao, guanLi, li, shu, chu, sheZhi, zhi, tongDao, dao, ying, sheMen};
    menuListWindow(4, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���4.���չ���
void menu_scsz(void)
{
    u8 modes[3] = {1, 0, 1};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {jie, shouDao, guanLi, li, shu, chu, sheZhi, zhi, tongDao, dao, ying, sheMen};
    menuListWindow(5, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���5.�������
void menu_tdys(void)
{
    u8 modes[3] = {1, 1, 0};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {jie, shouDao, guanLi, li, shu, chu, sheZhi, zhi, tongDao, dao, ying, sheMen};
    menuListWindow(6, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���6.ͨ��ӳ��
void menu_dljs(void)
{
    u8 modes[3] = {0, 1, 1};
    u8 str1[] = "7.";
    u8 str2[] = "8.";
    u8 str3[] = "9.";
    u8 hzIndex[12] = {duo, liang, jianShi, shiYe, xi, tongYi, sheZhi, zhi, guanMen, yu, ben, jiQi};
    menuListWindow(7, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���7.��������
void menu_xtsz(void)
{
    u8 modes[3] = {1, 0, 1};
    u8 str1[] = "7.";
    u8 str2[] = "8.";
    u8 str3[] = "9.";
    u8 hzIndex[12] = {duo, liang, jianShi, shiYe, xi, tongYi, sheZhi, zhi, guanMen, yu, ben, jiQi};
    menuListWindow(8, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���8.ϵͳ����
void menu_gybj(void)
{
    u8 modes[3] = {1, 1, 0};
    u8 str1[] = "7.";
    u8 str2[] = "8.";
    u8 str3[] = "9.";
    u8 hzIndex[12] = {duo, liang, jianShi, shiYe, xi, tongYi, sheZhi, zhi, guanMen, yu, ben, jiQi};
    menuListWindow(9, modes, str1, str2, str3, hzIndex, 9);
}//һ���˵���9.���ڱ���

//һ���˵�-�����˵� ͨ��΢������ �л�
void menuList2tdwt(void)
{
    OLED_Clear_GRAM();//���
    OLED_Fill(63, 0, 64, 63, 1); //�м�����
}
/*mode[]������CH����ʾģʽ����
lineIndex:�ڼ�ͨ��1-8
*/
void tdwtWindow(u8 mode[], u8 lineIndex)
{
    OLED_ShowString(1, 0, (u8 *)"CH1", 16, mode[0]);

    if(lineIndex == 1)  OLED_ShowNum(30, 0, abs(setData.PWMadjustValue[0]), 3, 16, !menuMode);
    else OLED_ShowNum(30, 0, abs(setData.PWMadjustValue[0]), 3, 16, 1);

    if(setData.PWMadjustValue[0] < 0) OLED_ShowString(54, 0, (u8 *)"-", 16, 1);
    else OLED_ShowString(54, 0, (u8 *)"+", 16, 1);

    OLED_ShowString(1, 16, (u8 *)"CH2", 16, mode[1]);

    if(lineIndex == 2)  OLED_ShowNum(30, 16, abs(setData.PWMadjustValue[1]), 3, 16, !menuMode);
    else OLED_ShowNum(30, 16, abs(setData.PWMadjustValue[1]), 3, 16, 1);

    if(setData.PWMadjustValue[1] < 0) OLED_ShowString(54, 16, (u8 *)"-", 16, 1);
    else OLED_ShowString(54, 16, (u8 *)"+", 16, 1);

    OLED_ShowString(1, 32, (u8 *)"CH3", 16, mode[2]);

    if(lineIndex == 3)  OLED_ShowNum(30, 32, abs(setData.PWMadjustValue[2]), 3, 16, !menuMode);
    else OLED_ShowNum(30, 32, abs(setData.PWMadjustValue[2]), 3, 16, 1);

    if(setData.PWMadjustValue[2] < 0) OLED_ShowString(54, 32, (u8 *)"-", 16, 1);
    else OLED_ShowString(54, 32, (u8 *)"+", 16, 1);

    OLED_ShowString(1, 48, (u8 *)"CH4", 16, mode[3]);

    if(lineIndex == 4)  OLED_ShowNum(30, 48, abs(setData.PWMadjustValue[3]), 3, 16, !menuMode);
    else OLED_ShowNum(30, 48, abs(setData.PWMadjustValue[3]), 3, 16, 1);

    if(setData.PWMadjustValue[3] < 0) OLED_ShowString(54, 48, (u8 *)"-", 16, 1);
    else OLED_ShowString(54, 48, (u8 *)"+", 16, 1);

    OLED_ShowString(66, 0, (u8 *)"CH5", 16, mode[4]);

    if(lineIndex == 5)  OLED_ShowNum(95, 0, abs(setData.PWMadjustValue[4]), 3, 16, !menuMode);
    else OLED_ShowNum(95, 0, abs(setData.PWMadjustValue[4]), 3, 16, 1);

    if(setData.PWMadjustValue[4] < 0) OLED_ShowString(119, 0, (u8 *)"-", 16, 1);
    else OLED_ShowString(119, 0, (u8 *)"+", 16, 1);

    OLED_ShowString(66, 16, (u8 *)"CH6", 16, mode[5]);

    if(lineIndex == 6)  OLED_ShowNum(95, 16, abs(setData.PWMadjustValue[5]), 3, 16, !menuMode);
    else OLED_ShowNum(95, 16, abs(setData.PWMadjustValue[5]), 3, 16, 1);

    if(setData.PWMadjustValue[5] < 0) OLED_ShowString(119, 16, (u8 *)"-", 16, 1);
    else OLED_ShowString(119, 16, (u8 *)"+", 16, 1);

    OLED_ShowString(66, 32, (u8 *)"CH7", 16, mode[6]);

    if(lineIndex == 7)  OLED_ShowNum(95, 32, abs(setData.PWMadjustValue[6]), 3, 16, !menuMode);
    else OLED_ShowNum(95, 32, abs(setData.PWMadjustValue[6]), 3, 16, 1);

    if(setData.PWMadjustValue[6] < 0) OLED_ShowString(119, 32, (u8 *)"-", 16, 1);
    else OLED_ShowString(119, 32, (u8 *)"+", 16, 1);

    OLED_ShowString(66, 48, (u8 *)"CH8", 16, mode[7]);

    if(lineIndex == 8)  OLED_ShowNum(95, 48, abs(setData.PWMadjustValue[7]), 3, 16, !menuMode);
    else OLED_ShowNum(95, 48, abs(setData.PWMadjustValue[7]), 3, 16, 1);

    if(setData.PWMadjustValue[7] < 0) OLED_ShowString(119, 48, (u8 *)"-", 16, 1);
    else OLED_ShowString(119, 48, (u8 *)"+", 16, 1);
}
void menu_tdwt1(void)
{
    u8 modes[8] = {0, 1, 1, 1, 1, 1, 1, 1};
    tdwtWindow(modes, 1);
}//�����˵���ͨ��1
void menu_tdwt2(void)
{
    u8 modes[8] = {1, 0, 1, 1, 1, 1, 1, 1};
    tdwtWindow(modes, 2);
}//�����˵���ͨ��2
void menu_tdwt3(void)
{
    u8 modes[8] = {1, 1, 0, 1, 1, 1, 1, 1};
    tdwtWindow(modes, 3);
}//�����˵���ͨ��3
void menu_tdwt4(void)
{
    u8 modes[8] = {1, 1, 1, 0, 1, 1, 1, 1};
    tdwtWindow(modes, 4);
}//�����˵���ͨ��4
void menu_tdwt5(void)
{
    u8 modes[8] = {1, 1, 1, 1, 0, 1, 1, 1};
    tdwtWindow(modes, 5);
}//�����˵���ͨ��5
void menu_tdwt6(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 0, 1, 1};
    tdwtWindow(modes, 6);
}//�����˵���ͨ��6
void menu_tdwt7(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 0, 1};
    tdwtWindow(modes, 7);
}//�����˵���ͨ��7
void menu_tdwt8(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 1, 0};
    tdwtWindow(modes, 8);
}//�����˵���ͨ��8

//һ���˵�-�����˵� ͨ��΢������ �л�
void menuList2tdzf(void)
{
    OLED_Clear_GRAM();//���
    OLED_Fill(63, 0, 64, 63, 1); //�м�����
}
/*mode[]������CH����ʾģʽ����
listIndex:�ڼ�ͨ��1-8
*/
void tdzfWindow(u8 mode[], u8 listIndex)
{
    OLED_ShowString(1, 0, (u8 *)"CH1", 16, mode[0]);

    if(setData.chReverse[0] == ON)OLED_DrawPointBMP(30, 1, onSign, 16, 14, listIndex == 1 ? !menuMode : 1); //��
    else OLED_DrawPointBMP(30, 1, offSign, 16, 14, listIndex == 1 ? !menuMode : 1); ///��

    OLED_ShowString(1, 16, (u8 *)"CH2", 16, mode[1]);

    if(setData.chReverse[1] == ON)OLED_DrawPointBMP(30, 17, onSign, 16, 14, listIndex == 2 ? !menuMode : 1); ///��
    else OLED_DrawPointBMP(30, 17, offSign, 16, 14, listIndex == 2 ? !menuMode : 1); //��

    OLED_ShowString(1, 32, (u8 *)"CH3", 16, mode[2]);

    if(setData.chReverse[2] == ON)OLED_DrawPointBMP(30, 33, onSign, 16, 14, listIndex == 3 ? !menuMode : 1); ///��
    else OLED_DrawPointBMP(30, 33, offSign, 16, 14, listIndex == 3 ? !menuMode : 1); //��

    OLED_ShowString(1, 48, (u8 *)"CH4", 16, mode[3]);

    if(setData.chReverse[3] == ON)OLED_DrawPointBMP(30, 49, onSign, 16, 14, listIndex == 4 ? !menuMode : 1); ///��
    else OLED_DrawPointBMP(30, 49, offSign, 16, 14, listIndex == 4 ? !menuMode : 1); //��

    OLED_ShowString(66, 0, (u8 *)"CH5", 16, mode[4]);

    if(setData.chReverse[4] == ON)OLED_DrawPointBMP(94, 1, onSign, 16, 14, listIndex == 5 ? !menuMode : 1); ///��
    else OLED_DrawPointBMP(94, 1, offSign, 16, 14, listIndex == 5 ? !menuMode : 1); //��

    OLED_ShowString(66, 16, (u8 *)"CH6", 16, mode[5]);

    if(setData.chReverse[5] == ON)OLED_DrawPointBMP(94, 17, onSign, 16, 14, listIndex == 6 ? !menuMode : 1); //��
    else OLED_DrawPointBMP(94, 17, offSign, 16, 14, listIndex == 6 ? !menuMode : 1); //��

    OLED_ShowString(66, 32, (u8 *)"CH7", 16, mode[6]);

    if(setData.chReverse[6] == ON)OLED_DrawPointBMP(94, 33, onSign, 16, 14, listIndex == 7 ? !menuMode : 1); //��
    else OLED_DrawPointBMP(94, 33, offSign, 16, 14, listIndex == 7 ? !menuMode : 1); //��

    OLED_ShowString(66, 48, (u8 *)"CH8", 16, mode[7]);

    if(setData.chReverse[7] == ON)OLED_DrawPointBMP(94, 49, onSign, 16, 14, listIndex == 8 ? !menuMode : 1); //��
    else OLED_DrawPointBMP(94, 49, offSign, 16, 14, listIndex == 8 ? !menuMode : 1); //��
}
void menu_tdzf1(void)
{
    u8 modes[8] = {0, 1, 1, 1, 1, 1, 1, 1};
    tdzfWindow(modes, 1);
}//�����˵���ͨ��1
void menu_tdzf2(void)
{
    u8 modes[8] = {1, 0, 1, 1, 1, 1, 1, 1};
    tdzfWindow(modes, 2);
}//�����˵���ͨ��2
void menu_tdzf3(void)
{
    u8 modes[8] = {1, 1, 0, 1, 1, 1, 1, 1};
    tdzfWindow(modes, 3);
}//�����˵���ͨ��3
void menu_tdzf4(void)
{
    u8 modes[8] = {1, 1, 1, 0, 1, 1, 1, 1};
    tdzfWindow(modes, 4);
}//�����˵���ͨ��4
void menu_tdzf5(void)
{
    u8 modes[8] = {1, 1, 1, 1, 0, 1, 1, 1};
    tdzfWindow(modes, 5);
}//�����˵���ͨ��5
void menu_tdzf6(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 0, 1, 1};
    tdzfWindow(modes, 6);
}//�����˵���ͨ��6
void menu_tdzf7(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 0, 1};
    tdzfWindow(modes, 7);
}//�����˵���ͨ��7
void menu_tdzf8(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 1, 0};
    tdzfWindow(modes, 8);
}//�����˵���ͨ��8
/*
listIndex��0-3
mode�����е���ʾģʽ
*/
void hkszWindow(u8 listIndex, u8 mode[])
{
    //��ͷ
    OLED_Fill(0, 0, 16, 63, 0); //��ͷ���
    OLED_DrawPointBMP(0, 16 * listIndex, iconHand, 16, 16, 1); //ָʾ��ͷ
    //��һ��
    OLED_ShowString(16, 0, (u8 *)"1.", 16, mode[0]);
    u8 line1Index[] = {san, jiaoDu, yi, hun, kong};
    OLED_ShowChineseWords(32, 0, line1Index, 5, mode[0]);
    //�ڶ���
    OLED_ShowString(16, 16, (u8 *)"2.", 16, mode[1]);
    OLED_ShowString(32, 16, (u8 *)"V ", 16, mode[1]);
    u8 line2Index[] = {yi, hun, kong};
    OLED_ShowChineseWords(48, 16, line2Index, 3, mode[1]);
    //������
    OLED_ShowString(16, 32, (u8 *)"3.", 16, mode[2]);
    u8 line3Index[] = {cha, su, hun, kong};
    OLED_ShowChineseWords(32, 32, line3Index, 4, mode[2]);
    //������
    OLED_ShowString(16, 48, (u8 *)"4.", 16, mode[3]);
    u8 line4Index[] = {jin, fuYi, yi, hun, kong};
    OLED_ShowChineseWords(32, 48, line4Index, 5, mode[3]);
}
void menu_sjyhk(void)
{
    u8 modes[4] = {0, 1, 1, 1};
    hkszWindow(0, modes);
}//�����˵�����������
void menu_vyhk(void)
{
    u8 modes[4] = {1, 0, 1, 1};
    hkszWindow(1, modes);
}//�����˵���V����
void menu_cshk(void)
{
    u8 modes[4] = {1, 1, 0, 1};
    hkszWindow(2, modes);
}//�����˵������ٻ��
void menu_jfyhk(void)
{
    u8 modes[4] = {1, 1, 1, 0};
    hkszWindow(3, modes);
}//�����˵���������
/*
listIndex��0-3
mode�����е���ʾģʽ
*/
void jsglWindow(u8 listIndex, u8 mode[])
{
    //��һ��
    OLED_ShowString(0, 0, (u8 *)"1.", 16, mode[0]);
    u8 line1Index[] = {xinJian, jianShe, jie, shouDao};
    OLED_ShowChineseWords(16, 0, line1Index, 4, mode[0]);
    //�ڶ���
    OLED_ShowString(0, 16, (u8 *)"2.", 16, mode[1]);
    u8 line2Index[] = {xuan, ze, moXing, xingHao};
    OLED_ShowChineseWords(16, 16, line2Index, 4, mode[1]);

    if(setData.modelType == 0)OLED_ShowChinese(88, 16, yi, 16, listIndex == 1 ? !menuMode : 1);
    else if(setData.modelType == 1)OLED_ShowChinese(88, 16, che, 16, listIndex == 1 ? !menuMode : 1);
    else OLED_ShowChinese(88, 16, chuanBoat, 16, listIndex == 1 ? !menuMode : 1);

    //������
    OLED_ShowString(0, 32, (u8 *)"3.", 16, mode[2]);
    u8 line3Index[] = {baoCun, cun, moXing, xingHao};
    OLED_ShowChineseWords(16, 32, line3Index, 4, mode[2]);
    //������
    OLED_ShowString(0, 48, (u8 *)"4.", 16, mode[3]);
    u8 line4Index[] = {tongDao, xinXin, mi, ma};
    OLED_ShowChineseWords(16, 48, line4Index, 4, mode[3]);
}
void menu_xjjs(void)
{
    u8 modes[4] = {0, 1, 1, 1};
    jsglWindow(0, modes);
}//�����˵����½�����
void menu_xzmx(void)
{
    u8 modes[4] = {1, 0, 1, 1};
    jsglWindow(1, modes);
}//�����˵���ѡ��ģ��
void menu_bcmx(void)
{
    u8 modes[4] = {1, 1, 0, 1};
    jsglWindow(2, modes);
}//�����˵�������ģ��
void menu_txmm(void)
{
    u8 modes[4] = {1, 1, 1, 0};
    jsglWindow(3, modes);
}//�����˵���ͨ������
/*
�������
listIndex��0-3
mode�����е���ʾģʽ
*/
void scszWindow(u8 listIndex, u8 mode[])
{
    //��һ��
    OLED_Fill(0, 0, 31, 15, 1); //�������
    u8 line1Index[] = {shu, chu, sheZhi, zhi};
    OLED_ShowChineseWords(32, 0, line1Index, 4, 0);
    OLED_Fill(96, 0, 127, 15, 1); //�������
    //�ڶ���
    OLED_ShowString(0, 16, (u8 *)"1.PPM", 16, mode[0]);
    u8 line2Index[] = {shu, chu};
    OLED_ShowChineseWords(40, 16, line2Index, 2, mode[0]);

    if(setData.PPM_Out == ON)
    {
        OLED_DrawPointBMP(88, 17, onSign, 16, 14, listIndex == 0 ? !menuMode : 1); //�򿪱�־
        PPM_ON();//����PPM���
    }
    else
    {
        OLED_DrawPointBMP(88, 17, offSign, 16, 14, listIndex == 0 ? !menuMode : 1); //�رձ�־
        PPM_OFF();//�ر�PPM���
    }

    //������
    OLED_ShowString(0, 32, (u8 *)"2.", 16, mode[1]);
    u8 line3Index[] = {wu, xian, fa, sheMen};
    OLED_ShowChineseWords(16, 32, line3Index, 4, mode[1]);

    if(setData.NRF_Mode == ON)
    {
        OLED_DrawPointBMP(88, 33, onSign, 16, 14, listIndex == 1 ? !menuMode : 1); //�򿪱�־
        NRF24L01_TX_Mode(setData.NRF_Power);//����ģʽ
    }
    else
    {
        OLED_DrawPointBMP(88, 33, offSign, 16, 14, listIndex == 1 ? !menuMode : 1); //�رձ�־
        NRF24L01_LowPower_Mode();//����ģʽ
    }

    //������
    OLED_ShowString(0, 48, (u8 *)"3.", 16, mode[2]);
    u8 line4Index[] = {fa, sheMen, gong, lv};
    OLED_ShowChineseWords(16, 48, line4Index, 4, mode[2]);

    switch (setData.NRF_Power)
    {
    case 0x0f:
        OLED_ShowChinese(88, 48, gao, 16, listIndex == 2 ? !menuMode : 1);

        if(setData.NRF_Mode == ON)
        {
            NRF24L01_TX_Mode(setData.NRF_Power);   //���书�ʣ���
        }

        break;

    case 0x0d:
        OLED_ShowChinese(88, 48, zhongXin, 16, listIndex == 2 ? !menuMode : 1);

        if(setData.NRF_Mode == ON)
        {
            NRF24L01_TX_Mode(setData.NRF_Power);   //���书�ʣ���
        }

        break;

    case 0x0b:
        OLED_ShowChinese(88, 48, di, 16, listIndex == 2 ? !menuMode : 1);

        if(setData.NRF_Mode == ON)
        {
            NRF24L01_TX_Mode(setData.NRF_Power);   //���书�ʣ���
        }

        break;

    default:
        break;
    }
}
void menu_ppmsc(void)
{
    u8 modes[3] = {0, 1, 1};
    scszWindow(0, modes);
}//�����˵���PPM���
void menu_wxfs(void)
{
    u8 modes[3] = {1, 0, 1};
    scszWindow(1, modes);
}//�����˵������߷���
void menu_fsgl(void)
{
    u8 modes[3] = {1, 1, 0};
    scszWindow(2, modes);
}//�����˵������书��

//һ���˵�-�����˵� ͨ��ӳ�䴰�� �л�
void menuList2tdys(void)
{
    OLED_Clear_GRAM();//���
    OLED_Fill(63, 0, 64, 63, 1); //�м�����
}

/*mode[]������CH����ʾģʽ����
listIndex��1-8
*/
void tdysWindow(u8 mode[], u8 listIndex)
{
    OLED_ShowString(1, 0, (u8 *)"CH1", 16, mode[0]);
    OLED_ShowString(40, 0, (u8 *)"Y1", 16, listIndex == 1 ? !menuMode : 1); //�˴�Ӧ��ȡsetData,��if�ж���ʾ

    OLED_ShowString(1, 16, (u8 *)"CH2", 16, mode[1]);
    OLED_ShowString(40, 16, (u8 *)"Y2", 16, listIndex == 2 ? !menuMode : 1);

    OLED_ShowString(1, 32, (u8 *)"CH3", 16, mode[2]);
    OLED_ShowString(40, 32, (u8 *)"Y3", 16, listIndex == 3 ? !menuMode : 1);

    OLED_ShowString(1, 48, (u8 *)"CH4", 16, mode[3]);
    OLED_ShowString(40, 48, (u8 *)"Y4", 16, listIndex == 4 ? !menuMode : 1);

    OLED_ShowString(66, 0, (u8 *)"CH5", 16, mode[4]);
    OLED_ShowString(105, 0, (u8 *)"K1", 16, listIndex == 5 ? !menuMode : 1);

    OLED_ShowString(66, 16, (u8 *)"CH6", 16, mode[5]);
    OLED_ShowString(105, 16, (u8 *)"K2", 16, listIndex == 6 ? !menuMode : 1);

    OLED_ShowString(66, 32, (u8 *)"CH7", 16, mode[6]);
    OLED_ShowString(105, 32, (u8 *)"K3", 16, listIndex == 7 ? !menuMode : 1);

    OLED_ShowString(66, 48, (u8 *)"CH8", 16, mode[7]);
    OLED_ShowString(105, 48, (u8 *)"K4", 16, listIndex == 8 ? !menuMode : 1);
}
void menu_tdys1(void)
{
    u8 modes[8] = {0, 1, 1, 1, 1, 1, 1, 1};
    tdysWindow(modes, 1);
}//�����˵���ͨ��1
void menu_tdys2(void)
{
    u8 modes[8] = {1, 0, 1, 1, 1, 1, 1, 1};
    tdysWindow(modes, 2);
}//�����˵���ͨ��2
void menu_tdys3(void)
{
    u8 modes[8] = {1, 1, 0, 1, 1, 1, 1, 1};
    tdysWindow(modes, 3);
}//�����˵���ͨ��3
void menu_tdys4(void)
{
    u8 modes[8] = {1, 1, 1, 0, 1, 1, 1, 1};
    tdysWindow(modes, 4);
}//�����˵���ͨ��4
void menu_tdys5(void)
{
    u8 modes[8] = {1, 1, 1, 1, 0, 1, 1, 1};
    tdysWindow(modes, 5);
}//�����˵���ͨ��5
void menu_tdys6(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 0, 1, 1};
    tdysWindow(modes, 6);
}//�����˵���ͨ��6
void menu_tdys7(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 0, 1};
    tdysWindow(modes, 7);
}//�����˵���ͨ��7
void menu_tdys8(void)
{
    u8 modes[8] = {1, 1, 1, 1, 1, 1, 1, 0};
    tdysWindow(modes, 8);
}//�����˵���ͨ��8
u8 chLineLen[8];
void menu_dljs18(void)
{
    u8 num, temp;

    for(num = 0; num < 8; num++)
    {
        temp = map(PWMvalue[num], 1000 + setData.PWMadjustValue[num], 2000 + setData.PWMadjustValue[num], 0, 63);

        if(nowMenuIndex != lastMenuIndex | chLineLen[num] != temp)
        {
            OLED_Fill(16 * num + 4, 63 - temp, 16 * num + 12, 63, 1); //�²���д1
            OLED_Fill(16 * num + 4, 0, 16 * num + 12, 63 - temp, 0); //�ϲ���д0
            chLineLen[num] = temp;
        }
    }
}//�����˵���ͨ��1-8��������

void menu_cysz(void)
{
    u8 modes[3] = {0, 1, 1};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {changYong, yong, sheZhi, zhi, dian, ya, sheZhi, zhi, nao, zhongDian, sheZhi, zhi};
    menuListWindow(1, modes, str1, str2, str3, hzIndex, 6);
}//�����˵�����������
void menu_dysz(void)
{
    u8 modes[3] = {1, 0, 1};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {changYong, yong, sheZhi, zhi, dian, ya, sheZhi, zhi, nao, zhongDian, sheZhi, zhi};
    menuListWindow(2, modes, str1, str2, str3, hzIndex, 6);
}//�����˵�����ѹ����
void menu_nzsz(void)
{
    u8 modes[3] = {1, 1, 0};
    u8 str1[] = "1.";
    u8 str2[] = "2.";
    u8 str3[] = "3.";
    u8 hzIndex[12] = {changYong, yong, sheZhi, zhi, dian, ya, sheZhi, zhi, nao, zhongDian, sheZhi, zhi};
    menuListWindow(3, modes, str1, str2, str3, hzIndex, 6);
}//�����˵�����������
void menu_ymsz(void)
{
    u8 modes[3] = {0, 1, 1};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {youMen, men, sheZhi, zhi, xingCheng, chengDu, jiaoZhun, zhun, huiFu, fuXing, moRen, ren};
    menuListWindow(4, modes, str1, str2, str3, hzIndex, 6);
}//�����˵�����������
void menu_xcjz(void)
{
    u8 modes[3] = {1, 0, 1};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {youMen, men, sheZhi, zhi, xingCheng, chengDu, jiaoZhun, zhun, huiFu, fuXing, moRen, ren};
    menuListWindow(5, modes, str1, str2, str3, hzIndex, 6);
}//�����˵����г�У׼
void menu_hfcc(void)
{
    u8 modes[3] = {1, 1, 0};
    u8 str1[] = "4.";
    u8 str2[] = "5.";
    u8 str3[] = "6.";
    u8 hzIndex[12] = {youMen, men, sheZhi, zhi, xingCheng, chengDu, jiaoZhun, zhun, huiFu, fuXing, moRen, ren};
    menuListWindow(6, modes, str1, str2, str3, hzIndex, 6);
}//�����˵����ָ���������
/*
hzIndex��16�������������е�����
mode�����е���ʾģʽ
*/
void line3Window(u8 hzIndex[], u8 mode[])
{
    //��һ��
    OLED_Fill(0, 0, 31, 15, 1); //�������
    u8 line1Index[] = {hzIndex[0], hzIndex[1], hzIndex[2], hzIndex[3]};
    OLED_ShowChineseWords(32, 0, line1Index, 4, 0);
    OLED_Fill(96, 0, 127, 15, 1); //�������
    //�ڶ���
    OLED_ShowString(0, 16, (u8 *)"1.", 16, mode[0]);
    u8 line2Index[] = {hzIndex[4], hzIndex[5], hzIndex[6], hzIndex[7]};
    OLED_ShowChineseWords(16, 16, line2Index, 4, mode[0]);
    //������
    OLED_ShowString(0, 32, (u8 *)"2.", 16, mode[1]);
    u8 line3Index[] = {hzIndex[8], hzIndex[9], hzIndex[10], hzIndex[11]};
    OLED_ShowChineseWords(16, 32, line3Index, 4, mode[1]);
    //������
    OLED_ShowString(0, 48, (u8 *)"3.", 16, mode[2]);
    u8 line4Index[] = {hzIndex[12], hzIndex[13], hzIndex[14], hzIndex[15]};
    OLED_ShowChineseWords(16, 48, line4Index, 4, mode[2]);
}

void menu_wtdw(void)
{
    u8 hzIndex[16] = {changYong, yong, sheZhi, zhi, weiTiao, tiao, dan, weiZhi, an, jianPan, yin, xiao, kai, jiQi, hua, mian};
    u8 modes[3] = {0, 1, 1};
    line3Window(hzIndex, modes);
    OLED_ShowNum(88, 16, setData.PWMadjustUnit, 1, 16, !menuMode);

    if(setData.keySound == ON)
    {
        OLED_DrawPointBMP(88, 33, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 33, offSign, 16, 14, 1); //�رձ�־
    }

    OLED_ShowNum(88, 48, setData.onImage, 1, 16, 1);
}//�����˵���΢����λ
void menu_ajyx(void)
{
    u8 hzIndex[16] = {changYong, yong, sheZhi, zhi, weiTiao, tiao, dan, weiZhi, an, jianPan, yin, xiao, kai, jiQi, hua, mian};
    u8 modes[3] = {1, 0, 1};
    line3Window(hzIndex, modes);
    OLED_ShowNum(88, 16, setData.PWMadjustUnit, 1, 16, 1);

    if(setData.keySound == ON)
    {
        OLED_DrawPointBMP(88, 33, onSign, 16, 14, !menuMode); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 33, offSign, 16, 14, !menuMode); //�رձ�־
    }

    OLED_ShowNum(88, 48, setData.onImage, 1, 16, 1);
}//�����˵���������Ч
void menu_kjhm(void)
{
    u8 hzIndex[16] = {changYong, yong, sheZhi, zhi, weiTiao, tiao, dan, weiZhi, an, jianPan, yin, xiao, kai, jiQi, hua, mian};
    u8 modes[3] = {1, 1, 0};
    line3Window(hzIndex, modes);
    OLED_ShowNum(88, 16, setData.PWMadjustUnit, 1, 16, 1);

    if(setData.keySound == ON)
    {
        OLED_DrawPointBMP(88, 33, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 33, offSign, 16, 14, 1); //�رձ�־
    }

    OLED_ShowNum(88, 48, setData.onImage, 1, 16, !menuMode);
}//�����˵�����������

void menu_dyjz(void)
{
    u8 hzIndex[16] = {dian, ya, sheZhi, zhi, dian, ya, jiaoZhun, zhun, baoJing, jing, dian, ya, jie, shouDao, baoJing, jing};
    u8 modes[3] = {0, 1, 1};
    line3Window(hzIndex, modes);
    sprintf((char *)batVoltStr, "%1.2fV", batVolt);
    OLED_ShowString(88, 16, (u8 *)batVoltStr, 16, !menuMode);
    sprintf((char *)batVoltStr, "%1.2fV", setData.warnBatVolt);
    OLED_ShowString(88, 32, (u8 *)batVoltStr, 16, 1);
    sprintf((char *)batVoltStr, "%2.1fV", setData.RecWarnBatVolt); //�ɻ��ĵ�ر���
    OLED_ShowString(88, 48, (u8 *)batVoltStr, 16, 1);
}//�����˵�����ѹУ׼
void menu_bjdy(void)
{
    u8 hzIndex[16] = {dian, ya, sheZhi, zhi, dian, ya, jiaoZhun, zhun, baoJing, jing, dian, ya, jie, shouDao, baoJing, jing};
    u8 modes[3] = {1, 0, 1};
    line3Window(hzIndex, modes);
    sprintf((char *)batVoltStr, "%1.2fV", batVolt);
    OLED_ShowString(88, 16, (u8 *)batVoltStr, 16, 1);
    sprintf((char *)batVoltStr, "%1.2fV", setData.warnBatVolt);
    OLED_ShowString(88, 32, (u8 *)batVoltStr, 16, !menuMode);
    sprintf((char *)batVoltStr, "%2.1fV", setData.RecWarnBatVolt); //�ɻ��ĵ�ر���
    OLED_ShowString(88, 48, (u8 *)batVoltStr, 16, 1);
}//�����˵���������ѹ
void menu_jsbj(void)
{
    u8 hzIndex[16] = {dian, ya, sheZhi, zhi, dian, ya, jiaoZhun, zhun, baoJing, jing, dian, ya, jie, shouDao, baoJing, jing};
    u8 modes[3] = {1, 1, 0};
    line3Window(hzIndex, modes);
    sprintf((char *)batVoltStr, "%1.2fV", batVolt);
    OLED_ShowString(88, 16, (u8 *)batVoltStr, 16, 1);
    sprintf((char *)batVoltStr, "%1.2fV", setData.warnBatVolt);
    OLED_ShowString(88, 32, (u8 *)batVoltStr, 16, 1);
    sprintf((char *)batVoltStr, "%2.1fV", setData.RecWarnBatVolt); //�ɻ��ĵ�ر���
    OLED_ShowString(88, 48, (u8 *)batVoltStr, 16, !menuMode);
}//�����˵������ձ���

void menu_nzbj(void)
{
    u8 hzIndex[16] = {nao, zhongDian, sheZhi, zhi, nao, zhongDian, baoJing, jing, nao, zhongDian, shiChang, changDu, kai, jiQi, zi, jianCha};
    u8 modes[3] = {0, 1, 1};
    line3Window(hzIndex, modes);

    if(setData.clockMode == ON)
    {
        OLED_DrawPointBMP(88, 17, onSign, 16, 14, !menuMode); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 17, offSign, 16, 14, !menuMode); //�رձ�־
    }

    sprintf((char *)timeStr, "%02dmin", setData.clockTime);
    OLED_ShowString(88, 32, (u8 *)timeStr, 16, 1); //���Ӷ�ʱʱ��min

    if(setData.clockCheck == ON)
    {
        OLED_DrawPointBMP(88, 49, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 49, offSign, 16, 14, 1); //�رձ�־
    }
}//�����˵������ӱ���
void menu_nzsc(void)
{
    u8 hzIndex[16] = {nao, zhongDian, sheZhi, zhi, nao, zhongDian, baoJing, jing, nao, zhongDian, shiChang, changDu, kai, jiQi, zi, jianCha};
    u8 modes[3] = {1, 0, 1};
    line3Window(hzIndex, modes);

    if(setData.clockMode == ON)
    {
        OLED_DrawPointBMP(88, 17, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 17, offSign, 16, 14, 1); //�رձ�־
    }

    sprintf((char *)timeStr, "%02dmin", setData.clockTime);
    OLED_ShowString(88, 32, (u8 *)timeStr, 16, !menuMode); //���Ӷ�ʱʱ��min

    if(setData.clockCheck == ON)
    {
        OLED_DrawPointBMP(88, 49, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 49, offSign, 16, 14, 1); //�رձ�־
    }
}//�����˵�������ʱ��
void menu_kjzj(void)
{
    u8 hzIndex[16] = {nao, zhongDian, sheZhi, zhi, nao, zhongDian, baoJing, jing, nao, zhongDian, shiChang, changDu, kai, jiQi, zi, jianCha};
    u8 modes[3] = {1, 1, 0};
    line3Window(hzIndex, modes);

    if(setData.clockMode == ON)
    {
        OLED_DrawPointBMP(88, 17, onSign, 16, 14, 1); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 17, offSign, 16, 14, 1); //�رձ�־
    }

    sprintf((char *)timeStr, "%02dmin", setData.clockTime);
    OLED_ShowString(88, 32, (u8 *)timeStr, 16, 1); //���Ӷ�ʱʱ��min

    if(setData.clockCheck == ON)
    {
        OLED_DrawPointBMP(88, 49, onSign, 16, 14, !menuMode); //�򿪱�־
    }
    else
    {
        OLED_DrawPointBMP(88, 49, offSign, 16, 14, !menuMode); //�رձ�־
    }
}//�����˵��������Լ�

void menu_ymph(void)
{
    u8 hzIndex[16] = {youMen, men, sheZhi, zhi, youMen, men, pian, hao, youMen, men, qu, xian, shiQu, kong, baoCun, hu};
    u8 modes[3] = {0, 1, 1};
    line3Window(hzIndex, modes);

    if(setData.throttlePreference)OLED_ShowChinese(88, 16, zuo, 16, !menuMode); //��
    else OLED_ShowChinese(88, 16, youShou, 16, !menuMode); //��

    OLED_ShowChinese(104, 16, shouJi, 16, !menuMode); //��
    sprintf((char *)timeStr, "%03d%%", setData.throttleProtect);
    OLED_ShowString(88, 48, (u8 *)timeStr, 16, 1); //���ű���ֵ0%
}//�����˵�������ƫ��
void menu_ymqx(void)
{
    u8 hzIndex[16] = {youMen, men, sheZhi, zhi, youMen, men, pian, hao, youMen, men, qu, xian, shiQu, kong, baoCun, hu};
    u8 modes[3] = {1, 0, 1};
    line3Window(hzIndex, modes);

    if(setData.throttlePreference)OLED_ShowChinese(88, 16, zuo, 16, 1); //��
    else OLED_ShowChinese(88, 16, youShou, 16, 1); //��

    OLED_ShowChinese(104, 16, shouJi, 16, 1); //��
    sprintf((char *)timeStr, "%03d%%", setData.throttleProtect);
    OLED_ShowString(88, 48, (u8 *)timeStr, 16, 1); //���ű���ֵ0%
}//�����˵�����������
void menu_skbh(void)
{
    u8 hzIndex[16] = {youMen, men, sheZhi, zhi, youMen, men, pian, hao, youMen, men, qu, xian, shiQu, kong, baoCun, hu};
    u8 modes[3] = {1, 1, 0};
    line3Window(hzIndex, modes);

    if(setData.throttlePreference)OLED_ShowChinese(88, 16, zuo, 16, 1); //��
    else OLED_ShowChinese(88, 16, youShou, 16, 1); //��

    OLED_ShowChinese(104, 16, shouJi, 16, 1); //��
    sprintf((char *)timeStr, "%03d%%", setData.throttleProtect);
    OLED_ShowString(88, 48, (u8 *)timeStr, 16, !menuMode); //���ű���ֵ0%
}//�����˵���ʧ�ر���

void menu_xcjzTip(void)
{
    OLED_Clear_GRAM();//���
    OLED_ShowString(0, 0, (u8 *)"1.", 16, 1);
    u8 qjyghzIndex[] = {qing, jiang, yao, gan, huiLai, zhongXin};
    OLED_ShowChineseWords(16, 0, qjyghzIndex, 6, 1);
    OLED_ShowString(112, 0, (u8 *)",", 16, 1);
    u8 bkymtdIndex[] = {baoKuo, kuo, youMen, men, tongDao, dao};
    OLED_ShowChineseWords(16, 16, bkymtdIndex, 6, 1);
    OLED_ShowString(0, 32, (u8 *)"2.", 16, 1);
    u8 qjjsjddIndex[] = {qing, jiang, jie, shouDao, jiQi, duan, dian};
    OLED_ShowChineseWords(16, 32, qjjsjddIndex, 7, 1);
    OLED_ShowChinese(0, 48, an, 16, 1);
    OLED_ShowString(16, 48, (u8 *)"Enter", 16, 1);
    u8 jxIndex[] = {jiXu, xu};
    OLED_ShowChineseWords(56, 48, jxIndex, 2, 1);
}//�����˵����뽫ң�˻���
void menu_xcjz14(void)
{
    OLED_ShowString(2, 0, (u8 *)"1:", 16, 1);
    OLED_ShowNum(18, 0, setData.chLower[0], 4, 16, 1);
    OLED_ShowNum(56, 0, setData.chMiddle[0], 4, 16, 1);
    OLED_ShowNum(95, 0, setData.chUpper[0], 4, 16, 1);

    OLED_ShowString(2, 16, (u8 *)"2:", 16, 1);
    OLED_ShowNum(18, 16, setData.chLower[1], 4, 16, 1);
    OLED_ShowNum(56, 16, setData.chMiddle[1], 4, 16, 1);
    OLED_ShowNum(95, 16, setData.chUpper[1], 4, 16, 1);

    OLED_ShowString(2, 32, (u8 *)"3:", 16, 1);
    OLED_ShowNum(18, 32, setData.chLower[2], 4, 16, 1);
    OLED_ShowNum(56, 32, setData.chMiddle[2], 4, 16, 1);
    OLED_ShowNum(95, 32, setData.chUpper[2], 4, 16, 1);

    OLED_ShowString(2, 48, (u8 *)"4:", 16, 1);
    OLED_ShowNum(18, 48, setData.chLower[3], 4, 16, 1);
    OLED_ShowNum(56, 48, setData.chMiddle[3], 4, 16, 1);
    OLED_ShowNum(95, 48, setData.chUpper[3], 4, 16, 1);
}//�����˵���ͨ��1-4��ʾ

void menu_hfccTip(void)
{
    OLED_Clear_GRAM();//���
    u8 jhfmrszIndex[] = {jiang, huiFu, fuXing, moRen, ren, sheZhi, zhi};
    OLED_ShowChineseWords(0, 16, jhfmrszIndex, 7, 1);
    OLED_ShowChinese(0, 32, an, 16, 1);
    OLED_ShowString(16, 32, (u8 *)"Enter", 16, 1);
    u8 jxIndex[] = {jiXu, xu};
    OLED_ShowChineseWords(56, 32, jxIndex, 2, 1);
}//�����˵������ָ�Ĭ��
void menu_hfcg(void)
{
    OLED_Clear_GRAM();//���
    u8 hfcgIndex[] = {huiFu, fuXing, chengGong, gong};
    OLED_ShowChineseWords(32, 24, hfcgIndex, 4, 1);
}//�����˵����ָ��ɹ�

void menu_gybjInf(void)
{
    OLED_Clear_GRAM();//���
    OLED_ShowString(0, 0, (u8 *)"J20RC", 16, 1);
    u8 hfcgIndex[] = {kai, fa, tuan, dui};
    OLED_ShowChineseWords(40, 0, hfcgIndex, 4, 1);
    OLED_ShowString(0, 16, (u8 *)FM_TIME, 16, 1);
    OLED_ShowString(0, 32, (u8 *)FM_VERSION, 16, 1);
}//�����˵���J20RC�����Ŷ�-V2.0
//�û��ɱ༭�Ĳ˵�ҳ��
u16 menus[43] =
{
    tdwt1,//�����˵���ͨ��1
    tdwt2,//�����˵���ͨ��2
    tdwt3,//�����˵���ͨ��3
    tdwt4,//�����˵���ͨ��4
    tdwt5,//�����˵���ͨ��5
    tdwt6,//�����˵���ͨ��6
    tdwt7,//�����˵���ͨ��7
    tdwt8,//�����˵���ͨ��8

    tdzf1,//�����˵���ͨ��1
    tdzf2,//�����˵���ͨ��2
    tdzf3,//�����˵���ͨ��3
    tdzf4,//�����˵���ͨ��4
    tdzf5,//�����˵���ͨ��5
    tdzf6,//�����˵���ͨ��6
    tdzf7,//�����˵���ͨ��7
    tdzf8,//�����˵���ͨ��8

    xjjs,//�����˵����½�����
    xzmx,//�����˵���ѡ��ģ��
    bcmx,//�����˵�������ģ��
    txmm,//�����˵���ͨ������

    ppmsc,//�����˵���PPM���
    wxfs,//�����˵������߷���
    fsgl,//�����˵������书��

    tdys1,//�����˵���ͨ��1
    tdys2,//�����˵���ͨ��2
    tdys3,//�����˵���ͨ��3
    tdys4,//�����˵���ͨ��4
    tdys5,//�����˵���ͨ��5
    tdys6,//�����˵���ͨ��6
    tdys7,//�����˵���ͨ��7
    tdys8,//�����˵���ͨ��8

    wtdw,//�����˵���΢����λ
    ajyx,//�����˵���������Ч
    kjhm,//�����˵�����������

    dyjz,//�����˵�����ѹУ׼
    bjdy,//�����˵���������ѹ
    jsbj,//�����˵������ձ���

    nzbj,//�����˵������ӱ���
    nzsc,//�����˵�������ʱ��
    kjzj,//�����˵��������Լ�

    ymph,//�����˵�������ƫ��
    ymqx,//�����˵�����������
    skbh,//�����˵���ʧ�ر���
};
/*
�жϲ˵������Ƿ�Ϊ�û��༭ҳ��
���أ�1Ϊ�ǣ�0Ϊ��
*/
u16 IsInKeyMenu(u16 num)
{
    int thisindex = -1;
    u16 i;

    for(i = 0; i < 43; i++)
    {
        if(menus[i] == num)
        {
            thisindex = i;
            break;
        }
        else if(menus[i] > num)
        {
            break;
        }
    }

    if(thisindex < 0) return 0;
    else return 1;
}
