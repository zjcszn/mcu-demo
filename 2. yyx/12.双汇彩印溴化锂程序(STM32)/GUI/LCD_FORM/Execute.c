#define _EXECUTE_C

#include "head.h"


//-------------------------

//-------------------------

int8u GetItemFocus(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    return ((k_text_t *)(item))->Focus;
  case K_MENU:
    return ((k_menu_t *)(item))->Focus;
  case K_LIST:
    return ((k_list_t *)(item))->Focus;
  case K_IP:
    return ((k_ipad_t *)(item))->Focus;
  default:
  	return 0;
  }
}
//�õ���ǰ����Ķ���
void *CurFocus( void **item, int8u f, int8u c)
{
  int8u i;
  for (i=0; i<c; i++)
  {
    if ( GetItemFocus(item[i]) == f)return item[i];
  }  
  return 0;
}
//��ǰ����Ķ����ͼ�ֵ
void PutKey(void *item, int16u k)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    ((k_text_t *)(item))->KeyProcess((struct _text_t *)item, k);
    break;
  case K_MENU:
    ((k_menu_t *)(item))->KeyProcess((struct _menu_t *)item, k);
    break;
  case K_LIST:
    ((k_list_t *)(item))->KeyProcess((struct _list_t *)item, k);
    break;
  case K_IP:
    ((k_ipad_t *)(item))->KeyProcess((struct _ipad_t *)item, k);
    break;
  }
}

void Inititem(void *item)
{
  	unsigned char tmp;  	
  
  tmp = ((unsigned char *)item)[0];
  switch( tmp )
  {
  case K_LIST:
    if (((k_list_t *)(item))->SetVal != 0)
      ((k_list_t *)(item))->SetVal((struct _list_t *)item, 0);
    break;
  case K_TEXT:
    if (((k_text_t *)(item))->SetVal != 0)
      ((k_text_t *)(item))->SetVal((struct _text_t *)item, 0);
    break;
  case K_IP:
    if (((k_ipad_t *)(item))->SetVal != 0)
      ((k_ipad_t *)(item))->SetVal((struct _ipad_t *)item, 0,0,0,0);
    break;
  }
}

void Submis(void *item)
{
  switch( ((int8u *)(item))[0] )
  {
  case K_TEXT:
    if (((k_text_t *)(item))->GetVal != 0)
      ((k_text_t *)(item))->GetVal((struct _text_t *)item, 0);
    break;
  case K_LIST:
    if (((k_list_t *)(item))->GetVal != 0)
      ((k_list_t *)(item))->GetVal((struct _list_t *)item, 0);
    break;
  case K_IP:
    if (((k_ipad_t *)(item))->GetVal != 0)
      ((k_ipad_t *)(item))->GetVal((struct _ipad_t *)item, 0,0,0,0);
    break;
  }
}

int8u find_down(struct _rq_t *Item)
{
  	int i;
  	int bkx,bky;
  	int8u item_bak=GetItemFocus(Item->cur_ocx);
  	void *item;
  	for (i=Item->cont-1;i>=0;i--)
  	{
    	item = CurFocus(Item->kj,i,Item->cont);
    	bkx = GetItemX(item);
    	bky = GetItemX(Item->cur_ocx);
    	if (bkx == bky)
    	{
      		bkx = GetItemY(item);
      		bky = GetItemY(Item->cur_ocx);
      		if (bkx == bky)
        		break;
      		item_bak = GetItemFocus(item);
    	}
 	}
  	return item_bak;
	
}

int8u find_up(struct _rq_t *Item)
{
  	int i;
  	int bkx,bky;
  	int8u item_bak=GetItemFocus(Item->cur_ocx);
  	void *item;
  	for (i=0;i<Item->cont;i++)
  	{
    	item = CurFocus(Item->kj,i,Item->cont);
    	bkx = GetItemX(item);
    	bky = GetItemX(Item->cur_ocx);
    	if (bkx == bky)
    	{
      		bkx = GetItemY(item);
      		bky = GetItemY(Item->cur_ocx);
      		if (bkx == bky)
        		break;
      		item_bak = GetItemFocus(item);
    	}
  	}
  	return item_bak;
}

int8u find_left(struct _rq_t *Item)
{
  int i;
  int bkx,bky;
  int8u item_bak=GetItemFocus(Item->cur_ocx);
  void *item;
//  if (GetItemX(Item->cur_ocx)!=0)
  for (i=0;i<Item->cont;i++)
  {
    item = CurFocus(Item->kj,i,Item->cont);
    bkx = GetItemY(item);
    bky = GetItemY(Item->cur_ocx);
    if (bkx == bky)
    {
      bkx = GetItemX(item);
      bky = GetItemX(Item->cur_ocx);
      if (bkx < bky)
      {
      	item_bak = GetItemFocus(item);
        break;
      }
    }
  }
  return item_bak;
}

int8u find_right(struct _rq_t *Item)
{
  int i;
  int bkx,bky;
  int8u item_bak=GetItemFocus(Item->cur_ocx);
  void *item;
//  if (GetItemX(Item->cur_ocx)==0)
  for (i=0;i<Item->cont;i++)
  {
    item = CurFocus(Item->kj,i,Item->cont);
    bkx = GetItemY(item);
    bky = GetItemY(Item->cur_ocx);
    if (bkx == bky)
    {
      bkx = GetItemX(item);
      bky = GetItemX(Item->cur_ocx);
      if (bkx > bky)
      {
      	item_bak = GetItemFocus(item);
        break;
      }
    }
  }
  return item_bak;	
}
//�������к���
void R_Execute(struct _rq_t *item)
{	
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif 
//	u8 test;
	int16u k;
	int8u i;
	int8u f; 
	int8u c; 
  
	UINT16 bak_key_value;   
  
	for (k=0; k<item->cont; k++)
	{
		//ÿ���ؼ���ֵ�ĳ�ʼ�����ڴ˴����еģ�
		//��ʵ�ǵ��õ�SETVALUE���������ԣ��Ǹ������о���������
		//����ʼ���ǲ˵��Ķ�����������������װ���ǲ˵��������ڴ˳�ʼ��
		Inititem(item->kj[k]);
	}	
	
	//��������������Ķ�
	item->Initsize(item);//��ʾ��Ӧ�Ľ���
	
	f = GetThisDs(item);						//��ñ�����һ������

	c = GetThisCont(item->df_line, item);		//��ñ����Ķ������

	REFURBISH_LCD();

  while(1)
  {
  	OSTimeDly(OS_TICKS_PER_SEC/20);    	
    if (0 == cur_key_value)	continue;
    //
    OS_ENTER_CRITICAL();
    
    bak_key_value = cur_key_value;		
	cur_key_value = 0;	

	OS_EXIT_CRITICAL();
	//
    switch(bak_key_value)
    {
    	case KEY_ESC:
		//printf("esc\r\n");
      		item->ct = item->cancel;
			ocx_key_type = KEY_ESC;
      		return;
      		//break;
    	case KEY_SET:
	//printf("set\r\n");
      		if (item->enter != 0x000000)//�����һ����MENU ,�����һ��menu
      		{
        		item->ct = item->enter;
        		return;
      		}
	  		for (i=0; i<item->cont; i++)
	  		{
	    		PutKey(item->kj[i], bak_key_value);
	    	}

      		item->ct = item->cancel;
			ocx_key_type = KEY_SET;
            
	  		return;
    	case KEY_ADD://����
		//printf("add\r\n");
			if (((int8u *)(item->cur_ocx))[0] == K_MENU)
			{
				//printf("add\r\n");
				if (item->Focus > item->cont-2)
				{
					item->Focus = 0;
					
					item->df_line =0;			          	
					
					//
					f = GetThisDs(item);
					c = GetThisCont(item->df_line, item);
					item->Initsize(item);
					item->Focus = 0;
					REFURBISH_LCD();                
				}
				else
				{
					ItemLFocus(item->cur_ocx);
	
					if (((int8u *)(item->cur_ocx))[0] == K_MENU)
					{
						item->Focus = find_down(item);
					}
					else
					{
						item->Focus ++;
					}
				}
				//�ҵ���ǰ����Ŀؼ�
				item->cur_ocx = CurFocus(item->kj, item->Focus, item->cont);
				//����ﵽ�˷���������д���
	
				if ((GetItemY(item->cur_ocx)+1) >= (item->Height+item->df_line+item->df_line))	
				{
                   // printf("1234");
					if ((f+c) < item->cont)
					{	        		
						//
						item->df_line +=4;			          	
						
						//
						f = GetThisDs(item);
						c = GetThisCont(item->df_line, item);
						item->Initsize(item);
						//ItemGFocus(item->cur_ocx);
						REFURBISH_LCD();
					}
				}
				else
				{
				   ItemGFocus(item->cur_ocx);
				}
				item->enter = GetEnter(item->cur_ocx);
				break;
			}
            else
            {
                PutKey(item->cur_ocx, bak_key_value);
                break;
            }

    	case KEY_DEC://����
		//printf("dec\r\n");
      		if (((int8u *)(item->cur_ocx))[0] == K_MENU)
      		{
				//printf("dec");
        		ItemLFocus(item->cur_ocx);
        		//item->Focus ++;
        		item->Focus = find_right(item);
        		item->cur_ocx = CurFocus(item->kj, item->Focus, item->cont);
        		if ((GetItemY(item->cur_ocx)+1) >= (item->Height+item->df_line+item->df_line))
        		{
          			if ((f+c) < item->cont)
          			{
            			item->df_line ++;
            			f = GetThisDs(item);
            			c = GetThisCont(item->df_line, item);
            			item->Initsize(item);
          			}
        		}
        		else
          			ItemGFocus(item->cur_ocx);
        		item->enter = GetEnter(item->cur_ocx);
        		break;
      		}
            else
            {
                PutKey(item->cur_ocx, bak_key_value);
                break;
            }
    	default:        		
      		break;
    }
  }
}

