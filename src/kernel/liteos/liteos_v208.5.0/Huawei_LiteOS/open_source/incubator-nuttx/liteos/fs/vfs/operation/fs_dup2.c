/****************************************************************************
 * fs/vfs/fs_dup2.c
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
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
#include <nuttx/fs/fs.h>
#endif

#include <unistd.h>
#include <sched.h>
#include <assert.h>
#include <errno.h>

#include "inode/inode.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: file_dup2
 *
 * Description:
 *   Assign an inode to a specific files structure.  This is the heart of
 *   dup2.
 *
 *   Equivalent to the non-standard dup2() function except that it
 *   accepts struct file instances instead of file descriptors and it does
 *   not set the errno variable.
 *
 * Returned Value:
 *   Zero (OK) is returned on success; a negated errno value is return on
 *   any failure.
 *
 ****************************************************************************/

int file_dup2(FAR struct file *filep1, FAR struct file *filep2)
{
  FAR struct inode *inode;
  struct file temp;
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
  char *fullpath = NULL;
  struct inode_search_s desc;
#endif
  int ret;

  if (filep1 == NULL || filep1->f_inode == NULL || filep2 == NULL)
    {
      return -EBADF;
    }

#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
  if (filep1->f_path == NULL)
    {
      return -EBADF;
    }
#endif

  if (filep1 == filep2)
    {
      return OK;
    }

  /* Increment the reference count on the contained inode */

  inode = filep1->f_inode;

#ifdef CONFIG_VFS_FILE_PRE_CLOSE
  /* If there is already an inode contained in the new file structure,
   * close the file and release the inode.
   */
  ret   = file_close(filep2, false);
#else
  ret   = inode_addref(inode);
#endif
  if (ret < 0)
    {
      return ret;
    }
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
  ret = vfs_dup_process_file_struct_path(filep1, &fullpath, &temp, &desc);
  if (ret < 0)
    {
      return ret;
    }
#else
  /* Then clone the file structure */


  temp.f_priv   = NULL;
#endif
  temp.f_oflags = filep1->f_oflags;
  temp.f_pos    = filep1->f_pos;
  temp.f_inode  = inode;

  /* Call the open method on the file, driver, mountpoint so that it
   * can maintain the correct open counts.
   */

  if (inode->u.i_ops)
    {
#ifndef CONFIG_DISABLE_MOUNTPOINT
      if (INODE_IS_MOUNTPT(inode))
        {
          /* Dup the open file on the in the new file structure */

          if (inode->u.i_mops->dup)
            {
              ret = inode->u.i_mops->dup(filep1, &temp);
            }
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
          else
            {
              vfs_release_for_dup_failed(fullpath, inode, &desc);
              return -ENOSYS;
            }
#endif
        }
      else
#endif
        {
          /* (Re-)open the pseudo file or device driver */

          temp.f_priv = filep1->f_priv;

          if (inode->u.i_ops->open)
            {
              ret = inode->u.i_ops->open(&temp);
            }
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
          else
            {
              vfs_release_for_dup_failed(fullpath, inode, &desc);
              return -ENOSYS;
            }
#endif
        }

      /* Handle open failures */

      if (ret < 0)
        {
#ifdef CONFIG_VFS_ENABLE_FILE_STRUCT_PATH
          vfs_release_for_dup_failed(fullpath, inode, &desc);
#else
          inode_release(inode);
#endif
          return ret;
        }
    }

#ifndef CONFIG_VFS_FILE_PRE_CLOSE
  /* If there is already an inode contained in the new file structure,
   * close the file and release the inode.
   */

  ret = file_close(filep2);
  DEBUGASSERT(ret == 0);
#endif

  /* Return the file structure */

  memcpy(filep2, &temp, sizeof(temp));
  return OK;
}
