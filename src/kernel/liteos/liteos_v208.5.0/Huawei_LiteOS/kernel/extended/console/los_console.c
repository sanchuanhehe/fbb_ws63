/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: System Console Implementation
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

#include "console_pri.h"
#include "fcntl.h"
#ifdef CONFIG_FILE_MODE
#include "stdarg.h"
#endif
#include "unistd.h"
#include "sys/ioctl.h"
#include "securec.h"
#include "inode/inode.h"
#ifdef LOSCFG_SHELL_DMESG
#include "dmesg_pri.h"
#endif
#ifdef LOSCFG_SHELL
#include "shcmd.h"
#include "shell_pri.h"
#endif
#include "los_task_pri.h"
#if defined(LOSCFG_DRIVERS_USB_SERIAL_GADGET) || defined(LOSCFG_DRIVERS_USB_ETH_SER_GADGET)
#include "implementation/usb_api_pri.h"
#endif

#define EACH_CHAR 1
#define RESERVED_NUM 3

STATIC BOOL SetSerialBlock(const CONSOLE_CB *consoleCB);
#ifdef LOSCFG_NET_TELNET
STATIC BOOL SetTelnetBlock(const CONSOLE_CB *consoleCB);
#endif

STATIC INT32 ConsoleFdGet(VOID);

/* Inter-module variable */
STATIC UINT32 ConsoleSendTask(const VOID *param);

/*
 * The taskId of the osMain task is LOSCFG_BASE_CORE_TSK_LIMIT.
 * Therefore, the array size must be set to LOSCFG_BASE_CORE_TSK_LIMIT + 1.
 */
STATIC UINT8 g_taskConsoleIDArray[LOSCFG_BASE_CORE_TSK_LIMIT + 1];
STATIC SPIN_LOCK_INIT(g_consoleSpin);
STATIC SPIN_LOCK_INIT(g_consoleWriteSpin);

#define CONSOLE_RINGBUF_EVENT     0x02U
#define CONSOLE_SEND_TASK_EXIT    0x04U
#define CONSOLE_SEND_TASK_RUNNING 0x10U

#define CONSOLE_EXC_OUT_BUF_SIZE  100

CONSOLE_CB *g_console[CONSOLE_NUM];
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef enum {
    CONSOLE_UNUSED = 0,
    CONSOLE_INITIALIZING,
    CONSOLE_READY,
    CONSOLE_DEINITIALIZING,
    CONSOLE_DISABLE,
} ConsoleStatus;

STATIC ConsoleStatus g_status[CONSOLE_NUM] = {0};

/*
 * acquire uart driver function and filep of /dev/console,
 * then store uart driver function in *filepOps
 * and store filep of /dev/console in *privFilep.
 */
INT32 GetFilepOps(const struct file *filep, struct file **privFilep, const struct file_operations_vfs **filepOps)
{
    INT32 ret;

    if ((filep == NULL) || (filep->f_inode == NULL) || (filep->f_inode->i_private == NULL)) {
        ret = EINVAL;
        goto ERROUT;
    }

    /* to find console device's filep(now it is *privFilep) through i_private */
    *privFilep = (struct file *)filep->f_inode->i_private;
    if (((*privFilep)->f_inode == NULL) || ((*privFilep)->f_inode->u.i_ops == NULL)) {
        ret = EINVAL;
        goto ERROUT;
    }

    /* to find uart driver operation function through u.i_ops */
    *filepOps = (*privFilep)->f_inode->u.i_ops;

    return ENOERR;
ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

STATIC INT32 ConsoleUpdateTermiosClFlag(INT32 fd, struct termios *termios, UINT32 mask, BOOL isSet)
{
    struct file *filep = NULL;
    CONSOLE_CB *consoleCB = NULL;
    int ret;

    if ((fd >= STDIN_FILENO) && (fd <= STDERR_FILENO)) {
        fd = ConsoleFdGet();
        if (fd < STDIN_FILENO) {
            return -EBADF;
        }
    }

    ret = fs_getfilep(fd, &filep);
    if (ret < 0) {
        return -EPERM;
    }

    consoleCB = (CONSOLE_CB *)filep->f_priv;
    if (consoleCB == NULL) {
        return -EFAULT;
    }

    if (isSet) {
        termios->c_lflag |= mask;
        consoleCB->consoleTermios.c_lflag |= mask;
    } else {
        termios->c_lflag &= ~mask;
        consoleCB->consoleTermios.c_lflag &= ~mask;
    }
    return LOS_OK;
}

STATIC UINT32 ConsoleRefcountGet(const CONSOLE_CB *consoleCB)
{
    return consoleCB->refCount;
}

STATIC VOID ConsoleRefcountSet(CONSOLE_CB *consoleCB, BOOL flag)
{
    if (flag == TRUE) {
        ++(consoleCB->refCount);
    } else {
        --(consoleCB->refCount);
    }
}

BOOL IsConsoleOccupied(const CONSOLE_CB *consoleCB)
{
    if (ConsoleRefcountGet(consoleCB) != FALSE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

STATIC INT32 ConsoleCtrlCaptureLine(CONSOLE_CB *consoleCB)
{
    struct termios *consoleTermios = NULL;
    INT32 ret;
    UINT32 intSave;

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    consoleTermios = &consoleCB->consoleTermios;
    ret = ConsoleUpdateTermiosClFlag(consoleCB->fd, consoleTermios, (ICANON | ECHO), TRUE);
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    return ret;
}

STATIC INT32 ConsoleCtrlCaptureChar(CONSOLE_CB *consoleCB)
{
    struct termios *consoleTermios = NULL;
    INT32 ret;
    UINT32 intSave;

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    consoleTermios = &consoleCB->consoleTermios;
    ret = ConsoleUpdateTermiosClFlag(consoleCB->fd, consoleTermios, (ICANON | ECHO), FALSE);
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    return ret;
}

STATIC INT32 ConsoleCtrlRightsCapture(CONSOLE_CB *consoleCB)
{
    (VOID)LOS_SemPend(consoleCB->consoleSem, LOS_WAIT_FOREVER);
    if (ConsoleRefcountGet(consoleCB) == 0) {
        (VOID)LOS_TaskSuspend(consoleCB->shellEntryId);
    }
    ConsoleRefcountSet(consoleCB, TRUE);
    (VOID)LOS_SemPost(consoleCB->consoleSem);
    return LOS_OK;
}

STATIC INT32 ConsoleCtrlRightsRelease(CONSOLE_CB *consoleCB)
{
    (VOID)LOS_SemPend(consoleCB->consoleSem, LOS_WAIT_FOREVER);

    if (ConsoleRefcountGet(consoleCB) == 0) {
        PRINT_ERR("console is free\n");
        (VOID)LOS_SemPost(consoleCB->consoleSem);
        return LOS_NOK;
    } else {
        ConsoleRefcountSet(consoleCB, FALSE);
        if (ConsoleRefcountGet(consoleCB) == 0) {
            (VOID)LOS_TaskResume(consoleCB->shellEntryId);
        }
    }
    (VOID)LOS_SemPost(consoleCB->consoleSem);
    return LOS_OK;
}

STATIC CONSOLE_CB *OsGetConsoleByDevice(const CHAR *deviceName)
{
    INT32 ret;
    CHAR *fullpath = NULL;
    struct inode *inode = NULL;
    struct inode_search_s desc;

    ret = vfs_normalize_path(NULL, deviceName, &fullpath);
    if (ret < 0) {
        set_errno(EINVAL);
        return NULL;
    }

    SETUP_SEARCH(&desc, fullpath, false);
    ret = inode_find(&desc);
    free(fullpath);
    if (ret < 0) {
        ret = ENOENT;
        goto errout;
    }

    inode = desc.node;
    if (g_console[CONSOLE_SERIAL - 1]->devInode == inode) {
        inode_release(inode);
        RELEASE_SEARCH(&desc);
        return g_console[CONSOLE_SERIAL - 1];
    } else if (g_console[CONSOLE_TELNET - 1]->devInode == inode) {
        inode_release(inode);
        RELEASE_SEARCH(&desc);
        return g_console[CONSOLE_TELNET - 1];
    } else {
        inode_release(inode);
        goto errout;
    }

errout:
    set_errno(ret);
    RELEASE_SEARCH(&desc);
    return NULL;
}

STATIC INT32 OsGetConsoleID(const CHAR *deviceName)
{
    if ((deviceName != NULL) &&
        (strlen(deviceName) == strlen(SERIAL)) &&
        (!strncmp(deviceName, SERIAL, strlen(SERIAL)))) {
        return CONSOLE_SERIAL;
    }
#ifdef LOSCFG_NET_TELNET
    else if ((deviceName != NULL) &&
             (strlen(deviceName) == strlen(TELNET)) &&
             (!strncmp(deviceName, TELNET, strlen(TELNET)))) {
        return CONSOLE_TELNET;
    }
#endif
    return -1;
}

STATIC INT32 OsConsoleFullpathToID(const CHAR *fullpath)
{
#define CONSOLE_SERIAL_1 "/dev/console1"
#define CONSOLE_TELNET_2 "/dev/console2"

    size_t len;

    if (fullpath == NULL) {
        return -1;
    }

    len = strlen(fullpath);
    if ((len == strlen(CONSOLE_SERIAL_1)) &&
        (!strncmp(fullpath, CONSOLE_SERIAL_1, strlen(CONSOLE_SERIAL_1)))) {
        return CONSOLE_SERIAL;
    }
#ifdef LOSCFG_NET_TELNET
    else if ((len == strlen(CONSOLE_TELNET_2)) &&
             (!strncmp(fullpath, CONSOLE_TELNET_2, strlen(CONSOLE_TELNET_2)))) {
        return CONSOLE_TELNET;
    }
#endif
    return -1;
}

STATIC BOOL ConsoleFifoEmpty(const CONSOLE_CB *console)
{
    if (console->fifoOut == console->fifoIn) {
        return TRUE;
    }
    return FALSE;
}

STATIC VOID ConsoleFifoClearup(CONSOLE_CB *console)
{
    console->fifoOut = 0;
    console->fifoIn = 0;
    (VOID)memset(console->fifo, 0, sizeof(console->fifo));
}

STATIC VOID ConsoleFifoLenUpdate(CONSOLE_CB *console)
{
    console->currentLen = console->fifoIn - console->fifoOut;
}

STATIC INT32 ConsoleReadFifo(CHAR *buffer, CONSOLE_CB *console, size_t bufLen)
{
    INT32 ret;
    UINT32 readNum;

    readNum = (UINT32)MIN(bufLen, console->currentLen);
    ret = memcpy_s(buffer, bufLen, console->fifo + console->fifoOut, readNum);
    if (ret != EOK) {
        PRINTK("%s,%d memcpy_s failed\n", __FUNCTION__, __LINE__);
        return -1;
    }
    console->fifoOut += readNum;
    if (ConsoleFifoEmpty(console)) {
        ConsoleFifoClearup(console);
    }
    ConsoleFifoLenUpdate(console);
    return (INT32)readNum;
}

INT32 FilepOpen(struct file *filep, const struct file_operations_vfs *fops)
{
    INT32 ret;
    if (fops->open == NULL) {
        return -EFAULT;
    }

    /*
     * adopt uart open function to open filep (filep is
     * corresponding to filep of /dev/console)
     */
    ret = fops->open(filep);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

STATIC INLINE VOID UserEndOfRead(CONSOLE_CB *consoleCB, struct file *filep,
                                 const struct file_operations_vfs *fops)
{
    CHAR ch;
    if (consoleCB->consoleTermios.c_lflag & ECHO) {
        ch = '\r';
        (VOID)fops->write(filep, &ch, 1);
    }
    consoleCB->fifo[consoleCB->fifoIn++] = '\n';
    consoleCB->fifo[consoleCB->fifoIn] = '\0';
    consoleCB->currentLen = consoleCB->fifoIn;
}
STATIC INT32 UserFilepRead(CONSOLE_CB *consoleCB, struct file *filep, const struct file_operations_vfs *fops,
                           CHAR *buffer, size_t bufLen)
{
    INT32 ret;
    CHAR ch;

    if (fops->read == NULL) {
        return -EFAULT;
    }

    /*
     * adopt uart read function to read data from filep
     * and write data to buffer (filep is
     * corresponding to filep of /dev/console)
     */
    if ((consoleCB->consoleTermios.c_lflag & ICANON) == 0) {
        ret = fops->read(filep, buffer, bufLen);
        if (ret < 0) {
            return -EPERM;
        }
        return ret;
    }
    /* store data to console buffer,  read data and stored data into console fifo */
    if (consoleCB->currentLen == 0) {
        while (1) {
            ret = fops->read(filep, &ch, EACH_CHAR);
            if (ret <= 0) {
                return ret;
            }

            if (ch == '\r') {
                ch = '\n';
            }

            if (consoleCB->consoleTermios.c_lflag & ECHO) {
                (VOID)fops->write(filep, &ch, EACH_CHAR);
            }

            /*
             * store what you input
             * 3 : fifoIn should less than (CONSOLE_FIFO_SIZE - EACH_CHAR) - 2(its space of '\n' and '\0')
             */
            if ((ret == EACH_CHAR) && (ch != '\n') && (consoleCB->fifoIn <= (CONSOLE_FIFO_SIZE - RESERVED_NUM))) {
                consoleCB->fifo[consoleCB->fifoIn] = (UINT8)ch;
                consoleCB->fifoIn++;
            }

            /* return what you input */
            if (ch == '\n') {
                UserEndOfRead(consoleCB, filep, fops);
                ret = ConsoleReadFifo(buffer, consoleCB, bufLen);
                break;
            }
        }
    } else {
        /* if data is already in console fifo, we return them immediately */
        ret = ConsoleReadFifo(buffer, consoleCB, bufLen);
    }

    return ret;
}

INT32 FilepRead(struct file *filep, const struct file_operations_vfs *fops, CHAR *buffer, size_t bufLen)
{
    INT32 ret;
    if (fops->read == NULL) {
        return -EFAULT;
    }
    /*
     * adopt uart read function to read data from filep
     * and write data to buffer (filep is
     * corresponding to filep of /dev/console)
     */
    ret = fops->read(filep, buffer, bufLen);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

INT32 FilepWrite(struct file *filep, const struct file_operations_vfs *fops, const CHAR *buffer, size_t bufLen)
{
    INT32 ret;
    if (fops->write == NULL) {
        return -EFAULT;
    }

    ret = fops->write(filep, buffer, bufLen);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

INT32 FilepClose(struct file *filep, const struct file_operations_vfs *fops)
{
    INT32 ret;
    if ((fops == NULL) || (fops->close == NULL)) {
        return -EFAULT;
    }

    /*
     * adopt uart close function to open filep (filep is
     * corresponding to filep of /dev/console)
     */
    ret = fops->close(filep);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

INT32 FilepIoctl(struct file *filep, const struct file_operations_vfs *fops, INT32 cmd, unsigned long arg)
{
    INT32 ret;
    if (fops->ioctl == NULL) {
        return -EFAULT;
    }

    ret = fops->ioctl(filep, cmd, arg);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

INT32 FilepPoll(struct file *filep, const struct file_operations_vfs *fops, poll_table *fds)
{
    INT32 ret;
    if (fops->poll == NULL) {
        return -EFAULT;
    }

    /*
     * adopt uart poll function to poll filep (filep is
     * corresponding to filep of /dev/serial)
     */
    ret = fops->poll(filep, fds);
    if (ret < 0) {
        return -EPERM;
    }
    return ret;
}

STATIC INT32 ConsoleOpen(struct file *filep)
{
    INT32 ret;
    UINT32 consoleId;
    struct file *privFilep = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    consoleId = (UINT32)OsConsoleFullpathToID(filep->f_path);
    if (consoleId == (UINT32)-1) {
        ret = EPERM;
        goto ERROUT;
    }
    filep->f_priv = g_console[consoleId - 1];

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = EINVAL;
        goto ERROUT;
    }
    ret = FilepOpen(privFilep, fileOps);
    if (ret < 0) {
        ret = EPERM;
        goto ERROUT;
    }
    return ENOERR;

ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

STATIC INT32 ConsoleClose(struct file *filep)
{
    INT32 ret;
    struct file *privFilep = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = EINVAL;
        goto ERROUT;
    }
    ret = FilepClose(privFilep, fileOps);
    if (ret < 0) {
        ret = EPERM;
        goto ERROUT;
    }

    return ENOERR;

ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

STATIC ssize_t ConsoleRead(struct file *filep, CHAR *buffer, size_t bufLen)
{
    INT32 ret;
    struct file *privFilep = NULL;
    CONSOLE_CB *consoleCB = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = -EINVAL;
        goto ERROUT;
    }
    consoleCB = (CONSOLE_CB *)filep->f_priv;
    if (consoleCB == NULL) {
        consoleCB = OsGetConsoleByTaskID(OsCurrTaskGet()->taskId);
        if (consoleCB == NULL) {
            return -EFAULT;
        }
    }

    /*
     * shell task use FilepRead function to get data,
     * user task use UserFilepRead to get data
     */
#ifdef LOSCFG_SHELL
    if (OsCurrTaskGet()->taskEntry == (TSK_ENTRY_FUNC)ShellEntry) {
        ret = FilepRead(privFilep, fileOps, buffer, bufLen);
        if (ret < 0) {
            goto ERROUT;
        }
    } else {
#endif
        (VOID)ConsoleCtrlRightsCapture(consoleCB);
        ret = UserFilepRead(consoleCB, privFilep, fileOps, buffer, bufLen);
        (VOID)ConsoleCtrlRightsRelease(consoleCB);
        if (ret < 0) {
            goto ERROUT;
        }
#ifdef LOSCFG_SHELL
    }
#endif
    return ret;

ERROUT:
    set_errno(-ret);
    return VFS_ERROR;
}

STATIC INT32 CopytoRingBuf(Ringbuf *ringBuf, const CHAR *buffer, size_t bufLen)
{
    size_t cnt = 0;
    size_t sentenceLen = 0;
    UINT32 flag = TRUE;
    UINT32 cpSize;
    while ((cnt + sentenceLen) < bufLen) {
        if ((buffer[cnt + sentenceLen] != '\n') && (buffer[cnt + sentenceLen] != '\r')) {
            sentenceLen++;
            continue;
        }

        if (sentenceLen != 0) {
            cpSize = LOS_RingbufWrite(ringBuf, &buffer[cnt], (UINT32)sentenceLen);
            cnt += cpSize;
            if ((cpSize == 0) || (cpSize < sentenceLen)) {
                flag = FALSE;
                break;
            }
        }

        cpSize = LOS_RingbufWrite(ringBuf, "\r", 1);
        cpSize = LOS_RingbufWrite(ringBuf, &buffer[cnt], 1);
        if (cpSize == 0) {
            flag = FALSE;
            break;
        }
        cnt += 1;
        sentenceLen = 0;
    }
    if ((cnt < bufLen) && (flag == TRUE)) {
        cpSize = LOS_RingbufWrite(ringBuf, &buffer[cnt], (UINT32)(bufLen - cnt));
        cnt += cpSize;
    }
    return (INT32)cnt;
}

STATIC ssize_t ConsoleWrite(struct file *filep, const CHAR *buffer, size_t bufLen)
{
    INT32 ret;
    INT32 cnt = 0;
    UINT32 intSave;
    CONSOLE_CB *consoleCB = NULL;
    RingbufSendCB *ringbufSendCB = NULL;
    struct file *privFilep = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = EINVAL;
        goto ERROUT;
    }

    consoleCB = (CONSOLE_CB *)filep->f_priv;
    if ((fileOps->write == NULL) || (consoleCB == NULL)) {
        ret = EFAULT;
        goto ERROUT;
    }

    ringbufSendCB = consoleCB->ringbufSendCB;

    /*
     * adopt uart open function to read data from buffer
     * and write data to filep (filep is
     * corresponding to filep of /dev/console)
     */
#ifdef LOSCFG_SHELL_DMESG
    (VOID)OsLogMemcpyRecord(buffer, bufLen);
    if (!OsCheckConsoleLock()) {
#endif
    LOS_SpinLockSave(&g_consoleWriteSpin, &intSave);
    cnt = CopytoRingBuf(&ringbufSendCB->ringbuf, buffer, bufLen);
    LOS_SpinUnlockRestore(&g_consoleWriteSpin, intSave);
    (VOID)LOS_EventWrite(&ringbufSendCB->sendEvent, CONSOLE_RINGBUF_EVENT);
#ifdef LOSCFG_SHELL_DMESG
    }
#endif
    return cnt;

ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

STATIC INT32 ConsoleIoctl(struct file *filep, INT32 cmd, unsigned long arg)
{
    INT32 ret;
    struct file *privFilep = NULL;
    CONSOLE_CB *consoleCB = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = EINVAL;
        goto ERROUT;
    }

    if (fileOps->ioctl == NULL) {
        ret = EFAULT;
        goto ERROUT;
    }

    consoleCB = (CONSOLE_CB *)filep->f_priv;
    if (consoleCB == NULL) {
        ret = EINVAL;
        goto ERROUT;
    }

    switch (cmd) {
        case CONSOLE_CONTROL_RIGHTS_CAPTURE:
            ret = ConsoleCtrlRightsCapture(consoleCB);
            break;
        case CONSOLE_CONTROL_RIGHTS_RELEASE:
            ret = ConsoleCtrlRightsRelease(consoleCB);
            break;
        case CONSOLE_CONTROL_CAPTURE_LINE:
            ret = ConsoleCtrlCaptureLine(consoleCB);
            break;
        case CONSOLE_CONTROL_CAPTURE_CHAR:
            ret = ConsoleCtrlCaptureChar(consoleCB);
            break;
        default:
            ret = fileOps->ioctl(privFilep, cmd, arg);
            break;
    }

    if (ret < 0) {
        ret = EPERM;
        goto ERROUT;
    }

    return ret;
ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

STATIC INT32 ConsolePoll(struct file *filep, poll_table *fds)
{
    INT32 ret;
    struct file *privFilep = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    ret = GetFilepOps(filep, &privFilep, &fileOps);
    if (ret != ENOERR) {
        ret = EINVAL;
        goto ERROUT;
    }

    ret = FilepPoll(privFilep, fileOps, fds);
    if (ret < 0) {
        ret = EPERM;
        goto ERROUT;
    }
    return ret;

ERROUT:
    set_errno(ret);
    return VFS_ERROR;
}

/* console device driver function structure */
STATIC const struct file_operations_vfs g_consoleDevOps = {
    .open = ConsoleOpen,   /* open */
    .close = ConsoleClose, /* close */
    .read = ConsoleRead,   /* read */
    .write = ConsoleWrite, /* write */
    .seek = NULL,
    .ioctl = ConsoleIoctl,
    .poll = ConsolePoll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
    .unlink = NULL
#endif
};

STATIC VOID OsConsoleTermiosInit(CONSOLE_CB *consoleCB, const CHAR *deviceName)
{
    struct termios consoleTermios = {0};

    if ((deviceName != NULL) &&
        (strlen(deviceName) == strlen(SERIAL)) &&
        (!strncmp(deviceName, SERIAL, strlen(SERIAL)))) {
        consoleCB->isNonBlock = SetSerialBlock(consoleCB);

        /* set console to have a buffer for user */
        (VOID)ConsoleUpdateTermiosClFlag(consoleCB->fd, &consoleTermios, (ICANON | ECHO), TRUE);
    }
#ifdef LOSCFG_NET_TELNET
    else if ((deviceName != NULL) &&
             (strlen(deviceName) == strlen(TELNET)) &&
             (!strncmp(deviceName, TELNET, strlen(TELNET)))) {
        consoleCB->isNonBlock = SetTelnetBlock(consoleCB);
    }
#endif
}

STATIC INT32 OsConsoleFileInit(CONSOLE_CB *consoleCB)
{
    INT32 ret;
    struct file filep;
    CHAR *fullpath = NULL;
    struct inode_search_s desc;

    ret = vfs_normalize_path(NULL, consoleCB->name, &fullpath);
    if (ret < 0) {
        return EINVAL;
    }

    SETUP_SEARCH(&desc, fullpath, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        ret = ENOENT;
        goto ERROUT_WITH_FULLPATH;
    }

    filep.f_inode = desc.node;
    filep.f_oflags = O_RDWR;
    filep.f_pos = 0;
    filep.f_priv = consoleCB;
    filep.f_path = fullpath;
    filep.f_relpath = NULL;

    consoleCB->fd = files_allocate(filep.f_inode, filep.f_oflags, filep.f_pos,
                                   filep.f_priv, STDERR_FILENO + 1, &filep);
    if (consoleCB->fd < 0) {
        ret = EMFILE;
        goto ERROUT_WITH_INODE;
    }

    RELEASE_SEARCH(&desc);
    return LOS_OK;

ERROUT_WITH_INODE:
    inode_release(desc.node);
ERROUT_WITH_FULLPATH:
    free(fullpath);
    RELEASE_SEARCH(&desc);
    return ret;
}

/*
 * Initialized console control platform so that when we operate /dev/console
 * as if we are operating /dev/ttyS0 (uart0).
 */
STATIC INT32 OsConsoleDevInit(CONSOLE_CB *consoleCB, const CHAR *deviceName)
{
    INT32 ret;
    CHAR *fullpath = NULL;
    struct file *filep = NULL;
    struct inode_search_s desc;

    /* allocate memory for filep, in order to unchange the value of filep */
    filep = (struct file *)LOS_MemAlloc(m_aucSysMem0, sizeof(struct file));
    if (filep == NULL) {
        ret = ENOMEM;
        goto ERROUT;
    }

    /* Adopt procedure of open function to allocate 'filep' to /dev/console */
    ret = vfs_normalize_path(NULL, deviceName, &fullpath);
    if (ret < 0) {
        ret = EINVAL;
        goto ERROUT_WITH_FILEP;
    }

    SETUP_SEARCH(&desc, fullpath, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        ret = ENOENT;
        goto ERROUT_WITH_FULLPATH;
    }

    RELEASE_SEARCH(&desc);
    consoleCB->devInode = desc.node;

    /*
     * initialize the console filep which is associated with /dev/console,
     * assign the uart0 inode of /dev/ttyS0 to console inode of /dev/console,
     * then we can operate console's filep as if we operate uart0 filep of
     * /dev/ttyS0.
     */
    (VOID)memset(filep, 0, sizeof(struct file));
    filep->f_oflags = O_RDWR;
    filep->f_pos = 0;
    filep->f_inode = desc.node;
    filep->f_path = NULL;
    filep->f_priv = NULL;

    if (desc.node->u.i_ops->open != NULL) {
        (VOID)desc.node->u.i_ops->open(filep);
    } else {
        ret = EFAULT;
        goto ERROUT_WITH_INODE;
    }

    /*
     * Use filep to connect console and uart, we can find uart driver function through filep.
     * now we can operate /dev/console to operate /dev/ttyS0 through filep.
     */
    (VOID)register_driver(consoleCB->name, &g_consoleDevOps, DEFFILEMODE, filep);
    inode_release(desc.node);
    free(fullpath);
    return LOS_OK;

ERROUT_WITH_INODE:
    inode_release(desc.node);
ERROUT_WITH_FULLPATH:
    free(fullpath);
ERROUT_WITH_FILEP:
    (VOID)LOS_MemFree(m_aucSysMem0, filep);
ERROUT:
    set_errno(ret);
    return LOS_NOK;
}

STATIC UINT32 OsConsoleDevDeinit(const CONSOLE_CB *consoleCB)
{
    INT32 ret;
    struct file *filep = NULL;
    struct inode_search_s desc;
    CHAR *fullpath = NULL;

    ret = vfs_normalize_path(NULL, consoleCB->name, &fullpath);
    if (ret < 0) {
        ret = EINVAL;
        goto ERROUT;
    }

    SETUP_SEARCH(&desc, fullpath, false);
    ret = inode_find(&desc);
    if (ret < 0) {
        ret = ENOENT;
        goto ERROUT_WITH_FULLPATH;
    }

    filep = desc.node->i_private;
    if (filep != NULL) {
        (VOID)LOS_MemFree(m_aucSysMem0, filep); /* free filep what you malloc from console_init */
        desc.node->i_private = NULL;
    } else {
        ret = EBADF;
        goto ERROUT_WITH_INODE;
    }
    inode_release(desc.node);
    free(fullpath);
    RELEASE_SEARCH(&desc);
    (VOID)unregister_driver(consoleCB->name);
    return LOS_OK;

ERROUT_WITH_INODE:
    inode_release(desc.node);
ERROUT_WITH_FULLPATH:
    free(fullpath);
    RELEASE_SEARCH(&desc);
ERROUT:
    set_errno(ret);
    return LOS_NOK;
}

STATIC RingbufSendCB *ConsoleRingbufCreate(VOID)
{
    UINT32 ret;
    CHAR *fifo = NULL;
    RingbufSendCB *ringbufSendCB = NULL;
    Ringbuf *ringbuf = NULL;

    ringbufSendCB = (RingbufSendCB *)LOS_MemAlloc(m_aucSysMem0, sizeof(RingbufSendCB));
    if (ringbufSendCB == NULL) {
        return NULL;
    }
    (VOID)memset(ringbufSendCB, 0, sizeof(RingbufSendCB));

    fifo = (CHAR *)LOS_MemAlloc(m_aucSysMem0, LOSCFG_CONSOLE_RINGBUFF_SIZE);
    if (fifo == NULL) {
        goto ERROR_WITH_SENDCB;
    }
    (VOID)memset(fifo, 0, LOSCFG_CONSOLE_RINGBUFF_SIZE);

    ringbuf = &ringbufSendCB->ringbuf;
    ret = LOS_RingbufInit(ringbuf, fifo, LOSCFG_CONSOLE_RINGBUFF_SIZE, RBUF_NORMAL);
    if (ret != LOS_OK) {
        goto ERROR_WITH_FIFO;
    }

    (VOID)LOS_EventInit(&ringbufSendCB->sendEvent);
    return ringbufSendCB;

ERROR_WITH_FIFO:
    (VOID)LOS_MemFree(m_aucSysMem0, ringbuf->fifo);
ERROR_WITH_SENDCB:
    (VOID)LOS_MemFree(m_aucSysMem0, ringbufSendCB);
    return NULL;
}

STATIC VOID ConsoleRingbufDelete(RingbufSendCB *ringbufSendCB)
{
    Ringbuf *ringbuf = &ringbufSendCB->ringbuf;

    (VOID)LOS_MemFree(m_aucSysMem0, ringbuf->fifo);
    ringbuf->fifo = NULL;
    (VOID)LOS_EventDestroy(&ringbufSendCB->sendEvent);
    (VOID)LOS_MemFree(m_aucSysMem0, ringbufSendCB);
}

STATIC UINT32 OsConsoleBufInit(CONSOLE_CB *consoleCB)
{
    UINT32 ret;
    TSK_INIT_PARAM_S initParam = {0};

    consoleCB->ringbufSendCB = ConsoleRingbufCreate();
    if (consoleCB->ringbufSendCB == NULL) {
        return LOS_NOK;
    }

    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ConsoleSendTask;
    /*
     * The priority of the console task is 1 higher than that of the idle task.
     * This prevents printing from affecting services.
     */
    initParam.usTaskPrio   = LOS_TASK_PRIORITY_LOWEST - 1;

    initParam.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    if (consoleCB->consoleID == CONSOLE_SERIAL) {
        initParam.pcName   = "SendToSer";
    } else {
        initParam.pcName   = "SendToTelnet";
    }
    initParam.uwResved     = LOS_TASK_STATUS_DETACHED;
    LOS_TASK_PARAM_INIT_ARG(initParam, consoleCB);

    ret = LOS_TaskCreate(&consoleCB->sendTaskID, &initParam);
    if (ret != LOS_OK) {
        ConsoleRingbufDelete(consoleCB->ringbufSendCB);
        consoleCB->ringbufSendCB = NULL;
        return LOS_NOK;
    }
    (VOID)LOS_EventRead(&consoleCB->ringbufSendCB->sendEvent, CONSOLE_SEND_TASK_RUNNING,
                        LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);

    return LOS_OK;
}

STATIC VOID OsConsoleBufDeinit(CONSOLE_CB *consoleCB)
{
    RingbufSendCB *ringbufSendCB = consoleCB->ringbufSendCB;

    consoleCB->ringbufSendCB = NULL;
    (VOID)LOS_EventWrite(&ringbufSendCB->sendEvent, CONSOLE_SEND_TASK_EXIT);
}

STATIC CONSOLE_CB *OsConsoleCBInit(UINT32 consoleId)
{
    CONSOLE_CB *consoleCB = (CONSOLE_CB *)LOS_MemAlloc((VOID *)m_aucSysMem0, sizeof(CONSOLE_CB));
    if (consoleCB == NULL) {
        return NULL;
    }
    (VOID)memset(consoleCB, 0, sizeof(CONSOLE_CB));

    consoleCB->consoleID = consoleId;
    consoleCB->shellEntryId = 0xffffffff;  /* initialize shellEntryId to an invalid value */
    consoleCB->name = LOS_MemAlloc((VOID *)m_aucSysMem0, CONSOLE_NAMELEN);
    if (consoleCB->name == NULL) {
        PRINT_ERR("consoleCB->name malloc failed\n");
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, consoleCB);
        return NULL;
    }
    return consoleCB;
}

STATIC VOID OsConsoleCBDeinit(CONSOLE_CB *consoleCB)
{
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, consoleCB->name);
    consoleCB->name = NULL;
    (VOID)LOS_MemFree((VOID *)m_aucSysMem0, consoleCB);
}

STATIC CONSOLE_CB *OsConsoleCreate(UINT32 consoleId, const CHAR *deviceName)
{
    INT32 ret;
    CONSOLE_CB *consoleCB = OsConsoleCBInit(consoleId);
    if (consoleCB == NULL) {
        PRINT_ERR("console malloc error.\n");
        return NULL;
    }

    ret = snprintf_s(consoleCB->name, CONSOLE_NAMELEN, CONSOLE_NAMELEN - 1,
                     "%s%u", CONSOLE, consoleCB->consoleID);
    if (ret == -1) {
        PRINT_ERR("consoleCB->name snprintf_s failed\n");
        goto ERR_WITH_NAME;
    }

    ret = (INT32)OsConsoleBufInit(consoleCB);
    if (ret != LOS_OK) {
        goto ERR_WITH_NAME;
    }

    ret = (INT32)LOS_SemCreate(1, &consoleCB->consoleSem);
    if (ret != LOS_OK) {
        PRINT_ERR("create sem for uart failed\n");
        goto ERR_WITH_BUF;
    }

    ret = OsConsoleDevInit(consoleCB, deviceName);
    if (ret != LOS_OK) {
        goto ERR_WITH_SEM;
    }

    ret = OsConsoleFileInit(consoleCB);
    if (ret != LOS_OK) {
        goto ERR_WITH_DEV;
    }

    OsConsoleTermiosInit(consoleCB, deviceName);
    return consoleCB;

ERR_WITH_DEV:
    ret = (INT32)OsConsoleDevDeinit(consoleCB);
    if (ret != LOS_OK) {
        PRINT_ERR("OsConsoleDevDeinit failed!\n");
    }
ERR_WITH_SEM:
    (VOID)LOS_SemDelete(consoleCB->consoleSem);
ERR_WITH_BUF:
    OsConsoleBufDeinit(consoleCB);
ERR_WITH_NAME:
    OsConsoleCBDeinit(consoleCB);
    return NULL;
}

STATIC UINT32 OsConsoleDelete(CONSOLE_CB *consoleCB)
{
    UINT32 ret;

    (VOID)close(consoleCB->fd);
    ret = OsConsoleDevDeinit(consoleCB);
    if (ret != LOS_OK) {
        PRINT_ERR("OsConsoleDevDeinit failed!\n");
    }
    OsConsoleBufDeinit((CONSOLE_CB *)consoleCB);
    (VOID)LOS_SemDelete(consoleCB->consoleSem);
    (VOID)LOS_MemFree(m_aucSysMem0, consoleCB->name);
    consoleCB->name = NULL;
    (VOID)LOS_MemFree(m_aucSysMem0, consoleCB);

    return ret;
}

/* Initialized system console and return stdinfd stdoutfd stderrfd */
INT32 system_console_init(const CHAR *deviceName)
{
#ifdef LOSCFG_SHELL
    UINT32 ret;
#endif
    INT32 consoleId;
    UINT32 intSave;
    CONSOLE_CB *consoleCB = NULL;

    if (!OS_SCHEDULER_ACTIVE) {
        return VFS_ERROR;
    }

    consoleId = OsGetConsoleID(deviceName);
    if (consoleId == -1) {
        PRINT_ERR("device is full.\n");
        return VFS_ERROR;
    }

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    if (g_status[consoleId - 1] != CONSOLE_UNUSED) {
        LOS_SpinUnlockRestore(&g_consoleSpin, intSave);
        PRINT_ERR("%s, %d, %u\n", __FUNCTION__, __LINE__, (UINT32)g_status[consoleId - 1]);
        return VFS_ERROR;
    }
    g_status[consoleId - 1] = CONSOLE_INITIALIZING;
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    consoleCB = OsConsoleCreate((UINT32)consoleId, deviceName);
    if (consoleCB == NULL) {
        PRINT_ERR("%s, %d\n", __FUNCTION__, __LINE__);
        goto ERROR_WITH_STATUS;
    }

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    g_console[consoleId - 1] = consoleCB;
    g_taskConsoleIDArray[OsCurrTaskGet()->taskId] = (UINT8)consoleId;
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

#ifdef LOSCFG_SHELL
    ret = OsShellInit(consoleId);
    if (ret != LOS_OK) {
        PRINT_ERR("%s, %d\n", __FUNCTION__, __LINE__);
        LOS_SpinLockSave(&g_consoleSpin, &intSave);
        g_console[consoleId - 1] = NULL;
        g_taskConsoleIDArray[OsCurrTaskGet()->taskId] = CONSOLE_SERIAL;
        LOS_SpinUnlockRestore(&g_consoleSpin, intSave);
        (VOID)OsConsoleDelete(consoleCB);
        goto ERROR_WITH_STATUS;
    }
#endif

    g_status[consoleId - 1] = CONSOLE_READY;

    return ENOERR;

ERROR_WITH_STATUS:
    g_status[consoleId - 1] = CONSOLE_UNUSED;
    return VFS_ERROR;
}

INT32 system_console_deinit(const CHAR *deviceName)
{
    CONSOLE_CB *consoleCB = NULL;
    UINT32 taskIdx;
    LosTaskCB *taskCB = NULL;
    UINT32 intSave;
    INT32 consoleId;

    consoleId = OsGetConsoleID(deviceName);
    if (consoleId == -1) {
        PRINT_ERR("device name incorrect.\n");
        return VFS_ERROR;
    }

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    if (g_status[consoleId - 1] != CONSOLE_READY) {
        LOS_SpinUnlockRestore(&g_consoleSpin, intSave);
        PRINT_ERR("failed to deinit console device.\n");
        return VFS_ERROR;
    }
    g_status[consoleId - 1] = CONSOLE_DEINITIALIZING;
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    consoleCB = OsGetConsoleByDevice(deviceName);
    if ((consoleCB == NULL) || (consoleCB->consoleID != (UINT32)consoleId)) {
        goto ERROR_WITH_STATUS;
    }

#ifdef LOSCFG_SHELL
    (VOID)OsShellDeinit((INT32)consoleCB->consoleID);
#endif

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    if (consoleId == CONSOLE_TELNET) {
        for (taskIdx = 0; taskIdx < g_taskMaxNum; taskIdx++) {
            taskCB = ((LosTaskCB *)g_osTaskCBArray) + taskIdx;
            if (taskCB->taskStatus & OS_TASK_IS_EXIT) {
                continue;
            }
            // Redirect to default output.
            g_taskConsoleIDArray[taskCB->taskId] = CONSOLE_SERIAL;
        }
    }
    g_console[consoleId - 1] = NULL;
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    (VOID)OsConsoleDelete(consoleCB);

    g_status[consoleId - 1] = CONSOLE_UNUSED;

    return ENOERR;

ERROR_WITH_STATUS:
    g_status[consoleId - 1] = CONSOLE_READY;
    return VFS_ERROR;
}

BOOL OsConsoleIsEnabled(VOID)
{
    if (!OS_SCHEDULER_ACTIVE) {
        return FALSE;
    }

#ifdef LOSCFG_SERIAL_OUTPUT_ENABLE
    UINT32 consoleId;
    UINT32 intSave;

    LOS_SpinLockSave(&g_consoleSpin, &intSave);

    if (OS_INT_ACTIVE) {
        consoleId = CONSOLE_SERIAL;
    } else {
        consoleId = g_taskConsoleIDArray[OsCurrTaskGet()->taskId];
    }

    if (g_status[consoleId - 1] != CONSOLE_READY) {
        LOS_SpinUnlockRestore(&g_consoleSpin, intSave);
        return FALSE;
    }
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    if (consoleId == CONSOLE_TELNET) {
        return TRUE;
    }
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    else if (SerialTypeGet() == SERIAL_TYPE_UART_DEV) {
        return TRUE;
    }
#endif
#if defined (LOSCFG_DRIVERS_USB_SERIAL_GADGET) || defined (LOSCFG_DRIVERS_USB_ETH_SER_GADGET)
    else if ((SerialTypeGet() == SERIAL_TYPE_USBTTY_DEV) && (userial_mask_get() == 1)) {
        return TRUE;
    }
#endif

    return FALSE;

#else /* !LOSCFG_SERIAL_OUTPUT_ENABLE */
    return (g_console[CONSOLE_TELNET - 1] != NULL) ? TRUE : FALSE;
#endif
}

UINT32 ConsoleTaskReg(INT32 consoleId, UINT32 taskId)
{
    if ((consoleId > CONSOLE_NUM) || (consoleId < 1)) {
        return LOS_NOK;
    }
    g_console[consoleId - 1]->shellEntryId = taskId;
    return LOS_OK;
}

STATIC BOOL SetSerialBlock(const CONSOLE_CB *consoleCB)
{
    INT32 ret;

    if (consoleCB == NULL) {
        PRINT_ERR("%s: Input parameter is illegal\n", __FUNCTION__);
        return TRUE;
    }
    ret = ioctl(consoleCB->fd, CONSOLE_CMD_RD_BLOCK_SERIAL, CONSOLE_RD_BLOCK);
    if (ret != 0) {
        return TRUE;
    }

    return FALSE;
}

#ifdef LOSCFG_NET_TELNET
STATIC BOOL SetTelnetBlock(const CONSOLE_CB *consoleCB)
{
    INT32 ret;

    if (consoleCB == NULL) {
        PRINT_ERR("%s: Input parameter is illegal\n", __FUNCTION__);
        return TRUE;
    }
    ret = ioctl(consoleCB->fd, CONSOLE_CMD_RD_BLOCK_TELNET, CONSOLE_RD_BLOCK);
    if (ret != 0) {
        return TRUE;
    }
    return FALSE;
}
#endif

BOOL is_nonblock(const CONSOLE_CB *consoleCB)
{
    if (consoleCB == NULL) {
        PRINT_ERR("%s: Input parameter is illegal\n", __FUNCTION__);
        return FALSE;
    }
    return consoleCB->isNonBlock;
}

STATIC INT32 ConsoleFdGet(VOID)
{
    UINT32 consoleId;

    LOS_ASSERT(LOS_SpinHeld(&g_consoleSpin));

    consoleId = g_taskConsoleIDArray[(OsCurrTaskGet())->taskId];
#ifdef LOSCFG_SERIAL_OUTPUT_ENABLE
    if (consoleId == 0) {
        if (g_console[CONSOLE_SERIAL - 1] != NULL) {
            consoleId = CONSOLE_SERIAL;
        } else if (g_console[CONSOLE_TELNET - 1] != NULL) {
            consoleId = CONSOLE_TELNET;
        } else {
            return -1;
        }
    }

    if (g_console[consoleId - 1] == NULL) {
        return -1;
    }
#else /* !LOSCFG_SERIAL_OUTPUT_ENABLE */
    if (g_console[CONSOLE_TELNET - 1] == NULL) {
        return -1;
    }
    consoleId = CONSOLE_TELNET;
#endif
    return g_console[consoleId - 1]->fd;
}

INT32 ConsoleUpdateFd(VOID)
{
    INT32 fd;
    UINT32 intSave;

    if (!OS_SCHEDULER_ACTIVE) {
        return -1;
    }

    LOS_SpinLockSave(&g_consoleSpin, &intSave);
    fd = ConsoleFdGet();
    LOS_SpinUnlockRestore(&g_consoleSpin, intSave);

    return fd;
}

CONSOLE_CB *OsGetConsoleByID(INT32 consoleId)
{
    if (consoleId != CONSOLE_TELNET) {
        consoleId = CONSOLE_SERIAL;
    }
    return g_console[consoleId - 1];
}

CONSOLE_CB *OsGetConsoleByTaskID(UINT32 taskId)
{
    if (taskId >= LOSCFG_BASE_CORE_TSK_LIMIT) {
        return NULL;
    }

    INT32 consoleId = g_taskConsoleIDArray[taskId];

    return OsGetConsoleByID(consoleId);
}

VOID OsSetConsoleID(UINT32 newTaskId, UINT32 curTaskId)
{
    if ((newTaskId >= LOSCFG_BASE_CORE_TSK_LIMIT) || (curTaskId > LOSCFG_BASE_CORE_TSK_LIMIT)) {
        return;
    }

    // The default value of consoleId for all tasks is CONSOLE_SERIAL.
    if (g_taskConsoleIDArray[curTaskId] == 0) {
        g_taskConsoleIDArray[curTaskId] = CONSOLE_SERIAL;
    }

    g_taskConsoleIDArray[newTaskId] = g_taskConsoleIDArray[curTaskId];
}

STATIC VOID WriteToTerminal(const CONSOLE_CB *consoleCB, const CHAR *buffer, size_t bufLen)
{
    INT32 ret, fd;
    struct file *privFilep = NULL;
    struct file *filep = NULL;
    const struct file_operations_vfs *fileOps = NULL;

    fd = consoleCB->fd;
    ret = fs_getfilep(fd, &filep);
    if (ret < 0) {
        ret = EPERM;
        goto ERROUT;
    }
    ret = GetFilepOps(filep, &privFilep, &fileOps);

    if ((fileOps == NULL) || (fileOps->write == NULL)) {
        ret = EFAULT;
        goto ERROUT;
    }
    (VOID)fileOps->write(privFilep, buffer, bufLen);

    return;

ERROUT:
    set_errno(ret);
    return;
}

STATIC VOID ConsoleFlush(const CONSOLE_CB *consoleCB, const CHAR *buf, UINT32 size)
{
    UINT32 cnt = 0;
    UINT32 start = 0;
    UINT32 temp = 0;

    /*
     * Since the console driver may require a lock which will affect the timing of
     * the irq response. Put every single line to the driver to reduce the impact.
     */
    while (cnt < size) {
        if (buf[cnt] == '\n') {
            WriteToTerminal(consoleCB, &buf[start], temp + 1);
            start = ++cnt;
            temp = 0;
            continue;
        }
        cnt++;
        temp++;
    }

    if (temp != 0) {
        WriteToTerminal(consoleCB, &buf[start], temp);
    }
}

STATIC UINT32 ConsoleSendTask(const VOID *param)
{
    CONSOLE_CB *consoleCB = (CONSOLE_CB *)param;
    RingbufSendCB *ringbufSendCB = consoleCB->ringbufSendCB;
    Ringbuf *ringbuf = &ringbufSendCB->ringbuf;
    UINT32 ret, size;
    CHAR *buf = NULL;

    buf = (CHAR *)LOS_MemAlloc(m_aucSysMem1, LOSCFG_CONSOLE_RINGBUFF_SIZE + 1);
    if (buf == NULL) {
        return LOS_NOK;
    }

    (VOID)LOS_EventWrite(&ringbufSendCB->sendEvent, CONSOLE_SEND_TASK_RUNNING);

    while (1) {
        ret = LOS_EventRead(&ringbufSendCB->sendEvent, CONSOLE_RINGBUF_EVENT | CONSOLE_SEND_TASK_EXIT,
                            LOS_WAITMODE_OR | LOS_WAITMODE_CLR, LOS_WAIT_FOREVER);
        if (ret == CONSOLE_RINGBUF_EVENT) {
            size = LOS_RingbufUsedSize(ringbuf);
            if (size == 0) {
                continue;
            }

            (VOID)memset(buf, 0, size + 1);
            size = LOS_RingbufRead(ringbuf, buf, size);
            if (size == 0) {
                continue;
            }

            ConsoleFlush(consoleCB, buf, size);
        } else if (ret == CONSOLE_SEND_TASK_EXIT) {
            break;
        }
    }

    (VOID)LOS_MemFree(m_aucSysMem1, buf);
    ConsoleRingbufDelete(ringbufSendCB);
    return LOS_OK;
}

VOID ConsoleExcOutRingbuf(UINT32 consoleId)
{
    CONSOLE_CB *consoleCB = g_console[consoleId - 1];
    Ringbuf *ringbuf = &consoleCB->ringbufSendCB->ringbuf;
    CHAR buf[CONSOLE_EXC_OUT_BUF_SIZE + 1];
    UINT32 size;

    size = LOS_RingbufUsedSize(ringbuf);
    if (size == 0) {
        return;
    }

    (VOID)memset(buf, 0, CONSOLE_EXC_OUT_BUF_SIZE + 1);

    do {
        size = LOS_RingbufRead(ringbuf, buf, CONSOLE_EXC_OUT_BUF_SIZE);
        if (size == 0) {
            return;
        }
        PRINTK("%s", buf);
    } while (size == CONSOLE_EXC_OUT_BUF_SIZE);

    return;
}

VOID OsConsoleDisable(VOID)
{
    for (UINT32 consoleId = CONSOLE_SERIAL; consoleId <= CONSOLE_NUM; consoleId++) {
        if (g_status[consoleId - 1] == CONSOLE_READY) {
            g_status[consoleId - 1] = CONSOLE_DISABLE;
            ConsoleExcOutRingbuf(consoleId);
        }
    }
}
