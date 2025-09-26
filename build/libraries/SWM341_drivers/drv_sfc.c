/*
 * Copyright (c) 2006-2024, Your Company
 * SPDX-License-Identifier: Apache-2.0
 */

#include "rtthread.h"
#include "board.h"
#include "drv_log.h"
#include "drv_sfc.h"

#ifdef BSP_USING_SFC

#include "SWM341.h"
#include "SWM341_sfc.h"

// /* Map Kconfig to vendor enums */
// #if defined(BSP_SFC_CLKDIV_1)
// #define SFC_CLKDIV_CFG SFC_CLKDIV_1
// #elif defined(BSP_SFC_CLKDIV_2)
// #define SFC_CLKDIV_CFG SFC_CLKDIV_2
// #elif defined(BSP_SFC_CLKDIV_4)
// #define SFC_CLKDIV_CFG SFC_CLKDIV_4
// #else
// #define SFC_CLKDIV_CFG SFC_CLKDIV_8
// #endif

// #if defined(BSP_SFC_READ_WIDTH_1)
// #define SFC_RDWIDTH_CFG SFC_RDWIDTH_1
// #else
// #define SFC_RDWIDTH_CFG SFC_RDWIDTH_4
// #endif

// #if defined(BSP_SFC_PP_WIDTH_1)
// #define SFC_PPWIDTH_CFG SFC_PPWIDTH_1
// #else
// #define SFC_PPWIDTH_CFG SFC_PPWIDTH_4
// #endif

/* Note: previously we had an optional post-init re-copy helper for ER_SDRAM2 here.
 * With early SDRAM+SFC init moved into SystemInit, that workaround is no longer needed. */

int swm_sfc_init(void)
{
    SFC_InitStructure SFC_initStruct;

    /** SFC使用专用的FSPI（Flash SPI）接口连接SPI Flash */
    PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK, 0);
    PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL, 0);
    PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI, 1);
    PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO, 1);
    PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
    PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);

    SFC_initStruct.ClkDiv = SFC_CLKDIV_2;
    SFC_initStruct.Cmd_Read = 0xEB;           /* Quad I/O Read */
    SFC_initStruct.Width_Read = SFC_RDWIDTH_4;/* 4-line read */
    SFC_initStruct.Cmd_PageProgram = 0x32;
    SFC_initStruct.Width_PageProgram = SFC_PPWIDTH_4;

    SFC_Init(&SFC_initStruct);
    return 0;
//     /* Pin mux fixed as per reference: PD5/CS0 PD6/SCLK PD8/IO0 PD7/IO1 PD3/IO2 PD4/IO3 */
//     PORT_Init(PORTD, PIN5, PORTD_PIN5_FSPI_SCLK, 1);
//     PORT_Init(PORTD, PIN6, PORTD_PIN6_FSPI_SSEL, 1);
//     PORT_Init(PORTD, PIN8, PORTD_PIN8_FSPI_MOSI, 1);
//     PORT_Init(PORTD, PIN7, PORTD_PIN7_FSPI_MISO, 1);
//     PORT_Init(PORTD, PIN3, PORTD_PIN3_FSPI_DATA2, 1);
//     PORT_Init(PORTD, PIN4, PORTD_PIN4_FSPI_DATA3, 1);

//     SFC_InitStructure SFC_InitStruct;
//     SFC_InitStruct.ClkDiv = SFC_CLKDIV_CFG;
//     SFC_InitStruct.Cmd_Read = (BSP_SFC_CMD_READ);
//     SFC_InitStruct.Width_Read = SFC_RDWIDTH_CFG;
//     SFC_InitStruct.Cmd_PageProgram = (BSP_SFC_CMD_PP);
//     SFC_InitStruct.Width_PageProgram = SFC_PPWIDTH_CFG;
// #ifdef BSP_SFC_ENABLE_XIP
//     SFC_InitStruct.XIP_Enable = 1; /* if field exists */
// #endif

//     SFC_Init(&SFC_InitStruct);

    // LOG_I("SFC initialized: clkdiv=%d, rd_cmd=0x%02X, rdW=%d, pp_cmd=0x%02X, ppW=%d",
    //       SFC_InitStruct.ClkDiv,
    //       SFC_InitStruct.Cmd_Read,
    //       SFC_InitStruct.Width_Read,
    //       SFC_InitStruct.Cmd_PageProgram,
    //       SFC_InitStruct.Width_PageProgram);

    // return 0;
}
// INIT_BOARD_EXPORT(swm_sfc_init);

#endif /* BSP_USING_SFC */
