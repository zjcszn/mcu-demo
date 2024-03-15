/*
*********************************************************************************************************
*	                                  
*	模块名称 : GUI界面主函数
*	文件名称 : MainTask.c
*	版    本 : V1.0
*	说    明 : LCD界面
*              
*	修改记录 :
*		版本号   日期         作者          说明
*		V1.0    2020-07-01   Eric2013  	    首版    
*                                     
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "MainTask.h"



/*
*********************************************************************************************************
*                                               变量
*********************************************************************************************************
*/
GX_WINDOW 		*pScreen;
GX_WINDOW_ROOT  *root;

/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: GUI主函数
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MainTask(void) 
{
	/* 避免上电后瞬间的撕裂感 */
	LCD_SetBackLight(0);
	
	/*
       触摸校准函数默认是注释掉的，电阻屏需要校准，电容屏无需校准。如果用户需要校准电阻屏的话，执行
	   此函数即可，会将触摸校准参数保存到EEPROM里面，以后系统上电会自动从EEPROM里面加载。
	*/
#if 0
	LCD_SetBackLight(255);
	LCD_InitHard();
    TOUCH_Calibration(2);
#endif
	
	/*初始化配置 */
	gx_initconfig();

	/* 配置显示屏 */
    gx_studio_display_configure(DISPLAY_1, stm32h7_graphics_driver_setup_565rgb,
        LANGUAGE_CHINESE, DISPLAY_1_THEME_1, &root);
	
    /* 创建窗口 */
    gx_studio_named_widget_create("window", (GX_WIDGET *)root, (GX_WIDGET **)&pScreen);

	/* 显示根窗口 */
    gx_widget_show(root);

    /* 启动GUIX */
    gx_system_start();
	
	tx_thread_sleep(300);
	LCD_SetBackLight(255);
	
	while(1)
	{
		bsp_LedToggle(1);
		bsp_LedToggle(2);
		bsp_LedToggle(3);
		bsp_LedToggle(4);
		tx_thread_sleep(20);
	}
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
