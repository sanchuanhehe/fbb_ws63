/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2021. All rights reserved.
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
 
#include "los_resource.h"
 
 
#ifdef LOSCFG_DEBUG_RESOURCE_INFO
VOID LOS_GetMuxInfo(ResourceInfo *info)
{
    if (info == NULL) {
        return;
    }
    info->maxNum = LOSCFG_BASE_IPC_MUX_LIMIT;
    info->peak = g_muxPeak;
    info->used = g_muxUsed;
    info->unused = LOSCFG_BASE_IPC_MUX_LIMIT - g_muxUsed;
}
 
VOID LOS_GetQueueInfo(ResourceInfo *info)
{
    if (info == NULL) {
        return;
    }
    info->maxNum = LOSCFG_BASE_IPC_QUEUE_LIMIT;
    info->peak = g_queuePeak;
    info->used = g_queueUsed;
    info->unused = LOSCFG_BASE_IPC_QUEUE_LIMIT - g_queueUsed;
}
 
VOID LOS_GetSemInfo(ResourceInfo *info)
{
    if (info == NULL) {
        return;
    }
    info->maxNum = LOSCFG_BASE_IPC_SEM_LIMIT;
    info->peak = g_semPeak;
    info->used = g_semUsed;
    info->unused = LOSCFG_BASE_IPC_SEM_LIMIT - g_semUsed;
}
 
VOID LOS_GetSwtmrInfo(ResourceInfo *info)
{
    if (info == NULL) {
        return;
    }
    info->maxNum = LOSCFG_BASE_CORE_SWTMR_LIMIT;
    info->peak = g_swtmrPeak;
    info->used = g_swtmrUsed;
    info->unused = LOSCFG_BASE_CORE_SWTMR_LIMIT - g_swtmrUsed;
}
 
VOID LOS_GetTaskInfo(ResourceInfo *info)
{
    if (info == NULL) {
        return;
    }
    info->maxNum = LOSCFG_BASE_CORE_TSK_LIMIT;
    info->peak = g_taskPeak;
    info->used = g_taskUsed;
    info->unused = LOSCFG_BASE_CORE_TSK_LIMIT - g_taskUsed;
}
#endif