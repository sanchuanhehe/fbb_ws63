/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: Mutex
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

/**
 * @defgroup los_resource Resource
 * @ingroup kernel
 */

#ifndef LOS_RESOURCE_H
#define LOS_RESOURCE_H

#include "los_typedef.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_DEBUG_RESOURCE_INFO
/**
 * @ingroup kernel
 * Resource information structure for debug use.
 *
 */
typedef struct {
    UINT32 maxNum; /**< Maximum number of resources */
    UINT32 peak;   /**< Peak usage of resources */
    UINT32 used;   /**< Currently used resources */
    UINT32 unused; /**< Unused resources */
} ResourceInfo;

extern UINT32 g_muxUsed;
extern UINT32 g_muxPeak;
extern VOID LOS_GetMuxInfo(ResourceInfo *info);

extern UINT32 g_queueUsed;
extern UINT32 g_queuePeak;
extern VOID LOS_GetQueueInfo(ResourceInfo *info);

extern UINT32 g_semUsed;
extern UINT32 g_semPeak;
extern VOID LOS_GetSemInfo(ResourceInfo *info);

extern UINT32 g_swtmrUsed;
extern UINT32 g_swtmrPeak;
extern VOID LOS_GetSwtmrInfo(ResourceInfo *info);

extern UINT32 g_taskUsed;
extern UINT32 g_taskPeak;
extern VOID LOS_GetTaskInfo(ResourceInfo *info);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  // LOS_RESOURCE_H
