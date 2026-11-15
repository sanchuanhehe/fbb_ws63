/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 * Author: 
 * Create:
 */

#ifndef LOG_DEF_BT_H
#define LOG_DEF_BT_H

typedef enum {
    CHBA_FILE_ID_EXT_START = 1001,
    SLE_CHBA_NETDEV_ACHBA_MSG_C,
    ACHBA_SLE_INTERFACE_C,
    SLE_CHBA_NETDEV_C,
    SLE_CHBA_NETDEV_THREAD_C,
    SLE_CHBA_NETDEV_BTS_CALL_C,
    SLE_CHBA_NETDEV_LINK_MNG_C,
    CHBA_FILE_ID_EXT_END = 1022,
} log_def_bt_enum;

#endif // LOG_DEF_BT_H
