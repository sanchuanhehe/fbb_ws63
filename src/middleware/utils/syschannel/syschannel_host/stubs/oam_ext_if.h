/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2022. All rights reserved.
 * Description: oam log printing interface
 */

#ifndef __OAM_EXT_IF_H__
#define __OAM_EXT_IF_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef unref_param
#define unref_param(P)  ((P) = (P))
#endif
#define oam_info_log0(_uc_vap_0, _en_feature_0, fmt)
#define oam_info_log1(_uc_vap_0, _en_feature_0, fmt, p1)
#define oam_info_log2(_uc_vap_0, _en_feature_0, fmt, p1, p2)
#define oam_info_log3(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3)
#define oam_info_log4(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3, p4)
#define oam_warning_log0(_uc_vap_0, _en_feature_0, fmt)
#define oam_warning_log1(_uc_vap_0, _en_feature_0, fmt, p1)
#define oam_warning_log2(_uc_vap_0, _en_feature_0, fmt, p1, p2)
#define oam_warning_log3(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3)
#define oam_warning_log4(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3, p4)
#define oam_error_log0(_uc_vap_0, _en_feature_0, fmt)
#define oam_error_log1(_uc_vap_0, _en_feature_0, fmt, p1)
#define oam_error_log2(_uc_vap_0, _en_feature_0, fmt, p1, p2)
#define oam_error_log3(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3)
#define oam_error_log4(_uc_vap_0, _en_feature_0, fmt, p1, p2, p3, p4)

#define osal_module_license(_license)
#define osal_module_version(_version)
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of oam_ext_if.h */