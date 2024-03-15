#define _FORM_CTRL_MANAGE_C
#include "head.h"
/*
ģ�飺<���ƹ���>


��ʾ������£�
		------------------------------------------
		Ŀ��:[�����·��]
		״̬:[��λ]			����:[��]
		------------------------------------------
				<ȷ�ϼ�>����ִ�в���
		------------------------------------------

��������	|�����·��
			|
			|���ÿ���1
			|
			|���ÿ���2
			|
			|���ÿ���3
			

*/

#define OPEN_SAFE_CHECK

const INT8 *ctrl_object[];
const INT8 *ctrl_type[];
const INT8 *opp_ctrl_type[];
void R_CtrlManageExecuteNew(struct _rq_t *item);
void R_Ctrl_Opt_Execute(struct _rq_t *item);
void R_Ctrl_Debug_Execute(struct _rq_t *item);

void R_SecondCtrlExecute(struct _rq_t *item);

void R_Ctrl_Manage_Execute(struct _rq_t *item)
{
	CLR_SCREEN();
	rq_first_rank.Execute = R_CtrlManageExecuteNew;	
	
	rq_first_rank_enter_flag = 0;

	R_Execute(item);
}

void R_CtrlManageExecuteNew(struct _rq_t *item)
{

	//���Ը��ݿؼ������趨��ʼ������
	if (rq_ctrl_manage_menu.cur_ocx == &ctrl_manage_opt)
	{
		R_Ctrl_Opt_Execute(item);
	}
	else if (rq_ctrl_manage_menu.cur_ocx == &ctrl_manage_debug)
	{
		//rq_first_rank.Execute = R_Edit_Addr_Execute;//gh	
		R_Ctrl_Debug_Execute(item);
	}
}

/*
���д���һ���˵���̬���ɼ�������������Ҫ�Ŀؼ���ʼ��

ң�ز����ڻ������Ĵ��˳���ǣ�
//ÿ��6���ֽ�
0	ң�غ�
1	��Ӧ���߼�����(b7) ��Ӧ������(b6-b4) ң������(b3-b0)
2	����
3	ң�ض�������
4	���Ʒ�ʽ ���Ʒ�ʽ����λ
5	����

6
.


*/
void R_Ctrl_Opt_Execute(struct _rq_t *item)
{
//#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
//    OS_CPU_SR     cpu_sr = 0;
//#endif
	UINT8 j;

	rq_first_rank.cancel = &rq_ctrl_manage_menu;
	
	if (rq_first_rank_enter_flag) goto ctrl_abcd;	
	//��ѯң�ز���
	
	j = 0;
	while (j<(CTRL_PARA_LENGTH/6))
	{
		//���һ���˵�
		menu_t_ocx[j].Type = K_MENU;
		menu_t_ocx[j].Focus = j;	
		//
		menu_t_ocx[j].Text = (INT8*)ctrl_object[ctrl_para_xl_a[(j*6) + 3]];
		//
		menu_t_ocx[j].x = 0;
		menu_t_ocx[j].y = j*2;			
		//
		menu_t_ocx[j].GetFocus = M_GetFocus;
		menu_t_ocx[j].LossFocus = M_LossFocus;
		menu_t_ocx[j].qbch = M_qbch;
	
		menu_t_ocx[j].Token = &rq_second_rank;
	
		share_plist[j] = &menu_t_ocx[j];
		
		j++;
	}
	//
	rq_first_rank.cont = j;
	rq_first_rank.df_line = 0;
	rq_first_rank.cur_line = 0;
	rq_first_rank.Focus = 0;
	rq_first_rank.x = 3;
	rq_first_rank.y = 0;
	rq_first_rank.Width = 24;
	rq_first_rank.Height = 8;
	
	rq_first_rank.cancel = &rq_ctrl_manage_menu;
	rq_first_rank.SetFocus = R_SetFocus;
	rq_first_rank.Initsize = R_Initsize;	
	
	rq_first_rank.enter = 0;		
	
	rq_first_rank.kj = &share_plist[0];
	
	rq_first_rank_enter_flag = 1;
    
    cur_s = 0;
ctrl_abcd:	CLR_SCREEN();

	GUI_PutStringTier(0,0,(INT8*)"��������",1);
	//ָ����������ִ�к���
	rq_second_rank.Execute = R_SecondCtrlExecute;
	//
	R_Execute(item);	
}

void R_Ctrl_Debug_Execute(struct _rq_t *item)
{
	/********
	CLR_SCREEN();
	rq_first_rank.cancel = &rq_ctrl_manage_menu;	
	GUI_PutString16_16(4*8,24,(INT8*)"Ȩ�޲���,�뷵��",0);	
	REFURBISH_LCD();
	//�˴���ʼ�ȴ��û���������
	WaitHitKeyReturn(item,0);	
	item->ct = item->cancel;
	return;
	***************/
}

//----------------------------------------------------------------------
void R_SecondCtrlExecute(struct _rq_t *item)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR     cpu_sr = 0;
#endif
    
	UINT8 i,index;
	//printf("test\r\n");
	//�ҵ���ǰ�˵��ţ����ݲ˵���ȥ������ȡ������Ȼ���γɿؼ��顣
	for (i=0; i<MENU_PLIST_MAX; i++)
	{
		if (rq_first_rank.cur_ocx == &menu_t_ocx[i])
		{
			index = i;
		//	printf("index=%d",index);
			break;
		}
	}	
	//
	//���ؼ�
	/*	
		Ŀ��:[�����·��]
		״̬:[��λ]			����:[��]
		------------------------------------------
				<ȷ�ϼ�>����ִ�в���
		------------------------------------------
	*/
	//Ŀ��:[�����·��]	(ֻ��)

//	ctrl_code = share_buf[index*6];
	
	list_t_ocx[0].Type = K_LIST;
	list_t_ocx[0].Focus = 0;
	//
	list_t_ocx[0].Caption1 = (INT8*)" ";
	list_t_ocx[0].Caption2 = (INT8*)" ";
	list_t_ocx[0].ReadOnly = 0xf0;
    
	//list_t_ocx[0].p_l_data = &share_buf[(index*6)+3];//���λ��
	//usr_GetVal_L_Gern(&list_t_ocx[0],&list_t_ocx[0].Cursor);
    list_t_ocx[0].Cursor = ctrl_para_xl_a[(index*6)+3];
    
	list_t_ocx[0].Value = (INT8**)ctrl_object;
    
	list_t_ocx[0].Width = 20;
	//
	list_t_ocx[0].x = 0;
	list_t_ocx[0].y = 0;
	list_t_ocx[0].Px = 0;
	list_t_ocx[0].Py = 0;
	//
	list_t_ocx[0].location = 0;	
//	list_t_ocx[0].Max = GetListCount((INT8**)ctrl_object);
	list_t_ocx[0].GetFocus = L_GetFocus;
	list_t_ocx[0].LossFocus = L_LossFocus;
	list_t_ocx[0].KeyProcess = L_KeyProcessGern;
	list_t_ocx[0].qbch = L_qbch;
	list_t_ocx[0].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[0].GetVal = usr_GetVal_L_Gern;
	share_plist_sub[0] = &list_t_ocx[0];
    
	//״̬:[��λ]
	list_t_ocx[1].Type = K_LIST;
	list_t_ocx[1].Focus = 1;
	//
	list_t_ocx[1].ReadOnly = 0x01;
	
	list_t_ocx[1].Caption1 = (INT8*)"״̬:";
	list_t_ocx[1].Caption2 = (INT8*)" ";
    
    list_t_ocx[1].Cursor = control_object_state[list_t_ocx[0].Cursor];
	//
//	if (0x00 == ctrl_para_xl_a[(i*6)+1] & 0x7f)			//YX
//	{
	//	list_t_ocx[1].ReadOnly |= lcd_ctrl_flag[list_t_ocx[0].Cursor];			//ȡ��·��λ�û������ڵ�λ�ã����ɵ������ݵõ�
	//list_t_ocx[1].p_l_data = (u16*)&control_object_state[list_t_ocx[0].Cursor];

    //���������������
   // if((list_t_ocx[0].Cursor == LCD_OPEN_PAIWU)||(list_t_ocx[0].Cursor == LCD_CLOSE_PAIWU))
    if(list_t_ocx[0].Cursor == LCD_OPEN_PAIWU)
    {
        //�����ʱ���۷����ڶ���
        if(control_object_state[list_t_ocx[0].Cursor] ==OBJECT_RUNNING)
        {
            CLR_SCREEN();
            while(1)
            {
                GUI_PutString16_16(8,8,(INT8*)"���۷����ڶ���",0);
                GUI_PutString16_16(32,24,(INT8*)"��ֹ����",0);
                GUI_PutString16_16(16,40,(INT8*)"�����������",0);
                if (cur_key_value)                            
                {
                    OS_ENTER_CRITICAL();    	
                    cur_key_value = 0;
                    OS_EXIT_CRITICAL();
                    item->ct = (void*)&rq_first_rank;
                    return;
                }
                OSTimeDly(OS_TICKS_PER_SEC/50 );
            }            
        }        
    }	
	//usr_GetVal_L(&list_t_ocx[1],&list_t_ocx[1].Cursor);
	list_t_ocx[1].Value = (INT8**)&ctrl_type[(ctrl_para_xl_a[(index*6)+1])*2];
	list_t_ocx[1].Width = 20;
	//
	list_t_ocx[1].x = 0;
	list_t_ocx[1].y = 3;
	list_t_ocx[1].Px = 0;
	list_t_ocx[1].Py = 0;
	//
	list_t_ocx[1].location = 6;	
	list_t_ocx[1].Max = 2;
	list_t_ocx[1].GetFocus = L_GetFocus;
	list_t_ocx[1].LossFocus = L_LossFocus;
	list_t_ocx[1].KeyProcess = L_KeyProcessGern;
	list_t_ocx[1].qbch = L_qbch;
	list_t_ocx[1].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[1].GetVal = usr_GetVal_L;
	share_plist_sub[1] = &list_t_ocx[1];
	//����:[��]
	list_t_ocx[2].Type = K_LIST;
	list_t_ocx[2].Focus = 2;
	//
	list_t_ocx[2].ReadOnly = 0x01;
	list_t_ocx[2].Caption1 = (INT8*)"����:";	//	
	list_t_ocx[2].Caption2 = (INT8*)" ";
	
    //�ڰ��������У��������Ǵ�ֵ

	list_t_ocx[2].p_l_data =(u16*)&lcd_ctrl_flag[index]; 

    
    //������״̬��ʾ
    if(control_object_state[index]==OBJECT_OPEN)
    {
        list_t_ocx[2].Cursor =1;
    }
    else if(control_object_state[index]==OBJECT_CLOSE)
    {
        list_t_ocx[2].Cursor =0;
    }
	//usr_GetVal_L(&list_t_ocx[2],&list_t_ocx[2].Cursor);
    
	list_t_ocx[2].Value = (INT8**)&ctrl_type[(ctrl_para_xl_a[(index*6)+1])*2];
	list_t_ocx[2].Width = 20;
	//
	list_t_ocx[2].x = 0;
	list_t_ocx[2].y = 5;
	list_t_ocx[2].Px = 0;
	list_t_ocx[2].Py = 0;
	//
	list_t_ocx[2].location = 6;	
	list_t_ocx[2].Max = 2;
	list_t_ocx[2].GetFocus = L_GetFocus;
	list_t_ocx[2].LossFocus = L_LossFocus;
	list_t_ocx[2].KeyProcess = L_KeyProcessControl;
	list_t_ocx[2].qbch = L_qbch;
	list_t_ocx[2].SetVal = usr_SetVal_L_Gern;
	list_t_ocx[2].GetVal = usr_GetVal_L;
	share_plist_sub[2] = &list_t_ocx[2];
	//---------
	rq_second_rank.cont = 3;
	rq_second_rank.df_line = 0;
	rq_second_rank.cur_line = 0;
	rq_second_rank.Focus = 2;
	rq_second_rank.x = 0;
	rq_second_rank.y = 0;
	rq_second_rank.Width = 16;
	rq_second_rank.Height = 8;
	
	rq_second_rank.cancel = &rq_first_rank;
	rq_second_rank.SetFocus = R_SetFocus;
	rq_second_rank.Initsize = R_Initsize;	
	
	rq_second_rank.enter = 0;	
		
	rq_second_rank.kj = &share_plist_sub[0];
	//------------------------------------------
	CLR_SCREEN();

    
	R_Execute(item);
   
	//
}

const INT8 *ctrl_object[] = 
{
    "[1�ż�ҩ]",
	"[2�ż�ҩ]",
	"[3�ż�ҩ]",
	"[4�ż�ҩ]",
	"[���۷�]",
	"[�����µ�]",
	"[����ص�]",
	"[ĸ�߲൶]",
	"[���Ե㵶]",
	"[��·��ص�]",
	"[��λ����]",
	"[���]",
	"[���ȱ�����բ���]",
	"[��ѹ����]",
	"[��λ����ͣ]",
	"[����]",
	//10H
	"[���ÿ���1]",
	"[���ÿ���2]",
	"[���ÿ���3]",
	"[���ÿ���4]",
	"[���ÿ���5]",
	"[���ÿ���6]",
	"[���ÿ���7]",
	"[���ÿ���8]",
	"[���ÿ���9]",
	"[���ÿ���10]",
	"[���ÿ���11]",
	"[���ÿ���12]",
	"[���ÿ���13]",
	"[���ÿ���14]",
	"[���ÿ���15]",
	"[���ÿ���16]",
	//20h
	"[���ߢ��·��]",
	"[���ߢ��·��]",
	"[�ֶο���]",
	"[PT�л�]",
};


const INT8 *ctrl_type[] = 
{
	"��","��",
	"��","��",
	"��ͣ","����",
	"��ԭ","�л�",
	"����","����",
	"����ͣ���","����ͣ",
};


