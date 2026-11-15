/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: Function related to wifi aware(NAN) feature.
 * Create: 2021-04-16
 */
#ifdef _PRE_WLAN_FEATURE_SDP
#include "wal_liteos_sdp.h"
#include "wal_config_sdp.h"
#include "wal_utils.h"
#include "wal_common.h"
#include "hmac_sdp.h"
#include "frw_hmac.h"
#include "wlan_msg.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_WIFI_HOST_WAL_LITEOS_SDP_C

#define WAL_CCPRIV_PARSE_TIME_TOTAL_LEN     4                       /* 时间属性总长度 */
#define WAL_CCPRIV_PARSE_TIME_WAKE_TYPE     2                       /* 时间属性总周期 */


/*****************************************************************************
 函 数 名  : uapi_ccpriv_adjust_tx_power
 功能描述  : 配置固定功率
 输入参数  : 无
 输出参数  : 无
*****************************************************************************/
td_u32  uapi_ccpriv_adjust_tx_power(oal_net_device_stru *net_dev, osal_s8 *param)
{
    mac_cfg_adjust_tx_power_stru *adjust_tx_power;
    td_u32 off_set;
    osal_s8 args[WAL_CCPRIV_CMD_NAME_MAX_LEN];
    td_u32 ret;
    osal_s8 *param_tmp = param;

    mac_cfg_adjust_tx_power_stru tx_power;
    (osal_void)memset_s(&tx_power, OAL_SIZEOF(tx_power), 0, OAL_SIZEOF(tx_power));
    adjust_tx_power = &tx_power;

    /* 获取第一个参数 ch */
    ret = wal_get_cmd_one_arg(param_tmp, args, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_adjust_tx_power::parse arg failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    adjust_tx_power->ch = (osal_u8)oal_atoi((const osal_s8 *)args);

    /* 获取第二个参数 power */
    ret = (osal_u32)wal_get_cmd_one_arg((osal_s8 *)param_tmp, args, WAL_CCPRIV_CMD_NAME_MAX_LEN, &off_set);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_adjust_tx_power::parse arg failed [%d]!}", ret);
        return ret;
    }
    param_tmp += off_set;
    adjust_tx_power->power = (osal_s8)oal_atoi((const osal_s8 *)args);

    ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_ADJUST_TX_POWER,
        (osal_u8 *)&tx_power, OAL_SIZEOF(tx_power));
    if (osal_unlikely(ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_adjust_tx_power::return err code [%d]!}", ret);
    }

    return ret;
}

osal_u32 uapi_ccpriv_sdp_init(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 send_ret;
    osal_s32 tmp;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (param == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_init::net_dev or param null ptr error %d, %d!}", net_dev,
                       param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* sdp初始化命令: ccpriv "wlan0 sdp_enable 0 | 1 | 2" */
    send_ret = wal_cmd_get_digit_with_range(&param, WAL_SDP_DISABLE, WAL_SDP_EN_SC, &tmp);
    if (send_ret != OAL_SUCC) {
        oam_warning_log0(0, OAM_SF_ANY, "{uapi_ccpriv_sdp_init::the sdp init command is error!}");
        return OAL_ERR_CODE_INVALID_CONFIG;
    }

    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SDP_INIT,
        (osal_u8 *)&tmp, OAL_SIZEOF(tmp));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_init::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

OAL_STATIC osal_void wal_ccpriv_sdp_srt_to_hex(const osal_s8 *param, osal_u8 len, osal_s8 *value)
{
    osal_u8 index;
    const osal_s8 *str = param;

    for (index = 0; index < len; index++) {
        /* 每2位字符转为一个十六进制，第一位乘以16 */
        value[index / 2] = (osal_s8)(value[index / 2] * 16 * (index % 2) + oal_strtohex(str));
        str++;
    }
}

OAL_STATIC osal_u32 wal_ccpriv_sdp_start_func(oal_net_device_stru *net_dev, osal_s8 *param,
    wlan_cfgid_enum_uint16 wid, osal_u8 type)
{
    osal_u32 offset;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_s32 send_ret;
    osal_u32 ret;
    osal_s32 value;
    osal_s8 *sdp_params = param;
    oal_sdp_service *service_para = OAL_PTR_NULL;
    oal_sdp_service service_info;
    (osal_void)memset_s(&service_info, OAL_SIZEOF(service_info), 0, OAL_SIZEOF(service_info));
    service_para = &service_info;

    /*
     * sdp发布服务命令:ccpriv "wlan0 sdp_start_publish service_name local_handle"
     * sdp订阅服务命令:ccpriv "wlan0 sdp_start_subscribe service_name local_handle"
     */
    ret = wal_get_cmd_one_arg(sdp_params, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    /* 传入的是十六进制字符，长度为name真实长度的2倍 */
    if ((ret != OAL_SUCC) || (osal_strlen((const char *)name) != SDP_HASH_SERVICE_NAME_LEN * 2)) {
        oam_warning_log2(0, OAM_SF_SDP, "{wal_ccpriv_sdp_start_func::err_code %d!, len(name):%d}", ret,
            osal_strlen((const char *)name));
        return OAL_FAIL;
    }
    wal_ccpriv_sdp_srt_to_hex((const osal_s8 *)name, (osal_u8)osal_strlen((const char *)name),
        (osal_s8 *)(service_para->name));

    /* 偏移，取下一个参数 */
    sdp_params = sdp_params + offset;
    send_ret = wal_cmd_get_digit_with_range(&sdp_params, WAL_LOCAL_HANDLE_LOW, WAL_LOCAL_HANDLE_HIGH, &value);
    if (send_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_sdp_start_func::return err_code[%u]!}", send_ret);
        return (osal_u32)send_ret;
    }
    service_para->local_handle = (osal_u8)value;
    service_para->type = type;

    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), wid,
        (osal_u8 *)&service_info, OAL_SIZEOF(service_info));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_start_func::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_sdp_start_subscribe(oal_net_device_stru *net_dev, osal_s8 *param)
{
    return wal_ccpriv_sdp_start_func(net_dev, param, WLAN_MSG_W2H_CFG_SDP_START_SUBSCRIBE,
        OAL_SDP_SERVICE_TYPE_SUBSCRIBE);
}

OAL_STATIC osal_u32 wal_ccpriv_sdp_cancle_func(oal_net_device_stru *net_dev, osal_s8 *param,
    wlan_cfgid_enum_uint16 wid)
{
    osal_s32 value;
    osal_u8 local_handle;
    osal_s32 send_ret;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (param == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{wal_ccpriv_sdp_cancle_func::net_dev or param null ptr error %d, %d!}",
                       net_dev, param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*
     * sdp取消订阅命令:ccpriv "wlan0 sdp_cancle_subscribe local_handle "
     * sdp取消发布命令:ccpriv "wlan0 sdp_cancle_publish local_handle "
     */
    send_ret = wal_cmd_get_digit_with_range(&param, WAL_LOCAL_HANDLE_LOW, WAL_LOCAL_HANDLE_HIGH,
        (osal_s32 *)&value);
    if (send_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{wal_ccpriv_sdp_cancle_func::return err_code[%u]!}", send_ret);
        return (osal_u32)send_ret;
    }

    local_handle = (osal_u8)value;
    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), wid,
        (osal_u8 *)&local_handle, OAL_SIZEOF(local_handle));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_cancle_func::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_sdp_cancle_subscribe(oal_net_device_stru *net_dev, osal_s8 *param)
{
    return wal_ccpriv_sdp_cancle_func(net_dev, param, WLAN_MSG_W2H_CFG_SDP_CANCEL_SUBSCRIBE);
}

OAL_STATIC osal_u32 wal_ccpriv_sdp_parse_info(osal_s8 **src_head, oal_sdp_info *sdp_param)
{
    osal_u32 offset;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32 ret;
    osal_s8 *param = *src_head;

    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    /* 传入的是十六进制字符，长度为name真实长度的2倍 */
    if ((ret != OAL_SUCC) || (osal_strlen((const char *)name) != SDP_HASH_SERVICE_NAME_LEN * 2)) {
        oam_warning_log2(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_info::err_code %d!, len(name):%d}", ret,
            osal_strlen((const char *)name));
        return OAL_FAIL;
    }
    wal_ccpriv_sdp_srt_to_hex((const osal_s8 *)name, (osal_u8)osal_strlen((const char *)name),
        (osal_s8 *)(sdp_param->name));

    /* 偏移，取下一个参数local_handle */
    param = param + offset;
    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_info::wal_get_cmd_one_arg return err_code %d!}",
                         ret);
        return ret;
    }
    sdp_param->local_handle = (osal_u8)oal_atoi((const osal_s8 *)name);

    /* 偏移，取下一个参数peer_handle */
    param = param + offset;
    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_info::wal_get_cmd_one_arg return err_code %d!}",
                         ret);
        return ret;
    }
    sdp_param->peer_handle = (osal_u8)oal_atoi((const osal_s8 *)name);

    /* 偏移，取下一个参数len */
    param = param + offset;
    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_info::wal_get_cmd_one_arg return err_code %d!}",
                         ret);
        return ret;
    }
    sdp_param->len = (osal_u8)oal_atoi((const osal_s8 *)name);

    /* 校验合法性，命令输入的是十六进制字符，data长度不能超过命令最大长度的2分之一 */
    if ((sdp_param->len == 0) || (sdp_param->len >= WAL_CCPRIV_CMD_NAME_MAX_LEN / 2) ||
        (sdp_param->local_handle == 0) || (sdp_param->peer_handle == 0)) {
        oam_warning_log3(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_info:: len:[%d],local_handle[%d],peer_handle[%d]!}",
                         sdp_param->len, sdp_param->local_handle, sdp_param->peer_handle);
        return OAL_FAIL;
    }
    param = param + offset;
    *src_head = param;

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_sdp_send_data(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 offset;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_s32 send_ret;
    osal_u32 ret;
    oal_sdp_info *sdp_param = OAL_PTR_NULL;
    osal_s8 *tmp = param;
    oal_sdp_info sdp_info;
    (osal_void)memset_s(&sdp_info, OAL_SIZEOF(sdp_info), 0, OAL_SIZEOF(sdp_info));
    sdp_param = &sdp_info;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (param == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_send_data::net_dev or param null ptr error %d, %d!}",
                       net_dev, param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* sdp发送数据命令: ccpriv "wlan0 sdp_send_data peer_mac local_handle peer_handle len data" */
    ret = wal_ccpriv_sdp_parse_info(&tmp, sdp_param);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_send_data::wal_ccpriv_sdp_parse_info return err_code %d!}",
                         ret);
        return ret;
    }
    ret = wal_get_cmd_one_arg(tmp, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    /* 命令输入的是十六进制字符，长度为data真实长度的2倍 */
    if ((ret != OAL_SUCC) || (osal_strlen((const char *)name) != sdp_param->len * 2)) {
        oam_warning_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_send_data::err_code %d!, len(name):%d}", ret,
            osal_strlen((const char *)name));
        return OAL_FAIL;
    }
    /*
     * 申请内存存放用户信息，将内存指针作为事件payload抛下去
     * 此处申请的内存在事件处理函数释放(hmac_sdp_send_data)
     */
    sdp_param->data = oal_mem_alloc(OAL_MEM_POOL_ID_LOCAL, sdp_param->len, OAL_TRUE);
    if (sdp_param->data == OAL_PTR_NULL) {
        oam_error_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_send_data::alloc data fail(size:%u), return null ptr!}",
                       sdp_param->len);
        return OAL_ERR_CODE_ALLOC_MEM_FAIL;
    }
    (osal_void)memset_s(sdp_param->data, sdp_param->len, 0, sdp_param->len);
    wal_ccpriv_sdp_srt_to_hex((const osal_s8 *)name, (osal_u8)osal_strlen((const char *)name),
        (osal_s8 *)(sdp_param->data));
    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SDP_SEND_DATA,
        (osal_u8 *)&sdp_info, OAL_SIZEOF(sdp_info));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oal_mem_free(sdp_param->data, OAL_TRUE);
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_send_data::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

OAL_STATIC osal_u32 wal_ccpriv_sdp_parse_time(osal_s8 *src_head, oal_sdp_wakeup_info *time_param)
{
    osal_u32 offset;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32 ret;
    osal_s8 *param = src_head;

    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_time::wal_get_cmd_one_arg return err_code %d!}",
                         ret);
        return ret;
    }
    time_param->total_period = (osal_u32)oal_atoi((const osal_s8 *)name);

    /* 偏移，取下一个参数 */
    param = param + offset;
    ret = wal_get_cmd_one_arg(param, name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_time::wal_get_cmd_one_arg return err_code %d!}",
                         ret);
        return ret;
    }
    time_param->wake_period = (td_u32)oal_atoi((const osal_s8 *)name);
    if ((time_param->total_period == 0) || (time_param->wake_period == 0)) {
        oam_warning_log2(0, OAM_SF_SDP, "{wal_ccpriv_sdp_parse_time::total[%d], wake[%d]!}",
                         time_param->total_period, time_param->wake_period);
        return OAL_FAIL;
    }
    /*
     * 私有同步帧格式：时间属性总长度0xC：
     * Total Period (T=1,L=4,V=0xxxxxxxxx)
     * Wake Period (T=2,L=4,V=0xxxxxxxxx)
     */
    time_param->attr_len = 0xC;
    time_param->total_type = 1;
    time_param->total_len = WAL_CCPRIV_PARSE_TIME_TOTAL_LEN;
    time_param->wake_type = WAL_CCPRIV_PARSE_TIME_WAKE_TYPE;
    time_param->wake_len = WAL_CCPRIV_PARSE_TIME_TOTAL_LEN;

    return OAL_SUCC;
}

osal_void wal_sdp_srt_to_hex(const osal_s8 *param, osal_u8 len, osal_s8 *value)
{
    osal_u8 index;
    const osal_s8 *str = param;

    for (index = 0; index < len; index++) {
        /* 每2位字符转为一个十六进制，第一位乘以16 */
        value[index / 2] = (osal_s8)(value[index / 2] * 16 * (index % 2) + oal_strtohex(str));
        str++;
    }
}

osal_u32 uapi_ccpriv_sdp_start_subscribe_svr(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 off_set;
    osal_s8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32 arg_ret;
    osal_s32 msg_ret;
    osal_s32 value;
    osal_s8 *sdp_params = param;
    oal_sdp_subscribe_info subscribe_info;
    oal_sdp_subscribe_info *subscribe_param = OAL_PTR_NULL;
    (osal_void)memset_s(&subscribe_info, OAL_SIZEOF(subscribe_info), 0, OAL_SIZEOF(subscribe_info));
    subscribe_param = &subscribe_info;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (sdp_params == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_start_subscribe_svr:net_dev or sdp_params null %p, %p!}",
            (uintptr_t)net_dev, (uintptr_t)sdp_params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*
     * sdp订阅服务命令:ccpriv "wlan0 sdp_start_subscribe_svr service_name local_handle"
     */
    arg_ret = wal_get_cmd_one_arg(sdp_params, name, OAL_SIZEOF(name), &off_set);
    /* 传入的是十六进制字符，长度为name真实长度的2倍, 目前手机APP发出的是 1E87E9A99321 */
    if ((arg_ret != OAL_SUCC) || (osal_strlen((const char *)name) != SDP_HASH_SERVICE_NAME_LEN * 2)) {
        oam_warning_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_start_subscribe_svr:err_code %d!, osal_strlen(name):%d}",
            arg_ret, osal_strlen((const char *)name));
        return OAL_FAIL;
    }
    wal_sdp_srt_to_hex((const osal_s8 *)name, (osal_u8)osal_strlen((const char *)name),
        (osal_s8 *)(subscribe_param->name));

    /* 偏移，取下一个参数 */
    sdp_params = sdp_params + off_set;
    msg_ret = wal_cmd_get_digit_with_range(&sdp_params, WAL_LOCAL_HANDLE_LOW, WAL_LOCAL_HANDLE_HIGH, &value);
    if (msg_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_sdp_start_subscribe_svr::return err_code[%u]!}", msg_ret);
        return (osal_u32)msg_ret;
    }
    subscribe_param->local_handle = (osal_u8)value;
    subscribe_param->type = 3; /* 3 表示 WAL_SDP_TEST_TYPE_SUBSCRIBE_SERVICE */

    msg_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SDP_TEST_SYNC_PRIVATE,
        (osal_u8 *)&subscribe_info, OAL_SIZEOF(subscribe_info));
    if (OAL_UNLIKELY(msg_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_start_subscribe_svr::return err code %d!}", msg_ret);
        return (osal_u32)msg_ret;
    }

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_sdp_cancel_subscribe_svr(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_s32 send_ret;
    osal_s32 value;
    oal_sdp_subscribe_info *sub_param = OAL_PTR_NULL;
    oal_sdp_subscribe_info subscribe_info;
    (osal_void)memset_s(&subscribe_info, OAL_SIZEOF(subscribe_info), 0, OAL_SIZEOF(subscribe_info));
    sub_param = &subscribe_info;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (param == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_cancel_subscribe_svr:net_dev or param null %p, %p!}",
            (uintptr_t)net_dev, (uintptr_t)param);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /*
     * sdp取消订阅服务命令:ccpriv "wlan0 sdp_cancel_subscribe_svr         local_handle "
     */
    send_ret = wal_cmd_get_digit_with_range(&param, WAL_LOCAL_HANDLE_LOW, WAL_LOCAL_HANDLE_HIGH, &value);
    if (send_ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_sdp_cancel_subscribe_svr::return err_code[%u]!}", send_ret);
        return (osal_u32)send_ret;
    }
    sub_param->local_handle = (osal_u8)value;
    sub_param->type = 4; /* 4 表示 WAL_SDP_TEST_TYPE_SUBSCRIBE_CANCEL */

    send_ret = wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SDP_TEST_SYNC_PRIVATE,
        (osal_u8 *)&subscribe_info, OAL_SIZEOF(subscribe_info));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_cancel_subscribe_svr::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

osal_u32 uapi_ccpriv_sdp_test(oal_net_device_stru *net_dev, osal_s8 *param)
{
    osal_u32 offset;
    osal_u8 name[WAL_CCPRIV_CMD_NAME_MAX_LEN] = {0};
    osal_u32 send_ret;
    osal_u32 ret;
    oal_sdp_wakeup_info *time_param = OAL_PTR_NULL;
    osal_s8 *sdp_params = param;
    osal_u32 type;
    oal_sdp_wakeup_info time_info;
    (osal_void)memset_s(&time_info, OAL_SIZEOF(time_info), 0, OAL_SIZEOF(time_info));
    time_param = &time_info;

    if (OAL_UNLIKELY((net_dev == OAL_PTR_NULL) || (sdp_params == OAL_PTR_NULL))) {
        oam_error_log2(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_test::net_dev or sdp_params null ptr %p, %p!}",
            (uintptr_t)net_dev, (uintptr_t)sdp_params);
        return OAL_ERR_CODE_PTR_NULL;
    }

    /* sdp发测试命令: ccpriv "wlan0 sdp_test type total wake" */
    ret = wal_cmd_get_digit_with_check_max(&sdp_params, WAL_SDP_EN_SC, &type);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_ANY, "{uapi_ccpriv_sdp_test:: return err_code[%u]!}", ret);
        return ret;
    }

    ret = wal_get_cmd_one_arg((const osal_s8 *)sdp_params, (osal_s8 *)name, WAL_CCPRIV_CMD_NAME_MAX_LEN, &offset);
    if (ret != OAL_SUCC) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_test::wal_get_cmd_one_arg return err_code %d!}", ret);
        return ret;
    }
    /* sta up时信道切换的百分比ratio 仅支持1-9, 非法值时 使用5, 仅在联网成功后vap为sta并up时使用, 其他场景忽略该参数 */
    time_param->ratio = (osal_u8)oal_atoi((const osal_s8 *)name);

    time_param->type = (osal_u8)type;
    if (time_param->type == 0) { /* 私有同步帧测试，需获取时间参数 */
        ret = wal_ccpriv_sdp_parse_time(sdp_params, time_param);
        if (ret != OAL_SUCC) {
            oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_test::wal_ccpriv_sdp_parse_time return err %d!}", ret);
            return ret;
        }
    }
    send_ret = (osal_u32)wal_sync_post2hmac_no_rsp(wal_util_get_vap_id(net_dev), WLAN_MSG_W2H_CFG_SDP_TEST_SYNC_PRIVATE,
        (osal_u8 *)&time_info, OAL_SIZEOF(time_info));
    if (OAL_UNLIKELY(send_ret != OAL_SUCC)) {
        oam_warning_log1(0, OAM_SF_SDP, "{uapi_ccpriv_sdp_test::return err code %d!}", send_ret);
        return (osal_u32)send_ret;
    }

    return OAL_SUCC;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif