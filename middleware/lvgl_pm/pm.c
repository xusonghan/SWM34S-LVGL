#include "pm.h"
#include "anima.h"
#include "pm_utils.h"

#include <stdlib.h>
#include <stdio.h>

/* 全局变量定义（与 pm.h 中的 extern 匹配）*/
uint8_t lv_pm_history_len = 0;
lv_pm_page_t *lv_pm_router[LV_PM_PAGE_NUM];
uint8_t lv_pm_history[LV_PM_PAGE_NUM];

static void _appear_complete_cb(lv_pm_page_t *pm_page, lv_pm_open_options_t options)
{
  if (pm_page->didAppear) {
    pm_page->didAppear(pm_page->page);
  }
}

static void _back_appear_complete_cb(lv_pm_page_t *pm_page, lv_pm_open_options_t options)
{
  if (pm_page->didAppear) {
    pm_page->didAppear(pm_page->page);
  }
}

static void _disAppear_complete_cb(lv_pm_page_t *pm_page, lv_pm_open_options_t options)
{
  if (options.animation != LV_PM_ANIMA_POPUP) {
    lv_obj_add_flag(pm_page->page, LV_OBJ_FLAG_HIDDEN);
  }
  if (pm_page->didDisappear) {
    pm_page->didDisappear(pm_page->page);
  }
  /* 统一释放策略：离开当前页就调用 unLoad（无论 NEW 还是 SELF），避免 NEW 模式残留控件占用堆 */
  if (pm_page->unLoad) {
    pm_page->unLoad(pm_page->page);
  }
}

static void _back_disAppear_complete_cb(lv_pm_page_t *pm_page, lv_pm_open_options_t options)
{
  lv_obj_add_flag(pm_page->page, LV_OBJ_FLAG_HIDDEN);
  if (pm_page->didDisappear) {
    pm_page->didDisappear(pm_page->page);
  }
  if (pm_page->unLoad) {
    pm_page->unLoad(pm_page->page);
  }
  /* 不再调用 lv_obj_clean */
}

uint8_t lv_pm_init()
{
  lv_pm_history_len = 0;
  for (uint8_t i = 0; i < LV_PM_PAGE_NUM; i++)
  {
    lv_pm_router[i] = 0;
  }

  lv_obj_t *screen = lv_scr_act();
  // turn off the scroll bar
  lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

  return 0;
}

lv_pm_page_t *lv_pm_create_page(uint8_t id)
{
  lv_pm_page_t *pm_page = (lv_pm_page_t *)malloc(sizeof(lv_pm_page_t));
  if (pm_page == NULL) {
    return NULL;
  }
  memset(pm_page, 0, sizeof(lv_pm_page_t));

  lv_pm_router[id] = pm_page;
  lv_obj_t *page = lv_obj_create(lv_scr_act());
  // reset style. border radius etc...
  pm_reset_style(page);
  lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
  lv_coord_t width = lv_disp_get_hor_res(NULL);
  lv_coord_t height = lv_disp_get_ver_res(NULL);
  lv_obj_set_width(page, width);
  lv_obj_set_height(page, height);

  pm_page->page = page;
  return pm_page;
}

uint8_t lv_pm_open_page(uint8_t id, lv_pm_open_options_t *behavior)
{
  if (lv_pm_router[id] == 0) {
    return 4;
  }
  if (lv_pm_history_len == LV_PM_PAGE_NUM) {
    return 5;
  }

  /* 如果是直接切换需求：我们用 target=NEW 但不想堆叠历史，先把当前顶部替换 */
  bool replace_top = false;
  if (behavior && behavior->target == LV_PM_TARGET_NEW) {
    if (lv_pm_history_len > 0) {
      replace_top = true;
    }
  }

  if (!replace_top) {
    lv_pm_history[lv_pm_history_len] = id;
  }
  lv_pm_page_t *pm_page = lv_pm_router[id];
  lv_obj_t *page = pm_page->page;
  if (behavior) {
    pm_page->_options = *behavior;
  }
  pm_page->_back = false;

  if (lv_pm_history_len > 0) {
    uint8_t pid = lv_pm_history[lv_pm_history_len - 1];
    lv_pm_page_t *prev_pm_page = lv_pm_router[pid];
    lv_obj_t *prev_page = prev_pm_page->page;
    prev_pm_page->_back = false;
    if (prev_pm_page->willDisappear) {
      prev_pm_page->willDisappear(prev_page);
    }
    _pm_anima_disAppear(prev_pm_page, &pm_page->_options, _disAppear_complete_cb);
  }

  pm_page->onLoad(page);
  lv_obj_clear_flag(page, LV_OBJ_FLAG_HIDDEN);
  if (pm_page->willAppear) {
    pm_page->willAppear(page);
  }
  _pm_anima_appear(pm_page, &pm_page->_options, _appear_complete_cb);

  if (replace_top) {
    /* 直接替换栈顶，不增加长度 */
    lv_pm_history[lv_pm_history_len - 1] = id;
  } else if (behavior && behavior->target == LV_PM_TARGET_SELF) {
    if (lv_pm_history_len == 0) {
      lv_pm_history_len++;
    } else {
      lv_pm_history[lv_pm_history_len - 1] = lv_pm_history[lv_pm_history_len];
    }
  } else {
    lv_pm_history_len++;
  }

  return 0;
}

uint8_t lv_pm_back()
{
  if (lv_pm_history_len < 2) {
    return 0;
  }
  uint8_t pid = lv_pm_history[lv_pm_history_len - 1];
  lv_pm_page_t *pm_page = lv_pm_router[pid];
  pm_page->_back = true;
  lv_obj_t *page = pm_page->page;

  if (pm_page->willDisappear) {
    pm_page->willDisappear(page);
  }
  _pm_anima_disAppear(pm_page, &pm_page->_options, _back_disAppear_complete_cb);

  lv_pm_history_len--;
  uint8_t prev_pid = lv_pm_history[lv_pm_history_len - 1];
  lv_pm_page_t *prev_pm_page = lv_pm_router[prev_pid];
  lv_obj_t *prev_page = prev_pm_page->page;
  prev_pm_page->_back = true;

  if (prev_pm_page->willAppear) {
    prev_pm_page->willAppear(prev_page);
  }
  lv_obj_clear_flag(prev_pm_page->page, LV_OBJ_FLAG_HIDDEN);
  _pm_anima_appear(prev_pm_page, &pm_page->_options, _back_appear_complete_cb);

  return 0;
}
