#define _FORM_SOE_REPORT_C
#include "user_head.h"
/*
ģ�飺<�¼�����>
----------------
�ڴ˲˵��µ������Ӳ˵�

   �¼�����	|---�¼����
			|
			|
			|---�¼����


����ʾ��ʽ����δ���������˵����
	1.��ʾ�ִ���ʽ
		ÿ��SOE��������ʾ��	��һ����ʾSOE���� + 	����(��)\����(��)
							����ֵ+��λ + ʱ��
			----------------------------------
	����	��·����λ��	��
			2007-10-18 21:59:59:369
			����һ��Iʱ��	����
			2007-10-18 21:59:59:369		2.01A
			----------------------------------
	����Լ�涨��
		

	�õ����¼����ݿ�����ݽṹ���£�
	0	�¼�
	1	SOE�¼���
	����ÿ14���ֽ�Ϊһ�飬�鶨����μ���Լ
*/


void DispSoeReport(struct _rq_t *item)
{	
	UINT8 max_line;
	UINT8 temp_need_pluse;	//��Ҫ������ʾ

	UINT16 bak_key_value;

	UINT8 soe_num;	
	//�ٻ�SOE����
	SetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
	ResetParaStatus(ENABLE_DATA_FLAG);
	if(!( ExeCallForm((UINT8 *)"SOE�¼��ٻ���,���Ժ�",0)))
	{
		ResetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
		item->ct = item->cancel;
		return;
	}
	//��ʾSOE
	soe_num = share_buf[1];

	if ((share_buf[0] != FRAME_TYPE_SEQ_EVENT))	//�Ƿ����
	{
		GUI_PutString16_16(0,0,(INT8*)"����!����!�Ƿ��¼���,����",0);
		REFURBISH_LCD();				//ˢ����ʾ��
		//�˴���ʼ�ȴ��û���������
		WaitHitKeyReturn(item,0);
		return;
	}
	if (0 == soe_num)
	{
		GUI_PutString16_16(0,0,(INT8*)"���¼���¼",0);
		REFURBISH_LCD();				//ˢ����ʾ��
		//�˴���ʼ�ȴ��û���������
		WaitHitKeyReturn(item,0);
		return;
	}
	
	CurHideCur();		

	max_line = soe_num * 2;		//ÿ��SOE��ʾ����
	item->df_line = 0;
	item->cur_line = 0;
	temp_need_pluse = 1;
		
	while (1)
	{
		OSTimeDly(OS_TICKS_PER_SEC/20);
	
		if (temp_need_pluse)
		{
			UINT8 *p,i;
			UINT16 evt_no;		//�����	
						
			INT8 temp_str[48];

			temp_need_pluse = 0;
			CLR_SCREEN();
			//��ʼ��ʾ�¼���ÿ������
			for (i=item->df_line; (i<item->df_line+4)&&(i<max_line); i++)
			{
				p = &share_buf[2+(i/2)*14];		//ָ��SOE�¼���������
				evt_no = (*(p+1)<<8) + *p;		//�õ������
				
				
				if (evt_no == 0xFF)
				{					
					GUI_PutString16_16(32,(i-item->df_line)*2*8,"SOE�����δ��Ӧ����",0);
					i++;
					sprintf(temp_str,"EVT: %04x",evt_no);
					GUI_PutString16_16(32,(i-item->df_line)*2*8,temp_str,0);
				}
				else
				{
					//��ʼ��ʾSOE				
					GUI_PutString16_16(0,(	i-item->df_line)*2*8,
											(INT8*)soe_type_list[evt_no],
											0);
					//��ʾ�����򸴹��
					if (*(p+11) & 0x08)		//�Ϸ�����
					{
						if (*(p+11) & 0x10)
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"��",
												0);
						}
						else
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"��",
												0);
						}
					}
					else					//����/�������� 
					{
						if (*(p+11) & 0x10)
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"����",
												0);
						}
						else
						{
							GUI_PutString16_16(	144,(i-item->df_line)*2*8,
												"����",
												0);
						}
					}
					//��һ����ʾ��ϣ�������ʾ�軻��
					i++;
					//��ʾʱ���					
					sprintf(temp_str,"%04d-%02d-%02d",(*(p+9)<<8) + (*(p+8))+2000,*(p+7),*(p+6));
					GUI_PutString8_8(0,(i-item->df_line)*2*8,temp_str);
					sprintf(temp_str,"%02d:%02d:%02d.%03d",
									*(p+5),
									*(p+4),
									((*(p+3)<<8) + (*(p+2)))/1000,
									((*(p+3)<<8) + (*(p+2)))%1000);
					GUI_PutString8_8(0,(i-item->df_line)*2*8+8,temp_str);
					//��ʾ����ֵ
					if(*(p+11) & 0x80)	//����ֵ��Ч
					{						
						UINT32 value;
						UINT8 dot;						
						
						value = ((UINT32)*(p+13)<<8) + (UINT32)(*(p+12));								
						
						//value1���ǽ��,ת�����ִ�
						sprintf(temp_str,"[%05d",value & 0xffff);
						//С���㴦��
						dot = (*(p+10) & 0xE0)>>5;	
						if (dot)
						{
							UINT8 temp1,temp2;
				  			//			  			
				  			temp1 = strlen(temp_str);
				  			temp_str[temp1+1] = 0;
				  			
				  			for (temp2=0; temp2<dot; temp2++)
				  			{
				  				temp_str[temp1] = temp_str[temp1-1];
				  				temp1--;
				  			}  		
				  			temp_str[temp1] = '.';
						}
						//��ʾ��������ֵ
						GUI_PutString16_16(	96,(i-item->df_line)*2*8,temp_str,0);
						//��λ��ʾ			
						GUI_PutString16_16(	96+strlen(temp_str)*8,
											(i-item->df_line)*2*8,
											(int8*)soe_unit[*(p+10)&0x1F],
											0);
					}
					

				}					
			}
			//-------------------------------------------------------------
			//��������
			if (item->df_line != 0)
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,0,"��",1);
			}
			else
			{
			//	GUI_PutString16_16(GUI_LCM_XMAX-16,0,"  ",0);
			}
			if ((item->df_line+4) < max_line)
			{
				GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"��",1);
			}
			else
			{
			//	GUI_PutString16_16(GUI_LCM_XMAX-16,GUI_LCM_YMAX-16,"  ",0);
			}
			//-------------------------------------------------------------
		}
		REFURBISH_LCD();
		//������
		//���а�������
		while (0 == cur_key_value)
		{
			OSTimeDly(OS_TICKS_PER_SEC/20);			
		}
		//
	    OS_ENTER_CRITICAL();
	    
	    bak_key_value = cur_key_value;		
		cur_key_value = 0;	
		
		OS_EXIT_CRITICAL();
		//
		switch (bak_key_value)
		{
			case KEY_ESC:
				ResetProRunStatus(STA_SEQ_EVENT_CALL_FLAG);
				item->ct = item->cancel;
				return;
			case KEY_DOWN:					
				if ((item->df_line+4) < max_line)
				{					
					//item->df_line++;
					item->df_line+=4;
					
					temp_need_pluse = 1;
					//GUI_RectangleFill(32,0,192,64,0);
				}
				break;
			case KEY_UP:
				if (4 <= item->df_line)
				{					
					item->df_line-=4;
					
					temp_need_pluse = 1;
					//GUI_RectangleFill(32,0,192,64,0);
				}
				break;
		}		
	}
}

void ClrSoeReport(struct _rq_t *item)
{
	CLR_SCREEN();
	//��ʾ��֧�ֵ��¼���ʾ
	GUI_PutString16_16(16,16,(INT8*)"SORRY,SOE CLR DISABLE!",0);	
	GUI_PutString8_8(0,32,(INT8*)"FormSoeReport.c");
	GUI_PutString8_8(0,48,(INT8*)"ClrSoeReport");
	REFURBISH_LCD();
	//�˴���ʼ�ȴ��û���������
	WaitHitKeyReturn(item,0);
	return;
}


