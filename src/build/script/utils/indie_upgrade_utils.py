#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    indie upgrade utils file
# Copyright HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================
import os
import re
import zlib
import json

from utils.build_utils import root_path

mapping_path = os.path.join(root_path, "application", "samples", "wifi", "hilink_indie_upgrade", "address_mapping")
func_list_file = os.path.join(mapping_path, "include", "func_call_list.h")
hilink_adapt_path = os.path.join(root_path, "application", "samples", "wifi", "ohos_connect", "hilink_adapt")

default_config = {
    "app": {
        "src_path": os.path.join(mapping_path, "hilinksdk", "app_uapi"),
        "src_file": (
            "uapi_hilink_kv_adapter.c",
            "uapi_hilink_mem_adapter.c",
            "uapi_hilink_socket_adapter.c",
            "uapi_hilink_stdio_adapter.c",
            "uapi_hilink_str_adapter.c",
            "uapi_hilink_thread_adapter.c",
            "uapi_hilink_time_adapter.c",
            "uapi_hilink_open_ota_adapter.c",
            "uapi_hilink_open_ota_mcu_adapter.c",
            "uapi_hilink_sys_adapter.c",
            "uapi_hilink_sal_rsa.c",
            "uapi_hilink_tls_client.c",
            "uapi_hilink_sal_aes.c",
            "uapi_hilink_sal_base64.c",
            "uapi_hilink_sal_drbg.c",
            "uapi_hilink_sal_kdf.c",
            "uapi_hilink_sal_md.c",
            "uapi_hilink_sal_mpi.c",
            "uapi_hilink_network_adapter.c",
            "uapi_hilink_softap_adapter.c",
            "uapi_hilink_device.c",
            "uapi_hichain.c",
            "uapi_hilink_ble_adapter.c",
            "uapi_hilink_ble_main.c",
            "uapi_cmsis_liteos2.c",
            "uapi_oh_sle_connection_manager.c",
            "uapi_oh_sle_device_discovery.c",
            "uapi_oh_sle_ssap_server.c",
            "uapi_cJSON.c",
            "uapi_mbed_tls.c",
        ),
        "output_mapping_file": os.path.join(mapping_path, "application", "app_function_mapping.c"),
        "mapping_header": {
            os.path.join(hilink_adapt_path, "adapter", "include"): (
                "hilink_kv_adapter.h",
                "hilink_mbedtls_utils.h",
                "hilink_mem_adapter.h",
                "hilink_network_adapter.h",
                "hilink_open_ota_adapter.h",
                "hilink_open_ota_mcu_adapter.h",
                "hilink_sal_aes.h",
                "hilink_sal_base64.h",
                "hilink_sal_defines.h",
                "hilink_sal_drbg.h",
                "hilink_sal_kdf.h",
                "hilink_sal_md.h",
                "hilink_sal_mpi.h",
                "hilink_sal_rsa.h",
                "hilink_socket_adapter.h",
                "hilink_softap_adapter.h",
                "hilink_stdio_adapter.h",
                "hilink_str_adapter.h",
                "hilink_sys_adapter.h",
                "hilink_thread_adapter.h",
                "hilink_time_adapter.h",
                "hilink_tls_client.h",
            ),
            os.path.join(hilink_adapt_path, "product"): (
                "hilink_device.h",
            ),
            os.path.join(hilink_adapt_path, "include"): (
                "ohos_bt_gatt.h",
                "ohos_bt_def.h",
                "ohos_bt_gatt_server.h",
                "hilink_bt_api.h",
                "hilink_bt_function.h",
                "ohos_bt_gap.h",
                "oh_sle_common.h",
                "oh_sle_connection_manager.h",
                "oh_sle_device_discovery.h",
                "oh_sle_ssap_server.h",
            ),
            os.path.join(root_path, "open_source", "deviceauth", "interfaces", "innerkits", "deviceauth_lite"): (
                "hichain.h",
            ),
            os.path.join(root_path, "kernel", "liteos", "liteos_v208.5.0", "Huawei_LiteOS", "open_source", "CMSIS", \
                         "CMSIS", "RTOS2", "Include"): (
                "cmsis_os2.h",
            ),
            os.path.join(root_path, "open_source", "cjson", "cjson"): (
                "cJSON.h",
            ),
            os.path.join(root_path, "open_source", "mbedtls", "mbedtls_v3.1.0", "include"): (
                "mbedtls/md.h",
                "mbedtls/ctr_drbg.h",
                "mbedtls/bignum.h",
                "mbedtls/ecdh.h",
                "mbedtls/hkdf.h",
                "mbedtls/entropy.h",
                "mbedtls/sha256.h",
                "mbedtls/ecp.h",
            ),
        },
        "white_list": (
            "malloc",
            "free",
            "mbedtls_ecdh_compute_shared",
            "mbedtls_ctr_drbg_seed",
        ),
    },
    "hilink": {
        "src_path": os.path.join(mapping_path, "application", "hilink_uapi"),
        "src_file": (
            "uapi_hilink.c",
            "uapi_hilink_log_manage.c",
            "uapi_hilink_device_ext.c",
            "uapi_ble_cfg_net_api.c",
            "uapi_hilink_bt_function.c",
            "uapi_hilink_network_adapter.c",
            "uapi_hilink_socket_adapter.c",
            "uapi_hilink_custom.c",
            "uapi_hilink_sle_api.c",
            "uapi_hilink_quick_netcfg_api.c",
        ),
        "output_mapping_file": os.path.join(mapping_path, "hilinksdk", "hilink_function_mapping.c"),
        "mapping_header": {
            os.path.join(hilink_adapt_path, "include"): (
                "hilink.h",
                "hilink_log_manage.h",
                "ble_cfg_net_api.h",
                "hilink_bt_function.h",
                "hilink_custom.h",
                "hilink_sle_api.h",
                "hilink_quick_netcfg_api.h",
                "hilink_quick_netcfg_adapter.h",
                "hilink_bt_api.h",
            ),
            os.path.join(hilink_adapt_path, "product"): (
                "hilink_device.h",
            ),
            os.path.join(hilink_adapt_path, "adapter", "include"): (
                "hilink_network_adapter.h",
                "hilink_socket_adapter.h",
            ),
        },
        "white_list": (
        ),
    },
}

indie_upg_check = {}


def search_prototype_from_src_file(filename):
    f = open(filename, "r", encoding="utf-8")
    rgl = r"((?:const )?(?:unsigned )?(?:struct )?(?:enum )?\w+[ \*]*)" + \
          r"(\w+)[\s\n]*" + \
          r"(\()" + \
          r"([\s\*,\w\[\]]*?)" + \
          r"(\))" + \
          r"\s*\{"
    res = re.findall(rgl, f.read())
    f.close()
    return res


def search_prototype_from_header_file(filename):
    f = open(filename, "r", encoding="utf-8")
    content = f.read()
    rgl = r"((?:const )?(?:unsigned )?(?:struct )?(?:enum )?\w+[ \*]*)" + \
          r"(\w+)[\s\n]*" + \
          r"(\()" + \
          r"([\s\*,\w\[\]]*?)" + \
          r"(\));"
    res = re.findall(rgl, content)
    rgl_cJSON = r"CJSON_PUBLIC\(([\w\s\*]+)\)\s" + \
          r"(\w+)[\s\n]*" + \
          r"(\()" + \
          r"([\s\*,\w\[\]]*?)" + \
          r"(\));"
    res_cJSON = re.findall(rgl_cJSON, content)
    f.close()
    return res + res_cJSON


def process_func_name(tag, func_name):
    for i in range(len(func_name) - 1, 0, -1):
        if (func_name[i - 1].islower() or func_name[i - 1].isdigit()) and func_name[i].isupper():
            func_name = func_name[:i] + "_" + func_name[i:]
    return tag.upper() + "_CALL_" + func_name.upper()


def process_prototype(prototype_tpl):
    # remove param name, param is in index 3 of the tuple
    param_list = prototype_tpl[3].split(",")
    param_type_list = []
    for param in param_list:
        match_type = re.findall(r"(.*?)\w+\s*(\[\])?\s*$", param)
        if len(match_type) > 0:
            param_type_list.append("".join(match_type[0]))
    param_type_list_str = ",".join(param_type_list)

    prototype = list(prototype_tpl)
    prototype[3] = param_type_list_str
    # remove space and \n
    return "".join("".join(prototype).replace("\n", "").split())


def make_func_call_list(match_res):
    f = open(func_list_file, "w", encoding="utf-8")

    f.write("\n#ifndef FUNC_CALL_LIST_H\n#define FUNC_CALL_LIST_H\n\n#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n")

    for key in match_res:
        f.write("typedef enum {\n    " + process_func_name(key, "base") + " = 0,\n")
        for ele in match_res[key]:
            f.write("    " + process_func_name(key, ele[0]) + ",\n")
        f.write("\n    " + process_func_name(key, "max") + "\n} " + key.lower() + "_call_func_list;\n\n")

    f.write("#ifdef __cplusplus\n}\n#endif\n#endif\n")
    f.close()


def make_func_mapping(tag, tag_config, tag_match_res):
    f = open(tag_config["output_mapping_file"], "w", encoding="utf-8")

    prototype_list = [ele[1] for ele in tag_match_res]
    func_name_list = [ele[0] for ele in tag_match_res]
    enum_name_list = [process_func_name(tag, func_name) for func_name in func_name_list]
    func_name_max_len = len(max(func_name_list, key=len))
    enum_name_max_len = len(max(enum_name_list, key=len))

    f.write("\n#include \"" + tag + "_function_mapping.h\"\n#include \"func_call.h\"\n#include \"func_call_list.h\"\n")
    for key in tag_config["mapping_header"]:
        for header in tag_config["mapping_header"][key]:
            f.write("#include \"" + header + "\"\n")
    f.write("\nstatic const struct mapping_tbl g_" + tag + "_call_tbl[" + process_func_name(tag, "max") + "] = {\n")

    total_checksum_str = ""
    for i in range(len(tag_match_res)):
        checksum = "{:08X}".format(zlib.crc32(prototype_list[i].encode("utf8")))
        total_checksum_str += checksum

        f.write("    [" + (enum_name_list[i] + "]").ljust(enum_name_max_len + 1) + \
                " = { " + (func_name_list[i] + ",").ljust(func_name_max_len + 1) + \
                " 0x" + checksum + " },\n")
    f.write("};\n\n")

    f.write("const struct mapping_tbl *get_" + tag + "_mapping_tbl(void)\n" + \
            "{\n    return g_" + tag + "_call_tbl;\n}\n\n")
    f.write("unsigned int get_" + tag + "_mapping_tbl_size(void)\n" + \
            "{\n    return " + process_func_name(tag, "max") + ";\n}\n")

    f.close()
    return total_checksum_str


def check_src_header_match(config, match_res):
    header_res = {}
    for key in config:
        res = []
        for path in config[key]["mapping_header"]:
            for file in config[key]["mapping_header"][path]:
                res.extend(search_prototype_from_header_file(os.path.join(path, file)))
        if key not in header_res:
            header_res[key] = []
        header_res[key].extend([process_prototype(tpl) for tpl in res])

    for key in match_res:
        for func_name, prototype in match_res[key]:
            if func_name in config[key]["white_list"]:
                print(key, "api:", prototype, "in white list, skip check")
                continue
            if prototype not in header_res[key]:
                print(key, "api:", prototype, "not match with header file, please check")
                return False

    return True


def make_indie_upg_src(config=default_config):
    global indie_upg_check
    indie_upg_check.clear()

    match_res = {}
    for key in config:
        res = []
        for file in config[key]["src_file"]:
            res.extend(search_prototype_from_src_file(os.path.join(config[key]["src_path"], file)))
        if key not in match_res:
            match_res[key] = []
        # func_name, prototype
        match_res[key].extend([[tpl[1], process_prototype(tpl)] for tpl in res])

    if not check_src_header_match(config, match_res):
        return False
    
    make_func_call_list(match_res)

    check_file = {
        "ver": 1,
    }
    for key in match_res:
        checksum_str = make_func_mapping(key, config[key], match_res[key])
        check_file[key] = checksum_str
    indie_upg_check = check_file

    print("create indie upg mapping files succ")
    return True


def dump_indie_upg_check_file(path, target_name):
    f = open(os.path.join(path, target_name + "-check.json"), "w", encoding="utf-8")
    f.write(json.dumps(indie_upg_check))
    f.close()
    print("dump_indie_upg_check_file succ")


def prt_not_match_info(tag, superset, subset, prt_all):
    print(tag, "checksum not match")

    # CRC32十六进制字符串长度8
    CRC32_len = 8
    subset_len = len(subset)
    superset_len = len(superset)
    if superset_len < subset_len:
        print(tag, "offer api less than need api")
    if subset_len % CRC32_len != 0 or superset_len % CRC32_len != 0:
        print(tag, "checksum len invalid:%d:%d" % (superset_len, subset_len))
        return

    func_size = int(min(superset_len, subset_len) / CRC32_len)
    not_match_set = {x for x in range(func_size) \
        if superset[CRC32_len * x : CRC32_len * x + CRC32_len] != subset[CRC32_len * x : CRC32_len * x + CRC32_len]}
    for ele in not_match_set:
        print("func %d [%s:%s] not match" % (ele + 1, superset[CRC32_len * ele : CRC32_len * ele + CRC32_len], \
                                             subset[CRC32_len * ele : CRC32_len * ele + CRC32_len]))
        if not prt_all:
            break


def check_indie_upg_match(hilink_check_file, app_check_file):
    f = open(hilink_check_file, "r", encoding="utf-8")
    hilink_check = json.loads(f.read())
    f.close()
    f = open(app_check_file, "r", encoding="utf-8")
    app_check = json.loads(f.read())
    f.close()

    if hilink_check["ver"] != 1 or app_check["ver"] != 1:
        print("check file ver[h:%d,a:%d] not match" % (hilink_check["ver"], app_check["ver"]))
        return False
    if not hilink_check["hilink"].startswith(app_check["hilink"]):
        prt_not_match_info("hilink", hilink_check["hilink"], app_check["hilink"], True)
        return False
    if not app_check["app"].startswith(hilink_check["app"]):
        prt_not_match_info("app", app_check["app"], hilink_check["app"], True)
        return False
    return True
