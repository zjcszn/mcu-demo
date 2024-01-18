/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "guider_fonts.h"

typedef struct
{
	lv_obj_t *screen;
	bool screen_del;
	lv_obj_t *screen_cont_1;
	lv_obj_t *screen_digital_clock_1;
	lv_obj_t *screen_datetext_1;
	lv_obj_t *screen_imgbtn_4;
	lv_obj_t *screen_imgbtn_4_label;
	lv_obj_t *screen_imgbtn_1;
	lv_obj_t *screen_imgbtn_1_label;
	lv_obj_t *screen_camera;
	lv_obj_t *screen_camera_label;
	lv_obj_t *screen_imgbtn_3;
	lv_obj_t *screen_imgbtn_3_label;
	lv_obj_t *screen_imgbtn_5;
	lv_obj_t *screen_imgbtn_5_label;
	lv_obj_t *screen_btn_1;
	lv_obj_t *screen_btn_1_label;
	lv_obj_t *screen_imgbtn_2;
	lv_obj_t *screen_imgbtn_2_label;
	lv_obj_t *screen_menu_1;
	lv_obj_t *menu_4;
	lv_obj_t *menu_3;
	lv_obj_t *menu_2;
	lv_obj_t *screen_menu_1_main_page;
	lv_obj_t *screen_menu_1_sub_page0;
	lv_obj_t *screen_menu_1_menu_cont0;
	lv_obj_t *screen_menu_1_menu_label0;
	lv_obj_t *screen_menu_1_sub_page1;
	lv_obj_t *screen_menu_1_menu_cont1;
	lv_obj_t *screen_menu_1_menu_label1;
	lv_obj_t *screen_menu_1_sub_page2;
	lv_obj_t *screen_menu_1_menu_cont2;
	lv_obj_t *screen_menu_1_menu_label2;
}lv_ui;

void ui_init_style(lv_style_t * style);
void init_scr_del_flag(lv_ui *ui);
void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_screen(lv_ui *ui);
LV_IMG_DECLARE(_temp_alpha_50x50);
LV_IMG_DECLARE(_camera_alpha_50x50);
LV_IMG_DECLARE(_FileManager_alpha_50x50);
LV_IMG_DECLARE(_photo_alpha_50x50);
LV_IMG_DECLARE(_network_alpha_50x50);
LV_IMG_DECLARE(_cloudshell_alpha_50x50);

#ifdef __cplusplus
}
#endif
#endif