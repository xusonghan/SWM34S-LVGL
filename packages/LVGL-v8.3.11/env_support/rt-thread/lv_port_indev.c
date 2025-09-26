/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "../../lvgl.h"
#include <rtthread.h>
#include <drivers/dev_touch.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;

/* GT9147 设备句柄及缓存 */
static rt_device_t tp_dev = RT_NULL;
static struct rt_touch_data tp_points[5]; /* GT9147 最多 5 点 */
static uint8_t tp_point_count = 0;
static volatile uint8_t tp_data_ready = 0;  /* 有新数据标志 */
static lv_coord_t last_x = 0;
static lv_coord_t last_y = 0;
static uint8_t tp_pressed = 0; /* 0: 未按下, 1: 按下 */

/* 可选: 若需要做抖动/超时释放，可记录上次事件 tick */
static rt_tick_t last_event_tick = 0;
#define TP_RELEASE_TIMEOUT_MS  80  /* 更短的释放超时，提升快速点击响应 */
/* 可选中断回调：若以后改为中断打开，可在回调里设置 tp_data_ready=1 */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /**
     * Here you will find example implementation of input devices supported by LittelvGL:
     *  - Touchpad
     *  - Mouse (with cursor support)
     *  - Keypad (supports GUI usage only with key)
     *  - Encoder (supports GUI usage only with: left, right, push)
     *  - Button (external buttons to press points on the screen)
     *
     *  The `..._read()` function are only examples.
     *  You should shape them according to your hardware
     */

    static lv_indev_drv_t indev_drv;

    /* 仅注册触摸输入设备 */
    touchpad_init();
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
static void touchpad_init(void)
{
    tp_dev = rt_device_find("gt9147");
    if(!tp_dev) {
        rt_kprintf("[lv_indev] gt9147 device not found\n");
        return;
    }
    /* 读模式打开; 若需中断可改为 RT_DEVICE_FLAG_INT_RX 并设置回调 */
    if(rt_device_open(tp_dev, RT_DEVICE_FLAG_RDONLY) != RT_EOK) {
        rt_kprintf("[lv_indev] open gt9147 failed\n");
        tp_dev = RT_NULL;
        return;
    }
    /* 可选：获取分辨率信息 (若驱动支持控制命令) */
    // struct rt_touch_info info; rt_device_control(tp_dev, RT_TOUCH_CTRL_GET_INFO, &info);
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) 
{
    LV_UNUSED(indev_drv);
    if(tp_dev) {
        /* 只有在上一帧消费完或需要更新时读，减少无效读取 */
        int readn = rt_device_read(tp_dev, 0, tp_points, 5);
        if(readn > 0) tp_data_ready = 1; /* 有新原始数据 */

        if(tp_data_ready) {
            int idx = -1;
            for(int i=0;i<5;i++) {
                if(tp_points[i].event == RT_TOUCH_EVENT_DOWN || tp_points[i].event == RT_TOUCH_EVENT_MOVE) { idx = i; break; }
                if(tp_points[i].event == RT_TOUCH_EVENT_NONE) break;
            }
            if(idx >= 0) {
                last_x = (lv_coord_t)tp_points[idx].x_coordinate;
                last_y = (lv_coord_t)tp_points[idx].y_coordinate;
                tp_pressed = 1;
                last_event_tick = rt_tick_get();
            } else {
                /* 查找 UP */
                for(int i=0;i<5;i++) {
                    if(tp_points[i].event == RT_TOUCH_EVENT_UP) { tp_pressed = 0; last_event_tick = rt_tick_get(); break; }
                    if(tp_points[i].event == RT_TOUCH_EVENT_NONE) break;
                }
            }
            /* 消费一次 */
            tp_data_ready = 0;
        } else {
            /* 没新数据时做释放超时判断 */
            if(tp_pressed) {
                rt_tick_t now = rt_tick_get();
                if(rt_tick_from_millisecond(TP_RELEASE_TIMEOUT_MS) < (now - last_event_tick)) tp_pressed = 0;
            }
        }
    }

    if(tp_pressed) data->state = LV_INDEV_STATE_PR; else data->state = LV_INDEV_STATE_REL;
    data->point.x = last_x;
    data->point.y = last_y;
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    return tp_pressed != 0;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    *x = last_x;
    *y = last_y;
}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
static void mouse_init(void) { }

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) { LV_UNUSED(indev_drv); data->state = LV_INDEV_STATE_REL; data->point.x = last_x; data->point.y = last_y; }

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void) { return false; }

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y) { *x = last_x; *y = last_y; }

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void) { }

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) { LV_UNUSED(indev_drv); data->state = LV_INDEV_STATE_REL; data->key = 0; }

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void) { return 0; }

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void) { }

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) { LV_UNUSED(indev_drv); data->enc_diff = 0; data->state = LV_INDEV_STATE_REL; }

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void) { }

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void) { }

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) { LV_UNUSED(indev_drv); data->state = LV_INDEV_STATE_REL; data->btn_id = 0; }

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void) { return -1; }

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id) { LV_UNUSED(id); return false; }

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
