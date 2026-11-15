/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Application core main function for radar socket \n
 * Author:  \n
 */

#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "td_base.h"
#include "td_type.h"
#include "stdlib.h"
#include "uart.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "soc_osal.h"
#include "radar_service.h"

#define RADAR_SOCKET_LOG                "[RADAR_SOCKET]"
#define RADAR_SOCKET_TASK_PRIO          27
#define RADAR_SOCKET_TASK_STACK_SIZE    0x4000

#define SOCK_TARGET_PORT 9999
#define IP_TCP_SERVER_LISTEN_NUM  4
#define OFFSET_TWENTY_FOUR  24
#define OFFSET_EIGHT        8

#define TLV_MIN_LEN           3 // Bytes
#define TLV_MAX_LEN           400 // Bytes
#define TLV_MAX_DBG_LEN       10752 // Bytes
#define TLV_HEADER            0xAA

#define TLV_CMD_SET_STS       0xA1
#define TLV_CMD_SET_DLY       0xA2
#define TLV_CMD_SET_CTRL      0xA3
#define TLV_CMD_SET_PARA      0xA4
#define TLV_CMD_SET_ALG_PARA      0xA5

#define TLV_CMD_SET_STS_LEN   4
#define TLV_CMD_SET_DLY_LEN   4
#define TLV_CMD_SET_CTRL_LEN  15
#define TLV_CMD_SET_PARA_LEN  8
#define TLV_CMD_SET_ALG_PARA_LEN  11

#define TLV_CMD_GET_RES       0xB1
#define TLV_CMD_GET_DBG       0xB2

#define BUF_TWO    2
#define BUF_THREE  3
#define BUF_FOUR   4
#define BUF_FIVE   5
#define BUF_SIX    6
#define BUF_SIVEN  7
#define BUF_EIGHT  8
#define BUF_NINE   9
#define BUF_TEN    10
#define BUF_ELEVEN  11

#define RADAR_SOCKET_SEND_RES (1U << 0)
#define RADAR_SOCKET_SEND_DBG (1U << 1)
#define RADAR_SOCKET_EVENT_MASK (RADAR_SOCKET_SEND_RES | RADAR_SOCKET_SEND_DBG)

#define RADAR_RET_SUCC 0
#define RADAR_RET_FAIL (-1)

#define SOCKET_SUB_FRAME_LEN 64
#define SOCKET_TAIL_LEN 4
#define PC_OUT_NUM 20 // 脉冲压缩后输出的距离门个数

typedef struct {
    int32_t real;
    int32_t imag;
} complex_stru_t;

typedef struct {
    uint16_t delta_time;
    int16_t iso_res;
} radar_debug_info_t;

typedef struct {
    complex_stru_t data[PC_OUT_NUM];
    radar_debug_info_t radar_debug_info;
    uint8_t tail[SOCKET_TAIL_LEN];
} radar_socket_raw_data_t;

typedef struct {
    uint8_t times; // 发射次数, 0-无限次, 默认值为0
    uint8_t loop; // 单次雷达工作, TRx的波形数量, 默认值为8
    uint8_t ant; // Rx天线数量, 默认值为0
    uint8_t wave; // 雷达发射波形类型选择, 默认值为2
    uint8_t dbg_type; // 维测方式. 0-不外发维测数据, 1-外发脉压后的数据, 2-外发相干累加后的数据, 默认值为0
    uint16_t period; // 雷达工作间隔, 默认值为5000
} radar_driver_para_t;

typedef struct {
    uint8_t d_th_1m;             // dispersion
    uint8_t d_th_2m;
    uint8_t p_th;                // presence
    uint8_t t_th_1m;             // track
    uint8_t t_th_2m;
    uint8_t b_th_ratio;          // bitmap
    uint8_t b_th_cnt;
    uint8_t a_th;                // ai
    uint8_t pt_cld_para_1;       // 0~1米范围点云门限, 范围0-20
    uint8_t pt_cld_para_2;       // 1~2米范围点云门限, 范围0-20
    uint8_t pt_cld_para_3;       // 2~5米范围点云门限, 范围0-20
    uint8_t pt_cld_para_4;       // 5+米范围点云门限, 范围0-20
    uint8_t rd_pwr_para_1;       // 0~1米范围点云门限, 范围1-100
    uint8_t rd_pwr_para_2;       // 1~2米范围点云门限, 范围1-100
    uint8_t rd_pwr_para_3;       // 2~6米范围点云门限, 范围1-100
} radar_alg_param_cmd_t;

typedef enum {
    D_TH_1M_IDX = 0,
    D_TH_2M_IDX,
    P_TH_IDX,
    T_TH_1M_IDX,
    T_TH_2M_IDX,
    B_TH_RATIO_IDX,
    B_TH_CNT_IDX,
    A_TH_IDX,
    PT_CLD_PARA1_IDX,
    PT_CLD_PARA2_IDX,
    PT_CLD_PARA3_IDX,
    PT_CLD_PARA4_IDX,
    RD_PWR_PARA1_IDX,
    RD_PWR_PARA2_IDX,
    RD_PWR_PARA3_IDX,
    ALG_PARA_BUTT
} radar_alg_param_enum_t;

static int32_t g_client = -1;
static int32_t g_sk = -1; // socket创建套接字

static void radar_update_alg_ctrl_weakref(uint8_t height, \
    uint8_t material, uint8_t scenario, bool fusion_track, bool fusion_ai) \
    __attribute__ ((weakref("radar_update_alg_param")));
static void radar_update_alg_para_weakref(radar_alg_param_cmd_t *alg_param, bool write_to_nv)
    __attribute__ ((weakref("radar_update_alg_param_by_cmd")));
static void radar_set_driver_para_weakref(radar_driver_para_t *para) \
    __attribute__ ((weakref("radar_set_driver_para")));
static const uint8_t *radar_get_socket_res_addr_weakref(uint16_t *len) \
    __attribute__ ((weakref("radar_get_socket_res_addr")));
static const uint8_t *radar_get_socket_dbg_addr_weakref(uint16_t *len) \
    __attribute__ ((weakref("radar_get_socket_dbg_addr")));

static osal_event g_radar_socket_events;
static radar_socket_raw_data_t g_socket_pc_data[SOCKET_SUB_FRAME_LEN] = {0};

uint8_t *radar_get_socket_data_addr(uint8_t subframe_idx)
{
    g_socket_pc_data[subframe_idx].tail[0] = 'n';
    g_socket_pc_data[subframe_idx].tail[1] = 'd';
    g_socket_pc_data[subframe_idx].tail[2] = '\r';
    g_socket_pc_data[subframe_idx].tail[3] = '\n';

    return (uint8_t*)&g_socket_pc_data[subframe_idx];
}

void radar_init_socket_event(void)
{
    memset_s(&g_radar_socket_events, sizeof(osal_event), 0, sizeof(osal_event));
    osal_event_init(&g_radar_socket_events);
}

int16_t radar_send_socket_event(uint32_t event)
{
    int16_t ret = osal_event_write(&g_radar_socket_events, event);
    if (ret == 0) {
        return RADAR_RET_SUCC;
    }
    return RADAR_RET_FAIL;
}

static int16_t radar_recv_socket_event(uint32_t *event_bits)
{
    int16_t ret = osal_event_read(&g_radar_socket_events, RADAR_SOCKET_EVENT_MASK, OSAL_EVENT_FOREVER,
        OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);
    if (ret == -1) {
        *event_bits = 0;
        return RADAR_RET_FAIL;
    }
    *event_bits = (uint32_t)ret;
    return RADAR_RET_SUCC;
}

static uint32_t radar_htonl(int16_t n)
{
    return ((uint32_t)((((n) & 0xff) << OFFSET_TWENTY_FOUR) | (((n) & 0xff00) << OFFSET_EIGHT) | \
            (((n) >> OFFSET_EIGHT)  & 0xff00) | (((n) >> OFFSET_TWENTY_FOUR) & 0xff)));
}

static uint16_t radar_htons(int16_t n)
{
    return ((uint16_t)((((n) & 0xff) << OFFSET_EIGHT) | (((n) >> OFFSET_EIGHT) & 0xff)));
}

static void unpack_set_sts(uint8_t *buf, int32_t idx)
{
    if (buf[idx + BUF_TWO] == TLV_CMD_SET_STS_LEN) {
        uapi_radar_set_status(buf[idx + BUF_THREE]);
    }
}

static void unpack_set_dly(uint8_t *buf, int32_t idx)
{
    if (buf[idx + BUF_TWO] == TLV_CMD_SET_DLY_LEN) {
        uapi_radar_set_delay_time(buf[idx + BUF_THREE]);
    }
}

static void unpack_set_ctrl(uint8_t *buf, int32_t idx)
{
    if (buf[idx + BUF_TWO] == TLV_CMD_SET_CTRL_LEN) {
        uint8_t height = buf[idx + BUF_THREE];
        uint8_t material = buf[idx + BUF_FOUR];
        uint8_t scenario = buf[idx + BUF_FIVE];
        uint8_t use_track = buf[idx + BUF_SIX];
        uint8_t use_ai = buf[idx + BUF_SIVEN];

        if (radar_update_alg_ctrl_weakref != NULL) {
            radar_update_alg_ctrl_weakref(height, material, scenario, use_track, use_ai);
        }
    }
}

static void unpack_set_alg_para(uint8_t *buf, int32_t idx)
{
    if (buf[idx + BUF_TWO] == TLV_CMD_SET_CTRL_LEN) {
        uint8_t cnt = idx + BUF_TWO + 1;
        radar_alg_param_cmd_t para;
        para.d_th_1m = buf[cnt + D_TH_1M_IDX];
        para.d_th_2m = buf[cnt + D_TH_2M_IDX];
        para.p_th = buf[cnt + P_TH_IDX];
        para.t_th_1m = buf[cnt + T_TH_1M_IDX];
        para.t_th_2m = buf[cnt + T_TH_2M_IDX];
        para.b_th_ratio = buf[cnt + B_TH_RATIO_IDX];
        para.b_th_cnt = buf[cnt + B_TH_CNT_IDX];
        para.a_th = buf[cnt + A_TH_IDX];
        para.pt_cld_para_1 = buf[cnt + PT_CLD_PARA1_IDX];
        para.pt_cld_para_2 = buf[cnt + PT_CLD_PARA2_IDX];
        para.pt_cld_para_3 = buf[cnt + PT_CLD_PARA3_IDX];
        para.pt_cld_para_4 = buf[cnt + PT_CLD_PARA4_IDX];
        para.rd_pwr_para_1 = buf[cnt + RD_PWR_PARA1_IDX];
        para.rd_pwr_para_2 = buf[cnt + RD_PWR_PARA2_IDX];
        para.rd_pwr_para_3 = buf[cnt + RD_PWR_PARA3_IDX];
        bool write_to_nv = buf[cnt + ALG_PARA_BUTT];

        if (radar_update_alg_para_weakref != NULL) {
            radar_update_alg_para_weakref(&para, write_to_nv);
        }
    }
}

static void unpack_set_para(uint8_t *buf, int32_t idx)
{
    if (buf[idx + BUF_TWO] == TLV_CMD_SET_PARA_LEN) {
        radar_driver_para_t para;
        para.times = buf[idx + BUF_THREE];
        para.loop = buf[idx + BUF_FOUR];
        para.ant = buf[idx + BUF_FIVE];
        para.wave = buf[idx + BUF_SIX];
        para.dbg_type = buf[idx + BUF_SIVEN];
        para.period = 5000;

        if (radar_set_driver_para_weakref != NULL) {
            radar_set_driver_para_weakref(&para);
        }
    }
}

static void unpack_tlv(uint8_t *buf, int32_t bytes)
{
    if ((buf == NULL) || (bytes < TLV_MIN_LEN)) {
        return;
    }

    for (int32_t i = 0; i < bytes; i++) {
        if (buf[i] == TLV_HEADER) { // find TLV header
            switch (buf[i + 1]) { // check TLV tag
                case TLV_CMD_SET_STS: {
                    unpack_set_sts(buf, i);
                    continue;
                }
                case TLV_CMD_SET_DLY: {
                    unpack_set_dly(buf, i);
                    continue;
                }
                case TLV_CMD_SET_CTRL: {
                    unpack_set_ctrl(buf, i);
                    continue;
                }
                case TLV_CMD_SET_PARA: {
                    unpack_set_para(buf, i);
                    continue;
                }
                case TLV_CMD_SET_ALG_PARA: {
                    unpack_set_alg_para(buf, i);
                    continue;
                }
                default:
                    continue;
            }
        }
    }
}

static int32_t recv_tlv(int32_t sk)
{
    uint8_t recv_buf[TLV_MAX_LEN];

    int32_t recv_bytes = recv(sk, &recv_buf, sizeof(uint8_t) * TLV_MAX_LEN, 0);
    if (recv_bytes < 0) {
        return -1;
    } else if (recv_bytes == 0) {
        // continue to recv
        return -1;
    }
    PRINT("%s::recv_bytes:%d\r\n", RADAR_SOCKET_LOG, recv_bytes);
    unpack_tlv(recv_buf, recv_bytes);

    return 0;
}

static int32_t radar_socket_recv_task_body(void *param)
{
    UNUSED(param);
    int32_t recv_data = 0;
    struct sockaddr_in client_addr = {0};
    socklen_t client_addr_len;

    for (;;) {
        g_client = accept(g_sk, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_len);
        if (g_client < 0) {
            PRINT("%s::radar socket error accept.\r\n", RADAR_SOCKET_LOG);
            continue;
        }

        for (;;) {
            recv_data = recv_tlv(g_client);
            if (recv_data < 0) {
                lwip_close(g_client);
                break;
            }
        }
    }

    return 0;
}

static void pack_tlv_res(uint8_t *buf, uint16_t *buf_idx)
{
    uint16_t data_len = 0;
    const uint8_t* res = radar_get_socket_res_addr_weakref(&data_len);
    // pack tlv len
    uint16_t idx = *buf_idx;
    (void)memcpy_s(&buf[idx], sizeof(uint16_t), &data_len, sizeof(uint16_t));
    idx += sizeof(uint16_t);
    // pack tlv value
    (void)memcpy_s(&buf[idx], data_len, res, data_len);
    idx += data_len;
    *buf_idx = idx;
}

static void pack_tlv_dbg(uint8_t *buf, uint16_t *buf_idx)
{
    uint16_t data_len = sizeof(radar_socket_raw_data_t) * SOCKET_SUB_FRAME_LEN;
    const uint8_t* dbg = (uint8_t*)g_socket_pc_data;
    // pack tlv len
    uint16_t idx = *buf_idx;
    (void)memcpy_s(&buf[idx], sizeof(uint16_t), &data_len, sizeof(uint16_t));
    idx += sizeof(uint16_t);
    // pack tlv value
    (void)memcpy_s(&buf[idx], data_len, dbg, data_len);
    idx += data_len;
    *buf_idx = idx;
}

static uint16_t pack_tlv(uint8_t *buf, uint16_t size, uint8_t tag)
{
    if ((buf == NULL) || (size < TLV_MIN_LEN)) {
        return 0;
    }

    uint16_t buf_idx = 0;
    // pack tlv header
    buf[buf_idx++] = TLV_HEADER;
    // pack tlv tag
    buf[buf_idx++] = tag;

    switch (tag) {
        case TLV_CMD_GET_RES: {
            pack_tlv_res(buf, &buf_idx);
            break;
        }
        case TLV_CMD_GET_DBG: {
            pack_tlv_dbg(buf, &buf_idx);
            break;
        }
        default:
            break;
    }
    return buf_idx;
}

static int32_t radar_socket_send_task_body(void *param)
{
    UNUSED(param);
    uint8_t send_buf[TLV_MAX_LEN];
    uint16_t send_len = 0;
    uint32_t event_bits = 0;
    for (;;) {
        int16_t ret = radar_recv_socket_event(&event_bits);
        if ((ret == -1) || (event_bits == 0)) {
            continue;
        }

        if (event_bits & RADAR_SOCKET_SEND_RES) {
            send_len = pack_tlv(send_buf, TLV_MAX_LEN, TLV_CMD_GET_RES);
            if (send_len == 0) {
                continue;
            }
            int32_t res = send(g_client, &send_buf, send_len, 0);
            if (res < 0) {
                continue;
            }
            (void)memset_s(send_buf, sizeof(uint8_t) * TLV_MAX_LEN, 0, sizeof(uint8_t) * TLV_MAX_LEN);
        }

        if (event_bits & RADAR_SOCKET_SEND_DBG) {
            uint8_t send_dbg_buf[TLV_MAX_DBG_LEN];
            send_len = pack_tlv(send_dbg_buf, TLV_MAX_LEN, TLV_CMD_GET_DBG);
            if (send_len == 0) {
                continue;
            }
            int32_t res = send(g_client, &send_dbg_buf, send_len, 0);
            if (res < 0) {
                continue;
            }
        }

        event_bits = 0;
    }

    return 0;
}

void radar_socket_recv_task(void)
{
    osThreadAttr_t attr;
    attr.name       = "radar_socket_recv";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = RADAR_SOCKET_TASK_STACK_SIZE;
    attr.priority   = RADAR_SOCKET_TASK_PRIO;
    (void)osThreadNew((osThreadFunc_t)radar_socket_recv_task_body, NULL, &attr);
}

void radar_socket_send_task(void)
{
    osThreadAttr_t attr;
    attr.name       = "radar_socket_send";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = RADAR_SOCKET_TASK_STACK_SIZE;
    attr.priority   = RADAR_SOCKET_TASK_PRIO;
    (void)osThreadNew((osThreadFunc_t)radar_socket_send_task_body, NULL, &attr);
}

int32_t radar_socket_server(void)
{
    int32_t ret;
    struct sockaddr_in server_addr = {0};
    uint32_t opt = 1;
    uint32_t buffer = 1024 * 1024; // 1M

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = radar_htons(SOCK_TARGET_PORT);
    server_addr.sin_addr.s_addr = radar_htonl(INADDR_ANY); // 0.0.0.0

    g_sk = socket(AF_INET, SOCK_STREAM, 0);
    if (g_sk < 0) {
        return -1;
    }

    setsockopt(g_sk, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(g_sk, SOL_SOCKET, SO_SNDBUF, &buffer, sizeof(buffer));
    setsockopt(g_sk, SOL_SOCKET, SO_RCVBUF, &buffer, sizeof(buffer));

    ret = bind(g_sk, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0) {
        return -1;
    }

    ret = listen(g_sk, IP_TCP_SERVER_LISTEN_NUM);
    if (ret < 0) {
        return -1;
    }

    radar_socket_recv_task();
    radar_socket_send_task();

    return 0;
}