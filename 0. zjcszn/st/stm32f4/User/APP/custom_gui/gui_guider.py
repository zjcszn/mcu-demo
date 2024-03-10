# Copyright 2023 NXP
# SPDX-License-Identifier: MIT
# The auto-generated can only be used on NXP devices

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct

lv.init()
SDL.init(w=480,h=800)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(480*10)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 800
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init() 
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)
        
def ta_zh_event_cb(e,kb):
    code = e.get_code()
    ta = e.get_target()
    if code == lv.EVENT.FOCUSED:
        kb.keyboard_set_textarea(ta)
        kb.move_foreground()
        kb.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.keyboard_set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)



# create screen
screen = lv.obj()
screen.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# create style style_screen_main_main_default
style_screen_main_main_default = lv.style_t()
style_screen_main_main_default.init()
style_screen_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_main_main_default.set_bg_opa(0)

# add style for screen
screen.add_style(style_screen_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_cont_1
screen_cont_1 = lv.obj(screen)
screen_cont_1.set_pos(int(0),int(0))
screen_cont_1.set_size(480,800)
screen_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)

# create screen_digital_clock_1

screen_digital_clock_1 = lv.dclock(screen_cont_1, "11:25:50 AM")
screen_digital_clock_1.set_text("11:25:50 AM")



class screen_digital_clock_1_timerClass():
    def __init__(self):
        self.hour = 11
        self.minute = 25
        self.second = 50
        self.meridiem = "AM"
  
    def count_24(self, timer):
        self.second += 1
        if self.second == 60:
            self.second = 0
            self.minute += 1
	
        if self.minute == 60:
            self.minute = 0
            self.hour +=1
            
        if self.hour == 24:
            self.hour = 0

        screen_digital_clock_1.set_text("%02d:%02d:%02d" %(self.hour, self.minute, self.second))

    def count_12(self, timer):
        self.second += 1
        if self.second == 60:
            self.second = 0
            self.minute += 1
        if self.minute == 60:
            self.minute = 0
            if self.hour < 12:
                self.hour += 1
            else:
                self.hour += 1
                self.hour = self.hour % 12
        if (self.hour == 12 and self.second == 0 and self.minute == 0):
            if(self.meridiem == "PM"):
                self.meridiem = "AM"
            else:
                self.meridiem = "PM"
		
        screen_digital_clock_1.set_text("%02d:%02d:%02d %s" %(self.hour, self.minute, self.second, self.meridiem))

screen_digital_clock_1_timerInstance = screen_digital_clock_1_timerClass()

screen_digital_clock_1_timer = lv.timer_create_basic()
screen_digital_clock_1_timer.set_period(1000)

screen_digital_clock_1_timer.set_cb(lambda src: screen_digital_clock_1_timerInstance.count_12(screen_digital_clock_1_timer))
lv.dclock.set_style_text_align(screen_digital_clock_1, lv.TEXT_ALIGN.CENTER, 0);

screen_digital_clock_1.set_pos(int(225),int(30))
screen_digital_clock_1.set_size(130,36)
screen_digital_clock_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# create style style_screen_digital_clock_1_main_main_default
style_screen_digital_clock_1_main_main_default = lv.style_t()
style_screen_digital_clock_1_main_main_default.init()
style_screen_digital_clock_1_main_main_default.set_radius(0)
style_screen_digital_clock_1_main_main_default.set_bg_color(lv.color_make(0x2F,0xCA,0xDA))
style_screen_digital_clock_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_digital_clock_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.VER)
style_screen_digital_clock_1_main_main_default.set_bg_opa(0)
style_screen_digital_clock_1_main_main_default.set_text_color(lv.color_make(0x21,0x95,0xf6))
try:
    style_screen_digital_clock_1_main_main_default.set_text_font(lv.font_montserratMedium_16)
except AttributeError:
    try:
        style_screen_digital_clock_1_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_digital_clock_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_digital_clock_1_main_main_default.set_text_letter_space(0)
style_screen_digital_clock_1_main_main_default.set_pad_left(0)
style_screen_digital_clock_1_main_main_default.set_pad_right(0)
style_screen_digital_clock_1_main_main_default.set_pad_top(7)
style_screen_digital_clock_1_main_main_default.set_pad_bottom(0)

# add style for screen_digital_clock_1
screen_digital_clock_1.add_style(style_screen_digital_clock_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_datetext_1
screen_datetext_1 = lv.label(screen_cont_1)
screen_datetext_1.set_text("2022/07/28")
screen_datetext_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, 0)

try:
    screen_datetext_1_calendar
except NameError:
    screen_datetext_1_calendar = lv.calendar(lv.layer_top())
    screen_datetext_1_calendar.set_size(240, 240)
    screen_datetext_1_calendar.set_pos(1,0)
    screen_datetext_1_calendar.set_showed_date(2022, 5)
    screen_datetext_1_calendar.align(lv.ALIGN.CENTER, 20, 20)
    lv.calendar_header_arrow(screen_datetext_1_calendar)

screen_datetext_1_calendar.add_flag(lv.obj.FLAG.HIDDEN)

def screen_datetext_1_calendar_event_handler(e, textV):
    code = e.get_code()
    obj = e.get_current_target()
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        obj.get_pressed_date(date) == lv.RES.OK
        textV.set_text("%02d/%02d/%02d"%(date.year, date.month, date.day))

def screen_datetext_1_event(e, date_ca):
    code = e.get_code()
    if code == lv.EVENT.FOCUSED:
        date_ca.add_event_cb(lambda e: screen_datetext_1_calendar_event_handler(e, screen_datetext_1), lv.EVENT.ALL, None)
        date_ca.move_foreground()
        date_ca.clear_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        date_ca.move_background()
        date_ca.add_flag(lv.obj.FLAG.HIDDEN)

screen_datetext_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_datetext_1.add_event_cb(lambda e: screen_datetext_1_event(e, screen_datetext_1_calendar), lv.EVENT.ALL, None)
screen_datetext_1.set_pos(int(100),int(30))
screen_datetext_1.set_size(130,36)
screen_datetext_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# create style style_screen_datetext_1_main_main_default
style_screen_datetext_1_main_main_default = lv.style_t()
style_screen_datetext_1_main_main_default.init()
style_screen_datetext_1_main_main_default.set_radius(0)
style_screen_datetext_1_main_main_default.set_bg_color(lv.color_make(0xff,0x00,0x27))
style_screen_datetext_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_datetext_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_datetext_1_main_main_default.set_bg_opa(0)
style_screen_datetext_1_main_main_default.set_text_color(lv.color_make(0x21,0x95,0xf6))
try:
    style_screen_datetext_1_main_main_default.set_text_font(lv.font_montserratMedium_16)
except AttributeError:
    try:
        style_screen_datetext_1_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_datetext_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_datetext_1_main_main_default.set_text_letter_space(0)
style_screen_datetext_1_main_main_default.set_pad_left(0)
style_screen_datetext_1_main_main_default.set_pad_right(0)
style_screen_datetext_1_main_main_default.set_pad_top(7)

# add style for screen_datetext_1
screen_datetext_1.add_style(style_screen_datetext_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_imgbtn_4
screen_imgbtn_4 = lv.imgbtn(screen_cont_1)
screen_imgbtn_4.set_pos(int(73),int(157))
screen_imgbtn_4.set_size(50,50)
screen_imgbtn_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp120866892.png','rb') as f:
        screen_imgbtn_4_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp120866892.png')
    sys.exit()

screen_imgbtn_4_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_imgbtn_4_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_imgbtn_4_imgReleased_data
})
screen_imgbtn_4.set_src(lv.imgbtn.STATE.RELEASED, None, screen_imgbtn_4_imgReleased, None)





screen_imgbtn_4.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_imgbtn_4_main_main_default
style_screen_imgbtn_4_main_main_default = lv.style_t()
style_screen_imgbtn_4_main_main_default.init()
style_screen_imgbtn_4_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_imgbtn_4_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_4_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_4_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_4_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_imgbtn_4_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_imgbtn_4_main_main_default.set_img_recolor_opa(0)
style_screen_imgbtn_4_main_main_default.set_img_opa(255)

# add style for screen_imgbtn_4
screen_imgbtn_4.add_style(style_screen_imgbtn_4_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_imgbtn_4_main_main_pressed
style_screen_imgbtn_4_main_main_pressed = lv.style_t()
style_screen_imgbtn_4_main_main_pressed.init()
style_screen_imgbtn_4_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_4_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_4_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_4_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_4_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_4_main_main_pressed.set_img_recolor_opa(0)
style_screen_imgbtn_4_main_main_pressed.set_img_opa(255)

# add style for screen_imgbtn_4
screen_imgbtn_4.add_style(style_screen_imgbtn_4_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_imgbtn_4_main_main_checked
style_screen_imgbtn_4_main_main_checked = lv.style_t()
style_screen_imgbtn_4_main_main_checked.init()
style_screen_imgbtn_4_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_4_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_4_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_4_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_4_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_4_main_main_checked.set_img_recolor_opa(0)
style_screen_imgbtn_4_main_main_checked.set_img_opa(255)

# add style for screen_imgbtn_4
screen_imgbtn_4.add_style(style_screen_imgbtn_4_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_imgbtn_1
screen_imgbtn_1 = lv.imgbtn(screen_cont_1)
screen_imgbtn_1.set_pos(int(215),int(156))
screen_imgbtn_1.set_size(50,50)
screen_imgbtn_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-1437957357.png','rb') as f:
        screen_imgbtn_1_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-1437957357.png')
    sys.exit()

screen_imgbtn_1_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_imgbtn_1_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_imgbtn_1_imgReleased_data
})
screen_imgbtn_1.set_src(lv.imgbtn.STATE.RELEASED, None, screen_imgbtn_1_imgReleased, None)





screen_imgbtn_1.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_imgbtn_1_main_main_default
style_screen_imgbtn_1_main_main_default = lv.style_t()
style_screen_imgbtn_1_main_main_default.init()
style_screen_imgbtn_1_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_imgbtn_1_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_1_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_1_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_imgbtn_1_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_imgbtn_1_main_main_default.set_img_recolor_opa(0)
style_screen_imgbtn_1_main_main_default.set_img_opa(255)

# add style for screen_imgbtn_1
screen_imgbtn_1.add_style(style_screen_imgbtn_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_imgbtn_1_main_main_pressed
style_screen_imgbtn_1_main_main_pressed = lv.style_t()
style_screen_imgbtn_1_main_main_pressed.init()
style_screen_imgbtn_1_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_1_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_1_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_1_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_1_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_1_main_main_pressed.set_img_recolor_opa(0)
style_screen_imgbtn_1_main_main_pressed.set_img_opa(255)

# add style for screen_imgbtn_1
screen_imgbtn_1.add_style(style_screen_imgbtn_1_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_imgbtn_1_main_main_checked
style_screen_imgbtn_1_main_main_checked = lv.style_t()
style_screen_imgbtn_1_main_main_checked.init()
style_screen_imgbtn_1_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_1_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_1_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_1_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_1_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_1_main_main_checked.set_img_recolor_opa(0)
style_screen_imgbtn_1_main_main_checked.set_img_opa(255)

# add style for screen_imgbtn_1
screen_imgbtn_1.add_style(style_screen_imgbtn_1_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_camera
screen_camera = lv.imgbtn(screen_cont_1)
screen_camera.set_pos(int(350),int(155))
screen_camera.set_size(50,50)
screen_camera.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-87095267.png','rb') as f:
        screen_camera_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-87095267.png')
    sys.exit()

screen_camera_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_camera_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_camera_imgReleased_data
})
screen_camera.set_src(lv.imgbtn.STATE.RELEASED, None, screen_camera_imgReleased, None)





screen_camera.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_camera_main_main_default
style_screen_camera_main_main_default = lv.style_t()
style_screen_camera_main_main_default.init()
style_screen_camera_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_camera_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_camera_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_camera_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_camera_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_camera_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_camera_main_main_default.set_img_recolor_opa(0)
style_screen_camera_main_main_default.set_img_opa(255)

# add style for screen_camera
screen_camera.add_style(style_screen_camera_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_camera_main_main_pressed
style_screen_camera_main_main_pressed = lv.style_t()
style_screen_camera_main_main_pressed.init()
style_screen_camera_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_camera_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_camera_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_camera_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_camera_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_camera_main_main_pressed.set_img_recolor_opa(0)
style_screen_camera_main_main_pressed.set_img_opa(255)

# add style for screen_camera
screen_camera.add_style(style_screen_camera_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_camera_main_main_checked
style_screen_camera_main_main_checked = lv.style_t()
style_screen_camera_main_main_checked.init()
style_screen_camera_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_camera_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_camera_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_camera_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_camera_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_camera_main_main_checked.set_img_recolor_opa(0)
style_screen_camera_main_main_checked.set_img_opa(255)

# add style for screen_camera
screen_camera.add_style(style_screen_camera_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_imgbtn_3
screen_imgbtn_3 = lv.imgbtn(screen_cont_1)
screen_imgbtn_3.set_pos(int(213),int(282))
screen_imgbtn_3.set_size(50,50)
screen_imgbtn_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp750771266.png','rb') as f:
        screen_imgbtn_3_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp750771266.png')
    sys.exit()

screen_imgbtn_3_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_imgbtn_3_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_imgbtn_3_imgReleased_data
})
screen_imgbtn_3.set_src(lv.imgbtn.STATE.RELEASED, None, screen_imgbtn_3_imgReleased, None)





screen_imgbtn_3.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_imgbtn_3_main_main_default
style_screen_imgbtn_3_main_main_default = lv.style_t()
style_screen_imgbtn_3_main_main_default.init()
style_screen_imgbtn_3_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_imgbtn_3_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_3_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_3_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_3_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_imgbtn_3_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_imgbtn_3_main_main_default.set_img_recolor_opa(0)
style_screen_imgbtn_3_main_main_default.set_img_opa(255)

# add style for screen_imgbtn_3
screen_imgbtn_3.add_style(style_screen_imgbtn_3_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_imgbtn_3_main_main_pressed
style_screen_imgbtn_3_main_main_pressed = lv.style_t()
style_screen_imgbtn_3_main_main_pressed.init()
style_screen_imgbtn_3_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_3_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_3_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_3_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_3_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_3_main_main_pressed.set_img_recolor_opa(0)
style_screen_imgbtn_3_main_main_pressed.set_img_opa(255)

# add style for screen_imgbtn_3
screen_imgbtn_3.add_style(style_screen_imgbtn_3_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_imgbtn_3_main_main_checked
style_screen_imgbtn_3_main_main_checked = lv.style_t()
style_screen_imgbtn_3_main_main_checked.init()
style_screen_imgbtn_3_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_3_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_3_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_3_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_3_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_3_main_main_checked.set_img_recolor_opa(0)
style_screen_imgbtn_3_main_main_checked.set_img_opa(255)

# add style for screen_imgbtn_3
screen_imgbtn_3.add_style(style_screen_imgbtn_3_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_imgbtn_5
screen_imgbtn_5 = lv.imgbtn(screen_cont_1)
screen_imgbtn_5.set_pos(int(351),int(280))
screen_imgbtn_5.set_size(50,50)
screen_imgbtn_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-118593594.png','rb') as f:
        screen_imgbtn_5_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp-118593594.png')
    sys.exit()

screen_imgbtn_5_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_imgbtn_5_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_imgbtn_5_imgReleased_data
})
screen_imgbtn_5.set_src(lv.imgbtn.STATE.RELEASED, None, screen_imgbtn_5_imgReleased, None)





screen_imgbtn_5.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_imgbtn_5_main_main_default
style_screen_imgbtn_5_main_main_default = lv.style_t()
style_screen_imgbtn_5_main_main_default.init()
style_screen_imgbtn_5_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_imgbtn_5_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_5_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_5_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_5_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_imgbtn_5_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_imgbtn_5_main_main_default.set_img_recolor_opa(0)
style_screen_imgbtn_5_main_main_default.set_img_opa(255)

# add style for screen_imgbtn_5
screen_imgbtn_5.add_style(style_screen_imgbtn_5_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_imgbtn_5_main_main_pressed
style_screen_imgbtn_5_main_main_pressed = lv.style_t()
style_screen_imgbtn_5_main_main_pressed.init()
style_screen_imgbtn_5_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_5_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_5_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_5_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_5_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_5_main_main_pressed.set_img_recolor_opa(0)
style_screen_imgbtn_5_main_main_pressed.set_img_opa(255)

# add style for screen_imgbtn_5
screen_imgbtn_5.add_style(style_screen_imgbtn_5_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_imgbtn_5_main_main_checked
style_screen_imgbtn_5_main_main_checked = lv.style_t()
style_screen_imgbtn_5_main_main_checked.init()
style_screen_imgbtn_5_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_5_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_5_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_5_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_5_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_5_main_main_checked.set_img_recolor_opa(0)
style_screen_imgbtn_5_main_main_checked.set_img_opa(255)

# add style for screen_imgbtn_5
screen_imgbtn_5.add_style(style_screen_imgbtn_5_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_btn_1
screen_btn_1 = lv.btn(screen_cont_1)
screen_btn_1.set_pos(int(190),int(663))
screen_btn_1.set_size(100,50)
screen_btn_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
screen_btn_1_label = lv.label(screen_btn_1)
screen_btn_1_label.set_text("Button")
screen_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_1_label.align(lv.ALIGN.CENTER,0,0)
# create style style_screen_btn_1_main_main_default
style_screen_btn_1_main_main_default = lv.style_t()
style_screen_btn_1_main_main_default.init()
style_screen_btn_1_main_main_default.set_radius(5)
style_screen_btn_1_main_main_default.set_bg_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_bg_grad_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_btn_1_main_main_default.set_bg_opa(255)
style_screen_btn_1_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_btn_1_main_main_default.set_border_width(0)
style_screen_btn_1_main_main_default.set_border_opa(255)
style_screen_btn_1_main_main_default.set_text_color(lv.color_make(0xff,0xff,0xff))
try:
    style_screen_btn_1_main_main_default.set_text_font(lv.font_montserratMedium_16)
except AttributeError:
    try:
        style_screen_btn_1_main_main_default.set_text_font(lv.font_montserrat_16)
    except AttributeError:
        style_screen_btn_1_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_btn_1_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)

# add style for screen_btn_1
screen_btn_1.add_style(style_screen_btn_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# create screen_imgbtn_2
screen_imgbtn_2 = lv.imgbtn(screen_cont_1)
screen_imgbtn_2.set_pos(int(72),int(285))
screen_imgbtn_2.set_size(50,50)
screen_imgbtn_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
try:
    with open('C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp1602476165.png','rb') as f:
        screen_imgbtn_2_imgReleased_data = f.read()
except:
    print('Could not open C:\\NXP\\GUI-Guider-Projects\\usb_demo\\generated\\mPythonImages\\mp1602476165.png')
    sys.exit()

screen_imgbtn_2_imgReleased = lv.img_dsc_t({
  'data_size': len(screen_imgbtn_2_imgReleased_data),
  'header': {'always_zero': 0, 'w': 50, 'h': 50, 'cf': lv.img.CF.TRUE_COLOR_ALPHA},
  'data': screen_imgbtn_2_imgReleased_data
})
screen_imgbtn_2.set_src(lv.imgbtn.STATE.RELEASED, None, screen_imgbtn_2_imgReleased, None)





screen_imgbtn_2.add_flag(lv.obj.FLAG.CHECKABLE)
# create style style_screen_imgbtn_2_main_main_default
style_screen_imgbtn_2_main_main_default = lv.style_t()
style_screen_imgbtn_2_main_main_default.init()
style_screen_imgbtn_2_main_main_default.set_text_color(lv.color_make(0x00,0x00,0x00))
try:
    style_screen_imgbtn_2_main_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_2_main_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_2_main_main_default.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_2_main_main_default.set_text_align(lv.TEXT_ALIGN.CENTER)
style_screen_imgbtn_2_main_main_default.set_img_recolor(lv.color_make(0xff,0xff,0xff))
style_screen_imgbtn_2_main_main_default.set_img_recolor_opa(0)
style_screen_imgbtn_2_main_main_default.set_img_opa(255)

# add style for screen_imgbtn_2
screen_imgbtn_2.add_style(style_screen_imgbtn_2_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_imgbtn_2_main_main_pressed
style_screen_imgbtn_2_main_main_pressed = lv.style_t()
style_screen_imgbtn_2_main_main_pressed.init()
style_screen_imgbtn_2_main_main_pressed.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_2_main_main_pressed.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_2_main_main_pressed.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_2_main_main_pressed.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_2_main_main_pressed.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_2_main_main_pressed.set_img_recolor_opa(0)
style_screen_imgbtn_2_main_main_pressed.set_img_opa(255)

# add style for screen_imgbtn_2
screen_imgbtn_2.add_style(style_screen_imgbtn_2_main_main_pressed, lv.PART.MAIN|lv.STATE.PRESSED)

# create style style_screen_imgbtn_2_main_main_checked
style_screen_imgbtn_2_main_main_checked = lv.style_t()
style_screen_imgbtn_2_main_main_checked.init()
style_screen_imgbtn_2_main_main_checked.set_text_color(lv.color_make(0xFF,0x33,0xFF))
try:
    style_screen_imgbtn_2_main_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_imgbtn_2_main_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_imgbtn_2_main_main_checked.set_text_font(lv.font_montserrat_16)
style_screen_imgbtn_2_main_main_checked.set_img_recolor(lv.color_make(0x00,0x00,0x00))
style_screen_imgbtn_2_main_main_checked.set_img_recolor_opa(0)
style_screen_imgbtn_2_main_main_checked.set_img_opa(255)

# add style for screen_imgbtn_2
screen_imgbtn_2.add_style(style_screen_imgbtn_2_main_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)


# create screen_menu_1
screen_menu_1 = lv.menu(screen_cont_1)
screen_menu_1.set_pos(int(77),int(422))
screen_menu_1.set_size(280,210)
screen_menu_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# create style style_screen_menu_1_extra_main_page_main_default
style_screen_menu_1_extra_main_page_main_default = lv.style_t()
style_screen_menu_1_extra_main_page_main_default.init()
style_screen_menu_1_extra_main_page_main_default.set_radius(0)
style_screen_menu_1_extra_main_page_main_default.set_bg_color(lv.color_make(0xda,0xf2,0xf8))
style_screen_menu_1_extra_main_page_main_default.set_bg_grad_color(lv.color_make(0x5a,0x61,0x73))
style_screen_menu_1_extra_main_page_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_menu_1_extra_main_page_main_default.set_bg_opa(255)


# create style style_screen_menu_1_extra_option_btns_main_default
style_screen_menu_1_extra_option_btns_main_default = lv.style_t()
style_screen_menu_1_extra_option_btns_main_default.init()
style_screen_menu_1_extra_option_btns_main_default.set_text_color(lv.color_make(0x15,0x12,0x12))
try:
    style_screen_menu_1_extra_option_btns_main_default.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_menu_1_extra_option_btns_main_default.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_menu_1_extra_option_btns_main_default.set_text_font(lv.font_montserrat_16)


# create style style_screen_menu_1_extra_option_btns_main_checked
style_screen_menu_1_extra_option_btns_main_checked = lv.style_t()
style_screen_menu_1_extra_option_btns_main_checked.init()
style_screen_menu_1_extra_option_btns_main_checked.set_radius(0)
style_screen_menu_1_extra_option_btns_main_checked.set_bg_color(lv.color_make(0x00,0xe0,0xb8))
style_screen_menu_1_extra_option_btns_main_checked.set_bg_grad_color(lv.color_make(0x32,0x9d,0xf6))
style_screen_menu_1_extra_option_btns_main_checked.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_menu_1_extra_option_btns_main_checked.set_bg_opa(60)
style_screen_menu_1_extra_option_btns_main_checked.set_border_color(lv.color_make(0x32,0x9d,0xf6))
style_screen_menu_1_extra_option_btns_main_checked.set_border_width(0)
style_screen_menu_1_extra_option_btns_main_checked.set_border_opa(255)
style_screen_menu_1_extra_option_btns_main_checked.set_border_side(lv.BORDER_SIDE.BOTTOM)
style_screen_menu_1_extra_option_btns_main_checked.set_text_color(lv.color_make(0x9a,0xb7,0x00))
try:
    style_screen_menu_1_extra_option_btns_main_checked.set_text_font(lv.font_montserratMedium_12)
except AttributeError:
    try:
        style_screen_menu_1_extra_option_btns_main_checked.set_text_font(lv.font_montserrat_12)
    except AttributeError:
        style_screen_menu_1_extra_option_btns_main_checked.set_text_font(lv.font_montserrat_16)



screen_menu_1_main_page = lv.menu_page(screen_menu_1, None)



screen_menu_1_sub_page0 = lv.menu_page(screen_menu_1, None)


# add style for screen_menu_1_main_page
screen_menu_1_main_page.add_style(style_screen_menu_1_extra_main_page_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_menu_1_menu_cont0 = lv.menu_cont(screen_menu_1_main_page)

# add style for screen_menu_1_menu_cont0
screen_menu_1_menu_cont0.add_style(style_screen_menu_1_extra_option_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# add style for screen_menu_1_menu_cont0
screen_menu_1_menu_cont0.add_style(style_screen_menu_1_extra_option_btns_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)

screen_menu_1_menu_label0 = lv.label(screen_menu_1_menu_cont0)
screen_menu_1_menu_label0.set_text("menu_4")
screen_menu_1.set_load_page_event(screen_menu_1_menu_cont0, screen_menu_1_sub_page0)

screen_menu_1_sub_page1 = lv.menu_page(screen_menu_1, None)


# add style for screen_menu_1_menu_cont0
screen_menu_1_menu_cont0.add_style(style_screen_menu_1_extra_option_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# add style for screen_menu_1_menu_cont0
screen_menu_1_menu_cont0.add_style(style_screen_menu_1_extra_option_btns_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)

screen_menu_1_menu_cont1 = lv.menu_cont(screen_menu_1_main_page)

# add style for screen_menu_1_menu_cont1
screen_menu_1_menu_cont1.add_style(style_screen_menu_1_extra_option_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# add style for screen_menu_1_menu_cont1
screen_menu_1_menu_cont1.add_style(style_screen_menu_1_extra_option_btns_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)

screen_menu_1_menu_label1 = lv.label(screen_menu_1_menu_cont1)
screen_menu_1_menu_label1.set_text("menu_3")
screen_menu_1.set_load_page_event(screen_menu_1_menu_cont1, screen_menu_1_sub_page1)

screen_menu_1_sub_page2 = lv.menu_page(screen_menu_1, None)


# add style for screen_menu_1_menu_cont1
screen_menu_1_menu_cont1.add_style(style_screen_menu_1_extra_option_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# add style for screen_menu_1_menu_cont1
screen_menu_1_menu_cont1.add_style(style_screen_menu_1_extra_option_btns_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)

screen_menu_1_menu_cont2 = lv.menu_cont(screen_menu_1_main_page)

# add style for screen_menu_1_menu_cont2
screen_menu_1_menu_cont2.add_style(style_screen_menu_1_extra_option_btns_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)


# add style for screen_menu_1_menu_cont2
screen_menu_1_menu_cont2.add_style(style_screen_menu_1_extra_option_btns_main_checked, lv.PART.MAIN|lv.STATE.CHECKED)

screen_menu_1_menu_label2 = lv.label(screen_menu_1_menu_cont2)
screen_menu_1_menu_label2.set_text("menu_2")
screen_menu_1.set_load_page_event(screen_menu_1_menu_cont2, screen_menu_1_sub_page2)


screen_menu_1.set_sidebar_page(screen_menu_1_main_page)

# create style style_screen_menu_1_main_main_default
style_screen_menu_1_main_main_default = lv.style_t()
style_screen_menu_1_main_main_default.init()
style_screen_menu_1_main_main_default.set_radius(10)
style_screen_menu_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_menu_1_main_main_default.set_bg_grad_color(lv.color_make(0x41,0x48,0x5a))
style_screen_menu_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_menu_1_main_main_default.set_bg_opa(255)

# add style for screen_menu_1
screen_menu_1.add_style(style_screen_menu_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_menu_1_main_main_default
style_screen_menu_1_main_main_default = lv.style_t()
style_screen_menu_1_main_main_default.init()
style_screen_menu_1_main_main_default.set_radius(10)
style_screen_menu_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_menu_1_main_main_default.set_bg_grad_color(lv.color_make(0x41,0x48,0x5a))
style_screen_menu_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_menu_1_main_main_default.set_bg_opa(255)

# add style for screen_menu_1
screen_menu_1.add_style(style_screen_menu_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)

# create style style_screen_cont_1_main_main_default
style_screen_cont_1_main_main_default = lv.style_t()
style_screen_cont_1_main_main_default.init()
style_screen_cont_1_main_main_default.set_radius(0)
style_screen_cont_1_main_main_default.set_bg_color(lv.color_make(0xff,0xff,0xff))
style_screen_cont_1_main_main_default.set_bg_grad_color(lv.color_make(0xff,0xff,0xff))
style_screen_cont_1_main_main_default.set_bg_grad_dir(lv.GRAD_DIR.NONE)
style_screen_cont_1_main_main_default.set_bg_opa(255)
style_screen_cont_1_main_main_default.set_border_color(lv.color_make(0x21,0x95,0xf6))
style_screen_cont_1_main_main_default.set_border_width(2)
style_screen_cont_1_main_main_default.set_border_opa(255)
style_screen_cont_1_main_main_default.set_pad_left(0)
style_screen_cont_1_main_main_default.set_pad_right(0)
style_screen_cont_1_main_main_default.set_pad_top(0)
style_screen_cont_1_main_main_default.set_pad_bottom(0)

# add style for screen_cont_1
screen_cont_1.add_style(style_screen_cont_1_main_main_default, lv.PART.MAIN|lv.STATE.DEFAULT)




# content from custom.py

# Load the default screen
lv.scr_load(screen)

while SDL.check():
    time.sleep_ms(5)
