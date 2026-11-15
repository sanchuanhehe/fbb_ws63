/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: PMP configuration and memory protection sample source. \n
 */

#include <stdint.h>
#include "drv_pmp.h"
#include "soc_osal.h"
#include "app_init.h"
#include "pmp_sample.h"

#define PMP_SAMPLE_PROTECTED_INIT_VALUE 0x11U
#define PMP_SAMPLE_UNPROTECTED_INIT_VALUE 0x33U
#define PMP_SAMPLE_UNPROTECTED_TEST_VALUE 0x22U
#define PMP_SAMPLE_FAULT_VALUE 0xA5U

/**
 * @brief PMP sample buffer containing adjacent protected and unprotected regions. \n
 *        包含相邻受保护区域和未保护区域的 PMP 样例缓冲区。 \n
 *
 * The first 32 bytes are configured as read-only, while the remaining 32 bytes stay writable. \n
 * 前 32 字节配置为只读，后 32 字节保持可写。 \n
 */
volatile uint8_t g_pmp_sample_buffer[PMP_SAMPLE_BUFFER_SIZE]
    __attribute__((aligned(PMP_SAMPLE_BUFFER_SIZE))) = {
        [0] = PMP_SAMPLE_PROTECTED_INIT_VALUE,
        [PMP_SAMPLE_PROTECTED_SIZE] = PMP_SAMPLE_UNPROTECTED_INIT_VALUE,
    };

/**
 * @brief Configure the PMP region and verify protected and unprotected memory access. \n
 *        配置 PMP 区域，并验证受保护和未保护内存的访问结果。 \n
 *
 * @note In TOR mode, the current entry address is the exclusive upper boundary. \n
 *       TOR 模式下，当前条目的地址是不包含在保护范围内的上边界。 \n
 */
static void pmp_sample_entry(void)
{
    /*
     * Step 1: Split the aligned buffer into adjacent protected and unprotected regions.
     * 步骤 1：将对齐缓冲区划分为相邻的受保护区域和未保护区域。
     */
    volatile uint8_t *protected_region = &g_pmp_sample_buffer[0];
    volatile uint8_t *protected_region_end = &g_pmp_sample_buffer[PMP_SAMPLE_PROTECTED_SIZE];
    volatile uint8_t *unprotected_region = protected_region_end;

    /*
     * Step 2: Use the protected-region end as the exclusive TOR upper boundary.
     * 步骤 2：使用受保护区域的结束地址作为 TOR 模式不包含在范围内的上边界。
     */
    pmp_conf_t config = {
        .idx = PMP_SAMPLE_REGION_INDEX,
        .addr = (uint32_t)(uintptr_t)protected_region_end,
        .size = 0,
        .conf = {
            .rwx_permission = PMPCFG_READ_ONLY_NEXECUTE,
            .addr_match = PMPCFG_ADDR_MATCH_TOR,
            .lock = true,
            .pmp_attr = PMP_ATTR_WRITEBACK_RWALLOCATE,
        },
    };
    errcode_t ret;

    osal_printk("[pmp] protected region: %p - %p\r\n", protected_region,
                &protected_region[PMP_SAMPLE_PROTECTED_SIZE - 1U]);
    osal_printk("[pmp] unprotected region: %p - %p\r\n", unprotected_region,
                &unprotected_region[PMP_SAMPLE_PROTECTED_SIZE - 1U]);
    osal_printk("[pmp] initial values: protected=0x%02x, unprotected=0x%02x\r\n",
                protected_region[0], unprotected_region[0]);

    /*
     * Step 3: Apply and lock the read-only PMP entry after all boundaries are ready.
     * 步骤 3：边界准备完成后应用并锁定只读 PMP 条目。
     */
    ret = uapi_pmp_config(&config, 1U);
    if (ret != ERRCODE_SUCC) {
        osal_printk("[pmp] configuration failed: 0x%x\r\n", ret);
        return;
    }
    osal_printk("[pmp] first 32 bytes configured as read-only\r\n");

    /*
     * Step 4: Read the protected region to prove that read permission remains available.
     * 步骤 4：读取受保护区域，证明该区域仍保留读权限。
     */
    osal_printk("[pmp] protected read succeeded: 0x%02x\r\n", protected_region[0]);

    /*
     * Step 5: Write and read back the adjacent unprotected region.
     * 步骤 5：写入并回读相邻的未保护区域。
     */
    unprotected_region[0] = PMP_SAMPLE_UNPROTECTED_TEST_VALUE;
    osal_printk("[pmp] unprotected write succeeded: 0x%02x\r\n", unprotected_region[0]);

#if defined(CONFIG_PMP_SAMPLE_TRIGGER_FAULT)
    /*
     * Step 6: Write the protected region last because the expected fault stops normal execution.
     * 步骤 6：最后写入受保护区域，因为预期异常会终止正常执行流程。
     */
    osal_printk("[pmp] triggering protected write; a store access fault is expected\r\n");
    protected_region[0] = PMP_SAMPLE_FAULT_VALUE;
    osal_printk("[pmp] ERROR: protected write was not blocked\r\n");
#else
    osal_printk("[pmp] fault test disabled; protected write skipped\r\n");
#endif
}

app_run(pmp_sample_entry);
