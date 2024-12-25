#!/usr/bin/env python3
# encoding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.

import os
import json
import sys
import platform
import subprocess
import shutil
import re

from utils.build_utils import root_path, exec_shell, compare_bin, output_root

from typing import Dict, Any


SCRIPT_DIR = os.path.abspath(os.path.dirname(__file__))

def do_cmd(target_name: str, hook_name: str, env: Dict[str, Any])->bool:
    python_path = sys.executable
    print("python path: ", python_path)

    # sdk中执行sample的编译守护
    if hook_name == 'build_pre' and env.get('build_type', '') == 'GuardSDKSample':
        print("==================env==================")
        for k, v in env.items():
            print(f"{k}:{v}")
        print("==================env==================")
        # 生成sdk
        build_sdk()
        # 切换当前工作目录
        cwd = os.getcwd()
        in_sdk_root = os.path.join(output_root,'package', 'ws63','sdk')
        os.chdir(in_sdk_root)
        # 在sdk中按设定的menuconfig配置编译
        guard_sample(target_name, env)
 
        os.chdir(cwd)

    target_array = ['ws63-flashboot', 'ws63-loaderboot']

    if not os.path.isfile(os.path.join(root_path, "output", "ws63", "acore", "boot_bin", "flashboot.bin")) and target_name not in target_array and hook_name == 'build_pre':
        print("flashboot start build .....")
        errcode = exec_shell([python_path, 'build.py', 'ws63-flashboot'], None, True)

    if not os.path.isfile(os.path.join(root_path, "output", "ws63", "acore", "boot_bin", "loaderboot.bin")) and target_name not in target_array and hook_name == 'build_pre':
        print("loaderboot start build .....")
        errcode = exec_shell([python_path, 'build.py', 'ws63-loaderboot'], None, True)
    
    if hook_name == 'build_pre':
        return True

    if target_name == 'ws63-liteos-mfg' and hook_name == 'build_post':
        errcode = exec_shell([python_path, "build.py", "ws63-liteos-app"], None, True)
        return True

    if hook_name != 'build_post':
        return True

    if env.get('gen_mem_bin'):
        script_path = os.path.join(SCRIPT_DIR, 'get_mem_bin.sh')
        print("gen_mem_bin ing...")
        errcode = exec_shell(['bash', script_path, root_path, target_name, env.get('bin_name')], None, True)
        if errcode != 0:
            print("gen_mem_bin failed!")
            return False
        print("gen_mem_bin done!")

    if env.get('generate_efuse_bin'):
        copy_py = os.path.join(SCRIPT_DIR, 'efuse_cfg_gen.py')
        print("generate_efuse_bin ing...")
        errcode = exec_shell([sys.executable, copy_py], None, True)
        if errcode != 0:
            print("generate_efuse_bin failed!")
            return False
        shutil.copy(os.path.join(root_path, 'output/ws63/acore/ws63-liteos-app/efuse_cfg.bin'), os.path.join(root_path, 'output/ws63/acore/boot_bin'))
        print("generate_efuse_bin done!")

    if env.get('copy_files_to_interim'):
        # copy_files_to_interim
        copy_py = os.path.join(SCRIPT_DIR, 'copy_files_to_interim.py')
        print("copy_files_to_interim ing...")
        errcode = exec_shell([sys.executable, copy_py, root_path], None, True)
        if errcode != 0:
            print("copy_files_to_interim failed!")
            return False
        print("copy_files_to_interim done!")

    if env.get('pke_rom_bin'):
        # gen pke_rom_bin
        gen_pke_rom_bin_sh = os.path.join(SCRIPT_DIR, 'pke_rom.sh')
        if os.path.exists(os.path.join(root_path, \
            'drivers/chips/ws63/rom/rom_boot/drivers/drivers/hal/security_unified/hal_cipher/pke/rom_lib.c')):
            print("generate pke_rom_bin ing...")
            errcode = exec_shell(['sh', gen_pke_rom_bin_sh, root_path], None, True)
            if errcode != 0:
                print("generate pke_rom_bin failed!")
                return False
            print("generate pke_rom_bin done!")

            # verify pke rom bin
            if env.get('fixed_pke'):
                bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), 'pke_rom', 'pke_rom.bin')
                bin2 = env.get('fixed_pke_path', '').replace('<root>', root_path)
                if not compare_bin(bin1, bin2):
                    print(f"Verify pke rom bin ERROR! :{bin1} is not same with {bin2}")
                    return False

    if env.get('rom_in_one'):
        # gen rom_in_one
        if "windows" in platform.platform().lower():
            rom_in_one = os.path.join(SCRIPT_DIR, 'rom_in_one.py')
            print("generate rom_in_one ing...")
            errcode = exec_shell([python_path, rom_in_one, root_path, env.get('bin_name')], None, True)
        else:
            rom_in_one = os.path.join(SCRIPT_DIR, 'rom_in_one.sh')
            print("generate rom_in_one ing...")
            errcode = exec_shell(['sh', rom_in_one, root_path, env.get('bin_name')], None, True)

        if errcode != 0:
            print("generate rom_in_one failed!")
            return False
        print("generate rom_in_one done!")

        # verify codepoint bin
        bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), \
                target_name, env.get('bin_name')+'_rompack.bin')
        if env.get('fixed_rom_in_one') and os.path.isfile(bin1):# only rompack bin exists
            bin2 = env.get('fixed_rom_in_one_path', '').replace('<root>', root_path)
            if not compare_bin(bin1, bin2):
                print(f"Verify rom_in_one bin ERROR! :{bin1} is not same with {bin2}")
                return False

    if env.get('fixed_bin_name'):
        bin1 = os.path.join(root_path, "output", env.get('chip'), env.get('core'), \
            target_name, env.get('fixed_bin_name'))
        bin2 = env.get('fixed_bin_path', '').replace('<root>', root_path)
        if not compare_bin(bin1, bin2):
            print(f"Verify bin ERROR! :{bin1} is not same with {bin2}")
            return False
    nv_handle = os.path.join(SCRIPT_DIR, 'nv_handle.py')
    exec_shell([python_path, nv_handle], None, True)
    return True


def config_check(root_path, target_name):
    config_file_path = os.path.join(root_path, "build", "config", "target_config", "ws63", "config.py")
    with open(config_file_path, "r", encoding="utf-8") as f:
        for i in f:
            if target_name in i:
                print(target_name, " in config.py.")
                return True
    return False


def build_sdk():
    if os.path.isdir(output_root):
        shutil.rmtree(output_root)
    errcode = exec_shell(["python3", "build.py", "pack_ws63_sdk"], None, True)
    if errcode != 0:
        print(f"build target pack_ws63_sdk  failed!")
        sys.exit(1)
    else:
        print(f"build target pack_ws63_sdk  success!")
 
 
def guard_sample(target_name, env):
    cfg_dir = os.path.dirname(SCRIPT_DIR)
    build_target = env.get('build_target', '')
    build_target_cop = build_target.replace('-', "_")
    in_sdk_root = os.path.join(output_root, 'package', 'ws63','sdk')
    in_sdk_menuconfig_dir = os.path.join(in_sdk_root, 'build', 'config','target_config', env.get('chip'), 'menuconfig', env.get('core'))
    base_cfg_path = os.path.join(in_sdk_menuconfig_dir, f'{build_target_cop}.config')
    base_cfg_path_bak = base_cfg_path + ".bak"
    shutil.move(base_cfg_path, base_cfg_path_bak)
    in_sdk_output_root = os.path.join(in_sdk_root, 'output')
    source_fwpkg_dir = os.path.join(in_sdk_output_root, env.get('chip'), 'fwpkg', f'{build_target}')
    dest_fwpkg_dir = os.path.join(in_sdk_output_root,  env.get('chip'),  'GuardSample', target_name)
    os.makedirs(dest_fwpkg_dir, exist_ok=True)
    # 汇总结果，编译sample是否成功
    build_result_content = []
    build_result_log_path = os.path.join(dest_fwpkg_dir, "build_result.log")

    for cfg_path in env.get('menuconfigs', []):
        cfg_base_name = os.path.splitext(os.path.basename(cfg_path))[0]
        cfg_full_path = os.path.join(cfg_dir, cfg_path)
        if not os.path.isfile(cfg_full_path):
            continue
        shutil.copyfile(cfg_full_path, base_cfg_path)
        errcode = exec_shell(["python3", "build.py", "-c", build_target], None, True)
        if errcode != 0:
            print(f"build target:{target_name}\tusing menuconfig:{cfg_full_path} failed!")
            build_result_content.append(f"[ERROR] build target:{target_name}\tusing menuconfig:{cfg_full_path} failed!")
        else:
            print(f"build target:{target_name}\tusing menuconfig:{cfg_full_path} success!")
            build_result_content.append(f"[INFO] build target:{target_name}\tusing menuconfig:{cfg_full_path} success!")
 
        all_in_one_fwpkg_path = os.path.join(source_fwpkg_dir, f'{build_target}_all.fwpkg')
        if os.path.isfile(all_in_one_fwpkg_path):
            dest_fwpkg = os.path.join(dest_fwpkg_dir, f'{cfg_base_name}_all.fwpkg')
            shutil.copyfile(all_in_one_fwpkg_path, dest_fwpkg)
        else:
            print(f"not find {all_in_one_fwpkg_path}!")
            build_result_content.append(f"[ERROR] not find {all_in_one_fwpkg_path}!")
            continue
 
        load_only_fwpkg_path = os.path.join(source_fwpkg_dir,f'{build_target}_load_only.fwpkg')
        if os.path.isfile(load_only_fwpkg_path):
            dest_fwpkg = os.path.join(dest_fwpkg_dir, f'{cfg_base_name}_load_only.fwpkg')
            shutil.copyfile(load_only_fwpkg_path, dest_fwpkg)
        else:
            print(f"not find {load_only_fwpkg_path}!")
            build_result_content.append(f"[ERROR] not find {load_only_fwpkg_path}!")
    with open(build_result_log_path, 'w') as w_f:
        w_f.write("\r\n".join(build_result_content))
    shutil.move(base_cfg_path_bak, base_cfg_path)
    print(f"fwpkg is created in :{dest_fwpkg_dir}")
    os._exit(0)
