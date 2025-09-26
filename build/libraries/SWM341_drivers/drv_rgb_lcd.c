/*
 * Copyright (c) 2006-2022, Synwit Technology Co.,Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2022-02-25     lik          first version
 */

#include "drv_rgb_lcd.h"
#include "SWM341.h"

#ifdef BSP_USING_RGB_LCD
#include "rgb_lcd_port.h"

#define DRV_DEBUG
#define LOG_TAG "drv.rgb_lcd"
#include <drv_log.h>

#define LCD_DEVICE(dev) (struct swm_rgb_lcd_device *)(dev)

struct swm_rgb_lcd_device rgb_lcd_obj;

/* 初始单缓冲（若上层没提供双缓冲仍可工作） */
static uint8_t lcd_fb[LCD_BUF_SIZE] __attribute__((aligned(4), section(".SDRAM1")));
static int lcd_inited = 0;
/* 若未启用 LVGL，提前做一个最小前向声明避免编译错误 */
#ifndef PKG_USING_LVGL
typedef struct _lv_disp_drv_t lv_disp_drv_t;
#endif
/* 不再使用异步 VSYNC 回调，回退为同步等待 */

static rt_err_t swm_rgb_lcd_configure(struct rt_device *device)
{
    struct swm_rgb_lcd_device *rgb_lcd = LCD_DEVICE(device);
    /* nothing, right now */
    rgb_lcd = rgb_lcd;
    return RT_EOK;
}

static rt_err_t swm_rgb_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct swm_rgb_lcd_device *rgb_lcd = LCD_DEVICE(device);

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format = rgb_lcd->lcd_info.pixel_format;
        info->bits_per_pixel = rgb_lcd->lcd_info.bits_per_pixel;
        info->width = rgb_lcd->lcd_info.width;
        info->height = rgb_lcd->lcd_info.height;
        info->framebuffer = rgb_lcd->lcd_info.framebuffer;
    }
    break;

    default:
        return -RT_EINVAL;
    }

    return RT_EOK;
}

#if defined(LCD_BACKLIGHT_USING_PWM)
static void turn_on_lcd_backlight(void)
{
    struct rt_device_pwm *pwm_dev;

    /* turn on the LCD backlight */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(LCD_PWM_DEV_NAME);
    /* pwm frequency:100K = 10000ns */
    rt_pwm_set(pwm_dev, LCD_PWM_DEV_CHANNEL, 10000, 10000);
    rt_pwm_enable(pwm_dev, LCD_PWM_DEV_CHANNEL);
}
#elif defined(LCD_BACKLIGHT_USING_GPIO)
static void turn_on_lcd_backlight(void)
{
    rt_pin_mode(LCD_DISP_GPIO, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_DISP_GPIO, PIN_HIGH);

    rt_pin_mode(LCD_BL_GPIO, PIN_MODE_OUTPUT);
    rt_pin_write(LCD_BL_GPIO, PIN_HIGH);
}
#else
static void turn_on_lcd_backlight(void)
{
}
#endif

#ifdef RT_USING_DEVICE_OPS
static const struct rt_device_ops swm_lcd_ops =
    {
        .init = swm_rgb_lcd_configure,
        .open = RT_NULL,
        .close = RT_NULL,
        .read = RT_NULL,
        .write = RT_NULL,
        .control = swm_rgb_lcd_control};
#endif

int swm_rgb_lcd_init(void)
{
    rt_err_t result = RT_EOK;
    struct rt_device *device = &rgb_lcd_obj.parent;

    /* memset rgb_lcd_obj to zero */
    memset(&rgb_lcd_obj, 0x00, sizeof(rgb_lcd_obj));

    /* config LCD dev info */
    rgb_lcd_obj.lcd_info.height = LCD_HEIGHT;
    rgb_lcd_obj.lcd_info.width = LCD_WIDTH;
    rgb_lcd_obj.lcd_info.bits_per_pixel = LCD_BITS_PER_PIXEL;
    rgb_lcd_obj.lcd_info.pixel_format = LCD_PIXEL_FORMAT;

    /* use static framebuffer in SDRAM1 */
    rgb_lcd_obj.lcd_info.framebuffer = lcd_fb; /* 可被后续 rgb_lcd_set_framebuffer 切换 */
    /* memset buff to 0xFF */
    memset(rgb_lcd_obj.lcd_info.framebuffer, 0xFF, LCD_BUF_SIZE);

    device->type = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops = &swm_lcd_ops;
#else
    device->init = swm_rgb_lcd_configure;
    device->control = swm_rgb_lcd_control;
#endif
    /* register lcd device */
    rt_device_register(device, "rgb_lcd", RT_DEVICE_FLAG_RDWR);

    LCD_InitStructure LCD_initStruct;

    // PORT_Init(PORTB, PIN1, PORTB_PIN1_LCD_B0, 0);
    // PORT_Init(PORTB, PIN11, PORTB_PIN11_LCD_B1, 0);
    // PORT_Init(PORTB, PIN13, PORTB_PIN13_LCD_B2, 0);
    PORT_Init(PORTB, PIN15, PORTB_PIN15_LCD_B3, 0);
    PORT_Init(PORTA, PIN2, PORTA_PIN2_LCD_B4, 0);
    PORT_Init(PORTA, PIN9, PORTA_PIN9_LCD_B5, 0);
    PORT_Init(PORTA, PIN10, PORTA_PIN10_LCD_B6, 0);
    PORT_Init(PORTA, PIN11, PORTA_PIN11_LCD_B7, 0);

    // PORT_Init(PORTA, PIN12, PORTA_PIN12_LCD_G0, 0);
    // PORT_Init(PORTA, PIN13, PORTA_PIN13_LCD_G1, 0);
    PORT_Init(PORTA, PIN14, PORTA_PIN14_LCD_G2, 0);
    PORT_Init(PORTA, PIN15, PORTA_PIN15_LCD_G3, 0);
    PORT_Init(PORTC, PIN0, PORTC_PIN0_LCD_G4, 0);
    PORT_Init(PORTC, PIN1, PORTC_PIN1_LCD_G5, 0);
    PORT_Init(PORTC, PIN2, PORTC_PIN2_LCD_G6, 0);
    PORT_Init(PORTC, PIN3, PORTC_PIN3_LCD_G7, 0);

    // PORT_Init(PORTC, PIN4, PORTC_PIN4_LCD_R0, 0);
    // PORT_Init(PORTC, PIN5, PORTC_PIN5_LCD_R1, 0);
    // PORT_Init(PORTC, PIN8, PORTC_PIN8_LCD_R2, 0);
    PORT_Init(PORTC, PIN9, PORTC_PIN9_LCD_R3, 0);
    PORT_Init(PORTC, PIN10, PORTC_PIN10_LCD_R4, 0);
    PORT_Init(PORTC, PIN11, PORTC_PIN11_LCD_R5, 0);
    PORT_Init(PORTC, PIN12, PORTC_PIN12_LCD_R6, 0);
    PORT_Init(PORTC, PIN13, PORTC_PIN13_LCD_R7, 0);

    PORT_Init(PORTB, PIN2, PORTB_PIN2_LCD_VSYNC, 0);
    PORT_Init(PORTM, PIN8, PORTB_PIN3_LCD_HSYNC, 0);
    PORT_Init(PORTM, PIN11, PORTB_PIN4_LCD_DEN, 0);
    PORT_Init(PORTB, PIN5, PORTB_PIN5_LCD_DCLK, 0);

    LCD_initStruct.ClkDiv = LCD_CLK_DIV;
    LCD_initStruct.Format = ((LCD_BITS_PER_PIXEL == 16) ? LCD_FMT_RGB565 : LCD_FMT_RGB888);
    LCD_initStruct.HnPixel = LCD_WIDTH;
    LCD_initStruct.VnPixel = LCD_HEIGHT;
    LCD_initStruct.Hfp = LCD_HFP;
    LCD_initStruct.Hbp = LCD_HBP;
    LCD_initStruct.Vfp = LCD_VFP;
    LCD_initStruct.Vbp = LCD_VBP;
    LCD_initStruct.HsyncWidth = LCD_HSYNC_WIDTH;
    LCD_initStruct.VsyncWidth = LCD_VSYNC_WIDTH;

    LCD_initStruct.DataSource = (uint32_t)rgb_lcd_obj.lcd_info.framebuffer;
    LCD_initStruct.Background = 0xFFFFFF;
    LCD_initStruct.SampleEdge = LCD_SAMPLE_FALL;
    LCD_initStruct.IntEOTEn = 0; /* 关闭中断，使用同步等待 */
    LCD_Init(LCD, &LCD_initStruct);
    LCD_Start(LCD);
    lcd_inited = 1;
    turn_on_lcd_backlight();
__exit:
    return result;
}
INIT_DEVICE_EXPORT(swm_rgb_lcd_init);

int rgb_lcd_set_framebuffer(void *addr, int wait_vsync)
{
    if(!lcd_inited || addr == RT_NULL) return -RT_ERROR;
    LCD->L[0].ADDR = (uint32_t)addr;
    LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
    if(wait_vsync)
    {
        while(LCD->CR & LCD_CR_VBPRELOAD_Msk) __NOP();
    }
    rgb_lcd_obj.lcd_info.framebuffer = addr; /* 立即生效（已等待或不等待） */
    return RT_EOK;
}

int lcd_test(void)
{
    struct swm_rgb_lcd_device *rgb_lcd;
    rgb_lcd = (struct swm_rgb_lcd_device *)rt_device_find("rgb_lcd");

    // while (1)
    {
        /* red */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            rgb_lcd->lcd_info.framebuffer[2 * i] = 0x00;
            rgb_lcd->lcd_info.framebuffer[2 * i + 1] = 0xF8;
        }
        rgb_lcd->parent.control(&rgb_lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
        /* green */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            rgb_lcd->lcd_info.framebuffer[2 * i] = 0xE0;
            rgb_lcd->lcd_info.framebuffer[2 * i + 1] = 0x07;
        }
        rgb_lcd->parent.control(&rgb_lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
        /* blue */
        for (int i = 0; i < LCD_BUF_SIZE / 2; i++)
        {
            rgb_lcd->lcd_info.framebuffer[2 * i] = 0x1F;
            rgb_lcd->lcd_info.framebuffer[2 * i + 1] = 0x00;
        }
        rgb_lcd->parent.control(&rgb_lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
    }
    return 0;
}
MSH_CMD_EXPORT(lcd_test, lcd_test);

#endif /* BSP_USING_RGB_LCD */
