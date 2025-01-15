/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 *
 * Create: 2021-03-09
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

/* MCU ITCM config */
#define APP_ITCM_ORIGIN 0x20000
#define APP_ITCM_LENGTH 0x10000

#define APP_SHARE_RAM_ORIGIN 0xA00000
#define APP_SHARE_RAM_LENGTH 0x98000

/* reserve 128KB ram for romboot */
#define ROMBOOT_RAM_START APP_SHARE_RAM_ORIGIN
#define ROMBOOT_RAM_LEN  0x20000
#define ROMBOOT_RAM_END  (ROMBOOT_RAM_START + ROMBOOT_RAM_LEN) // 0xA20000 hiburn loaderboot address

/* Boot Load RAM Addr */
#define BOOT_RAM_MIN    ROMBOOT_RAM_END
#define BOOT_RAM_MAX    (APP_ITCM_ORIGIN + APP_ITCM_LENGTH)

#define BOOT_HEAD_LEN   0x380

/* reseerve 40K for loader boot codesize */
#define LOADER_PROGRAM_ORIGIN (ROMBOOT_RAM_END + BOOT_HEAD_LEN)
#define LOADER_PROGRAM_LENGTH 0xA000

/* MCU DTCM config currently use share ram */
#define APP_SRAM_ORIGIN 0xA00000
#define APP_SRAM_LENGTH 0x20000

/* stack for normal 7k */
#define USER_STACK_BASEADDR APP_SRAM_ORIGIN
#define USER_STACK_LEN      0x1c00
#define USER_STACK_LIMIT    (USER_STACK_BASEADDR + USER_STACK_LEN)

/* stack for irq 1k */
#define IRQ_STACK_BASEADDR USER_STACK_LIMIT
#define IRQ_STACK_LEN      0x400
#define IRQ_STACK_LIMIT    (IRQ_STACK_BASEADDR + IRQ_STACK_LEN)

/* stack for exception 1k */
#define EXCP_STACK_BASEADDR IRQ_STACK_LIMIT
#define EXCP_STACK_LEN      0x400
#define EXCP_STACK_LIMIT    (EXCP_STACK_BASEADDR + EXCP_STACK_LEN)

#define LOADER_STACK_LEN       (USER_STACK_LEN + IRQ_STACK_LEN + EXCP_STACK_LEN)

#endif
