/****************************************************************************
 * fs/vfs/fs_pread64.c
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

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>

#include <fs/fs.h>
#include <fs/file.h>

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: file_pread64
 *
 * Description:
 *   Equivalent to the standard pread function except that is accepts a
 *   struct file instance instead of a file descriptor.  Currently used
 *   only by aio_read();
 *
 ****************************************************************************/

static ssize_t file_pread64(FAR struct file *filep, FAR void *buf, size_t nbytes,
                     off64_t offset)
{
  off64_t savepos;
  off64_t pos;
  ssize_t ret;

  /* Perform the seek to the current position.  This will not move the
   * file pointer, but will return its current setting
   */

  savepos = file_seek64(filep, 0, SEEK_CUR);
  if (savepos < 0)
    {
      /* file_seek64 might fail if this if the media is not seekable */

      return (ssize_t)savepos;
    }

  /* Then seek to the correct position in the file */

  pos = file_seek64(filep, offset, SEEK_SET);
  if (pos < 0)
    {
      /* This might fail is the offset is beyond the end of file */

      return (ssize_t)pos;
    }

  /* Then perform the read operation */

  ret = file_read(filep, buf, nbytes);

  /* Restore the file position */

  pos = file_seek64(filep, savepos, SEEK_SET);
  if (pos < 0 && ret >= 0)
    {
      /* This really should not fail */

      ret = (ssize_t)pos;
    }

  return ret;
}

/****************************************************************************
 * Name: pread64
 *
 * Description:
 *   The pread64() function performs the same action as read(), except that it
 *   reads from a given position in the file without changing the file
 *   pointer. The first three arguments to pread64() are the same as read()
 *   with the addition of a fourth argument offset for the desired position
 *   inside the file. An attempt to perform a pread64() on a file that is
 *   incapable of seeking results in an error.
 *
 *   NOTE: This function could have been wholly implemented within libc but
 *   it is not.  Why?  Because if pread64 were implemented in libc, it would
 *   require four system calls.  If it is implemented within the kernel,
 *   only three.
 *
 * Input Parameters:
 *   file     File structure instance
 *   buf      User-provided to save the data
 *   nbytes   The maximum size of the user-provided buffer
 *   offset   The file offset
 *
 * Returned Value:
 *   The positive non-zero number of bytes read on success, 0 on if an
 *   end-of-file condition, or -1 on failure with errno set appropriately.
 *   See read() return values
 *
 ****************************************************************************/

ssize_t pread64(int fd, FAR void *buf, size_t nbytes, off64_t offset)
{
  FAR struct file *filep;
  ssize_t ret;

  /* Get the file structure corresponding to the file descriptor. */

  ret = (ssize_t)fs_getfilep(fd, &filep);
  if (ret < 0)
    {
      goto errout;
    }

  DEBUGASSERT(filep != NULL);

  /* Let file_pread do the real work */

  ret = file_pread64(filep, buf, nbytes, offset);
  if (ret < 0)
    {
      goto errout;
    }

  return ret;

errout:
  set_errno((int)-ret);
  return (ssize_t)VFS_ERROR;
}
