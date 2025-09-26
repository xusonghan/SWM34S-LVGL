/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: MIT
 *
 * Change Logs:
 * Date           Author        Notes
 * 2022-05-13     Meco Man      First version
 */

#ifdef __RTTHREAD__

#include <lvgl.h>

/* ������������ SquareLine ���ɵ� ui_init����������������ʽ���ݵ��ã����������Թ� */
#if defined(__clang__) || defined(__CC_ARM) || defined(__ARMCC_VERSION)
__attribute__((weak)) void ui_init(void);
#else
void ui_init(void) __attribute__((weak));
#endif

/* �� UI ���Կؼ���� */
static lv_obj_t *g_label_title = NULL;
static lv_obj_t *g_label_value = NULL;

static void btn_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED && g_label_title)
    {
        static bool toggled = false;
        toggled = !toggled;
        lv_label_set_text(g_label_title, toggled ? "Button Clicked" : "Hello LVGL");
    }
}

static void slider_event_cb(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED && g_label_value)
    {
        lv_obj_t *slider = lv_event_get_target(e);
        int32_t v = lv_slider_get_value(slider);
        char buf[24];
        lv_snprintf(buf, sizeof(buf), "Value: %d", (int)v);
        lv_label_set_text(g_label_value, buf);
    }
}

void lv_user_gui_init(void)
{
    /* ����һ�������ǩ */
    g_label_title = lv_label_create(lv_scr_act());
    lv_label_set_text(g_label_title, "Hello LVGL");
    lv_obj_align(g_label_title, LV_ALIGN_TOP_MID, 0, 8);

    /* �������� + ��ֵ��ǩ */
    lv_obj_t *slider = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider, 220);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, -10);
    lv_slider_set_range(slider, 0, 100);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    g_label_value = lv_label_create(lv_scr_act());
    lv_label_set_text(g_label_value, "Value: 0");
    lv_obj_align_to(g_label_value, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    /* ������ť */
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click me");
    lv_obj_center(btn_label);

    /* ������ SquareLine ���ɵ� UI�������Բ�ؼ�֮��������أ���ѡ�� */
    if (ui_init)
    {
        ui_init();
    }


}

#endif /* __RTTHREAD__ */
