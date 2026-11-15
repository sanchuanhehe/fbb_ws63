  /**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: hilink function mapping \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */
#ifndef HILINK_CALL_H
#define HILINK_CALL_H

#include "func_call_list.h"
#include "app_call_entry.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0L
#else
#define NULL ((void*)0)
#endif
#endif

#define hilink_call0(idx, nr, t) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"()"); \
    if (call_addr != NULL) { \
        return ((t (*)(void))call_addr)(); \
    } \
} while (0)
#define hilink_call1(idx, nr, t, t1, p1) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1))call_addr)(p1); \
    } \
} while (0)
#define hilink_call2(idx, nr, t, t1, p1, t2, p2) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1","#t2")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1, t2))call_addr)(p1, p2); \
    } \
} while (0)
#define hilink_call3(idx, nr, t, t1, p1, t2, p2, t3, p3) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1","#t2","#t3")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1, t2, t3))call_addr)(p1, p2, p3); \
    } \
} while (0)
#define hilink_call4(idx, nr, t, t1, p1, t2, p2, t3, p3, t4, p4) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1","#t2","#t3","#t4")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1, t2, t3, t4))call_addr)(p1, p2, p3, p4); \
    } \
} while (0)
#define hilink_call5(idx, nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1","#t2","#t3","#t4","#t5")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1, t2, t3, t4, t5))call_addr)(p1, p2, p3, p4, p5); \
    } \
} while (0)
#define hilink_call6(idx, nr, t, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) do { \
    void *call_addr = get_hilink_api_addr(idx, #t#nr"("#t1","#t2","#t3","#t4","#t5","#t6")"); \
    if (call_addr != NULL) { \
        return ((t (*)(t1, t2, t3, t4, t5, t6))call_addr)(p1, p2, p3, p4, p5, p6); \
    } \
} while (0)

#define hilink_call0_ret_void(idx, nr) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"()"); \
    if (call_addr != NULL) { \
        ((void (*)(void))call_addr)(); \
        return; \
    } \
} while (0)
#define hilink_call1_ret_void(idx, nr, t1, p1) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1))call_addr)(p1); \
        return; \
    } \
} while (0)
#define hilink_call2_ret_void(idx, nr, t1, p1, t2, p2) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1","#t2")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1, t2))call_addr)(p1, p2); \
        return; \
    } \
} while (0)
#define hilink_call3_ret_void(idx, nr, t1, p1, t2, p2, t3, p3) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1","#t2","#t3")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1, t2, t3))call_addr)(p1, p2, p3); \
        return; \
    } \
} while (0)
#define hilink_call4_ret_void(idx, nr, t1, p1, t2, p2, t3, p3, t4, p4) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1","#t2","#t3","#t4")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1, t2, t3, t4))call_addr)(p1, p2, p3, p4); \
        return; \
    } \
} while (0)
#define hilink_call5_ret_void(idx, nr, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1","#t2","#t3","#t4","#t5")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1, t2, t3, t4, t5))call_addr)(p1, p2, p3, p4, p5); \
        return; \
    } \
} while (0)
#define hilink_call6_ret_void(idx, nr, t1, p1, t2, p2, t3, p3, t4, p4, t5, p5, t6, p6) do { \
    void *call_addr = get_hilink_api_addr(idx, "void"#nr"("#t1","#t2","#t3","#t4","#t5","#t6")"); \
    if (call_addr != NULL) { \
        ((void (*)(t1, t2, t3, t4, t5, t6))call_addr)(p1, p2, p3, p4, p5, p6); \
        return; \
    } \
} while (0)

#ifdef __cplusplus
}
#endif
#endif
