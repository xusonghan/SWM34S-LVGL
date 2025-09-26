#include "page1.h"
#include "pm.h" /* for lv_pm_open_options_t */
#include <stdio.h>

/* 静态指针保存本页控件，方便在 unLoad 时释放 */
#define P1_BTN_COUNT 40
static lv_obj_t *p1_btn[P1_BTN_COUNT];
static uint8_t p1_created = 0;

/* 按钮事件回调：P1-1 -> 打开 page2 */
static void p1_btn_1_event_cb(lv_event_t *e)
{
    if (e->code == LV_EVENT_CLICKED)
    {
        extern lv_pm_open_options_t options;
        options.target = LV_PM_TARGET_NEW;
        lv_pm_open_page(1, &options);
    }
}

void page_page1_onLoad(lv_obj_t *page)
{
    printf("lifecycle: page1 onLoad\n");
    lv_obj_set_style_bg_color(page, lv_color_make(237, 175, 50), 0);

    /* 获取页面尺寸，若尚未布局得到0则回退到显示分辨率 */
    lv_coord_t W = lv_obj_get_width(page);
    lv_coord_t H = lv_obj_get_height(page);
    if (W <= 0 || H <= 0)
    {
        lv_coord_t disp_w = lv_disp_get_hor_res(NULL);
        lv_coord_t disp_h = lv_disp_get_ver_res(NULL);
        if (W <= 0) W = disp_w;
        if (H <= 0) H = disp_h;
        printf("page1 fallback size W=%d H=%d\n", W, H);
    }
    lv_coord_t margin = 6; /* 缩小边距以容纳 8x5 */
    int cols = 8, rows = 5;
    lv_coord_t bw = (W - (cols + 1) * margin) / cols;
    lv_coord_t bh = (H - (rows + 1) * margin) / rows;
    if (bw < 10) bw = 10;
    if (bh < 10) bh = 10; /* 下限保护 */

    if (!p1_created)
    {
        /* 计算每个格子左上角：x = margin + col*(bw+margin), y 同理 */
        /* 单独逐项创建（不使用循环，保持与之前“单个创建”风格一致） */
        int x0 = margin, y0 = margin;
        p1_btn[0] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[0], bw, bh);
        lv_obj_set_pos(p1_btn[0], x0 + 0 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[0], lv_color_make(200, 120, 60), 0);
        lv_obj_t *l1 = lv_label_create(p1_btn[0]);
        lv_label_set_text(l1, "P1-1");
        lv_obj_center(l1);
        /* 点击 P1-1 跳转 page2 (id=1) */
        lv_obj_add_event_cb(p1_btn[0], p1_btn_1_event_cb, LV_EVENT_ALL, NULL);
        p1_btn[1] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[1], bw, bh);
        lv_obj_set_pos(p1_btn[1], x0 + 1 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[1], lv_color_make(210, 130, 60), 0);
        lv_obj_t *l2 = lv_label_create(p1_btn[1]);
        lv_label_set_text(l2, "P1-2");
        lv_obj_center(l2);
        p1_btn[2] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[2], bw, bh);
        lv_obj_set_pos(p1_btn[2], x0 + 2 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[2], lv_color_make(220, 140, 60), 0);
        lv_obj_t *l3 = lv_label_create(p1_btn[2]);
        lv_label_set_text(l3, "P1-3");
        lv_obj_center(l3);
        p1_btn[3] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[3], bw, bh);
        lv_obj_set_pos(p1_btn[3], x0 + 3 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[3], lv_color_make(230, 150, 60), 0);
        lv_obj_t *l4 = lv_label_create(p1_btn[3]);
        lv_label_set_text(l4, "P1-4");
        lv_obj_center(l4);
        p1_btn[4] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[4], bw, bh);
        lv_obj_set_pos(p1_btn[4], x0 + 4 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[4], lv_color_make(240, 160, 60), 0);
        lv_obj_t *l5 = lv_label_create(p1_btn[4]);
        lv_label_set_text(l5, "P1-5");
        lv_obj_center(l5);
        p1_btn[5] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[5], bw, bh);
        lv_obj_set_pos(p1_btn[5], x0 + 5 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[5], lv_color_make(240, 170, 60), 0);
        lv_obj_t *l6 = lv_label_create(p1_btn[5]);
        lv_label_set_text(l6, "P1-6");
        lv_obj_center(l6);
        p1_btn[6] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[6], bw, bh);
        lv_obj_set_pos(p1_btn[6], x0 + 6 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[6], lv_color_make(240, 180, 60), 0);
        lv_obj_t *l7 = lv_label_create(p1_btn[6]);
        lv_label_set_text(l7, "P1-7");
        lv_obj_center(l7);
        p1_btn[7] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[7], bw, bh);
        lv_obj_set_pos(p1_btn[7], x0 + 7 * (bw + margin), y0 + 0 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[7], lv_color_make(240, 190, 60), 0);
        lv_obj_t *l8 = lv_label_create(p1_btn[7]);
        lv_label_set_text(l8, "P1-8");
        lv_obj_center(l8);
        p1_btn[8] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[8], bw, bh);
        lv_obj_set_pos(p1_btn[8], x0 + 0 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[8], lv_color_make(200, 110, 70), 0);
        lv_obj_t *l9 = lv_label_create(p1_btn[8]);
        lv_label_set_text(l9, "P1-9");
        lv_obj_center(l9);
        p1_btn[9] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[9], bw, bh);
        lv_obj_set_pos(p1_btn[9], x0 + 1 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[9], lv_color_make(210, 120, 70), 0);
        lv_obj_t *l10 = lv_label_create(p1_btn[9]);
        lv_label_set_text(l10, "P1-10");
        lv_obj_center(l10);
        p1_btn[10] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[10], bw, bh);
        lv_obj_set_pos(p1_btn[10], x0 + 2 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[10], lv_color_make(220, 130, 70), 0);
        lv_obj_t *l11 = lv_label_create(p1_btn[10]);
        lv_label_set_text(l11, "P1-11");
        lv_obj_center(l11);
        p1_btn[11] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[11], bw, bh);
        lv_obj_set_pos(p1_btn[11], x0 + 3 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[11], lv_color_make(230, 140, 70), 0);
        lv_obj_t *l12 = lv_label_create(p1_btn[11]);
        lv_label_set_text(l12, "P1-12");
        lv_obj_center(l12);
        p1_btn[12] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[12], bw, bh);
        lv_obj_set_pos(p1_btn[12], x0 + 4 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[12], lv_color_make(240, 150, 70), 0);
        lv_obj_t *l13 = lv_label_create(p1_btn[12]);
        lv_label_set_text(l13, "P1-13");
        lv_obj_center(l13);
        p1_btn[13] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[13], bw, bh);
        lv_obj_set_pos(p1_btn[13], x0 + 5 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[13], lv_color_make(240, 160, 70), 0);
        lv_obj_t *l14 = lv_label_create(p1_btn[13]);
        lv_label_set_text(l14, "P1-14");
        lv_obj_center(l14);
        p1_btn[14] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[14], bw, bh);
        lv_obj_set_pos(p1_btn[14], x0 + 6 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[14], lv_color_make(240, 170, 70), 0);
        lv_obj_t *l15 = lv_label_create(p1_btn[14]);
        lv_label_set_text(l15, "P1-15");
        lv_obj_center(l15);
        p1_btn[15] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[15], bw, bh);
        lv_obj_set_pos(p1_btn[15], x0 + 7 * (bw + margin), y0 + 1 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[15], lv_color_make(240, 180, 70), 0);
        lv_obj_t *l16 = lv_label_create(p1_btn[15]);
        lv_label_set_text(l16, "P1-16");
        lv_obj_center(l16);
        p1_btn[16] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[16], bw, bh);
        lv_obj_set_pos(p1_btn[16], x0 + 0 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[16], lv_color_make(200, 100, 80), 0);
        lv_obj_t *l17 = lv_label_create(p1_btn[16]);
        lv_label_set_text(l17, "P1-17");
        lv_obj_center(l17);
        p1_btn[17] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[17], bw, bh);
        lv_obj_set_pos(p1_btn[17], x0 + 1 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[17], lv_color_make(210, 110, 80), 0);
        lv_obj_t *l18 = lv_label_create(p1_btn[17]);
        lv_label_set_text(l18, "P1-18");
        lv_obj_center(l18);
        p1_btn[18] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[18], bw, bh);
        lv_obj_set_pos(p1_btn[18], x0 + 2 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[18], lv_color_make(220, 120, 80), 0);
        lv_obj_t *l19 = lv_label_create(p1_btn[18]);
        lv_label_set_text(l19, "P1-19");
        lv_obj_center(l19);
        p1_btn[19] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[19], bw, bh);
        lv_obj_set_pos(p1_btn[19], x0 + 3 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[19], lv_color_make(230, 130, 80), 0);
        lv_obj_t *l20 = lv_label_create(p1_btn[19]);
        lv_label_set_text(l20, "P1-20");
        lv_obj_center(l20);
        p1_btn[20] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[20], bw, bh);
        lv_obj_set_pos(p1_btn[20], x0 + 4 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[20], lv_color_make(240, 140, 80), 0);
        lv_obj_t *l21 = lv_label_create(p1_btn[20]);
        lv_label_set_text(l21, "P1-21");
        lv_obj_center(l21);
        p1_btn[21] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[21], bw, bh);
        lv_obj_set_pos(p1_btn[21], x0 + 5 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[21], lv_color_make(240, 150, 80), 0);
        lv_obj_t *l22 = lv_label_create(p1_btn[21]);
        lv_label_set_text(l22, "P1-22");
        lv_obj_center(l22);
        p1_btn[22] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[22], bw, bh);
        lv_obj_set_pos(p1_btn[22], x0 + 6 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[22], lv_color_make(240, 160, 80), 0);
        lv_obj_t *l23 = lv_label_create(p1_btn[22]);
        lv_label_set_text(l23, "P1-23");
        lv_obj_center(l23);
        p1_btn[23] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[23], bw, bh);
        lv_obj_set_pos(p1_btn[23], x0 + 7 * (bw + margin), y0 + 2 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[23], lv_color_make(240, 170, 80), 0);
        lv_obj_t *l24 = lv_label_create(p1_btn[23]);
        lv_label_set_text(l24, "P1-24");
        lv_obj_center(l24);
        p1_btn[24] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[24], bw, bh);
        lv_obj_set_pos(p1_btn[24], x0 + 0 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[24], lv_color_make(200, 90, 90), 0);
        lv_obj_t *l25 = lv_label_create(p1_btn[24]);
        lv_label_set_text(l25, "P1-25");
        lv_obj_center(l25);
        p1_btn[25] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[25], bw, bh);
        lv_obj_set_pos(p1_btn[25], x0 + 1 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[25], lv_color_make(210, 100, 90), 0);
        lv_obj_t *l26 = lv_label_create(p1_btn[25]);
        lv_label_set_text(l26, "P1-26");
        lv_obj_center(l26);
        p1_btn[26] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[26], bw, bh);
        lv_obj_set_pos(p1_btn[26], x0 + 2 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[26], lv_color_make(220, 110, 90), 0);
        lv_obj_t *l27 = lv_label_create(p1_btn[26]);
        lv_label_set_text(l27, "P1-27");
        lv_obj_center(l27);
        p1_btn[27] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[27], bw, bh);
        lv_obj_set_pos(p1_btn[27], x0 + 3 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[27], lv_color_make(230, 120, 90), 0);
        lv_obj_t *l28 = lv_label_create(p1_btn[27]);
        lv_label_set_text(l28, "P1-28");
        lv_obj_center(l28);
        p1_btn[28] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[28], bw, bh);
        lv_obj_set_pos(p1_btn[28], x0 + 4 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[28], lv_color_make(240, 130, 90), 0);
        lv_obj_t *l29 = lv_label_create(p1_btn[28]);
        lv_label_set_text(l29, "P1-29");
        lv_obj_center(l29);
        p1_btn[29] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[29], bw, bh);
        lv_obj_set_pos(p1_btn[29], x0 + 5 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[29], lv_color_make(240, 140, 90), 0);
        lv_obj_t *l30 = lv_label_create(p1_btn[29]);
        lv_label_set_text(l30, "P1-30");
        lv_obj_center(l30);
        p1_btn[30] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[30], bw, bh);
        lv_obj_set_pos(p1_btn[30], x0 + 6 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[30], lv_color_make(240, 150, 90), 0);
        lv_obj_t *l31 = lv_label_create(p1_btn[30]);
        lv_label_set_text(l31, "P1-31");
        lv_obj_center(l31);
        p1_btn[31] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[31], bw, bh);
        lv_obj_set_pos(p1_btn[31], x0 + 7 * (bw + margin), y0 + 3 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[31], lv_color_make(240, 160, 90), 0);
        lv_obj_t *l32 = lv_label_create(p1_btn[31]);
        lv_label_set_text(l32, "P1-32");
        lv_obj_center(l32);
        p1_btn[32] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[32], bw, bh);
        lv_obj_set_pos(p1_btn[32], x0 + 0 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[32], lv_color_make(200, 80, 100), 0);
        lv_obj_t *l33 = lv_label_create(p1_btn[32]);
        lv_label_set_text(l33, "P1-33");
        lv_obj_center(l33);
        p1_btn[33] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[33], bw, bh);
        lv_obj_set_pos(p1_btn[33], x0 + 1 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[33], lv_color_make(210, 90, 100), 0);
        lv_obj_t *l34 = lv_label_create(p1_btn[33]);
        lv_label_set_text(l34, "P1-34");
        lv_obj_center(l34);
        p1_btn[34] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[34], bw, bh);
        lv_obj_set_pos(p1_btn[34], x0 + 2 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[34], lv_color_make(220, 100, 100), 0);
        lv_obj_t *l35 = lv_label_create(p1_btn[34]);
        lv_label_set_text(l35, "P1-35");
        lv_obj_center(l35);
        p1_btn[35] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[35], bw, bh);
        lv_obj_set_pos(p1_btn[35], x0 + 3 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[35], lv_color_make(230, 110, 100), 0);
        lv_obj_t *l36 = lv_label_create(p1_btn[35]);
        lv_label_set_text(l36, "P1-36");
        lv_obj_center(l36);
        p1_btn[36] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[36], bw, bh);
        lv_obj_set_pos(p1_btn[36], x0 + 4 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[36], lv_color_make(240, 120, 100), 0);
        lv_obj_t *l37 = lv_label_create(p1_btn[36]);
        lv_label_set_text(l37, "P1-37");
        lv_obj_center(l37);
        p1_btn[37] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[37], bw, bh);
        lv_obj_set_pos(p1_btn[37], x0 + 5 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[37], lv_color_make(240, 130, 100), 0);
        lv_obj_t *l38 = lv_label_create(p1_btn[37]);
        lv_label_set_text(l38, "P1-38");
        lv_obj_center(l38);
        p1_btn[38] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[38], bw, bh);
        lv_obj_set_pos(p1_btn[38], x0 + 6 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[38], lv_color_make(240, 140, 100), 0);
        lv_obj_t *l39 = lv_label_create(p1_btn[38]);
        lv_label_set_text(l39, "P1-39");
        lv_obj_center(l39);
        p1_btn[39] = lv_btn_create(page);
        lv_obj_set_size(p1_btn[39], bw, bh);
        lv_obj_set_pos(p1_btn[39], x0 + 7 * (bw + margin), y0 + 4 * (bh + margin));
        lv_obj_set_style_bg_color(p1_btn[39], lv_color_make(240, 150, 100), 0);
        lv_obj_t *l40 = lv_label_create(p1_btn[39]);
        lv_label_set_text(l40, "P1-40");
        lv_obj_center(l40);
        p1_created = 1;
    }

    /* 调试：打印子控件数量与尺寸，确认已创建 */
    int child_cnt = lv_obj_get_child_cnt(page);
    printf("page1 children=%d W=%d H=%d bw=%d bh=%d created=%d\n", child_cnt, W, H, bw, bh, p1_created);
}

void page_page1_unLoad(lv_obj_t *page)
{
    printf("lifecycle: page1 unLoad\n");
    if (!p1_created) return;
    for (int i = 0; i < P1_BTN_COUNT; i++)
    {
        if (p1_btn[i])
        {
            lv_obj_del(p1_btn[i]);
            p1_btn[i] = NULL;
        }
    }
    p1_created = 0;
}