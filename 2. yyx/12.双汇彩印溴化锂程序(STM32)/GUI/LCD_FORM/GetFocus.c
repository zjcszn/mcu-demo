#define _GETFOCUS_C

#include "head.h"

//�ı����ý��㺯��
void T_GetFocus( struct _text_t *item )
{
//  int8u tx,ty;  //��������
//  int8s str[20];
//  item->Cursor = item->Width-1;
//  tx = item->x + item->Px + strlen(item->Caption1);
//  ty = item->y + item->Py;
//  if (item->MaskChar == 0)
//    sprintf(str,wid_dec_strs[item->Width-1],item->Value);
//  else
//  {
//    int8u i;
//    for (i=0; i<item->Width; i++)
//    {
//      str[i] = item->MaskChar;
//    }
//    str[item->Cursor] = getbit(item->Value, item->Width-item->Cursor-1)+0x30;
//  }
//  outtextxy(tx,ty,str,item->Width,1);
//  setcurmode(1,2,C_CUR_FLASH);
//  gotoxy(tx+item->Cursor,ty);
	
	UINT8 tx,ty;
	
	if (item->dot)
	{
		if (item->Cursor >= item->dot)
		{
			tx = item->Px + (item->location & 0x7f) + (item->Width - item->Cursor) - 1;
		}
		else
		{
			tx = item->Px + (item->location & 0x7f) + (item->Width - item->Cursor);
		}
	}
	else
	{
		tx = item->Px + (item->location & 0x7f) + (item->Width - item->Cursor) - 1;
	}
	
	
	
	ty = item->y + item->Py;
	CurHideCur();
	//��ʱ�Ȳ������ַ����Σ��Ժ�����Ҫ��ס�ڴ˴�����
	
	//��һ���Ƿ���ֻ�����ԣ����������ʾ���
	//if (!(item->ReadOnly & 0xf0))
	//{
	
	if (item->location & 0x80)	//������ʾ��
	{
		CurDisplayCur(tx*8,(ty+2)*8,1*8,16,C_CUR_NO_FLASH);
	}
	else
	{
		CurDisplayCur(tx*8,ty*8,1*8,16,C_CUR_NO_FLASH);
	}
	//}
}

//�˵���ý��㺯��
void M_GetFocus( struct _menu_t *item )
{
  	int8u tx,ty;   //��������
  	tx = item->x+ item->Px;
  	ty = item->y+ item->Py;

	CurHideCur();
	
	//�˴������Ƿ���ʾ��������,���Ҹ�����������
	//	
	CurDisplayCur(tx*8,ty*8,strlen(item->Text)*8,16,C_CUR_NO_FLASH);
}

void L_GetFocus( struct _list_t *item )
{
//  int8u tx,ty;
//  tx = item->x+ item->Px+ strlen(item->Caption1);
//  ty = item->y+ item->Py;
//  hide_cur();
//  gotoxy(tx,ty);
//  setcurmode(strlen(item->Value[item->Cursor]), 2, C_CUR_NO_FLASH);

	UINT8 tx,ty;	
	
	tx = item->Px + (item->location & 0x7f);
	ty = item->y+ item->Py;
	
	CurHideCur();
	
	//�˴������Ƿ���ʾ��������,���Ҹ�����������
	//	
	
	//��һ���Ƿ���ֻ�����ԣ����������ʾ���
	//if (!(item->ReadOnly & 0xf0))
	//{
	if (item->location & 0x80)	//������ʾ��
	{
		CurDisplayCur(tx*8,(ty+2)*8,strlen(item->Value[item->Cursor])*8,16,C_CUR_NO_FLASH);	
	}
	else
	{
		CurDisplayCur(tx*8,ty*8,strlen(item->Value[item->Cursor])*8,16,C_CUR_NO_FLASH);	
	}
	//}
	
}

void IP_GetFocus( struct _ipad_t *item )
{
//  int8u tx,ty;
//  int8s s[10];
//  tx = item->x+ item->Px+ strlen(item->Caption1)+ 4*(item->wz);
//  ty = item->y+ item->Py;
//  sprintf(s,"%03d",item->Val[item->wz]);
//  outtextxy(tx, ty, s, 3, 1);
//  hide_cur();
//  tx+= item->Cursor;
//  gotoxy(tx, ty);
//  setcurmode(1,2,C_CUR_FLASH);
}

void T_GetFocus1( struct _text_t *item )
{
//  int8u tx,ty;  //��������
//  int8s str[20];
//  item->Cursor = 0;
//  tx = item->x + item->Px + strlen(item->Caption1);
//  ty = item->y + item->Py;
//  setcurmode(item->Width,2,C_CUR_NO_FLASH);
//  gotoxy(tx,ty);
}

  