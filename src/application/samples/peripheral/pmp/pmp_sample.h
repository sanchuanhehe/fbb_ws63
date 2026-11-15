/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: PMP memory protection sample definitions.
 */

#ifndef PMP_SAMPLE_H
#define PMP_SAMPLE_H

#include <stdint.h>

#define PMP_SAMPLE_REGION_INDEX 8U
#define PMP_SAMPLE_PROTECTED_SIZE 32U
#define PMP_SAMPLE_BUFFER_SIZE 64U

extern volatile uint8_t g_pmp_sample_buffer[PMP_SAMPLE_BUFFER_SIZE];

#endif
