#include "page2.h"
#include "pm.h" /* for lv_pm_open_options_t */

#include <stdio.h>

static lv_obj_t *p2_btn[4];
static uint8_t p2_created = 0;

/* 按钮事件回调：P2-1 -> 打开 page1 */
static void p2_btn_1_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_CLICKED)
    {
        extern lv_pm_open_options_t options;
        options.target = LV_PM_TARGET_NEW;
        lv_pm_open_page(0, &options);
    }
}

void page_page2_onLoad(lv_obj_t *page)
{
    printf("lifecycle: page2 onLoad\n");
    lv_obj_set_style_bg_color(page, lv_color_make(20, 40, 120), 0);

    lv_coord_t W = lv_obj_get_width(page);
    lv_coord_t H = lv_obj_get_height(page);
    if (W <= 0 || H <= 0)
    {
        lv_coord_t disp_w = lv_disp_get_hor_res(NULL);
        lv_coord_t disp_h = lv_disp_get_ver_res(NULL);
        if (W <= 0) W = disp_w;
        if (H <= 0) H = disp_h;
        printf("page2 fallback size W=%d H=%d\n", W, H);
    }
    lv_coord_t margin = 10;
    lv_coord_t bw = 120;
    lv_coord_t bh = 60;
    if (W > 0 && bw > W / 2) bw = W / 2;
    if (H > 0 && bh > H / 2) bh = H / 2;

    if (!p2_created)
    {
        p2_btn[0] = lv_btn_create(page);
        lv_obj_set_size(p2_btn[0], bw, bh);
        lv_obj_set_pos(p2_btn[0], margin, margin);
        lv_obj_set_style_bg_color(p2_btn[0], lv_color_make(40, 180, 200), 0);
        lv_obj_t *l1 = lv_label_create(p2_btn[0]);
        lv_label_set_text(l1, "P2-1");
        lv_obj_center(l1);
        /* 点击 P2-1 跳转 page1 (id=0) */
        lv_obj_add_event_cb(p2_btn[0], p2_btn_1_event_cb, LV_EVENT_ALL, NULL);
        p2_btn[1] = lv_btn_create(page);
        lv_obj_set_size(p2_btn[1], bw, bh);
        lv_obj_set_pos(p2_btn[1], W - bw - margin, margin);
        lv_obj_set_style_bg_color(p2_btn[1], lv_color_make(60, 200, 160), 0);
        lv_obj_t *l2 = lv_label_create(p2_btn[1]);
        lv_label_set_text(l2, "P2-2");
        lv_obj_center(l2);
        p2_btn[2] = lv_btn_create(page);
        lv_obj_set_size(p2_btn[2], bw, bh);
        lv_obj_set_pos(p2_btn[2], margin, H - bh - margin);
        lv_obj_set_style_bg_color(p2_btn[2], lv_color_make(30, 150, 220), 0);
        lv_obj_t *l3 = lv_label_create(p2_btn[2]);
        lv_label_set_text(l3, "P2-3");
        lv_obj_center(l3);
        p2_btn[3] = lv_btn_create(page);
        lv_obj_set_size(p2_btn[3], bw, bh);
        lv_obj_set_pos(p2_btn[3], W - bw - margin, H - bh - margin);
        lv_obj_set_style_bg_color(p2_btn[3], lv_color_make(50, 130, 180), 0);
        lv_obj_t *l4 = lv_label_create(p2_btn[3]);
        lv_label_set_text(l4, "P2-4");
        lv_obj_center(l4);
        p2_created = 1;
    }

    int child_cnt = lv_obj_get_child_cnt(page);
    printf("page2 children=%d W=%d H=%d bw=%d bh=%d created=%d\n", child_cnt, W, H, bw, bh, p2_created);
}

void page_page2_unLoad(lv_obj_t *page)
{
    printf("lifecycle: page2 unLoad\n");
    if (!p2_created) return;
    for (int i = 0; i < 4; i++)
    {
        if (p2_btn[i])
        {
            lv_obj_del(p2_btn[i]);
            p2_btn[i] = NULL;
        }
    }
    p2_created = 0;
}