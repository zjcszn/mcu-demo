#define _LCD_MOUDLE_C

#include "head.h"

//�˳����ļ�ʵ������LCD��ʾ��������

//
UINT8 ExeSaveForm(UINT8 *str, UINT8 mode)
{
	return 0;
}

UINT8 ExeCallForm(UINT8 *str, UINT8 mode)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif
	UINT8 i;
	UINT8 temp;
	UINT16 bak_key_value;

	CurHideCur();
	GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);
	//���о�����ʾ������1�У��������ʾ
	i = strlen((INT8*)str);
	if (i>20)
	{
		i = 20;
	}
	GUI_PutString16_16((24-i)/2*8,3*8,(INT8*)str,0);
	REFURBISH_LCD();				//ˢ����ʾ��

	temp = 200;
	//
	while (!(GetParaStatus() & ENABLE_DATA_FLAG))
	{
		if ((--temp) == 0)
		{
			//����û���ٻ��ɹ�����ʾһ��Ȼ�󷵻�
			GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);
			GUI_PutString16_16(64,24,(INT8*)"��ʱʧ��",0);
			REFURBISH_LCD();				//ˢ����ʾ��
			OSTimeDly(OS_TICKS_PER_SEC);
			OSTimeDly(OS_TICKS_PER_SEC);
			//						
			return 0;	
		}
		//
		OSTimeDly(OS_TICKS_PER_SEC/20);
		//
		//�����û���ȡ��������
		if (0 == cur_key_value)	continue;
		//
	    OS_ENTER_CRITICAL();
	    
	    bak_key_value = cur_key_value;		
		cur_key_value = 0;	
		
		OS_EXIT_CRITICAL();
		//			
		if (bak_key_value == KEY_ESC)
		{			
			return 0;
		}
	}
	
	return 1;
}

/*
������֤�Ի�����
Ӧ�������û�3���������룬��Ϊ�˰�ȫ�����ֻ��������һ�Ρ�
����ֵ��
		0:����
		1:��ȷ
		2:ȡ��		
*/
UINT8 ExePasswordComp(UINT8 *str, UINT8 mode)
{
#if OS_CRITICAL_METHOD == 3                      /* Allocate storage for CPU status register           */
    OS_CPU_SR  cpu_sr = 0;
#endif	
	k_text_t temp_text_ocx;
	UINT16 temp_password;
	UINT16 bak_key_value; 
	UINT16 flag;
	
	str = str;
	temp_password = 0;
	flag = 0;

	//
	//��ʱ����һ���ı��ؼ�
	temp_text_ocx.Type = K_TEXT;
	temp_text_ocx.Focus = 0;
	//
	temp_text_ocx.Caption1 = "��������:";
	temp_text_ocx.Caption2 = " ";
	//
	temp_text_ocx.ReadOnly = 1;

	temp_text_ocx.p_t_data = &temp_password;
	temp_text_ocx.Width = 4;
	//
	temp_text_ocx.x = 0;
	temp_text_ocx.y = 3;
	temp_text_ocx.Px = 0;
	temp_text_ocx.Py = 0;
	//
	temp_text_ocx.Cursor = 0;
	temp_text_ocx.location = 9;

	temp_text_ocx.dot = 0;
	temp_text_ocx.Max = 60000;
	temp_text_ocx.Min = 0;

	temp_text_ocx.MaskChar = 0;
	temp_text_ocx.GetFocus = T_GetFocus;
	temp_text_ocx.LossFocus = T_LossFocus;
	temp_text_ocx.KeyProcess = T_KeyProcess;
	temp_text_ocx.qbch = T_qbch;
	temp_text_ocx.SetVal = usr_SetVal_T;
	temp_text_ocx.GetVal = usr_GetVal_T;
	
	temp_text_ocx.ReadOnly &= 0x0F;
	//��ʼ���ؼ�
	usr_GetVal_T(&temp_text_ocx,&temp_text_ocx.Value);
	CurHideCur();
	GUI_RectangleFill(0, 0, 24*8, 8*8,back_color);  
	SetItemPx(&temp_text_ocx,0);
	SetItemPy(&temp_text_ocx,0);
	qbchItem(&temp_text_ocx);
	T_GetFocus(&temp_text_ocx);
	//
	REFURBISH_LCD();	
	//�����ı��ؼ�
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20);
	
		if (0 == cur_key_value) continue;
		//
		OS_ENTER_CRITICAL();

		bak_key_value = cur_key_value;
		cur_key_value = 0;

		OS_EXIT_CRITICAL();
		//
		switch(bak_key_value)
		{
			case KEY_ESC:
				return 2;
//				break;
			case KEY_SET:
				flag = 1;
				break;				
			default:

				T_KeyProcess(&temp_text_ocx,bak_key_value);
				break;
		}
		//
		if (flag)
		{
			usr_SetVal_T(&temp_text_ocx,temp_password);
			break;
		}
	}
	//�Ƚ�����������ϵͳ�����Ƿ�һ��
	if (para_temp.value.password != temp_password)
	{
		return 0;
	}
	//
	return 1;
}