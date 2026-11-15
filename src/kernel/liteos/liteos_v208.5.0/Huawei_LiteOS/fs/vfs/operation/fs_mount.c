/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: LiteOS Fs Mount Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2023-1-29
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

#include "vfs_config.h"

#include "sys/mount.h"
#include "string.h"
#include "errno.h"
#include "assert.h"
#include "debug.h"

#include "fs/fs.h"

#include "inode/inode.h"
#include "stdlib.h"
#include "driver/driver.h"
#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined(LOSCFG_FS_LITTLEFS)
#include "mtd_partition_pri.h"
#endif
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
#include "errcode_fat.h"
#endif
#include "los_tables.h"
#include "los_fs.h"

/* Fs map table for searching fs type */
extern struct fsmap_t g_fsmap_wow[];
LOS_HAL_TABLE_WOW_BEGIN(g_fsmap_wow, fsmap);

extern struct fsmap_t g_fsmap_wow_end;
LOS_HAL_TABLE_WOW_END(g_fsmap_wow_end, fsmap);

extern struct fsmap_t g_fsmap_scatter[];
LOS_HAL_TABLE_SCATTER_BEGIN(g_fsmap_scatter, fsmap);

extern struct fsmap_t g_fsmap_scatter_end;
LOS_HAL_TABLE_SCATTER_END(g_fsmap_scatter_end, fsmap);

extern struct fsmap_t g_fsmap[];
LOS_HAL_TABLE_BEGIN(g_fsmap, fsmap);

extern struct fsmap_t g_fsmap_end;
LOS_HAL_TABLE_END(g_fsmap_end, fsmap);

static const struct fsmap_t *MountFindFs(const char *fileSystemType)
{
    struct fsmap_t *m = NULL;

    for (m = &g_fsmap_wow[0]; m != &g_fsmap_wow_end; ++m) {
        if (m->fs_filesystemtype &&
            strcmp(fileSystemType, m->fs_filesystemtype) == 0) {
            return m;
        }
    }

    for (m = &g_fsmap_scatter[0]; m != &g_fsmap_scatter_end; ++m) {
        if (m->fs_filesystemtype &&
            strcmp(fileSystemType, m->fs_filesystemtype) == 0) {
            return m;
        }
    }

    for (m = &g_fsmap[0]; m != &g_fsmap_end; ++m) {
        if (m->fs_filesystemtype &&
            strcmp(fileSystemType, m->fs_filesystemtype) == 0) {
            return m;
        }
    }

    return (const struct fsmap_t *)NULL;
}

int mount(FAR const char *source, FAR const char *target,
             FAR const char *filesystemtype, unsigned long mountflags,
             FAR const void *data)
{
    char *fullpath = NULL;
    char *fullpathBackup = NULL;

    FAR struct inode *drvrInode = NULL;
    FAR struct inode *mountptInode = NULL;
    struct inode_search_s desc;
    const struct fsmap_t *fsmap = NULL;
    FAR const struct mountpt_operations *mops = NULL;

#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined(LOSCFG_FS_LITTLEFS)
    mtd_partition *partition = NULL;
#endif
    void *fshandle;
    bool target_exit = false;
    int ret;

    if (filesystemtype == NULL) {
        ret = -EINVAL;
        goto ERR;
    }

    /* Verify required pointer arguments */
    DEBUGASSERT(target && filesystemtype);

    ret = vfs_normalize_path((const char *)NULL, target, &fullpath);
    if (ret < 0) {
        PRINT_ERR("Failed to get fullpath,target: %s\n", target);
        goto ERR;
    }
    fullpathBackup = fullpath;

    /* Find the specified filesystem.  Try the block driver file systems first */
    fsmap = MountFindFs(filesystemtype);
    if (fsmap == NULL || (fsmap->is_bdfs && !source)) {
        PRINT_ERR("Failed to find file system %s\n", filesystemtype);
        ret = -ENODEV;
        free(fullpathBackup);
        goto ERR;
    }

    mops = fsmap->fs_mops;

    if (fsmap->is_bdfs && source) {
        /* Make sure that a block driver argument was provided */
        DEBUGASSERT(source);

        /* Find the block driver */
        ret = find_blockdriver(source, (int)mountflags, &drvrInode);
        if (ret < 0) {
            PRINT_ERR("Failed to find block driver %s\n", source);
            free(fullpathBackup);
            goto ERR;
        }
    }

    /*
     * Insert a dummy node -- we need to hold the inode semaphore
     * to do this because we will have a momentarily bad structure.
     */
    ret = inode_semtake();
    if (ret < 0) {
        free(fullpathBackup);
        goto ERR;
    }

    ret = inode_reserve(fullpath, 0777, &mountptInode);
    if (ret < 0) {
        /*
         * inode_reserve can fail for a couple of reasons, but the most
         * likely one is that the inode already exists. inode_reserve may
         * return:
         *
         *  -EINVAL - 'path' is invalid for this operation
         *  -EEXIST - An inode already exists at 'path'
         *  -ENOMEM - Failed to allocate in-memory resources for the
         *            operation
         */
        if (ret == -EEXIST) {
            SETUP_SEARCH(&desc, fullpath, false);
            target_exit = true;
            if (inode_search(&desc) < 0) {
                RELEASE_SEARCH(&desc);
                PRINT_ERR("Failed to reserve inode, %d\n", -ret);
                goto ERR_WITH_SEM;
            }
            mountptInode = desc.node;
            RELEASE_SEARCH(&desc);

            if (INODE_IS_MOUNTPT(mountptInode) || mountptInode->i_child || mountptInode->u.i_ops) {
                PRINT_ERR("Can't to mount to this inode, %d\n", -ret);
                goto ERR_WITH_SEM;
            }
        } else {
            PRINT_ERR("Failed to reserve inode, %d\n", -ret);
            goto ERR_WITH_SEM;
        }
    }
    mountptInode ->mountflags = mountflags;

    /*
     * Bind the block driver to an instance of the file system.  The file
     * system returns a reference to some opaque, fs-dependent structure
     * that encapsulates this binding.
     */
    if (mops->bind == NULL) {
        /* The filesystem does not support the bind operation ??? */
        ferr("ERROR: Filesystem does not support bind\n");
        ret = -EINVAL;
        goto ERR_WITH_MOUNTPT;
    }

    /* Increment reference count for the reference we pass to the file system */
    if (drvrInode != NULL) {
        drvrInode->i_crefs++;

        /* On failure, the bind method returns -errorcode */
        if (drvrInode->e_status != STAT_UNMOUNTED) {
            fdbg("ERROR: The node is busy\n");
            ret = -EBUSY;
            drvrInode->i_crefs--;
            goto ERR_WITH_MOUNTPT;
        }
    }
#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined(LOSCFG_FS_LITTLEFS)
    if (fsmap->is_mtd_support && drvrInode != NULL) {
        if (!(drvrInode->i_flags & FSNODEFLAG_EXTEND_TYPE_MTD)) {
            ret = -EOPNOTSUPP;
            drvrInode->i_crefs--;
            goto ERR_WITH_MOUNTPT;
        }

        partition = (mtd_partition *)drvrInode->i_private;
        partition->mountpoint_name = (char *)zalloc(strlen(target) + 1);
        if (partition->mountpoint_name == NULL) {
            ret = -ENOMEM;
            drvrInode->i_crefs--;
            goto ERR_WITH_MOUNTPT;
        }

        ret = strncpy_s(partition->mountpoint_name, strlen(target) + 1, target, strlen(target));
        if (ret != EOK) {
            free(partition->mountpoint_name);
            drvrInode->i_crefs--;
            goto ERR_WITH_MOUNTPT;
        }
        partition->mountpoint_name[strlen(target)] = '\0';
    }
#endif
    mountptInode ->mountflags = mountflags;
    ret = mops->bind(drvrInode, data, &fshandle, fullpathBackup);
#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
    if (ret >= VIRERR_BASE) {
        ferr("ERROR: Virtual partition bind failed: %d\n", ret);
    } else
#endif
    if (ret != 0) {
        /*
         * The inode is unhappy with the driver for some reason.  Back out
         * the count for the reference we failed to pass and exit with an
         * error.
         */
        ferr("ERROR: Bind method failed: %d\n", ret);
        if (drvrInode != NULL) {
            drvrInode->i_crefs--;
        }

#if defined(LOSCFG_FS_YAFFS) || defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined(LOSCFG_FS_LITTLEFS)
        if (fsmap->is_mtd_support && drvrInode != NULL && partition != NULL) {
            free(partition->mountpoint_name);
            partition->mountpoint_name = NULL;
        }
#endif
        goto ERR_WITH_MOUNTPT;
    }

    /* We have it, now populate it with driver specific information. */
    INODE_SET_MOUNTPT(mountptInode);

    mountptInode->u.i_mops  = mops;
    mountptInode->i_private = fshandle;

    if (drvrInode) {
        drvrInode->e_status = STAT_MOUNTED;
    }
    inode_semgive();

    /*
     * We can release our reference to the blkdrver_inode, if the filesystem
     * wants to retain the blockdriver inode (which it should), then it must
     * have called inode_addref().  There is one reference on mountpt_inode
     * that will persist until umount() is called.
     */
    if (drvrInode != NULL) {
        inode_release(drvrInode);
    }

    free(fullpathBackup);

#ifdef LOSCFG_FS_FAT_VIRTUAL_PARTITION
    if (ret >= VIRERR_BASE) {
        set_errno(ret);
    }
#endif

    return OK;

    /* A lot of goto's!  But they make the error handling much simpler */
ERR_WITH_MOUNTPT:
    if (target_exit == false) {
        (void)inode_remove(fullpath);
    }
ERR_WITH_SEM:
    inode_semgive();
    if (drvrInode != NULL) {
        inode_release(drvrInode);
    }
    free(fullpathBackup);

ERR:
    set_errno(-ret);
    return VFS_ERROR;
}
