/****************************************************************************
 * fs/vfs/fs_lseek64.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <errno.h>

#include "inode/inode.h"

#if CONFIG_NFILE_DESCRIPTORS > 0

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: file_seek64
 *
 * Description:
 *   This is the internal implementation of lseek64.  See the comments in
 *   lseek64() for further information.
 *
 * Input Parameters:
 *   file     File structure instance
 *   offset   Defines the offset to position to
 *   whence   Defines how to use offset
 *
 * Returned Value:
 *   The resulting offset on success.  A negated errno value is returned on
 *   any failure (see lseek comments).
 *
 ****************************************************************************/

off64_t file_seek64(FAR struct file *filep, off64_t offset, int whence)
{
  FAR struct inode *inode;
  off64_t ret;

  DEBUGASSERT(filep);
  inode =  filep->f_inode;

  /* Invoke the file seek method if available */

  if (inode == NULL)
    {
      return -EBADF;
    }
  if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END)
    {
      return -EINVAL;
    }

  if (INODE_IS_MOUNTPT(inode) && inode->u.i_mops && inode->u.i_mops->seek64)
    {
      ret = inode->u.i_mops->seek64(filep, offset, whence);
      if (ret < 0)
        {
          return ret;
        }
    }
  else
    {
      /* No... Just set the common file position value */

      switch (whence)
        {
          case SEEK_CUR:
            offset += filep->f_pos;

            /* FALLTHROUGH */

          case SEEK_SET:
            if (offset >= 0)
              {
                filep->f_pos = offset; /* Might be beyond the end-of-file */
                break;
              }
            else
              {
                return -EINVAL;
              }
            break;

          case SEEK_END:
            return -ENOSYS;

          default:
            return -EINVAL;
        }
    }

  return filep->f_pos;
}

/****************************************************************************
 * Name: lseek64
 *
 * Description:
 *   The lseek64() function repositions the offset of the open file associated
 *   with the file descriptor fd to the argument 'offset' according to the
 *   directive 'whence' as follows:
 *
 *   SEEK_SET
 *      The offset is set to offset bytes.
 *   SEEK_CUR
 *      The offset is set to its current location plus offset bytes.
 *   SEEK_END
 *      The offset is set to the size of the file plus offset bytes.
 *
 *  The lseek64() function allows the file offset to be set beyond the end of the
 *  file (but this does not change the size of the file). If data is later written
 *  at this point, subsequent reads of the data in the gap (a "hole") return null
 *  bytes ('\0') until data is actually written into the gap.
 *
 * Input Parameters:
 *   fd       File descriptor of device
 *   offset   Defines the offset to position to
 *   whence   Defines how to use offset
 *
 * Returned Value:
 *   The resulting offset on success.  -1 on failure withi errno set properly:
 *
 *   EBADF      fd is not an open file descriptor.
 *   EINVAL     whence  is  not one of SEEK_SET, SEEK_CUR, SEEK_END; or the
 *              resulting file offset would be negative, or beyond the end of a
 *              seekable device.
 *   EOVERFLOW  The resulting file offset cannot be represented in an off_t.
 *   ESPIPE     fd is associated with a pipe, socket, or FIFO.
 *
 ****************************************************************************/

off64_t lseek64(int fd, off64_t offset, int whence)
{
  FAR struct file *filep;
  off64_t newpos;
  int errcode;
  int ret;

  /* Get the file structure corresponding to the file descriptor. */

  ret = fs_getfilep(fd, &filep);
  if (ret < 0)
    {
      errcode = -ret;
      goto errout;
    }

  DEBUGASSERT(filep != NULL);

  /* Then let file_seek do the real work */

  newpos = file_seek64(filep, offset, whence);
  if (newpos < 0)
    {
      errcode = (int)-newpos;
      goto errout;
    }

  return newpos;

errout:
  set_errno(errcode);
  return (off_t)VFS_ERROR;
}
#endif
