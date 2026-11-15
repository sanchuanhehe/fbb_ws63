/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  common logging producer interface - need to change name of log.h in all the protocol core files
 * Author: 
 * Create:
 */
 
#ifndef LOG_DEF_SLP_RADAR_H_
#define LOG_DEF_SLP_RADAR_H_
 
typedef enum {
// only for glp extern part
    SLP_FILE_ID_EXT_START = 901,
    ASSEMBLE_RADAR_MSG_C,
    PARSE_RADAR_MFG_MSG_C,
    SLP_LOG_UTILS_C,
    SLP_RADAR_CLIENT_MAIN_C,
    SLP_RADAR_INTERFACE_C,
    SLP_RADAR_SERVICE_C,
    SLP_RADAR_AT_C,
    SLP_FILE_ID_EXT_END = 1023,
} LogFileListEnumSlpRadar;
#endif
