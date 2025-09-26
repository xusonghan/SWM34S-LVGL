/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include <stdbool.h>
#include <string.h>
#include <rtthread.h>
#include <rtdevice.h>
#ifdef SWM341
#include "SWM341.h" /* 若需要直接访问 LCD 寄存器 */
#endif

/*********************
 *      DEFINES
 *********************/

#define MY_DISP_HOR_RES    480

#define MY_DISP_VER_RES    272


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//        const lv_area_t * fill_area, lv_color_t color);

/**********************
 *  STATIC VARIABLES
 **********************/
/* RT-Thread 图形设备与帧缓冲信息 */
static rt_device_t lcd_dev = RT_NULL;
static struct rt_device_graphic_info lcd_info = {0};
static lv_color_t *fb = RT_NULL; /* 指向 RGB565 帧缓冲（与 LVGL lv_color_t 对齐） */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*-----------------------------
     * Create a buffer for drawing
     *----------------------------*/

    /**
     * LVGL requires a buffer where it internally draws the widgets.
     * Later this buffer will passed to your display driver's `flush_cb` to copy its content to your display.
     * The buffer has to be greater than 1 display row
     *
     * There are 3 buffering configurations:
     * 1. Create ONE buffer:
     *      LVGL will draw the display's content here and writes it to your display
     *
     * 2. Create TWO buffer:
     *      LVGL will draw the display's content to a buffer and writes it your display.
     *      You should use DMA to write the buffer's content to the display.
     *      It will enable LVGL to draw the next part of the screen to the other buffer while
     *      the data is being sent form the first buffer. It makes rendering and flushing parallel.
     *
     * 3. Double buffering
     *      Set 2 screens sized buffers and set disp_drv.full_refresh = 1.
     *      This way LVGL will always provide the whole rendered screen in `flush_cb`
     *      and you only need to change the frame buffer's address.
     */

    /* 使用方案3：双全屏缓冲 + full_refresh；缓冲区放入 .SDRAM1 */
    static lv_disp_draw_buf_t draw_buf_dsc_fb;
    static LV_ATTRIBUTE_LARGE_RAM_ARRAY lv_color_t fb0[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    static LV_ATTRIBUTE_LARGE_RAM_ARRAY lv_color_t fb1[MY_DISP_HOR_RES * MY_DISP_VER_RES];
    lv_disp_draw_buf_init(&draw_buf_dsc_fb, fb0, fb1, MY_DISP_HOR_RES * MY_DISP_VER_RES);

    // /* Example for 2) */
    // static lv_disp_draw_buf_t draw_buf_dsc_2;
    // static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];                        /*A buffer for 10 rows*/
    // static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];                        /*An other buffer for 10 rows*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_2, buf_2_1, buf_2_2, MY_DISP_HOR_RES * 10);   /*Initialize the display buffer*/

    // /* Example for 3) also set disp_drv.full_refresh = 1 below*/
    // static lv_disp_draw_buf_t draw_buf_dsc_3;
    // static lv_color_t buf_3_1[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*A screen sized buffer*/
    // static lv_color_t buf_3_2[MY_DISP_HOR_RES * MY_DISP_VER_RES];            /*Another screen sized buffer*/
    // lv_disp_draw_buf_init(&draw_buf_dsc_3, buf_3_1, buf_3_2,
    //                       MY_DISP_VER_RES * LV_VER_RES_MAX);   /*Initialize the display buffer*/

    /*-----------------------------------
     * Register the display in LVGL
     *----------------------------------*/

    static lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/

    /*Set the resolution of the display*/
    /* 优先使用底层驱动报告的分辨率，回退到宏 */
    disp_drv.hor_res = lcd_info.width  ? (lv_coord_t)lcd_info.width  : MY_DISP_HOR_RES;
    disp_drv.ver_res = lcd_info.height ? (lv_coord_t)lcd_info.height : MY_DISP_VER_RES;

    /*Used to copy the buffer's content to the display*/
    disp_drv.flush_cb = disp_flush;

    /*Set a display buffer*/
    disp_drv.draw_buf = &draw_buf_dsc_fb;

    /*Required for Example 3)*/
    disp_drv.full_refresh = 1; /* LVGL 每帧提供整屏缓冲，直接切换 LCD 显存地址 */

    /* Fill a memory array with a color if you have GPU.
     * Note that, in lv_conf.h you can enable GPUs that has built-in support in LVGL.
     * But if you have a different GPU you can use with this callback.*/
    //disp_drv.gpu_fill_cb = gpu_fill;

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
    /* 绑定 RT-Thread 的 rgb_lcd 图形设备，获取帧缓冲 */
    lcd_dev = rt_device_find("rgb_lcd");
    if (lcd_dev == RT_NULL)
    {
        /* 找不到设备则保持软件回退模式（逐像素写） */
        return;
    }

    /* 读写打开设备（有些驱动需要） */
    if (rt_device_open(lcd_dev, RT_DEVICE_OFLAG_RDWR) != RT_EOK)
    {
        return;
    }

    /* 获取图形参数：分辨率与帧缓冲指针 */
    if (rt_device_control(lcd_dev, RTGRAPHIC_CTRL_GET_INFO, &lcd_info) == RT_EOK)
    {
        fb = (lv_color_t *)lcd_info.framebuffer; /* 假定 LV_COLOR_DEPTH=16 且 RGB565 小端，直接复用 */
    }
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

static inline void put_px(int16_t x, int16_t y, lv_color_t color)
{
    /* 若已拿到帧缓冲，直接写显存；否则保持为空（由 flush 的软件回退路径处理） */
    if (fb &&
            (x >= 0) && (y >= 0) &&
            ((uint16_t)x < (lcd_info.width  ? lcd_info.width  : MY_DISP_HOR_RES)) &&
            ((uint16_t)y < (lcd_info.height ? lcd_info.height : MY_DISP_VER_RES)))
    {
        uint32_t stride = lcd_info.width ? lcd_info.width : MY_DISP_HOR_RES;
        fb[(uint32_t)y * stride + (uint32_t)x] = color;
    }
}

/*Flush the content of the internal buffer the specific area on the display
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_disp_flush_ready()' has to be called when finished.*/
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (!disp_flush_enabled)
    {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    /* full_refresh=1 时 LVGL 始终给整屏区域，可直接把 color_p 作为新帧地址切换 */
    if ((area->x1 == 0) && (area->y1 == 0) &&
        (area->x2 == (disp_drv->hor_res - 1)) &&
        (area->y2 == (disp_drv->ver_res - 1)))
    {
#if defined(SWM341) && defined(LCD)
        /* 同步模式：切换寄存器并等待生效，再通知 LVGL */
        extern int rgb_lcd_set_framebuffer(void *addr, int wait_vsync);
        rgb_lcd_set_framebuffer(color_p, 1); /* 等待 VSYNC 翻转完成 */
#else
        if (fb && fb != color_p)
        {
            size_t bytes = (size_t)disp_drv->hor_res * (size_t)disp_drv->ver_res * sizeof(lv_color_t);
            memcpy(fb, color_p, bytes);
        }
        struct rt_device_rect_info full = {0};
        full.width = (rt_uint16_t)disp_drv->hor_res;
        full.height = (rt_uint16_t)disp_drv->ver_res;
        (void)rt_device_control(lcd_dev, RTGRAPHIC_CTRL_RECT_UPDATE, &full);
#endif
    }
    else
    {
        if (fb)
        {
            uint32_t stride = lcd_info.width ? lcd_info.width : MY_DISP_HOR_RES;
            uint32_t w = (uint32_t)(area->x2 - area->x1 + 1);
            for (int32_t y = area->y1; y <= area->y2; y++)
            {
                lv_color_t *dst = fb + (uint32_t)y * stride + (uint32_t)area->x1;
                memcpy(dst, color_p, w * sizeof(lv_color_t));
                color_p += w;
            }
        }
    }

    lv_disp_flush_ready(disp_drv);
}

/*OPTIONAL: GPU INTERFACE*/

/*If your MCU has hardware accelerator (GPU) then you can use it to fill a memory with a color*/
//static void gpu_fill(lv_disp_drv_t * disp_drv, lv_color_t * dest_buf, lv_coord_t dest_width,
//                    const lv_area_t * fill_area, lv_color_t color)
//{
//    /*It's an example code which should be done by your GPU*/
//    int32_t x, y;
//    dest_buf += dest_width * fill_area->y1; /*Go to the first line*/
//
//    for(y = fill_area->y1; y <= fill_area->y2; y++) {
//        for(x = fill_area->x1; x <= fill_area->x2; x++) {
//            dest_buf[x] = color;
//        }
//        dest_buf+=dest_width;    /*Go to the next line*/
//    }
//}


#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
