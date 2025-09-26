#include "pm.h"

#include "page1.h"
#include "page2.h"
#include <rtthread.h>
#include <stdlib.h> /* for atoi */
#include "lvgl.h"   /* LVGL 上下文 */

/* 全局页面打开参数，必须初始化所有字段，防止未初始化成员被使用出现随机值 */
lv_pm_open_options_t options =
{
    .animation = LV_PM_ANIMA_NONE,
    .target = LV_PM_TARGET_NEW,
    .direction = LV_PM_ANIMA_LEFT,
};

/* MSH 命令：打开指定页面或返回上一页面 */
typedef struct
{
    uint8_t type; /* 0 open 1 back */
    uint8_t id;
} pm_async_req_t;

static void _pm_async_cb(void *user_data)
{
    pm_async_req_t *req = (pm_async_req_t *)user_data;
    if (!req) return;
    if (req->type == 0)
    {
        /* 在 LVGL 线程上下文内做安全检查 */
        if (req->id >= LV_PM_PAGE_NUM)
        {
            rt_kprintf("invalid id %d\n", req->id);
        }
        else if (lv_pm_history_len > 0 && lv_pm_history[lv_pm_history_len - 1] == req->id)
        {
            rt_kprintf("already on page %d\n", req->id);
        }
        else if (lv_anim_count_running() > 0)
        {
            rt_kprintf("pm_open busy (animation running)\n");
        }
        else
        {
            int ret = lv_pm_open_page(req->id, &options);
            rt_kprintf("pm_open %d ret=%d\n", req->id, ret);
        }
    }
    else     /* back */
    {
        if (lv_pm_history_len < 2)
        {
            rt_kprintf("no previous page\n");
        }
        else if (lv_anim_count_running() > 0)
        {
            rt_kprintf("pm_back busy (animation running)\n");
        }
        else
        {
            int ret = lv_pm_back();
            rt_kprintf("pm_back ret=%d\n", ret);
        }
    }
    lv_mem_free(req);
}

static int cmd_pm_open(int argc, char *argv[])
{
    if (argc < 2)
    {
        rt_kprintf("Usage: pm_open <id>\n");
        return -1;
    }
    int id = atoi(argv[1]);
    pm_async_req_t *req = lv_mem_alloc(sizeof(pm_async_req_t));
    if (!req)
    {
        rt_kprintf("alloc fail\n");
        return -5;
    }
    req->type = 0;
    req->id = (uint8_t)id;
    lv_async_call(_pm_async_cb, req);
    return 0;
}
MSH_CMD_EXPORT(cmd_pm_open, open lvgl page by id async);

static int cmd_pm_back(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    pm_async_req_t *req = lv_mem_alloc(sizeof(pm_async_req_t));
    if (!req)
    {
        rt_kprintf("alloc fail\n");
        
        return -5;
    }
    req->type = 1;
    req->id = 0;
    lv_async_call(_pm_async_cb, req);
    return 0;
}
MSH_CMD_EXPORT(cmd_pm_back, back to previous lvgl page async);

/* 为便于使用，再导出无前缀的命令别名 pm_open / pm_back */
static int pm_open(int argc, char *argv[])
{
    return cmd_pm_open(argc, argv);
}
MSH_CMD_EXPORT(pm_open, open lvgl page by id async);

static int pm_back(int argc, char *argv[])
{
    return cmd_pm_back(argc, argv);
}
MSH_CMD_EXPORT(pm_back, back to previous lvgl page async);

static int pm_list(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    rt_kprintf("history_len=%d\n", lv_pm_history_len);
    for (uint8_t i = 0; i < lv_pm_history_len; i++)
    {
        rt_kprintf("[%d]=%d ", i, lv_pm_history[i]);
    }
    rt_kprintf("\n");
    return 0;
}
MSH_CMD_EXPORT(pm_list, list page stack);

bool open = false;
void timer_cb(lv_timer_t *timer)
{
    if (open)
    {
        lv_pm_back();
        open = false;
    }
    else
    {
        lv_pm_open_page(1, &options);
        open = true;
    }
}

void lv_app_start(void)
{
    lv_pm_init();

    lv_pm_page_t *page1 = lv_pm_create_page(0);
    page1->onLoad = page_page1_onLoad;
    page1->unLoad = page_page1_unLoad;

    lv_pm_page_t *page2 = lv_pm_create_page(1);
    page2->onLoad = page_page2_onLoad;
    page2->unLoad = page_page2_unLoad;

    lv_pm_open_page(0, &options); /* 默认打开第一页，之后用 MSH 命令操控 */
    // rt_thread_mdelay(500);
    // lv_timer_create(timer_cb, 1000, NULL);
}
