#define _FORM_SYS_MANAGE_C
#include "head.h"
/*
ģ�飺<ϵͳ����>
----------------
�ڴ˲˵��µ������Ӳ˵�
�˴�Ϊ�˽�ʡ�ڴ棬��ΪFLASH�󣬹��Դ�����ʽ���֡�
�Ժ����ŵ�RAM�У����������Ӧ�ļ����ؼ��������鼴�ɡ�
*/

static u8 comm_para_check_flag;
void R_Sys_Manage_Select_Execute(struct _rq_t *item);
void R_Comm_Manage_Select_Execute(struct _rq_t *item);
void R_JiankongOcxOptExecute(struct _rq_t *item);
void R_GprsOcxOptExecute(struct _rq_t *item);
void R_SensorOcxOptExecute(struct _rq_t *item);
//
void R_Edit_Comm_Execute(struct _rq_t *item);
void R_Edit_Addr_Execute(struct _rq_t *item);
void R_Edit_Password_Execute(struct _rq_t *item);
void R_Edit_Clock_Execute(struct _rq_t *item);
void R_Edit_Disp_Execute(struct _rq_t *item);

void R_Edit_JianKong_Execute(struct _rq_t *item);
void R_Edit_Gprs_Execute(struct _rq_t *item);
void R_Edit_Sensor_Execute(struct _rq_t *item);
void CommPara_load_exe(void);

/*
ϵͳ����ִ�к���
*/
void R_Sys_Manage_Execute(struct _rq_t *item)
{
	//���Ը��ݿؼ������趨��ʼ������
	
	rq_first_rank.Execute = R_Sys_Manage_Select_Execute;//gh	  	
    
  	R_Execute(item);
}
void R_Comm_Manage_Execute(struct _rq_t *item)
{
	rq_first_rank.Execute = R_Comm_Manage_Select_Execute;//gh
	rq_first_rank_enter_flag = 0;  	
    
  	R_Execute(item);
}
void R_Comm_Manage_Select_Execute(struct _rq_t *item)
{
	if (rq_comm_manage_menu.cur_ocx == &comm_manage_jiankong)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_JianKong_Execute(item);
		
	}
	else if (rq_comm_manage_menu.cur_ocx == &comm_manage_gprs)
	{
		//rq_first_rank.Execute = R_Edit_Clock_Execute;
		R_Edit_Gprs_Execute(item);
	}	
	else if (rq_comm_manage_menu.cur_ocx == &comm_manage_sensor)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Sensor_Execute(item);
	}
}
void R_Edit_JianKong_Execute(struct _rq_t *item)
{

	if (rq_first_rank_enter_flag) goto abcd;	
	

	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_jiankong_para+3);
		len <<= 8;
		len += *(p_jiankong_para+2);
		
		cur_index = 4;//��Ϊ�Ǳ��������Ǵӵ�4��ʼ
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)jiankong_group_type[*(p_jiankong_para+cur_index)];
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
			cur_index += *(p_jiankong_para+cur_index)*3 + 1;//�ڸ�������4�����ԣ����Գ���4			
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
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"�������",1);

	//ָ����������ִ�к���
	rq_second_rank.Execute = R_JiankongOcxOptExecute;
	//
	R_Execute(item);
}
void R_JiankongOcxOptExecute(struct _rq_t *item)
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
	p_data = p_jiankong_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//ָ����һ��ĵ�ַ
	}
	//
	i = *p_data;			//��ǰ�˵��µĿؼ���
	//printf("i=%d\r\n",i);
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
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)jiankong_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//�˴�Ϊ��λ
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_jiankong_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//���б�־
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =JIANKONG_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
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

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void CommPara_load_exe(void)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	u8 i;	
//     if(load_fix_first_enter_flag == 1)
//     {
//         CLR_SCREEN();
//     }
	if (change_para_type_flag ==JIANKONG_EDIT_FLAG)
	{
         if( (temp_jiankong_commpara_buf[0] >4) //�˿ںż��
           ||((temp_jiankong_commpara_buf[1] != 9600)&&(temp_jiankong_commpara_buf[1] != 19200))
           ||(temp_jiankong_commpara_buf[2] >2 )//ֹͣλ
           ||(temp_jiankong_commpara_buf[3] >2 )//����λ
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<JIANKONG_PARA_NUM;i++)
            { 
                if(temp_jiankong_commpara_buf[i]  != para_temp.value_group[i+JIANKONG_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+JIANKONG_PARA_OFFSET] = temp_jiankong_commpara_buf[i];
                
                }	
            }
        }
	}
	else if (change_para_type_flag ==GPRS_EDIT_FLAG)
	{
         if( (temp_gprs_commpara_buf[0] >4) //�˿ںż��
           ||((temp_gprs_commpara_buf[1] != 9600)&&(temp_gprs_commpara_buf[1] != 19200))
           ||(temp_gprs_commpara_buf[2] >2 )//ֹͣλ
           ||(temp_gprs_commpara_buf[3] >2 )//����λ
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<GPRS_PARA_NUM;i++)
            {
                if(temp_gprs_commpara_buf[i]  != para_temp.value_group[i+GPRS_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+GPRS_PARA_OFFSET] = temp_gprs_commpara_buf[i];
                
                }		
            }
        }
	}
	else if (change_para_type_flag ==SENSOR_EDIT_FLAG)
	{
         if( (temp_sensor_commpara_buf[0] >4) //�˿ںż��
           ||(temp_sensor_commpara_buf[1] >4)
           ||((temp_sensor_commpara_buf[2] != 9600)&&(temp_sensor_commpara_buf[2] != 19200))
           ||(temp_sensor_commpara_buf[3] >2 )//ֹͣλ
           ||(temp_sensor_commpara_buf[4] >2 )//����λ
          )
        {
            comm_para_check_flag =1;
        }
        else
        {
            for(i=0;i<SENSOR_PARA_NUM;i++)
            {
                if(temp_sensor_commpara_buf[i]  != para_temp.value_group[i+SENSOR_PARA_OFFSET])
                {
                    lcd_comm_para_change_flag =  1;
                    para_temp.value_group[i+SENSOR_PARA_OFFSET] = temp_sensor_commpara_buf[i];
                
                }		
            }
        }
	}
    
	//û�а��κμ�֮ǰ�����ı����е�����
	//item->ct = &rq_comm_manage_menu;
    if(comm_para_check_flag ==1)
    {
        //����װ�غ��֮ǰ������
        Load_TmepPara_Buf();
        comm_para_check_flag = 0;
        CLR_SCREEN();
        GUI_PutString16_16(16,16,(INT8*)"�����������",0);

        GUI_PutString16_16(16,32,(INT8*)"�����������",0);
        while(1)
        {
            OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
            if (cur_key_value)
            {
                OS_ENTER_CRITICAL();    	
                cur_key_value = 0;
                OS_EXIT_CRITICAL();
                break;	
            }				
        }
    }
    else
    {        
        if(lcd_comm_para_change_flag ==1)
        {
            lcd_comm_para_change_flag = 0;
            SavePara();
            comm_para_change_flag = 1;//
            CLR_SCREEN();
            GUI_PutString16_16(24,16,(INT8*)"�����ѹ̻�",0);
    
            GUI_PutString16_16(16,32,(INT8*)"�����������",0);
            while(1)
            {
                OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
                if (cur_key_value)
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    break;	
                }				
            }
        }
        else
        {	
            CLR_SCREEN();
            GUI_PutString16_16(24,8,(INT8*)"��ֵ�޸Ķ�",0);
            GUI_PutString16_16(32,24,(INT8*)"���ù̻�",0);
            GUI_PutString16_16(16,40,(INT8*)"�����������",0);
            
            while(1)
            {
                OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
                if (cur_key_value)
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    break;	
                }				
            }
        }
    }
}
void R_Edit_Gprs_Execute(struct _rq_t *item)
{
    if (rq_first_rank_enter_flag) goto abcd;
	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_gprs_para+3);
		len <<= 8;
		len += *(p_gprs_para+2);
		
		cur_index = 4;//��Ϊ�Ǳ��������Ǵӵ�4��ʼ
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)gprs_group_type[*(p_gprs_para+cur_index)];
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
			cur_index += *(p_gprs_para+cur_index)*3 + 1;//�ڸ�������4�����ԣ����Գ���4			
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
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"��������",1);

	//ָ����������ִ�к���
	rq_second_rank.Execute = R_GprsOcxOptExecute;
	//
	R_Execute(item);
}
void R_GprsOcxOptExecute(struct _rq_t *item)
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
	p_data = p_gprs_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//ָ����һ��ĵ�ַ
	}
	//
	i = *p_data;			//��ǰ�˵��µĿؼ���
	//printf("i=%d\r\n",i);
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
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)gprs_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//�˴�Ϊ��λ
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_gprs_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//���б�־
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =GPRS_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
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

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void R_Edit_Sensor_Execute(struct _rq_t *item)
{
    if (rq_first_rank_enter_flag) goto abcd;
	//
	if (1)//(valid_para_flag & ENABLE_TYPE_FIX_FLAG)
	{
		//�˶δ�����Ҫ��ɣ���ֵ������д����
		UINT32 len;
		UINT32 cur_index;
		UINT8 j;		
		//		
		len = *(p_sensor_para+3);
		len <<= 8;
		len += *(p_sensor_para+2);
		
		cur_index = 4;//��Ϊ�Ǳ��������Ǵӵ�4��ʼ
		//
		j = 0;		
		while (cur_index<len)
		{
			menu_t_ocx[j].Type = K_MENU;
			menu_t_ocx[j].Focus = j;	
			//
			menu_t_ocx[j].Text = (INT8*)sensor_group_type[*(p_sensor_para+cur_index)];
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
			cur_index += *(p_sensor_para+cur_index)*3 + 1;//�ڸ�������4�����ԣ����Գ���4			
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
		
		rq_first_rank.cancel = &rq_comm_manage_menu;
		rq_first_rank.SetFocus = R_SetFocus;
		rq_first_rank.Initsize = R_Initsize;	
		
		rq_first_rank.enter = 0;		
		
		rq_first_rank.kj = &share_plist[0];
	}		

//----------------------------------------------------------------	

	rq_first_rank_enter_flag = 1;
	cur_s = 0;
abcd:	CLR_SCREEN();	

		GUI_PutStringTier(0,0,(INT8*)"��������",1);

	//ָ����������ִ�к���
	rq_second_rank.Execute = R_SensorOcxOptExecute;
	//
	R_Execute(item);
}
void R_SensorOcxOptExecute(struct _rq_t *item)
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
	p_data = p_sensor_para+5;	//��һ��Ĵ�Ŷ�ֵ������ĵ�ַ
	while (i--)
	{
		p_data += (*p_data)*3;
		p_data += 2;		//ָ����һ��ĵ�ַ
	}
	//
	i = *p_data;			//��ǰ�˵��µĿؼ���
	//printf("i=%d\r\n",i);
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
            text_t_ocx[text_ocx_num].Caption1 = (INT8*)sensor_group_type[*(p_data+1)];
            text_t_ocx[text_ocx_num].Caption2 = "";	//�˴�Ϊ��λ
            
            
            text_t_ocx[text_ocx_num].Width = 5;
            //
            text_t_ocx[text_ocx_num].x = 0;
            text_t_ocx[text_ocx_num].y = j*2 + mult_adjust;
            //
            text_t_ocx[text_ocx_num].Px = 0;
            text_t_ocx[text_ocx_num].Py = 0;
            text_t_ocx[text_ocx_num].Value = temp_sensor_commpara_buf[*(p_data+2)];            
            
            text_t_ocx[text_ocx_num].Cursor = 0;
            text_t_ocx[text_ocx_num].location = 6;
            if (*p_data & 0x80)		//���б�־
            {
                text_t_ocx[text_ocx_num].location |= 0x80;
                mult_adjust += 2;
            }
           // text_t_ocx[text_ocx_num].dot = (*(p_data+3)) & 0x0f;
			/****
            text_t_ocx[text_ocx_num].Max 
                        = (UINT32)((*(p_data+7))<<8) + *(p_data+6);
            text_t_ocx[text_ocx_num].Min 
                        = (UINT32)((*(p_data+5))<<8) + *(p_data+4);
           // printf("a123=%d\r\n",text_t_ocx[text_ocx_num].Value);
			*********/
            text_t_ocx[text_ocx_num].MaskChar = 0;
            text_t_ocx[text_ocx_num].GetFocus = T_GetFocus;
            text_t_ocx[text_ocx_num].LossFocus = T_LossFocus;
            text_t_ocx[text_ocx_num].KeyProcess = T_KeyProcess;
            text_t_ocx[text_ocx_num].qbch = T_qbch;
            text_t_ocx[text_ocx_num].SetVal = usr_SetVal_T;
            text_t_ocx[text_ocx_num].GetVal = usr_GetVal_T;
       //     printf("oce_edit_fag");

			//	 printf("oce_edit_fag......");
                //text_t_ocx[text_ocx_num].ReadOnly &= 0x0f;		
			text_t_ocx[text_ocx_num].ReadOnly = 1;	
			change_para_type_flag =SENSOR_EDIT_FLAG;

    
            //
            share_plist_sub[j] = &text_t_ocx[text_ocx_num];
            //
            text_ocx_num++;
			//
			p_data += 3;
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

 	if (ocx_key_type == KEY_SET)
 	{
 		CommPara_load_exe();
 	}	
}
void R_Sys_Manage_Select_Execute(struct _rq_t *item)
{
	//���Ը��ݿؼ������趨��ʼ������
     if (rq_sys_manage_menu.cur_ocx == &sys_manage_addr)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Addr_Execute(item);
	}
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_password)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Password_Execute(item);
	}
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_rtc)
	{
		//rq_first_rank.Execute = R_Edit_Clock_Execute;
		R_Edit_Clock_Execute(item);
	}	
	else if (rq_sys_manage_menu.cur_ocx == &sys_manage_disp)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Edit_Disp_Execute(item);
	}
  	//���ϵͳ���û��������˵����ŵ��˴�
  	//R_Execute(item);
}


/*
��ַ���ö�
*/
void R_Edit_Addr_Execute(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	//��ѯ��ַ
	UINT8 temp;	
	temp_local_addr = para_temp.value.addr;
	//��ʼ��һ���ı��ؼ��������ַ
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "������ַ:";
	text_t_ocx[0].Caption2 = " ";	//�˴�Ϊ��λ	
	//
	text_t_ocx[0].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[0].p_t_data = &temp_local_addr ;
	//s	
	text_t_ocx[0].Width = 3;
	//
	text_t_ocx[0].x = 2;
	text_t_ocx[0].y = 3;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;			
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 8;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 254;
	text_t_ocx[0].Min = 1;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;
				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	
	//
	share_plist[0] = &text_t_ocx[0];
	
	//---------------------------------------------
	rq_first_rank.cont = 1;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 16;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	//
	//CLR_SCREEN();

	//
    cur_s = 0;
	CLR_SCREEN();
	GUI_PutStringTier(0,0,(INT8*)"��ַ����",1);
	//
	temp = para_temp.value.addr ;		//����ԭ��ַ
	//
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);
	//����
	ocx_key_type = 0;
	R_Execute(item);  
	//��ִ�к����˳�����һ����Ҫ����Ҫ�������
	if ((temp != temp_local_addr) && (ocx_key_type == KEY_SET))
	{
		UINT8 temp_flag;
		temp_flag = 0;
		
		temp_flag = ExePasswordComp(0, 0);
		if( temp_flag == 0)
		{
			//
			GUI_PutString16_16(4*8,3*8,"�������",0);
			REFURBISH_LCD();
			OSTimeDly(OS_TICKS_PER_SEC/1);
		}
		else if (temp_flag == 1)
		{
			CLR_SCREEN();
			para_temp.value.addr = temp_local_addr;
			SavePara();
			GUI_PutString16_16(24,16,(INT8*)"�����ѹ̻�",0);
	
			GUI_PutString16_16(16,32,(INT8*)"�����������",0);
			while(1)
			{
				OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
				if (cur_key_value)
				{
					OS_ENTER_CRITICAL();    	
					cur_key_value = 0;
					OS_EXIT_CRITICAL();
					break;	
				}				
			}
		}
		else if (temp_flag == 2)
		{
			//
		}	
	}	
}
/*
�������ö�
*/
void R_Edit_Password_Execute(struct _rq_t *item)
{
	#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	//��ѯ��ַ
    temp_local_password = 0;
	//temp_local_password = para_temp.value.password;
	//��ʼ��һ���ı��ؼ��������ַ
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "������:";
	text_t_ocx[0].Caption2 = " ";	//�˴�Ϊ��λ	
	//
	text_t_ocx[0].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[0].p_t_data = &temp_local_password ;
	//s	
	text_t_ocx[0].Width = 4;
	//
	text_t_ocx[0].x = 2;
	text_t_ocx[0].y = 3;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;			
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 8;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 254;
	text_t_ocx[0].Min = 1;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;
				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	
	//
	share_plist[0] = &text_t_ocx[0];
	
	//---------------------------------------------
	rq_first_rank.cont = 1;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 16;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	//
	//CLR_SCREEN();

	//
    cur_s = 0;
	CLR_SCREEN();
	GUI_PutStringTier(0,0,(INT8*)"��������",1);
	//
	//temp = para_temp.value.addr ;		//����ԭ��ַ
	//
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);
	//����
	ocx_key_type = 0;
	R_Execute(item);  
	//��ִ�к����˳�����һ����Ҫ����Ҫ�������
	if ( (ocx_key_type == KEY_SET))
	{
		UINT8 temp_flag;
		temp_flag = 0;
		
		temp_flag = ExePasswordComp(0, 0);
		if( temp_flag == 0)
		{
			//
			GUI_PutString16_16(4*8,3*8,"�������",0);
			REFURBISH_LCD();
			OSTimeDly(OS_TICKS_PER_SEC/1);
		}
		else if (temp_flag == 1)
		{
			CLR_SCREEN();
			para_temp.value.password = temp_local_password;
			SavePara();
			GUI_PutString16_16(24,16,(INT8*)"�����ѹ̻�",0);
	
			GUI_PutString16_16(16,32,(INT8*)"�����������",0);
			while(1)
			{
				OSTimeDly(OS_TICKS_PER_SEC/50);//delay 10ms		
				if (cur_key_value)
				{
					OS_ENTER_CRITICAL();    	
					cur_key_value = 0;
					OS_EXIT_CRITICAL();
					break;	
				}				
			}
		}
		else if (temp_flag == 2)
		{
			//
		}	
	}	
}
/*
ʱ�����ö�
*/
void R_Edit_Clock_Execute(struct _rq_t *item)
{
	//����Һ����ʱ��
	UINT16 clock_year;
	UINT16 clock_month,clock_day,clock_hour,clock_minuter,clock_second;
	//������Ҫ���жϣ�����ʱ��

	//
	//��ʼ���ꡢ�¡��ա�ʱ���֡��� 6���ؼ���
	//��--------------------------------
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "����:";
	text_t_ocx[0].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[0].ReadOnly = 2;		//����Դ����
	
	text_t_ocx[0].p_t_data = (&clock_year);
	//s	
	text_t_ocx[0].Width = 4;
	//
	text_t_ocx[0].x = 1;
	text_t_ocx[0].y = 2;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);		
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 6;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 2099;
	text_t_ocx[0].Min = 2007;
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	//��--------------------------------
	text_t_ocx[1].Type = K_TEXT;
	text_t_ocx[1].Focus = 1;
	//
	text_t_ocx[1].Caption1 = "-";
	text_t_ocx[1].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[1].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[1].p_t_data = &clock_month;
	//s	
	text_t_ocx[1].Width = 2;
	//
	text_t_ocx[1].x = 10;
	text_t_ocx[1].y = 2;
	//
	text_t_ocx[1].Px = 0;
	text_t_ocx[1].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[1],&text_t_ocx[1].Value);		
				
	text_t_ocx[1].Cursor = 0;
	text_t_ocx[1].location = 11;
	
	text_t_ocx[1].dot = 0;
	text_t_ocx[1].Max = 12;
	text_t_ocx[1].Min = 1;
	text_t_ocx[1].MaskChar = 0;
	text_t_ocx[1].GetFocus = T_GetFocus;
	text_t_ocx[1].LossFocus = T_LossFocus;
	text_t_ocx[1].KeyProcess = T_KeyProcess;
	text_t_ocx[1].qbch = T_qbch;
	text_t_ocx[1].SetVal = usr_SetVal_T;
	text_t_ocx[1].GetVal = usr_GetVal_T;				
	
	text_t_ocx[1].ReadOnly &= 0x0f;	

	//��--------------------------------
	text_t_ocx[2].Type = K_TEXT;
	text_t_ocx[2].Focus = 2;
	//
	text_t_ocx[2].Caption1 = "-";
	text_t_ocx[2].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[2].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[2].p_t_data = &clock_day;
	//s	
	text_t_ocx[2].Width = 2;
	//
	text_t_ocx[2].x = 13;
	text_t_ocx[2].y = 2;
	//
	text_t_ocx[2].Px = 0;
	text_t_ocx[2].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[2],&text_t_ocx[2].Value);		
				
	text_t_ocx[2].Cursor = 0;
	text_t_ocx[2].location = 14;
	
	text_t_ocx[2].dot = 0;
	text_t_ocx[2].Max = 31;
	text_t_ocx[2].Min = 1;
	text_t_ocx[2].MaskChar = 0;
	text_t_ocx[2].GetFocus = T_GetFocus;
	text_t_ocx[2].LossFocus = T_LossFocus;
	text_t_ocx[2].KeyProcess = T_KeyProcess;
	text_t_ocx[2].qbch = T_qbch;
	text_t_ocx[2].SetVal = usr_SetVal_T;
	text_t_ocx[2].GetVal = usr_GetVal_T;				
	
	text_t_ocx[2].ReadOnly &= 0x0f;	
	//ʱ--------------------------------
	text_t_ocx[3].Type = K_TEXT;
	text_t_ocx[3].Focus = 3;
	//
	text_t_ocx[3].Caption1 = "ʱ��:";
	text_t_ocx[3].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[3].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[3].p_t_data = &clock_hour;
	//s	
	text_t_ocx[3].Width = 2;
	//
	text_t_ocx[3].x = 1;
	text_t_ocx[3].y = 5;
	//
	text_t_ocx[3].Px = 0;
	text_t_ocx[3].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[3],&text_t_ocx[3].Value);		
				
	text_t_ocx[3].Cursor = 0;
	text_t_ocx[3].location = 6;
	
	text_t_ocx[3].dot = 0;
	text_t_ocx[3].Max = 23;
	text_t_ocx[3].Min = 0;
	text_t_ocx[3].MaskChar = 0;
	text_t_ocx[3].GetFocus = T_GetFocus;
	text_t_ocx[3].LossFocus = T_LossFocus;
	text_t_ocx[3].KeyProcess = T_KeyProcess;
	text_t_ocx[3].qbch = T_qbch;
	text_t_ocx[3].SetVal = usr_SetVal_T;
	text_t_ocx[3].GetVal = usr_GetVal_T;				
	
	text_t_ocx[3].ReadOnly &= 0x0f;	
	//��--------------------------------
	text_t_ocx[4].Type = K_TEXT;
	text_t_ocx[4].Focus = 4;
	//
	text_t_ocx[4].Caption1 = ":";
	text_t_ocx[4].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[4].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[4].p_t_data = &clock_minuter;
	//s	
	text_t_ocx[4].Width = 2;
	//
	text_t_ocx[4].x = 8;
	text_t_ocx[4].y = 5;
	//
	text_t_ocx[4].Px = 0;
	text_t_ocx[4].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[4],&text_t_ocx[4].Value);		
				
	text_t_ocx[4].Cursor = 0;
	text_t_ocx[4].location = 9;
	
	text_t_ocx[4].dot = 0;
	text_t_ocx[4].Max = 59;
	text_t_ocx[4].Min = 0;
	text_t_ocx[4].MaskChar = 0;
	text_t_ocx[4].GetFocus = T_GetFocus;
	text_t_ocx[4].LossFocus = T_LossFocus;
	text_t_ocx[4].KeyProcess = T_KeyProcess;
	text_t_ocx[4].qbch = T_qbch;
	text_t_ocx[4].SetVal = usr_SetVal_T;
	text_t_ocx[4].GetVal = usr_GetVal_T;				
	
	text_t_ocx[4].ReadOnly &= 0x0f;	
	//��--------------------------------
	text_t_ocx[5].Type = K_TEXT;
	text_t_ocx[5].Focus = 5;
	//
	text_t_ocx[5].Caption1 = ":";
	text_t_ocx[5].Caption2 = "";	//�˴�Ϊ��λ	
	//
	text_t_ocx[5].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[5].p_t_data = &clock_second;
	//s	
	text_t_ocx[5].Width = 2;
	//
	text_t_ocx[5].x = 11;
	text_t_ocx[5].y = 5;
	//
	text_t_ocx[5].Px = 0;
	text_t_ocx[5].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[5],&text_t_ocx[5].Value);		
				
	text_t_ocx[5].Cursor = 0;
	text_t_ocx[5].location = 12;
	
	text_t_ocx[5].dot = 0;
	text_t_ocx[5].Max = 59;
	text_t_ocx[5].Min = 0;
	text_t_ocx[5].MaskChar = 0;
	text_t_ocx[5].GetFocus = T_GetFocus;
	text_t_ocx[5].LossFocus = T_LossFocus;
	text_t_ocx[5].KeyProcess = T_KeyProcess;
	text_t_ocx[5].qbch = T_qbch;
	text_t_ocx[5].SetVal = usr_SetVal_T;
	text_t_ocx[5].GetVal = usr_GetVal_T;				
	
	text_t_ocx[5].ReadOnly &= 0x0f;	
	//
	share_plist[0] = &text_t_ocx[0];
	share_plist[1] = &text_t_ocx[1];
	share_plist[2] = &text_t_ocx[2];
	share_plist[3] = &text_t_ocx[3];
	share_plist[4] = &text_t_ocx[4];
	share_plist[5] = &text_t_ocx[5];

	//---------------------------------------------
	rq_first_rank.cont = 6;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 2;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	
	GUI_PutStringTier(0,0,(INT8*)"ʱ������",1);
	
	//����
	ocx_key_type = 0;
	R_Execute(item);  
	//��ִ�к����˳�����һ����Ҫ����Ҫ�������
	if (ocx_key_type == KEY_SET)
	{
		CLR_SCREEN();
		//��Ҫ���ж�


		SetProRunStatus(STA_SAVE_TIME_FLAG);
		ResetParaStatus(ENABLE_DATA_FLAG);

		if (!( ExeCallForm((UINT8 *)"���ڹ̻���,���Ժ�",0)))
		{
			ResetProRunStatus(STA_SAVE_TIME_FLAG);
			item->ct = item->cancel;
		}
		else
		{
			ResetProRunStatus(STA_SAVE_TIME_FLAG);
		}
		
	}	
}
/*
��ʾ���ö�
�����ؼ���	1��������ʱ		TEXT	Ϊ0ʱ����
			2��������ʾģʽ	LIST
			
�ϴ�������֡��ʽ��
�����ֽڣ���һ������ʱ����2������ʾģʽ			
*/
void R_Edit_Disp_Execute(struct _rq_t *item)
{	
	UINT16 disp_bkg_delay,disp_yc_disp_mode;
	disp_bkg_delay = bkg_delay;
	disp_yc_disp_mode = yc_disp_mode;
	//������ʱ---------------------------
	text_t_ocx[0].Type = K_TEXT;
	text_t_ocx[0].Focus = 0;
	//
	text_t_ocx[0].Caption1 = "������ʱ:";
	text_t_ocx[0].Caption2 = "��";	//�˴�Ϊ��λ	
	//
	text_t_ocx[0].ReadOnly = 1;		//����Դ����
	
	text_t_ocx[0].p_t_data = &bkg_delay;
	//s	
	text_t_ocx[0].Width = 3;
	//
	text_t_ocx[0].x = 0;
	text_t_ocx[0].y = 0;
	//
	text_t_ocx[0].Px = 0;
	text_t_ocx[0].Py = 0;
	
	//text_t_ocx[text_ocx_num].Value = 0;		//��ֵ������Ҫ����,�˴����ó�ʼ��
	usr_GetVal_T(&text_t_ocx[0],&text_t_ocx[0].Value);		
				
	text_t_ocx[0].Cursor = 0;
	text_t_ocx[0].location = 13;
	
	text_t_ocx[0].dot = 0;
	text_t_ocx[0].Max = 255;
	text_t_ocx[0].Min = 0;				
	text_t_ocx[0].MaskChar = 0;
	text_t_ocx[0].GetFocus = T_GetFocus;
	text_t_ocx[0].LossFocus = T_LossFocus;
	text_t_ocx[0].KeyProcess = T_KeyProcess;
	text_t_ocx[0].qbch = T_qbch;
	text_t_ocx[0].SetVal = usr_SetVal_T;
	text_t_ocx[0].GetVal = usr_GetVal_T;				
	
	text_t_ocx[0].ReadOnly &= 0x0f;				
	//��ʾģʽ--------------------------------
	list_t_ocx[0].Type = K_LIST;
	list_t_ocx[0].Focus = 1;
	//
	list_t_ocx[0].Caption1 = "������ʾģʽ:";
	
	list_t_ocx[0].ReadOnly = 0;	//λ��ֵ����λ
	list_t_ocx[0].p_l_data = &yc_disp_mode; 
	usr_GetVal_L(&list_t_ocx[0],&list_t_ocx[0].Cursor); 
					
	
	list_t_ocx[0].Value = (INT8**)dlist_rcz_ycz;
	list_t_ocx[0].Width = 5;
			
	list_t_ocx[0].x = 0;
	list_t_ocx[0].y = 2;
	//
	list_t_ocx[0].Px = 0;
	list_t_ocx[0].Py = 0;
	//		
	list_t_ocx[0].location = 13;	
	//
	list_t_ocx[0].Max = 2;
	list_t_ocx[0].GetFocus = L_GetFocus;
	list_t_ocx[0].LossFocus = L_LossFocus;
	list_t_ocx[0].KeyProcess = L_KeyProcess;
	list_t_ocx[0].qbch = L_qbch;
	list_t_ocx[0].SetVal = usr_SetVal_L;
	list_t_ocx[0].GetVal = usr_GetVal_L;				
	
	list_t_ocx[0].ReadOnly &= 0x0f;			
						
	//
	share_plist_sub[1] = &list_t_ocx[0];

	//
	share_plist[0] = &text_t_ocx[0];
	share_plist[1] = &list_t_ocx[0];

	//---------------------------------------------
	rq_first_rank.cont = 2;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 3;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_sys_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;	
		
	rq_first_rank.kj = &share_plist[0];
	
	GUI_PutStringTier(0,0,(INT8*)"��ʾ����",1);
	//����
	ocx_key_type = 0;
	R_Execute(item);  
	//��ִ�к����˳�����һ����Ҫ����Ҫ�������
	if (ocx_key_type == KEY_SET)
	{
		CLR_SCREEN();
		SetProRunStatus(STA_SEQ_DISP_FLAG);
		ResetParaStatus(ENABLE_DATA_FLAG);

		if (!( ExeCallForm((UINT8 *)"���ڹ̻���,���Ժ�",0)))
		{
			//����̻����ɹ����ָ���ǰ����
			bkg_delay = disp_bkg_delay;
			yc_disp_mode = disp_yc_disp_mode;

			ResetProRunStatus(STA_SEQ_DISP_FLAG);
			item->ct = item->cancel;

		}
		else
		{
			ResetProRunStatus(STA_SEQ_DISP_FLAG);
		}		
	}	
}

//********************************************************************
