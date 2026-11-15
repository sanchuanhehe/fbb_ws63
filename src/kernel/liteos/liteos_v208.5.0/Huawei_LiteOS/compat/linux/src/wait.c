/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Waitqueue Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-07-28
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

#include "poll.h"
#include "linux/wait.h"
#include "linux/spinlock.h"
#include "los_event.h"

void __init_waitqueue_head(wait_queue_head_t *wait)
{
    if (wait == NULL) {
        return;
    }
    (VOID)LOS_EventInit(&wait->stEvent);
    spin_lock_init(&wait->lock);
    LOS_ListInit(&wait->poll_queue);
}

void __wake_up_interruptible(wait_queue_head_t *wait)
{
    if (wait == NULL) {
        return;
    }
    (VOID)LOS_EventWrite(&wait->stEvent, 0x1);
#ifdef LOSCFG_FS_VFS
    notify_poll(wait);
#endif
}

void __wake_up_interruptible_poll(wait_queue_head_t *wait, unsigned int key)
{
    if (wait == NULL) {
        return;
    }
    (VOID)LOS_EventWrite(&wait->stEvent, 0x1);
#ifdef LOSCFG_FS_VFS
    notify_poll_with_key(wait, key);
#endif
}
