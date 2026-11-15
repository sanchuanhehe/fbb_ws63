STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} )
set(MODULE_NAME ${CURRENT_FOLDER})

set(NUTTX_VFS_PATH ../../${LITEOS_OPENSOURCE_PATHNAME}/incubator-nuttx/liteos/fs/vfs)

# add global include, compile macro and options to public target #
set(MODULE_INCLUDE_PUB)
set(MODULE_CXXINCLUDE_PUB)
set(MODULE_COPTS_PUB)
set(MODULE_ASOPTS_PUB)
set(MODULE_CXXOPTS_PUB)
set(MODULE_CMACRO_PUB)
set(MODULE_ASMACRO_PUB)
set(MODULE_CXXMACRO_PUB)

# add local include, compile macro and options to private target #
set(MODULE_INCLUDE_PRI
    ${LITEOSTOPDIR}/fs/vfs/include
    ${LITEOSTOPDIR}/fs/compat
    ${LITEOSTOPDIR}/fs/vfs/include/disk
    ${LITEOSTOPDIR}/fs/vfs/include/multi_partition
    ${NUTTX_VFS_PATH}/include
    ${LITEOSTOPDIR}/kernel/extended/include
)

set(MODULE_COPTS_PRI)
set(MODULE_ASOPTS_PRI)
set(MODULE_CXXOPTS_PRI)
set(MODULE_CMACRO_PRI)
set(MODULE_ASMACRO_PRI)
set(MODULE_CXXMACRO_PRI)

# add srcs to private target #
set(LOCAL_SRCS_y)
set(LOCAL_DIRS_y operation vfs_cmd 
    ${NUTTX_VFS_PATH}/operation
    ${NUTTX_VFS_PATH}/inode
    ${NUTTX_VFS_PATH}/dirent
    ${NUTTX_VFS_PATH}/mount
    ${NUTTX_VFS_PATH}/driver
)

if(LOSCFG_COMPAT_LINUX)
    list(APPEND LOCAL_DIRS_y ${NUTTX_VFS_PATH}/poll)
endif()

FOREACH(CUR_DIR ${LOCAL_DIRS_y})
    aux_source_directory(${CUR_DIR} LOCAL_SRCS_y)
ENDFOREACH(CUR_DIR)

set(LOCAL_SRCS ${LOCAL_SRCS_y})  # module.cmake will sort LOCAL_SRCS files

if (NOT LOSCFG_COMPAT_LINUX)
    list(REMOVE_ITEM LOCAL_SRCS operation/fs_poll.c)
endif()

if (NOT LOSCFG_FS_VFS_BLOCK_DEVICE)
    list(REMOVE_ITEM LOCAL_SRCS ${NUTTX_VFS_PATH}/driver/fs_blockproxy.c)
endif()

list(APPEND MODULE_COPTS_PRI ${LITEOS_SUGGESTED_COMPILE_OPTS})

include(${MODULE})
