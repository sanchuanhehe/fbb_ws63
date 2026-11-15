/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2019. All rights reserved.
 * Description: implementation for vfs commands.
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

#ifdef LOSCFG_SHELL
#include "los_typedef.h"
#include "shell.h"
#include "fs/fs.h"
#include "sys/stat.h"
#include "inode/inode.h"
#include "stdlib.h"
#include "unistd.h"
#include "fs_other.h"
#include "fcntl.h"
#include "sys/statfs.h"
#include "sys/mount.h"
#include "stdio.h"
#include "pthread.h"
#include "shcmd.h"
#include "securec.h"
#include "show.h"

typedef enum {
    RM_RECURSIVER,
    RM_FILE,
    RM_DIR,
    CP_FILE,
    CP_COUNT
} WildcardType;

#define ERROR_OUT_IF(condition, messageFunction, handler) do { \
    if (condition) { \
        messageFunction; \
        handler; \
    } \
} while (0)

STATIC INLINE VOID OsSetErr(INT32 errcode, const CHAR *errMessage)
{
    set_errno(errcode);
    PRINTK("%s errno:%d\n", errMessage, errno);
}

INT32 OsShellCmdDoChdir(const CHAR *path)
{
    DIR *dirent = NULL;
    INT32 ret;
    CHAR *fullpathBak = NULL;
    CHAR *fullpath = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    if (path == NULL) {
        LOS_TaskLock();
        PRINTK("%s\n", shellWorkingDirectory);
        LOS_TaskUnlock();
        return 0;
    }

    ERROR_OUT_IF(strlen(path) > PATH_MAX, OsSetErr(ENOTDIR, "cd error"), return -1);

    ret = vfs_normalize_path(shellWorkingDirectory, path, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "cd error"), return -1);

    fullpathBak = fullpath;

    dirent = opendir(fullpath);
    if (dirent == NULL) {
        free(fullpathBak);

        /* this is a not exist directory */
        PRINTK("no such file or directory\n");
        return -1;
    }

    /* close directory stream */
    (VOID)closedir(dirent);

    /* copy full path to working directory */
    LOS_TaskLock();
    ret = strncpy_s(shellWorkingDirectory, PATH_MAX, fullpath, strlen(fullpath));
    if (ret != EOK) {
        free(fullpathBak);
        LOS_TaskUnlock();
        return -1;
    }
    LOS_TaskUnlock();

    /* release normalize directory path name */
    free(fullpathBak);

    return 0;
}

INT32 OsShellCmdLs(INT32 argc, const CHAR **argv)
{
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    INT32 ret;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc > 1, PRINTK("ls or ls [DIRECTORY]\n"), return -1);

    if (argc == 0) {
        ls(shellWorkingDirectory);
        return 0;
    }

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "ls error"), return -1);

    ls(fullpath);
    free(fullpath);

    return 0;
}

INT32 OsShellCmdCd(INT32 argc, const CHAR **argv)
{
    if (argc == 0) {
        (VOID)OsShellCmdDoChdir("/");
        return 0;
    }

    (VOID)OsShellCmdDoChdir(argv[0]);

    return 0;
}

#define CAT_BUF_SIZE 512
#define CAT_TASK_PRIORITY 10
#define CAT_TASK_STACK_SIZE 0x3000

INT32 OsShellCmdDoCatShow(VOID *arg)
{
    CHAR buf[CAT_BUF_SIZE];
    ssize_t size;
    INT32 fd = -1;
    CHAR *fullpath = (CHAR *)arg;

    fd = open(fullpath, O_RDONLY, 0777);
    if (fd < 0) {
        PRINTK("cat open failed errno:%d\n", errno);
        free(fullpath);
        return -1;
    }

    do {
        (VOID)memset_s(buf, sizeof(buf), 0, sizeof(buf));
        size = read(fd, buf, CAT_BUF_SIZE);
        if (size < 0) {
            PRINTK("cat read failed errno:%d\n", errno);
            free(fullpath);
            (VOID)close(fd);
            return -1;
        }
        (VOID)write(1, buf, size);
        (VOID)LOS_TaskDelay(1);
    } while (size == CAT_BUF_SIZE);

    free(fullpath);
    (VOID)close(fd);
    return 0;
}

INT32 OsShellCmdCat(INT32 argc, const CHAR **argv)
{
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    CHAR *fullpathBak = NULL;
    struct inode_search_s desc;
    INT32 ret;
    UINT32 catTaskId;
    FAR struct inode *inode = NULL;
    TSK_INIT_PARAM_S initParam;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc != 1, PRINTK("cat [FILE]\n"), return -1);

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "cat error"), return -1);

    inode_semtake();
    fullpathBak = fullpath;
    SETUP_SEARCH(&desc, fullpath, FALSE);
    ret = inode_search(&desc);
    if (ret < 0) {
        set_errno(ENOENT);
        PRINTK("No such file or directory\n");
        inode_semgive();
        free(fullpathBak);
        RELEASE_SEARCH(&desc);
        return -1;
    }
    inode = desc.node;
    if (INODE_IS_BLOCK(inode) || INODE_IS_DRIVER(inode)) {
        set_errno(EPERM);
        PRINTK("Operation not permitted\n");
        inode_semgive();
        free(fullpathBak);
        RELEASE_SEARCH(&desc);
        return -1;
    }
    inode_semgive();
    (VOID)memset_s(&initParam, sizeof(initParam), 0, sizeof(TSK_INIT_PARAM_S));
    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsShellCmdDoCatShow;
    initParam.usTaskPrio   = CAT_TASK_PRIORITY;
    initParam.uwStackSize  = CAT_TASK_STACK_SIZE;
    initParam.pcName       = "shellcmd_cat";
    initParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    LOS_TASK_PARAM_INIT_ARG(initParam, fullpathBak);
    ret = (INT32)LOS_TaskCreate(&catTaskId, &initParam);
    RELEASE_SEARCH(&desc);
    return ret;
}

STATIC INT32 NfsMountRef(const CHAR *serverIpAndPath, const CHAR *mountPath,
                         UINT32 uid, UINT32 gid) __attribute__((weakref("nfs_mount")));

STATIC INLINE VOID OsPrintMountUsage(VOID)
{
    PRINTK("mount [DEVICE] [PATH] [NAME]\n");
}

INT32 OsShellCmdMount(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
#ifdef LOSCFG_FS_NFS
    UINT32 gid, uid;
#endif
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc < 3, OsPrintMountUsage(), return OS_FAIL);

    if ((strncmp(argv[0], "-t", 2) == 0) || (strncmp(argv[0], "-o", 2) == 0)) {
        filename = argv[2];
        ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
        ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "mount error"), return -1);

#ifdef LOSCFG_FS_NFS
        if (strncmp(argv[3], "nfs", 3) == 0) {
            if (argc <= 6) {
                uid = (argv[4] != NULL) ? (UINT32)strtoul(argv[4], (CHAR **)NULL, 0) : 0;
                gid = (argv[5] != NULL) ? (UINT32)strtoul(argv[5], (CHAR **)NULL, 0) : 0;

                if (NfsMountRef != NULL) {
                    ret = NfsMountRef(argv[1], fullpath, uid, gid);
                    if (ret != LOS_OK) {
                        PRINTK("mount -t [DEVICE] [PATH] [NAME]\n[DEVICE] format error, should be IP:PATH\n");
                    }
                } else {
                    PRINTK("can't find nfs_mount\n");
                }
                free(fullpath);
                return 0;
            }
        }
#endif

        if (strcmp(argv[1], "0") == 0) {
            ret = mount((const CHAR *)NULL, fullpath, argv[3], 0, NULL);
        } else {
            ret = mount(argv[1], fullpath, argv[3], 0, NULL);
        }
        if (ret != LOS_OK) {
            PRINTK("mount error %d\n", errno);
        } else {
            PRINTK("mount ok\n");
        }
    } else {
        filename = argv[1];
        ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
        ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "mount error"), return -1);

#ifdef LOSCFG_FS_NFS
        if (strncmp(argv[2], "nfs", 3) == 0) {
            if (argc <= 5) {
                uid = (argv[3] != NULL) ? (UINT32)strtoul(argv[3], (CHAR **)NULL, 0) : 0;
                gid = (argv[4] != NULL) ? (UINT32)strtoul(argv[4], (CHAR **)NULL, 0) : 0;

                if (NfsMountRef != NULL) {
                    ret = NfsMountRef(argv[0], fullpath, uid, gid);
                    if (ret != LOS_OK) {
                        PRINTK("mount [DEVICE] [PATH] [NAME]\n[DEVICE] format error, should be IP:PATH\n");
                    }
                } else {
                    PRINTK("can't find nfs_mount\n");
                }
                free(fullpath);
                return 0;
            }

            OsPrintMountUsage();
            free(fullpath);
            return 0;
        }
#endif

        if (strcmp(argv[0], "0") == 0) {
            ret = mount((const CHAR *)NULL, fullpath, argv[2], 0, NULL);
        } else {
            ret = mount(argv[0], fullpath, argv[2], 0, NULL);
        }
        if (ret != LOS_OK) {
            PRINTK("mount error %d\n", errno);
        } else {
            PRINTK("mount ok\n");
        }
    }

    free(fullpath);
    return 0;
}

INT32 OsShellCmdUmount(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    const CHAR *filename = NULL;
    CHAR *fullpath = NULL;
    CHAR *targetPath = NULL;
    size_t fullpathLen;
    CHAR *workpath = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }
    workpath = shellWorkingDirectory;

    ERROR_OUT_IF(argc == 0, PRINTK("umount [PATH]\n"), return 0);

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "umount error"), return -1);

    targetPath = fullpath;
    fullpathLen = strlen(fullpath);
    ret = strncmp(workpath, targetPath, fullpathLen);
    if (ret == 0) {
        workpath += fullpathLen;
        if ((*workpath == '/') || (*workpath == '\0')) {
            set_errno(EBUSY);
            PRINTK("Resource busy\n");
            free(fullpath);
            return -1;
        }
    }

    ret = umount(fullpath);
    free(fullpath);
    if (ret != LOS_OK) {
        PRINTK("umount error %d\n", errno);
        return 0;
    }

    PRINTK("umount ok\n");
    return 0;
}

INT32 OsShellCmdMkdir(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc != 1, PRINTK("mkdir [DIRECTORY]\n"), return 0);

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "mkdir error"), return -1);

    ret = mkdir(fullpath, 0);
    if (ret == -1) {
        PRINTK("mkdir error:%d\n", errno);
    }
    free(fullpath);
    return 0;
}

INT32 OsShellCmdPwd(INT32 argc, const CHAR **argv)
{
    CHAR buf[SHOW_MAX_LEN] = {0};
    DIR *dir = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc > 0, PRINTK("\nUsage: pwd\n"), return -1);

    dir = opendir(shellWorkingDirectory);
    if (dir == NULL) {
        PRINTK("pwd error %d\n", errno);
        return -1;
    }

    LOS_TaskLock();
    if (strncpy_s(buf, SHOW_MAX_LEN, shellWorkingDirectory, SHOW_MAX_LEN - 1) != EOK) {
        LOS_TaskUnlock();
        PRINTK("pwd error: strncpy_s error!\n");
        (VOID)closedir(dir);
        return -1;
    }
    LOS_TaskUnlock();

    PRINTK("%s\n", buf);
    (VOID)closedir(dir);
    return 0;
}

STATIC INLINE VOID OsPrintStatfsUsage(VOID)
{
    PRINTK("Usage  :\n");
    PRINTK("    statfs <path>\n");
    PRINTK("    path  : Mounted file system path that requires query information\n");
    PRINTK("Example:\n");
    PRINTK("    statfs /ramfs\n");
}

INT32 OsShellCmdStatfs(INT32 argc, const CHAR **argv)
{
    struct statfs sfs;
    INT32 result;
    unsigned long long totalSize, freeSize;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;

    if (argc != 1) {
        return -1;
    }
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    (VOID)memset_s(&sfs, sizeof(sfs), 0, sizeof(sfs));

    filename = argv[0];
    result = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(result < 0, OsSetErr(-result, "statfs error"), return -1);

    result = statfs(fullpath, &sfs);
    free(fullpath);
    if ((result != 0) || (sfs.f_type == 0)) {
        PRINTK("statfs failed! Invalid argument!\n");
        OsPrintStatfsUsage();
        return -1;
    }

    totalSize = (unsigned long long)sfs.f_bsize * sfs.f_blocks;
    freeSize = (unsigned long long)sfs.f_bsize * sfs.f_bfree;

    PRINTK("statfs got:\n f_type     = %lu\n cluster_size   = %lu\n", sfs.f_type, sfs.f_bsize);
    PRINTK(" total_clusters = %llu\n free_clusters  = %llu\n", sfs.f_blocks, sfs.f_bfree);
    PRINTK(" avail_clusters = %llu\n f_namelen    = %lu\n", sfs.f_bavail, sfs.f_namelen);
    PRINTK("\n%s\n total size: %4llu Bytes\n free  size: %4llu Bytes\n", argv[0], totalSize, freeSize);

    return 0;
}

INT32 OsShellCmdTouch(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    INT32 fd = -1;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc != 1, PRINTK("touch [FILE]\n"), return -1);

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "touch error"), return -1);

    fd = open(fullpath, O_RDWR | O_CREAT, 0666);
    free(fullpath);
    if (fd == -1) {
        PRINTK("touch error %d\n", errno);
        return -1;
    }

    (VOID)close(fd);
    return 0;
}

#define CP_BUF_SIZE 4096
STATIC INT32 OsShellCmdDoCp(const CHAR *srcFilepath, const CHAR *dstFilename)
{
    INT32  ret;
    CHAR *srcFullpath = NULL;
    CHAR *dstFullpath = NULL;
    const CHAR *srcFilename = NULL;
    CHAR *dstFilepath = NULL;
    CHAR *buf = NULL;
    const CHAR *filename = NULL;
    ssize_t readSize, writeSize;
    INT32 srcFd = -1;
    INT32 dstFd = -1;
    struct stat statBuf;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    buf = (CHAR *)malloc(CP_BUF_SIZE);
    if (buf == NULL) {
        PRINTK("cp error: Out of memory!\n");
        return -1;
    }

    /* Get source fullpath. */
    ret = vfs_normalize_path(shellWorkingDirectory, srcFilepath, &srcFullpath);
    if (ret < 0) {
        set_errno(-ret);
        PRINTK("cp error:%d\n", errno);
        free(buf);
        return -1;
    }

    /* Is source path exist? */
    ret = stat(srcFullpath, &statBuf);
    if (ret == -1) {
        PRINTK("cp %s error:%d\n", srcFullpath, errno);
        goto ERROUT_WITH_SRC_PATH;
    }

    /* Is source path a directory? */
    if (S_ISDIR(statBuf.st_mode)) {
        PRINTK("cp %s error: Source file can't be a directory.\n", srcFullpath);
        goto ERROUT_WITH_SRC_PATH;
    }

    /* Get dest fullpath. */
    dstFullpath = strdup(dstFilename);
    if (dstFullpath == NULL) {
        PRINTK("cp error: Out of memory.\n");
        goto ERROUT_WITH_SRC_PATH;
    }

    /* Is dest path exist? */
    ret = stat(dstFullpath, &statBuf);
    if (ret == 0) {
        /* Is dest path a directory? */
        if (S_ISDIR(statBuf.st_mode)) {
            /* Get source file name without '/'. */
            srcFilename = srcFilepath;
            while (1) {
                filename = strchr(srcFilename, '/');
                if (filename == NULL) {
                    break;
                }
                srcFilename = filename + 1;
            }

            /* Add the source file after dest path. */
            ret = vfs_normalize_path(dstFullpath, srcFilename, &dstFilepath);
            if (ret < 0) {
                set_errno(-ret);
                PRINTK("cp error.%d.\n", errno);
                goto ERROUT_WITH_PATH;
            }
            free(dstFullpath);
            dstFullpath = dstFilepath;
        }
    }

    /* Is dest file same as source file? */
    if (strcmp(srcFullpath, dstFullpath) == 0) {
        PRINTK("cp error: '%s' and '%s' are the same file\n", srcFullpath, dstFullpath);
        goto ERROUT_WITH_PATH;
    }

    /* Copy begins. */
    srcFd = open(srcFullpath, O_RDONLY | O_NOFOLLOW, 0777);
    if (srcFd < 0) {
        PRINTK("cp error: can't open %s. errno:%d.\n", srcFullpath, errno);
        goto ERROUT_WITH_PATH;
    }

    /* Open dest with O_NOFOLLOW to avoid symlink attack (TOCTOU) */
    dstFd = open(dstFullpath, O_CREAT | O_WRONLY | O_TRUNC | O_NOFOLLOW, 0777);
    if (dstFd < 0) {
        PRINTK("cp error: can't open %s. errno%d\n", dstFullpath, errno);
        goto ERROUT_WITH_SRC_FD;
    }

    do {
        (VOID)memset_s(buf, CP_BUF_SIZE, 0, CP_BUF_SIZE);
        readSize = read(srcFd, buf, CP_BUF_SIZE);
        if (readSize == EOF) {
            PRINTK("cp %s %s failed.%d.\n", srcFullpath, dstFullpath, errno);
            goto ERROUT_WITH_FD;
        }
        writeSize = write(dstFd, buf, readSize);
        if (writeSize != readSize) {
            PRINTK("cp %s %s failed. Check space left on device.\n", srcFullpath, dstFullpath);
            goto ERROUT_WITH_FD;
        }
    } while (readSize == CP_BUF_SIZE);

    /* Release resource. */
    free(buf);
    free(srcFullpath);
    free(dstFullpath);
    (VOID)close(srcFd);
    (VOID)close(dstFd);
    return LOS_OK;

ERROUT_WITH_FD:
    (VOID)close(dstFd);
ERROUT_WITH_SRC_FD:
    (VOID)close(srcFd);
ERROUT_WITH_PATH:
    free(dstFullpath);
ERROUT_WITH_SRC_PATH:
    free(srcFullpath);
    free(buf);
    return -1;
}

/* The separator and EOF for a directory fullpath: '/'and '\0' */
#define SEPARATOR_EOF_LEN 2

STATIC INT32 OsShellCmdDoRmdir(const CHAR *pathname)
{
    struct dirent *dirent = NULL;
    struct stat statInfo;
    DIR *d = NULL;
    CHAR *fullpath = NULL;
    INT32 ret;

    (VOID)memset_s(&statInfo, sizeof(statInfo), 0, sizeof(struct stat));
    if (stat(pathname, &statInfo) != 0) {
        return -1;
    }

    if (S_ISREG(statInfo.st_mode)) {
        return unlink(pathname);
    }
    d = opendir(pathname);
    while (1) {
        dirent = readdir(d);
        if (dirent == NULL) {
            break;
        }
        if (strcmp(dirent->d_name, "..") && strcmp(dirent->d_name, ".")) {
            size_t fullpathBufSize = strlen(pathname) + strlen(dirent->d_name) + SEPARATOR_EOF_LEN;
            fullpath = (CHAR *)malloc(fullpathBufSize);
            if (fullpath == NULL) {
                PRINTK("malloc failure!\n");
                (VOID)closedir(d);
                return -1;
            }
            ret = snprintf_s(fullpath, fullpathBufSize, fullpathBufSize - 1, "%s/%s", pathname, dirent->d_name);
            if (ret < 0) {
                PRINTK("name is too long!\n");
                free(fullpath);
                (VOID)closedir(d);
                return -1;
            }
            (VOID)OsShellCmdDoRmdir(fullpath);
            free(fullpath);
        }
    }
    (VOID)closedir(d);
    return rmdir(pathname);
}

/* Wildcard matching operations */
STATIC INT32 OsWildcardMatch(const CHAR *src, const CHAR *filename)
{
    INT32 ret;

    if (*src != '\0') {
        if (*filename == '*') {
            while ((*filename == '*') || (*filename == '?')) {
                filename++;
            }

            if (*filename == '\0') {
                return 0;
            }

            while ((*src != '\0') && !(*src == *filename)) {
                src++;
            }

            if (*src == '\0') {
                return -1;
            }

            ret = OsWildcardMatch(src, filename);

            while ((ret != 0) && (*(++src) != '\0')) {
                if (*src == *filename) {
                    ret = OsWildcardMatch(src, filename);
                }
            }
            return ret;
        } else {
            if ((*src == *filename) || (*filename == '?')) {
                return OsWildcardMatch(++src, ++filename);
            }
            return -1;
        }
    }

    while (*filename != '\0') {
        if (*filename != '*') {
            return -1;
        }
        filename++;
    }
    return 0;
}

/* To determine whether a wildcard character exists in a path */
STATIC INT32 OsIsContainersWildcard(const CHAR *filename)
{
    while (*filename != '\0') {
        if ((*filename == '*') || (*filename == '?')) {
            return 1;
        }
        filename++;
    }
    return 0;
}

/* Delete a matching file or directory */
STATIC INT32 OsWildcardDeleteFileOrDir(const CHAR *fullpath, WildcardType mark)
{
    INT32 ret;

    switch (mark) {
        case RM_RECURSIVER:
            ret = OsShellCmdDoRmdir(fullpath);
            break;
        case RM_FILE:
            ret = unlink(fullpath);
            break;
        case RM_DIR:
            ret = rmdir(fullpath);
            break;
        default:
            return VFS_ERROR;
    }
    if (ret == -1) {
        PRINTK("%s  ", fullpath);
        PRINTK("rm/rmdir error! %d\n", errno);
        return ret;
    }

    PRINTK("%s match successful!delete!\n", fullpath);
    return 0;
}

/* Split the path with wildcard characters */
STATIC CHAR *OsWildcardSplitPath(CHAR *fullpath, CHAR **handle, CHAR **wait)
{
    size_t n;
    size_t a = 0;
    size_t b = 0;
    size_t len = strlen(fullpath);

    for (n = 0; n < len; n++) {
        if (fullpath[n] == '/') {
            if (b != 0) {
                fullpath[n] = '\0';
                *wait = fullpath + n + 1;
                break;
            }
            a = n;
        } else if ((fullpath[n] == '*') || (fullpath[n] == '?')) {
            b = n;
            fullpath[a] = '\0';
            if (a == 0) {
                *handle = fullpath + a + 1;
                continue;
            }
            *handle = fullpath + a + 1;
        }
    }
    return fullpath;
}

/* Handling entry of the path with wildcard characters */
STATIC INT32 OsWildcardExtractDirectory(CHAR *fullpath, VOID *dst, WildcardType mark)
{
    CHAR separator[] = "/";
    CHAR src[PATH_MAX] = {0};
    struct dirent *dirent = NULL;
    CHAR *f = NULL;
    CHAR *s = NULL;
    CHAR *t = NULL;
    INT32 ret = 0;
    DIR *d = NULL;
    struct stat statBuf;

    f = OsWildcardSplitPath(fullpath, &s, &t);

    if (s == NULL) {
        if (mark == CP_FILE) {
            ret = OsShellCmdDoCp(fullpath, dst);
        } else if (mark == CP_COUNT) {
            ret = stat(fullpath, &statBuf);
            if ((ret == 0) && S_ISREG(statBuf.st_mode)) {
                (*(INT32 *)dst)++;
            }
        } else {
            ret = OsWildcardDeleteFileOrDir(fullpath, mark);
        }
        return ret;
    }

    d = (*f == '\0') ? opendir("/") : opendir(f);
    if (d == NULL) {
        PRINTK("opendir error %d\n", errno);
        return VFS_ERROR;
    }

    while (1) {
        dirent = readdir(d);
        if (dirent == NULL) {
            break;
        }

        ret = strcpy_s(src, PATH_MAX, f);
        if (ret != EOK) {
            goto CLOSEDIR_OUT;
        }

        ret = OsWildcardMatch(dirent->d_name, s);
        if (ret == 0) {
            ret = strcat_s(src, sizeof(src), separator);
            if (ret != EOK) {
                goto CLOSEDIR_OUT;
            }
            ret = strcat_s(src, sizeof(src), dirent->d_name);
            if (ret != EOK) {
                goto CLOSEDIR_OUT;
            }
            if (t == NULL) {
                if (mark == CP_FILE) {
                    ret = OsShellCmdDoCp(src, dst);
                } else if (mark == CP_COUNT) {
                    ret = stat(src, &statBuf);
                    if ((ret == 0) && S_ISREG(statBuf.st_mode)) {
                        (*(INT32 *)dst)++;
                        if ((*(INT32 *)dst) > 1) {
                            break;
                        }
                    }
                } else {
                    ret = OsWildcardDeleteFileOrDir(src, mark);
                }
            } else {
                ret = strcat_s(src, sizeof(src), separator);
                if (ret != EOK) {
                    goto CLOSEDIR_OUT;
                }
                ret = strcat_s(src, sizeof(src), t);
                if (ret != EOK) {
                    goto CLOSEDIR_OUT;
                }
                ret = OsWildcardExtractDirectory(src, dst, mark);
                if ((mark == CP_COUNT) && ((*(INT32 *)dst) > 1)) {
                    break;
                }
            }
        }
    }
    (VOID)closedir(d);
    return ret;
CLOSEDIR_OUT:
    (VOID)closedir(d);
    return VFS_ERROR;
}

INT32 OsShellCmdCp(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    const CHAR *src = NULL;
    const CHAR *dst = NULL;
    CHAR *srcFullpath = NULL;
    CHAR *dstFullpath = NULL;
    struct stat statBuf;
    INT32 count = 0;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc < 2, PRINTK("cp [SOURCEFILE] [DESTFILE]\n"), return -1);

    src = argv[0];
    dst = argv[1];

    /* Get source fullpath. */
    ret = vfs_normalize_path(shellWorkingDirectory, src, &srcFullpath);
    if (ret < 0) {
        set_errno(-ret);
        PRINTK("cp error:%d\n", errno);
        return -1;
    }

    if (src[strlen(src) - 1] == '/') {
        PRINTK("cp %s error: Source file can't be a directory.\n", src);
        goto ERROUT_WITH_SRC_PATH;
    }

    /* Get dest fullpath. */
    ret = vfs_normalize_path(shellWorkingDirectory, dst, &dstFullpath);
    if (ret < 0) {
        set_errno(-ret);
        PRINTK("cp error: can't open %s. %d\n", dst, errno);
        goto ERROUT_WITH_SRC_PATH;
    }

    /* Is dest path exist? */
    ret = stat(dstFullpath, &statBuf);
    if (ret < 0) {
        /* Is dest path a directory? */
        if (dst[strlen(dst) - 1] == '/') {
            PRINTK("cp error: %s, %d.\n", dstFullpath, errno);
            goto ERROUT_WITH_PATH;
        }
    } else {
        if (S_ISREG(statBuf.st_mode) && (dst[strlen(dst) - 1] == '/')) {
            PRINTK("cp error: %s is not a directory.\n", dstFullpath);
            goto ERROUT_WITH_PATH;
        }
    }

    if (OsIsContainersWildcard(srcFullpath)) {
        if ((ret < 0) || S_ISREG(statBuf.st_mode)) {
            CHAR *srcCopy = strdup(srcFullpath);
            if (srcCopy == NULL) {
                PRINTK("cp error : Out of memory.\n");
                goto ERROUT_WITH_PATH;
            }
            (VOID)OsWildcardExtractDirectory(srcCopy, &count, CP_COUNT);
            free(srcCopy);
            if (count > 1) {
                PRINTK("cp error : %s is not a directory.\n", dstFullpath);
                goto ERROUT_WITH_PATH;
            }
        }
        ret = OsWildcardExtractDirectory(srcFullpath, dstFullpath, CP_FILE);
    } else {
        ret = OsShellCmdDoCp(srcFullpath, dstFullpath);
    }
    free(dstFullpath);
    free(srcFullpath);
    return ret;

ERROUT_WITH_PATH:
    free(dstFullpath);
ERROUT_WITH_SRC_PATH:
    free(srcFullpath);
    return VFS_ERROR;
}

STATIC INLINE VOID OsPrintRmUsage(VOID)
{
    PRINTK("rm [FILE] or rm [-r/-R] [FILE]\n");
}

INT32 OsShellCmdRm(INT32 argc, const CHAR **argv)
{
    INT32 ret = 0;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF((argc != 1) && (argc != 2), OsPrintRmUsage(), return -1);

    if (argc == 2) {
        ERROR_OUT_IF((strcmp(argv[0], "-r") != 0) && (strcmp(argv[0], "-R") != 0), OsPrintRmUsage(), return -1);

        filename = argv[1];
        ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
        ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "rm error"), return -1);

        if (OsIsContainersWildcard(fullpath)) {
            ret = OsWildcardExtractDirectory(fullpath, NULL, RM_RECURSIVER);
        } else {
            ret = OsShellCmdDoRmdir(fullpath);
        }
    } else {
        filename = argv[0];
        ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
        ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "rm error"), return -1);

        if (OsIsContainersWildcard(fullpath)) {
            ret = OsWildcardExtractDirectory(fullpath, NULL, RM_FILE);
        } else {
            ret = unlink(fullpath);
        }
    }
    if (ret == -1) {
        PRINTK("rm error %d\n", errno);
    }
    free(fullpath);
    return 0;
}

INT32 OsShellCmdRmdir(INT32 argc, const CHAR **argv)
{
    INT32 ret;
    CHAR *fullpath = NULL;
    const CHAR *filename = NULL;
    CHAR *shellWorkingDirectory = OsShellGetWorkingDirectory();
    if (shellWorkingDirectory == NULL) {
        return -1;
    }

    ERROR_OUT_IF(argc == 0, PRINTK("rmdir [DIRECTORY]\n"), return -1);

    filename = argv[0];
    ret = vfs_normalize_path(shellWorkingDirectory, filename, &fullpath);
    ERROR_OUT_IF(ret < 0, OsSetErr(-ret, "rmdir error"), return -1);

    if (OsIsContainersWildcard(fullpath)) {
        ret = OsWildcardExtractDirectory(fullpath, NULL, RM_DIR);
    } else {
        ret = rmdir(fullpath);
    }
    free(fullpath);

    if (ret == -1) {
        PRINTK("rmdir error %d\n", errno);
    }

    return 0;
}

INT32 OsShellCmdSync(INT32 argc, const CHAR **argv)
{
    ERROR_OUT_IF(argc > 0, PRINTK("\nUsage: sync\n"), return -1);

    sync();
    return 0;
}

INT32 OsShellCmdLsfd(INT32 argc, const CHAR **argv)
{
    ERROR_OUT_IF(argc > 0, PRINTK("\nUsage: lsfd\n"), return -1);

    lsfd();

    return 0;
}

SHELLCMD_ENTRY(lsfd_shellcmd, CMD_TYPE_EX, "lsfd", XARGS, (CmdCallBackFunc)OsShellCmdLsfd);
SHELLCMD_ENTRY(ls_shellcmd, CMD_TYPE_EX, "ls", XARGS, (CmdCallBackFunc)OsShellCmdLs);
SHELLCMD_ENTRY(pwd_shellcmd, CMD_TYPE_EX, "pwd", XARGS, (CmdCallBackFunc)OsShellCmdPwd);
SHELLCMD_ENTRY(cd_shellcmd, CMD_TYPE_EX, "cd", XARGS, (CmdCallBackFunc)OsShellCmdCd);
SHELLCMD_ENTRY(cat_shellcmd, CMD_TYPE_EX, "cat", XARGS, (CmdCallBackFunc)OsShellCmdCat);
SHELLCMD_ENTRY(rm_shellcmd, CMD_TYPE_EX, "rm", XARGS, (CmdCallBackFunc)OsShellCmdRm);
SHELLCMD_ENTRY(rmdir_shellcmd, CMD_TYPE_EX, "rmdir", XARGS, (CmdCallBackFunc)OsShellCmdRmdir);
SHELLCMD_ENTRY(mkdir_shellcmd, CMD_TYPE_EX, "mkdir", XARGS, (CmdCallBackFunc)OsShellCmdMkdir);
#if (defined(LOSCFG_FS_FAT))
SHELLCMD_ENTRY(sync_shellcmd, CMD_TYPE_EX, "sync", XARGS, (CmdCallBackFunc)OsShellCmdSync);
#endif
#if (defined(LOSCFG_FS_FAT) || defined(LOSCFG_FS_RAMFS) || defined(LOSCFG_FS_YAFFS) || \
    defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_ROMFS) || defined(LOSCFG_FS_LITTLEFS))
SHELLCMD_ENTRY(statfs_shellcmd, CMD_TYPE_EX, "statfs", XARGS, (CmdCallBackFunc)OsShellCmdStatfs);
SHELLCMD_ENTRY(mount_shellcmd, CMD_TYPE_EX, "mount", XARGS, (CmdCallBackFunc)OsShellCmdMount);
SHELLCMD_ENTRY(umount_shellcmd, CMD_TYPE_EX, "umount", XARGS, (CmdCallBackFunc)OsShellCmdUmount);
#endif
#if (defined(LOSCFG_FS_FAT) || defined(LOSCFG_FS_RAMFS) || defined(LOSCFG_FS_YAFFS) || \
    defined(LOSCFG_FS_JFFS) || defined(LOSCFG_FS_LITTLEFS))
SHELLCMD_ENTRY(touch_shellcmd, CMD_TYPE_EX, "touch", XARGS, (CmdCallBackFunc)OsShellCmdTouch);
SHELLCMD_ENTRY(cp_shellcmd, CMD_TYPE_EX, "cp", XARGS, (CmdCallBackFunc)OsShellCmdCp);
#endif

#endif
