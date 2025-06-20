/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Common operations on the hichain, including session creation and destruction. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hichain.h"

DLL_API_PUBLIC void registe_log(struct log_func_group *log)
{
    app_call1_ret_void(APP_CALL_REGISTE_LOG, registe_log, struct log_func_group *, log);
}

DLL_API_PUBLIC hc_handle get_instance(const struct session_identity *identity, enum hc_type type,
    const struct hc_call_back *call_back)
{
    app_call3(APP_CALL_GET_INSTANCE, get_instance, hc_handle, const struct session_identity *, identity,
        enum hc_type, type, const struct hc_call_back *, call_back);
    return NULL;
}

DLL_API_PUBLIC void destroy(hc_handle *handle)
{
    app_call1_ret_void(APP_CALL_DESTROY, destroy, hc_handle *, handle);
}

DLL_API_PUBLIC void set_context(hc_handle handle, void *context)
{
    app_call2_ret_void(APP_CALL_SET_CONTEXT, set_context, hc_handle, handle, void *, context);
}

DLL_API_PUBLIC int32_t receive_data(hc_handle handle, struct uint8_buff *data)
{
    app_call2(APP_CALL_RECEIVE_DATA, receive_data, int32_t, hc_handle, handle, struct uint8_buff *, data);
    return 0;
}

DLL_API_PUBLIC int32_t receive_data_with_json_object(hc_handle handle, const void *json_object)
{
    app_call2(APP_CALL_RECEIVE_DATA_WITH_JSON_OBJECT, receive_data_with_json_object, int32_t,
        hc_handle, handle, const void *, json_object);
    return 0;
}

#ifndef _CUT_API_
DLL_API_PUBLIC int32_t init_center(const struct hc_package_name *package_name,
    const struct hc_service_type *service_type, const struct hc_auth_id *auth_id, struct hc_key_alias *dek)
{
    app_call4(APP_CALL_INIT_CENTER, init_center, int32_t, const struct hc_package_name *, package_name,
        const struct hc_service_type *, service_type, const struct hc_auth_id *, auth_id,
        struct hc_key_alias *, dek);
    return 0;
}

DLL_API_PUBLIC int32_t start_pake(hc_handle handle, const struct operation_parameter *params)
{
    app_call2(APP_CALL_START_PAKE, start_pake, int32_t, hc_handle, handle, const struct operation_parameter *, params);
    return 0;
}

DLL_API_PUBLIC int32_t authenticate_peer(hc_handle handle, struct operation_parameter *params)
{
    app_call2(APP_CALL_AUTHENTICATE_PEER, authenticate_peer, int32_t,
        hc_handle, handle, struct operation_parameter *, params);
    return 0;
}

DLL_API_PUBLIC int32_t delete_local_auth_info(hc_handle handle, struct hc_user_info *user_info)
{
    app_call2(APP_CALL_DELETE_LOCAL_AUTH_INFO, delete_local_auth_info, int32_t,
        hc_handle, handle, struct hc_user_info *, user_info);
    return 0;
}

DLL_API_PUBLIC int32_t import_auth_info(hc_handle handle, struct hc_user_info *user_info,
    struct hc_auth_id *auth_id, enum hc_export_type auth_info_type, struct uint8_buff *auth_info)
{
    app_call5(APP_CALL_IMPORT_AUTH_INFO, import_auth_info, int32_t, hc_handle, handle, struct hc_user_info *,
        user_info, struct hc_auth_id *, auth_id, enum hc_export_type, auth_info_type, struct uint8_buff *, auth_info);
    return 0;
}

int32_t add_auth_info(hc_handle handle, const struct operation_parameter *params,
    const struct hc_auth_id *auth_id, int32_t user_type)
{
    app_call4(APP_CALL_ADD_AUTH_INFO, add_auth_info, int32_t, hc_handle, handle,
        const struct operation_parameter *, params, const struct hc_auth_id *, auth_id, int32_t, user_type);
    return 0;
}

int32_t remove_auth_info(hc_handle handle, const struct operation_parameter *params,
    const struct hc_auth_id *auth_id, int32_t user_type)
{
    app_call4(APP_CALL_REMOVE_AUTH_INFO, remove_auth_info, int32_t, hc_handle, handle,
        const struct operation_parameter *, params, const struct hc_auth_id *, auth_id, int32_t, user_type);
    return 0;
}

DLL_API_PUBLIC int32_t is_trust_peer(hc_handle handle, struct hc_user_info *user_info)
{
    app_call2(APP_CALL_IS_TRUST_PEER, is_trust_peer, int32_t, hc_handle, handle, struct hc_user_info *, user_info);
    return 0;
}

DLL_API_PUBLIC uint32_t list_trust_peers(hc_handle handle, int32_t trust_user_type,
    struct hc_auth_id *owner_auth_id, struct hc_auth_id **auth_id_list)
{
    app_call4(APP_CALL_LIST_TRUST_PEERS, list_trust_peers, uint32_t, hc_handle, handle, int32_t, trust_user_type,
        struct hc_auth_id *, owner_auth_id, struct hc_auth_id **, auth_id_list);
    return 0;
}
#endif /* _CUT_XXX_ */

DLL_API_PUBLIC void set_self_auth_id(hc_handle handle, struct uint8_buff *data)
{
    app_call2_ret_void(APP_CALL_SET_SELF_AUTH_ID, set_self_auth_id, hc_handle, handle, struct uint8_buff *, data);
}