/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: main
 *
 * Create: 2021-03-09
 */

#include "boot_cmd_loop.h"
#include "boot_serial.h"
#include "boot_flash.h"
#include "sfc.h"
#include "chip_io.h"
#include "pinctrl.h"
#include "boot_delay.h"
#include "tcxo.h"
#include "watchdog.h"
#include "soc_porting.h"
#include "efuse.h"
#include "efuse_porting.h"
#include "boot_malloc.h"
#include "boot_erase.h"
#include "sfc_protect.h"

#define DELAY_100MS             100

__attribute__((section("BOOT_UART")))  uart_param_stru g_uart_prag;

const sfc_flash_config_t sfc_cfg = {
    .read_type = FAST_READ_QUAD_OUTPUT,
    .write_type = PAGE_PROGRAM,
    .mapping_addr = 0x200000,
    .mapping_size = 0x800000,
};

static uint32_t sfc_flash_init(void)
{
    return uapi_sfc_init((sfc_flash_config_t *)&sfc_cfg);
}

static uint32_t sfc_flash_read(uint32_t flash_addr, uint32_t read_size, uint8_t *read_buffer)
{
    return uapi_sfc_reg_read(flash_addr, read_buffer, read_size);
}

// addr 不是总线地址，没有 FLASH_START 偏移
static bool is_flash_addr(uint32_t addr, uint32_t size)
{
    return ((addr >= 0) && (addr + size < FLASH_LEN));
}

static uint32_t sfc_flash_write(uint32_t flash_addr, uint32_t flash_write_size, const uint8_t *p_flash_write_data,
                                bool do_erase)
{
    unused(do_erase);

    if (is_flash_addr(flash_addr, flash_write_size)) {
        return uapi_sfc_reg_write(flash_addr, (uint8_t *)p_flash_write_data, flash_write_size);
    }

    // 使用memcpy_s时，操作的是总线地址，需要加回偏移FLASH_START
    uintptr_t store_addr = (uintptr_t)((int)flash_addr + (int)FLASH_START);
    if (memcpy_s((void *)store_addr, flash_write_size, p_flash_write_data, flash_write_size) != EOK) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static uint32_t sfc_flash_erase(uint32_t flash_addr, uint32_t flash_erase_size)
{
    if (flash_erase_size == FLASH_CHIP_ERASE_SIZE) {
        return uapi_sfc_reg_erase_chip();
    }

    return uapi_sfc_reg_erase(flash_addr - sfc_cfg.mapping_addr, flash_erase_size);
}

static void boot_flash_init(void)
{
    flash_cmd_func flash_funcs = {0};
    flash_funcs.init = sfc_flash_init;
    flash_funcs.read = sfc_flash_read;
    flash_funcs.write = sfc_flash_write;
    flash_funcs.erase = sfc_flash_erase;
    boot_regist_flash_cmd(&flash_funcs);
#define REG_PMU_CMU_CTL_PMU_SIG 0x40003154
#define PMU_FLASH_SW_EN_BIT   24
#define DELAY_5MS 5
    reg32_setbit(REG_PMU_CMU_CTL_PMU_SIG, PMU_FLASH_SW_EN_BIT);
    mdelay(DELAY_5MS);

    uint32_t ret = sfc_flash_init();
    if (ret != ERRCODE_SUCC) {
        boot_msg1("Flash Init Fail! ret = ", ret);
    } else {
        boot_msg0("Flash Init Succ!");
    }
    switch_flash_clock_to_pll();
    config_sfc_ctrl_ds();
}

static uint32_t ws63_loaderboot_init(void)
{
    errcode_t err;
    uapi_tcxo_init();
    hiburn_uart_init(g_uart_prag, HIBURN_CODELOADER_UART);
    boot_msg0("Loadboot Uart Init Succ!");
    boot_flash_init();
    err = sfc_port_fix_sr();
    if (err != ERRCODE_SUCC) {
        boot_msg1("SFC fix SR ret =", err);
    }
    set_efuse_period();
    uapi_efuse_init(); // efuse函数初始化
    boot_malloc_init();
    return 0;
}

#define BOOT_WATCH_DOG_TIMOUT 7    // 7s
/* the entry of C. */
void start_loaderboot(void)
{
    uart_ctx cmd_ctx = { 0 };
    boot_clock_adapt();
    uapi_watchdog_init(BOOT_WATCH_DOG_TIMOUT);
    uapi_watchdog_enable(WDT_MODE_RESET);
    ws63_loaderboot_init();
    /* Enter the waiting command cycle. */
    loader_ack(ACK_SUCCESS);
    boot_msg0("================ Load Cmd Loop ================\r\n");
    cmd_loop(&cmd_ctx);
    while (1) {}
}

#ifdef CONFIG_LOADERBOOT_SUPPORT_SET_BUADRATE
uint32_t serial_port_set_baudrate(const uart_param_stru *uart)
{
#define CLDO_CRG_CLK_SEL    0x44001134
#define CLDO_SUB_CRG_CKEN_CTL1 0x44001104
#define UART_PLL_CLOCK  160000000
    hiburn_uart_deinit();
    // 切uart时钟
    reg_clrbit(CLDO_SUB_CRG_CKEN_CTL1, 0, POS_18);      // close uart clock
    reg_setbit(CLDO_CRG_CLK_SEL, 0, POS_1);             // switch uart clock to pll
    reg_setbit(CLDO_SUB_CRG_CKEN_CTL1, 0, POS_18);      // open uart clock
    uart_port_set_clock_value(UART_BUS_0, UART_PLL_CLOCK);
    hiburn_uart_init(*uart, HIBURN_CODELOADER_UART);
    return ERRCODE_SUCC;
}
#endif