#define _FORM_PARA_MANAGE_C
#include "head.h"
/*
ģ�飺<��������>
----------------
�ڴ˲˵��µ������Ӳ˵�
�˴�Ϊ�˽�ʡ�ڴ棬��ΪFLASH�󣬹��Դ�����ʽ���֡�
�Ժ����ŵ�RAM�У����������Ӧ�ļ����ؼ��������鼴�ɡ�
*/
u8 adj_ocx_edit_flag;
u8 jiayao_ocx_edit_flag;

void R_Para_XiShu_Manage_Select_Execute(struct _rq_t *item);
//��������ִ��
void R_Para_JiaYao_Manage_Select_Execute(struct _rq_t *item);
//
//void R_Ki_Config_Execute(struct _rq_t *item);	//��������
void R_Adj_Xs_Execute(struct _rq_t *item);		//����ϵ��

void R_Adj_JiaYao_Execute(struct _rq_t *item);		//����ϵ��

void R_Adj_Xs_OcxOptExecute(struct _rq_t *item);

void R_Adj_JiaYao_OcxOptExecute(struct _rq_t *item);


/*
��������ִ�к���
*/
void R_Para_XiShu_Manage_Execute(struct _rq_t *item)
{
	//���Ը��ݿؼ������趨��ʼ������
	
	rq_first_rank.Execute = R_Para_XiShu_Manage_Select_Execute;
	rq_first_rank_enter_flag = 0;	
    
    load_xishu_first_enter_flag = 1;
 // 	printf("para.....>\r\n");
  	R_Execute(item);
}

/*
������������ִ�к���
*/
void R_Para_JiaYao_Manage_Execute(struct _rq_t *item)
{
	//���Ը��ݿؼ������趨��ʼ������
	
	rq_first_rank.Execute = R_Para_JiaYao_Manage_Select_Execute;
	rq_first_rank_enter_flag = 0;	
    
    load_jiayao_first_enter_flag = 1;
 // 	printf("para.....>\r\n");
	
  	R_Execute(item);
}

void R_Para_XiShu_Manage_Select_Execute(struct _rq_t *item)
{
	//���Ը��ݿؼ������趨��ʼ������
// 	if (rq_para_manage_menu.cur_ocx == &ki_set_seq)
// 	{	
// 		R_Ki_Config_Execute(item);
// 
// 	}
// 	else if (rq_para_manage_menu.cur_ocx == &adj_coef_edit)
// 	{	
// 		R_Adj_Xs_Execute(item);
// 	}
    
	R_Adj_Xs_Execute(item);

  	//���ϵͳ���û��������˵����ŵ��˴�
  	//R_Execute(item);
}


void R_Para_JiaYao_Manage_Select_Execute(struct _rq_t *item)
{
    
	R_Adj_JiaYao_Execute(item);


}

/*
����ϵ���ؼ���ʼ����ִ��
����ϵ��Ϊ����
*/
void R_Adj_Xs_Execute(struct _rq_t *item)
{
//#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
//    OS_CPU_SR  cpu_sr = 0;
//#endif
   
	if (rq_first_rank_enter_flag) goto abcd;	
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_adj_xs_para+3);
		len <<= 8;
		len += *(p_adj_xs_para+2);
		
		cur_index = 4;//��ȥÿ�β�����ǰ�ߵ�4��

		//
		j = 0;		
		while (cur_index<len)
		{
            
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)adj_group_type[*(p_adj_xs_para+cur_index)];
			
			menu_t_ocx[j].x = 2;
			menu_t_ocx[j].y = j*2;			
			//
			menu_t_ocx[j].GetFocus = M_GetFocus;
			menu_t_ocx[j].LossFocus = M_LossFocus;
			menu_t_ocx[j].qbch = M_qbch;

			menu_t_ocx[j].Token = &rq_second_rank;

			share_plist[j] = &menu_t_ocx[j];
			//
			j++;
			//�ı�Pָ��
			cur_index++;
			cur_index += *(p_adj_xs_para+cur_index)*8 + 1;	
		}
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//��ʼ������
		rq_first_rank.x = 4;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 8;
		rq_first_rank.Height = 8;

		rq_first_rank.cancel = &rq_para_xishu_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	

		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}
	rq_first_rank_enter_flag = 1;
	
	//��һ�ν���ʱ�ù������ѡ
 	cur_s = 0;
abcd:	CLR_SCREEN();	
	//CLR_SCREEN();

	if (rq_para_xishu_manage_menu.cur_ocx == &adj_coef_seq)
	{
		GUI_PutStringTier(0,0,(INT8*)"ϵ�����",1);
		
		adj_ocx_edit_flag = 0;
	}
	else if (rq_para_xishu_manage_menu.cur_ocx == &adj_coef_edit)
	{

		GUI_PutStringTier(0,0,(INT8*)"ϵ������",1);
		
		adj_ocx_edit_flag = 1;
	}
	//ָ����������ִ�к���
	rq_second_rank.Execute = R_Adj_Xs_OcxOptExecute;
	//
	R_Execute(item);	
}


/*
���������ؼ���ʼ����ִ��
��������Ϊ����
*/
void R_Adj_JiaYao_Execute(struct _rq_t *item)
{

   
	if (rq_first_rank_enter_flag) goto abcd;	
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_jiayao_para+3);
		len <<= 8;
		len += *(p_jiayao_para+2);
		
		cur_index = 4;//��ȥÿ�β�����ǰ�ߵ�4��

		//
		j = 0;		
		while (cur_index<len)
		{
            
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)Work_Para_Group_Type[*(p_jiayao_para+cur_index)];
			
			menu_t_ocx[j].x = 1;
			menu_t_ocx[j].y = j*2;			
			//
			menu_t_ocx[j].GetFocus = M_GetFocus;
			menu_t_ocx[j].LossFocus = M_LossFocus;
			menu_t_ocx[j].qbch = M_qbch;

			menu_t_ocx[j].Token = &rq_second_rank;

			share_plist[j] = &menu_t_ocx[j];
			//
			j++;
			//�ı�Pָ��
			cur_index++;
			cur_index += *(p_jiayao_para+cur_index)*8 + 1;	
		}
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//��ʼ������
		rq_first_rank.x = 2;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 16;
		rq_first_rank.Height = 8;

		rq_first_rank.cancel = &rq_para_jiayao_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	

		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}
	rq_first_rank_enter_flag = 1;
    cur_s = 0;
abcd:	CLR_SCREEN();	

	if (rq_para_jiayao_manage_menu.cur_ocx == &jiayao_para_seq)
	{
		GUI_PutStringTier(0,0,(INT8*)"ҩ�����",1);
		
		jiayao_ocx_edit_flag = 0;
	}
	else if (rq_para_jiayao_manage_menu.cur_ocx == &jiayao_para_edit)
	{

		GUI_PutStringTier(0,0,(INT8*)"ҩ������",1);
		
		jiayao_ocx_edit_flag = 1;
	}
	//ָ����������ִ�к���
	rq_second_rank.Execute = R_Adj_JiaYao_OcxOptExecute;
	//
	R_Execute(item);	
}
/*
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
*/
void R_Adj_Xs_OcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;

	//-------------------
	//�ҵ���ǰ�˵��ţ����ݲ˵���ȥ������ȡ������Ȼ���γɿؼ��顣
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			break;
		}
	}    
	//�ҵ���ֵ��������
	p_data = p_adj_xs_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)//��
	{
		p_data += (*p_data)*8;
		p_data += 2;		//ָ����һ��ĵ�ַ
	}
	//
	i = *p_data;			//��ǰ�˵��µĿؼ���
	//���ݲ�ͬ�Ŀؼ����в�ͬ�ĳ�ʼ��
	if (i>0)
	{
		UINT8 text_ocx_num;
		UINT8 list_ocx_num;	
		UINT8 mult_adjust;	
		//UINT8 ipad_t_ocx_num;
		//
		text_ocx_num = 0;
		list_ocx_num = 0;
		mult_adjust = 0;
		
		p_data++;		//��ֵ�ṹ�ĵ�һ���ֽ�	
						//	|����B8|��ֵ����B7-B5|λ��ֵ����λB4-B0|
		//

		for (j=0; j<i; j++)		
		{	            		
            text_t_ocx[text_ocx_num].Type = K_TEXT;
            text_t_ocx[text_ocx_num].Focus = text_ocx_num + list_ocx_num;
            //
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)Adj_name_list[*(p_data+1)];
          //  text_t_ocx[text_ocx_num].Caption2 = " \";	//�˴�Ϊ��λ
            
            //

            text_t_ocx[text_ocx_num].ReadOnly = 1;//����Դ����

         //   text_t_ocx[text_ocx_num].p_t_data = &share_buf[*(p_data+2)];
         //   printf("data1=%d\r\n",share_buf[*(p_data+2)] );
            //
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_xishu_para_buf[*(p_data+2)];
            //text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
         //   usr_GetVal_T(&text_t_ocx[text_ocx_num],&text_t_ocx[text_ocx_num].Value);
         //   printf("data=%d\r\n",text_t_ocx[text_ocx_num].Value);
            
            
            text_t_ocx[text_ocx_num].Cursor = 0;

            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//���б�־
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }

            text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;

            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
            
            if (adj_ocx_edit_flag)
            {
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
                text_t_ocx[text_ocx_num].ReadOnly = 1;
                change_para_type_flag = XISHU_EDIT_FLAG;
            }
            else
            {
               // text_t_ocx[text_ocx_num].ReadOnly |= 0xf0;	
                text_t_ocx[text_ocx_num].ReadOnly =0;	
            }
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 8;
		}
	}
	//-------------------
	//
	rq_second_rank.cont = j;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 0;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];
	//�˴������������ʾ��ֵ����	
	//GUI_PutStringTier(GUI_LCM_XMAX-16,0,(INT8*)((k_menu_t*)(rq_first_rank.cur_ocx)->Text),0x11);
	//GUI_PutStringTier(	0,0,
	//					((k_menu_t*)(rq_first_rank.cur_ocx))->Text,
	//					1
	//					);
	//
	R_Execute(item);
}
/*
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
*/
void R_Adj_JiaYao_OcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;

	//-------------------
	//�ҵ���ǰ�˵��ţ����ݲ˵���ȥ������ȡ������Ȼ���γɿؼ��顣
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			//printf("i=%d\r\n",i);
			break;
		}
	}    
	//�ҵ���������
	p_data = p_jiayao_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)//��
	{
		p_data += (*p_data)*8;
		p_data += 2;		//ָ����һ��ĵ�ַ
	}
	//
	i = *p_data;			//��ǰ�˵��µĿؼ���
	//���ݲ�ͬ�Ŀؼ����в�ͬ�ĳ�ʼ��
	if (i>0)
	{
		UINT8 text_ocx_num;
		UINT8 list_ocx_num;	
		UINT8 mult_adjust;	
		//UINT8 ipad_t_ocx_num;
		//
		text_ocx_num = 0;
		list_ocx_num = 0;
		mult_adjust = 0;
		
		p_data++;		//��ֵ�ṹ�ĵ�һ���ֽ�	
						//	|����B8|��ֵ����B7-B5|λ��ֵ����λB4-B0|
		//

		for (j=0; j<i; j++)		
		{	            		
            text_t_ocx[text_ocx_num].Type = K_TEXT;
            text_t_ocx[text_ocx_num].Focus = text_ocx_num + list_ocx_num;
            //
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)jiayao_name_list[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = (char*)yc_unit[((*(p_data+3))&0xf0)>>4];;	//�˴�Ϊ��λ
            
            //yc_unit

            text_t_ocx[text_ocx_num].ReadOnly = 1;//

         //   text_t_ocx[text_ocx_num].p_t_data = &share_buf[*(p_data+2)];
         //   printf("data1=%d\r\n",share_buf[*(p_data+2)] );
            //
            
            text_t_ocx[text_ocx_num].Width = 4;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = 2+ j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0 ;
            text_t_ocx[text_ocx_num].Value = temp_jiayao_para_buf[*(p_data+2)];
            //text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
         //   usr_GetVal_T(&text_t_ocx[text_ocx_num],&text_t_ocx[text_ocx_num].Value);
         //   printf("data=%d\r\n",text_t_ocx[text_ocx_num].Value);
            
            
            text_t_ocx[text_ocx_num].Cursor = 0;

            text_t_ocx[text_ocx_num].location = 8;
        //    if (*p_data & 0x80)		//���б�־
        //    {
        //        text_t_ocx[text_ocx_num].location |= 0x80;
        //        mult_adjust += 2;
        //    }

            text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;

            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
            
            if (jiayao_ocx_edit_flag)
            {
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
                text_t_ocx[text_ocx_num].ReadOnly = 1;
                change_para_type_flag = WORK_EDIT_FLAG;
            }
            else
            {
               // text_t_ocx[text_ocx_num].ReadOnly |= 0xf0;	
                text_t_ocx[text_ocx_num].ReadOnly =0;	
            }
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 8;
		}
	}
	//-------------------
	//
	rq_second_rank.cont = j;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 0;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];
	//�˴������������ʾ��ֵ����	
	//GUI_PutStringTier(GUI_LCM_XMAX-16,0,(INT8*)((k_menu_t*)(rq_first_rank.cur_ocx)->Text),0x11);
	//GUI_PutStringTier(	0,0,
	//					((k_menu_t*)(rq_first_rank.cur_ocx))->Text,
	//					1
	//					);
	//
	R_Execute(item);
}
/*
ϵ���̻�����ʵ��
*/
void XiShu_load_exe(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	u8 i;	
    if(load_xishu_first_enter_flag == 1)
    {
        CLR_SCREEN();
    }
    for(i=0;i<XISHU_PARA_NUM;i++)
	{
		if(temp_xishu_para_buf[i]  != para_temp.value_group[i+XISHU_PARA_OFFSET])
		{
			lcd_xishu_para_change_flag =  1;
			para_temp.value_group[i+XISHU_PARA_OFFSET] = temp_xishu_para_buf[i];
            
		}		
	}
	//û�а��κμ�֮ǰ�����ı����е�����
    
	item->ct = &rq_xishu_manage_load;
	if(lcd_xishu_para_change_flag ==1)
	{		
		SavePara();
        lcd_xishu_para_change_flag = 2;
		GUI_PutString16_16(24,16,(INT8*)"ϵ���ѹ̻�",0);

		GUI_PutString16_16(16,32,(INT8*)"�����������",0);
		if (cur_key_value)
		{
			
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}
	}
	else if(lcd_xishu_para_change_flag ==2)
	{	
		if (cur_key_value)
		{
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}		
	}	
	else
	{	
		GUI_PutString16_16(24,8,(INT8*)"ϵ���޸Ķ�",0);
        GUI_PutString16_16(32,24,(INT8*)"���ù̻�",0);
		GUI_PutString16_16(16,40,(INT8*)"�����������",0);
		if (cur_key_value)
		{
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}
		
	}	
    if(load_xishu_first_enter_flag == 1)
    {
        load_xishu_first_enter_flag = 0;
        REFURBISH_LCD();
    }
}


/*
ϵ���̻�����ʵ��
*/
void Work_load_exe(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	u8 i;	
    if(load_jiayao_first_enter_flag == 1)
    {
        CLR_SCREEN();
    }
    for(i=0;i<WORK_PARA_NUM;i++)
	{
		if(temp_jiayao_para_buf[i]  != para_temp.value_group[i+WORK_PARA_OFFSET])
		{
			lcd_work_para_change_flag =  1;
			para_temp.value_group[i+WORK_PARA_OFFSET] = temp_jiayao_para_buf[i];
            
		}		
	}
	//û�а��κμ�֮ǰ�����ı����е�����
    
	item->ct = &rq_work_manage_load;
	if(lcd_work_para_change_flag ==1)
	{		
		SavePara();
		GUI_PutString16_16(24,16,(INT8*)"�����ѹ̻�",0);

		GUI_PutString16_16(16,32,(INT8*)"�����������",0);
		if (cur_key_value)
		{
			lcd_work_para_change_flag = 0;
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}
	}
	else
	{	
		GUI_PutString16_16(24,8,(INT8*)"�������޸Ķ�",0);
        GUI_PutString16_16(32,24,(INT8*)"���ù̻�",0);
		GUI_PutString16_16(16,40,(INT8*)"�����������",0);
		if (cur_key_value)
		{
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}
		
	}	
    if(load_jiayao_first_enter_flag == 1)
    {
        load_jiayao_first_enter_flag = 0;
        REFURBISH_LCD();
    }
}