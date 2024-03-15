/*
*********************************************************************************************************
*	                                  
*	ģ������ : GUI�����ϵͳ����
*	�ļ����� : App_SysFunction.c
*	��    �� : V1.1
*	˵    �� : GUIX��ϵͳAPI
*	�޸ļ�¼ :
*		�汾��    ����          ����         ˵��
*		V1.0    2020-06-21    Eric2013       �׷�
*                                     
*	Copyright (C), 2020-2030, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"



/*
*********************************************************************************************************
*	                                        ��̬�ڴ����
*********************************************************************************************************
*/
#define 	  GUI_NUMBYTES     1024*1024*24                        /* ���ö�̬�ڴ��С */
#define 	  Canvas_Memory    0xC0400000                          /* ����Canvas��ַ   */
TX_BYTE_POOL  memory_pool;
uint8_t       *MemoryBlock = (uint8_t *)(0xC0000000 + 1024*1024*8); /* ��̬�ڴ��ַ    */


/*
*********************************************************************************************************
*	                                        ����
*********************************************************************************************************
*/
extern GX_STUDIO_DISPLAY_INFO guiapp_display_table[1];


/*
*********************************************************************************************************
*	                                       ��̬�ڴ溯��
*********************************************************************************************************
*/
VOID *memory_allocate(ULONG size)
{
    VOID *memptr;

    if (tx_byte_allocate(&memory_pool, &memptr, size, TX_NO_WAIT) == TX_SUCCESS)
    {
        return memptr;
    }
    return NULL;
}

void memory_free(VOID *mem)
{
    tx_byte_release(mem);
}

/*
*********************************************************************************************************
*	�� �� ��: gx_initconfig
*	����˵��: GUIX
*	��    ��: ��   	
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void gx_initconfig(void) 
{
	 /* ��ʼ���ڴ�� */
    tx_byte_pool_create(&memory_pool, "MemoryBlock",  MemoryBlock,  GUI_NUMBYTES);
	
	/* ��ʼ��GUIX */
    gx_system_initialize();

    /* ע�ᶯ̬�ڴ�������ͷź��� */
    gx_system_memory_allocator_set(memory_allocate, memory_free);
	
	/* ����Ӧ��ͬ�ֱ�����ʾ�� */
	switch (g_LcdType)
	{
		
		case LCD_43_480X272:		/* 4.3�� 480 * 272 */	
		case LCD_50_480X272:		/* 5.0�� 480 * 272 */
			guiapp_display_table[0].x_resolution = 480;
			guiapp_display_table[0].y_resolution = 272;
			break;
		
		case LCD_50_800X480:		/* 5.0�� 800 * 480 */
		case LCD_70_800X480:		/* 7.0�� 800 * 480 */	
			guiapp_display_table[0].x_resolution = 800;
			guiapp_display_table[0].y_resolution = 480;
			break;
		
		default:	
			break;
	}

	guiapp_display_table[0].canvas_memory = (GX_COLOR *)Canvas_Memory;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
