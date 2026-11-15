/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: implementation for vfs extend.
 * Author: Huawei LiteOS Team
 * Create: 2022-09-13
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

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include "inode/inode.h"
#include "nuttx/config.h"
#include "errno.h"
#include "unistd.h"

#include "console_pri.h"
#include "disk_pri.h"
#ifdef LOSCFG_COMPAT_LINUX
#include "poll.h"
#endif

#ifdef LOSCFG_NET_LWIP_SACK
#include "sys/socket.h"
#include "lwip/sockets.h"
#include "net/net.h"
#endif

#define TIME_CONVERT_SCALE 1000000 /* 1000000: Convert seconds to microseconds. */

/****************************************************************************
 * Private Functions
 ****************************************************************************/
int vfs_acquire_fullpath(const char *path, char **fullpath)
{
    int ret;

    ret = vfs_normalize_path((const char *)NULL, path, fullpath);
    if (ret < 0) {
        set_errno(-ret);
        return ret;
    }
    return OK;
}

void vfs_files_initlist(FAR struct filelist *list)
{
    FAR struct file *tmp;

    tmp = kmm_zalloc(CONFIG_NFILE_DESCRIPTORS_PER_BLOCK * sizeof(FAR struct file));
    if (tmp == NULL) {
        return;
	}

    list->fl_files = kmm_zalloc(sizeof(FAR struct file *));
    if (list->fl_files == NULL) {
        kmm_free(tmp);
        return;
    }

    list->fl_files[0] = tmp;
    list->fl_rows = 1;
}

void vfs_minfd_verify(int *minfd)
{
    /* minfd should be a positive number,and 0,1,2 had be distributed to stdin,stdout,stderr */
    if (*minfd < FILE_START_FD) {
        *minfd = FILE_START_FD;
    }
}

void vfs_filep_path_set(FAR struct file *files, struct file *filep)
{
    files->f_path    = filep->f_path;
    files->f_relpath = filep->f_relpath;
}

#ifdef LOSCFG_NET_LWIP_SACK
int vfs_close_socket(int fd)
{
    int ret;

    ret = closesocket(fd);
    if (ret < 0) {
        return ERROR;
    }

    return ret;
}

int vfs_ioctl_socket(int fd, int req, va_list ap)
{
    UINTPTR arg = 0;

    arg = va_arg(ap, UINTPTR);
    /* Ruturn the errno instead of 'ERROR' for user application */
    return lwip_ioctl(fd, (long)req, (void *)arg);
}

int vfs_vfcntl_socket(int fd, int cmd, va_list ap)
{
    int val, ret;

    val = va_arg(ap, int);
    ret = lwip_fcntl(fd, cmd, val);
    if (ret < 0) {
        return ERROR;
    }

    return ret;
}

ssize_t vfs_read_socket(int fd, void *buf, size_t nbytes)
{
    ssize_t ret;

    /*
     * No.. If networking is enabled, read() is the same as recv() with
     * the flags parameter set to zero.
     */
    ret = recv(fd, buf, nbytes, 0);
    if (ret < 0) {
        return ERROR;
    }

    return ret;
}

ssize_t vfs_write_socket(int fd, const void *buf, size_t nbytes)
{
    ssize_t ret;

    /* Write to a socket descriptor is equivalent to send with flags == 0. */
    ret = send(fd, buf, nbytes, 0);
    if (ret < 0) {
        return ERROR;
    }

    return ret;
}
#else
int vfs_close_socket(int fd)
{
    return OK;
}

int vfs_vfcntl_socket(int fd, int cmd, va_list ap)
{
    return OK;
}

int vfs_ioctl_socket(int fd, int req, va_list ap)
{
    return OK;
}

ssize_t vfs_read_socket(int fd, void *buf, size_t nbytes)
{
    return 0;
}

ssize_t vfs_write_socket(int fd, const void *buf, size_t nbytes)
{
    return 0;
}
#endif

#ifdef LOSCFG_KERNEL_CONSOLE
ssize_t vfs_console_update_fd(int *fd)
{
    *fd = ConsoleUpdateFd();
    if (*fd < 0) {
        return -EBADF;
    }

    return OK;
}
#else
ssize_t vfs_console_update_fd(int *fd)
{
    return OK;
}
#endif

int vfs_inode_path_verify(const char *path)
{
    /* Don't insert a "/" empty directory */
    if (strcmp(path, "/") == 0) {
        set_errno(EEXIST);
        return ERROR;
    }
    return OK;
}

int inode_getpath(FAR struct inode *node, FAR char *path)
{
  return -ENOTSUP;
}

int vfs_seek_verify(struct inode *inode, int whence)
{
    if (inode == NULL) {
        return -EBADF;
    }

    if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
        return -EINVAL;
    }

    return OK;
}

bool vfs_disk_is_inuse(struct inode *inode)
{
#ifdef LOSCFG_DRIVER_DISK
    return IsBlockStatusReady(inode);
#else
    return true;
#endif
}

#if CONFIG_NFILE_DESCRIPTORS > 0
struct filelist tg_filelist;
#endif

struct filelist *nxsched_get_files(void)
{
    return &tg_filelist;
}

FAR struct inode *vfs_inode_zalloc(int namelen)
{
    FAR struct inode *node;

    node = (FAR struct inode *)LOS_MemAlloc(m_aucSysMem0, FSNODE_SIZE(namelen));
    if (node == NULL) {
        return NULL;
    }
    (void)memset_s(node, FSNODE_SIZE(namelen), 0, FSNODE_SIZE(namelen));

    return node;
}

int vfs_dup_process_file_struct_path(FAR struct file *filep1, char **fullpath,
                                 struct file *temp, struct inode_search_s *desc)
{
    size_t len;
	int ret;
    len = strlen(filep1->f_path);
    if ((len == 0) || (len >= PATH_MAX)) {
        return -EINVAL;
    }

    *fullpath = (char *)malloc(len + 1);
    if (*fullpath == NULL) {
        return -ENOMEM;
    }

    (void)memset_s(*fullpath, len + 1, 0, len + 1);

    /* Then clone the file structure */

    temp->f_priv = filep1->f_priv;

    (void)strncpy_s(*fullpath, len + 1, filep1->f_path, len);

    SETUP_SEARCH(desc, *fullpath, true);
    ret = inode_find(desc);
    if (ret < 0) {
        free(*fullpath);
        RELEASE_SEARCH(desc);
        return ret;
    }

    temp->f_path = *fullpath;
    temp->f_relpath = desc->relpath;

    return ret;
}

void vfs_release_for_dup_failed(char *fullpath, FAR struct inode *inode, struct inode_search_s *desc)
{
    free(fullpath);
    inode_release(inode);
    RELEASE_SEARCH(desc);
}

#ifdef LOSCFG_COMPAT_LINUX
int vfs_optimization_alloc_pollfd(struct pollfd **pollset, struct pollfd *pfd, int npfds, int *pfd_alloc_flag)
{
    /* use stack variable in order to avoid small memory allocation. */
    if (npfds <= POLL_STACK_CNT) {
        *pollset = pfd;
        (void)memset_s(*pollset, (unsigned int)npfds * sizeof(struct pollfd), 0,
            (unsigned int)npfds * sizeof(struct pollfd));
    } else {
        *pollset = (struct pollfd *)zalloc((unsigned int)npfds * sizeof(struct pollfd));
        if (*pollset == NULL) {
            set_errno(ENOMEM);
            return ERROR;
        }
        *pfd_alloc_flag = 1;
    }
    return OK;
}

void vfs_optimization_free_pollfd(struct pollfd *pollset, int pfd_alloc_flag)
{
    if (pfd_alloc_flag) {
        free(pollset);
    }
}
#endif

/*
 * If the readfds, writefds, and exceptfds arguments are all null pointers
 * and the timeout argument is a null pointer, this isn't permitted as LiteOS
 * doesn't support Signal machanism, so select() can't come back anymore.
 */
int vfs_select_wait(FAR struct timeval *timeout)
{
    if (timeout != NULL) {
        if ((timeout->tv_sec > (LONG_MAX / TIME_CONVERT_SCALE)) ||
            (timeout->tv_usec > LONG_MAX - (timeout->tv_sec * TIME_CONVERT_SCALE))) {
            /* Total us is overflow, use sleep instead of usleep */
            (void)sleep((unsigned int)timeout->tv_sec);
        } else {
            (void)usleep((unsigned int)((timeout->tv_sec * TIME_CONVERT_SCALE) + timeout->tv_usec));
        }

        return OK;
    }

    set_errno(EINVAL);
    return ERROR;
}

int vfs_mountpt_inode_unbind(FAR struct inode **mountpt_inode, FAR struct inode **blkdrvr_inode)
{
    int ret;

    if ((*mountpt_inode)->i_crefs == 1) {
        ret = (*mountpt_inode)->u.i_mops->unbind((*mountpt_inode)->i_private,
                                       blkdrvr_inode);
        if (ret < 0) {
            /* The inode is unhappy with the blkdrvr for some reason */
            return ret;
        } else if (ret > 0) {
            ret = -EBUSY;
            return ret;
        }
    } else {
        ret = -EBUSY;
	    return ret;
    }
    /* Successfully unbound */
    (*mountpt_inode)->i_private = NULL;
    (*mountpt_inode)->u.i_ops= (const struct file_operations_vfs *)NULL;
    (*mountpt_inode)->i_flags = 0;
    inode_release(*mountpt_inode);

    /*
     * Successfully unbound, remove the mountpoint inode from
     * the inode tree.  The inode will not be deleted yet because
     * there is still at least reference on it (from the mount)
     */
    if (*blkdrvr_inode) {
        (*blkdrvr_inode)->e_status = STAT_UNMOUNTED;
    }

    inode_semgive();
    return OK;
}

int vfs_rmdir_relpath_verify(const char *relpath)
{
    if ((relpath == NULL) || (strlen(relpath) == 0)) {
        return -EPERM;
    }

    return OK;
}

int isatty(int fd)
{
    (VOID)fd;
    return 0;
}

