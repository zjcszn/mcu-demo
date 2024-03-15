#define _FORM_FIXED_MANAGE_C
#include "head.h"
/*
ģ�飺<��ֵ����>
----------------
�ڴ˲˵��µ������Ӳ˵�

*/

UINT8 ocx_edit_flag;

void R_FixSeqExecute(struct _rq_t *item);
void R_FixOcxOptExecute(struct _rq_t *item);

/*
��ֵ����ִ�к���
*/
void R_Fixed_Manage_Execute(struct _rq_t *item)
{	
//	u8 i;

	//���Ը��ݿؼ������趨��ʼ������
	rq_first_rank.Execute = R_FixSeqExecute;//gh

	rq_first_rank_enter_flag = 0;
    
    load_fix_first_enter_flag = 1;
      	
  	R_Execute(item);  	
	
}

/*
�ú�����ɶ�ֵ��ѯ���޸ĵ��б����ʼ����ִ���������к���
�˺���Ҫ��rq_fix_opt�����ڵĲ˵�����г�ʼ��
*/
void R_FixSeqExecute(struct _rq_t *item)
{
	//UINT8 i;	

	if (rq_first_rank_enter_flag) goto abcd;	
	

	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_fix_para+3);
		len <<= 8;
		len += *(p_fix_para+2);
		
		cur_index = 4;
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)fix_group_type[*(p_fix_para+cur_index)];
			//
			menu_t_ocx[j].x = 0;
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
			cur_index += *(p_fix_para+cur_index)*8 + 1;			
		}
		//
		rq_first_rank.cont = j;
		rq_first_rank.df_line = 0;
		rq_first_rank.cur_line = 0;
		rq_first_rank.Focus = 0;//��ʼ������
		rq_first_rank.x = 4;
		rq_first_rank.y = 0;
		rq_first_rank.Width = 16;
		rq_first_rank.Height = 8;
		
		rq_first_rank.cancel = &rq_fixed_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;

abcd:	CLR_SCREEN();	

	if (rq_fixed_manage_menu.cur_ocx == &fixed_manage_seq)
	{
		GUI_PutStringTier(0,0,(INT8*)"��ֵ���",1);
		
		ocx_edit_flag = 0;
	}
	else //if (rq_fixed_manage_menu.cur_ocx == &fixed_manage_edit)
	{
		GUI_PutStringTier(0,0,(INT8*)"��ֵ����",1);
        
        		
		ocx_edit_flag = 1;
	}
	//ָ����������ִ�к���
	rq_second_rank.Execute = R_FixOcxOptExecute;
	//
	R_Execute(item);
}



/*
------------------------------------------------------------------------------------
------------------------------------------------------------------------------------
*/
void R_FixOcxOptExecute(struct _rq_t *item)
{
	UINT8 i,j;
	UINT8 *p_data;
	
	//��ʼ����ֵ�˵�����Ŀؼ������ݶ�ֵ�������ڲ���
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
	p_data = p_fix_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)
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
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)fix_name_list[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = (INT8*)yc_unit[*(p_data+3)>>3];	//�˴�Ϊ��λ
            
            //

         //   text_t_ocx[text_ocx_num].ReadOnly = 1;//����Դ����

         //   text_t_ocx[text_ocx_num].p_t_data = &share_buf[*(p_data+2)];
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_fix_para_buf[*(p_data+2)];

            
            
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
       //     printf("oce_edit_fag");
            if (ocx_edit_flag)
            {
			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
                text_t_ocx[text_ocx_num].ReadOnly = 1;	
                change_para_type_flag =FIX_EDIT_FLAG;
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

	R_Execute(item);
}
/*
��ֵ�̻�����ʵ��
*/
void Fixed_load_exe(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	u8 i;	
    if(load_fix_first_enter_flag == 1)
    {
        CLR_SCREEN();
    }
    for(i=0;i<FIX_PARA_NUM;i++)
	{
		if(temp_fix_para_buf[i]  != para_temp.value_group[i+FIX_PARA_OFFSET])
		{
			lcd_fix_para_change_flag =  1;
			para_temp.value_group[i+FIX_PARA_OFFSET] = temp_fix_para_buf[i];
            
		}		
	}
	//û�а��κμ�֮ǰ�����ı����е�����
	item->ct = &rq_fixed_manage_load;
	if(lcd_fix_para_change_flag ==1)
	{		
		SavePara();
		GUI_PutString16_16(24,16,(INT8*)"��ֵ�ѹ̻�",0);

		GUI_PutString16_16(16,32,(INT8*)"�����������",0);
		if (cur_key_value)
		{
			lcd_fix_para_change_flag = 0;
			OS_ENTER_CRITICAL();    	
			cur_key_value = 0;
			OS_EXIT_CRITICAL();
			
			item->ct = item->cancel;
			//return (k_rq_t*)&mainmenu;
		}
	}
	else
	{	
		GUI_PutString16_16(24,8,(INT8*)"��ֵ�޸Ķ�",0);
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
    if(load_fix_first_enter_flag == 1)
    {
        load_fix_first_enter_flag = 0;
        REFURBISH_LCD();
    }
}
