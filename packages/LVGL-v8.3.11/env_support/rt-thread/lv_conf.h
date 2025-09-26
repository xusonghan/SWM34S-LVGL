/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-18     Meco Man      First version
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_16_SWAP    0
#define LV_COLOR_DEPTH      16
#define LV_USE_PERF_MONITOR 1
#define LV_USE_MEM_MONITOR  1

/* �ر�Ĭ������Ĺ��ɶ����밴�·Ŵ�Ч�� */
#define LV_THEME_DEFAULT_TRANSITION_TIME 0
#define LV_THEME_DEFAULT_GROW 0

/* ���������豸��ȡ���ڣ�Ĭ�� 30ms -> 10ms ���������� */
#define LV_INDEV_DEF_READ_PERIOD 10

/*Attribute to mark large constant arrays for example font's bitmaps*/
#define LV_ATTRIBUTE_LARGE_CONST __attribute__((section(".SDRAM2")))

// /*Compiler prefix for a big array declaration in RAM*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY __attribute__((section(".SDRAM1")))

#endif