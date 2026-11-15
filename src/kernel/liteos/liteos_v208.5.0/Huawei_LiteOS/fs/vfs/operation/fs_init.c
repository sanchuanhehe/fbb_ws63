/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
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

#include "los_printf.h"
#include "fs/fs.h"
#include "inode/inode.h"
#include "unistd.h"
#include "fcntl.h"
#include "sys/statfs.h"
#include "los_spinlock.h"
#include "los_atomic.h"
#include "los_init.h"
#ifdef LOSCFG_DRIVER_DISK
#include "disk_pri.h"
#endif
#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined (LOSCFG_FS_LITTLEFS)
#include "mtd_partition_pri.h"
#endif

SPIN_LOCK_S g_workdir_lock;
void los_vfs_init(void)
{
  static bool g_vfs_init = false;
  static Atomic  g_filelist_init = 0;
  if (g_vfs_init)
    {
      return;
    }

  LOS_SpinInit(&g_workdir_lock);
#ifdef LOSCFG_DRIVER_DISK
  OsDiskInit();
#endif
#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined (LOSCFG_FS_LITTLEFS)
  OsMtdPartitionInit();
#endif

  if (!LOS_AtomicCmpXchg32bits(&g_filelist_init, 1, 0))
    {
      files_initlist(&tg_filelist);
      fs_initialize();
    }

  g_vfs_init = true;
}

UINT32 OsVfsInit(VOID)
{
    los_vfs_init();
    return LOS_OK;
}
LOS_SYS_INIT(OsVfsInit, SYS_INIT_LEVEL_COMPONENT, SYS_INIT_SYNC_0);
