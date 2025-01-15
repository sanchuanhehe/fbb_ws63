#!/usr/bin/env python3
# encoding=utf-8

import os
import sys
import shutil
from typing import List
from packet_create import packet_bin

def get_file_size(file_path: str)->int:
    try:
        return os.stat(file_path).st_size
    except BaseException as e:
        print(e)
        exit(-1)

# ws63
def make_all_in_one_packet(argv):
    fwpkg_dir = argv[1]
    extr_defines = argv[2]

    # loader boot
    loadboot_bin = os.path.join(fwpkg_dir, "root_loaderboot_sign.bin")
    loadboot_bx = loadboot_bin + "|0x0|0x200000|0"
    # efuse bin
    efuse_bin = os.path.join(fwpkg_dir, "efuse_cfg.bin")
    efuse_bx = efuse_bin + "|0x0|0x200000|3"
    # params
    params_bin = os.path.join(fwpkg_dir, "root_params_sign.bin")
    params_bx = params_bin + f"|0x200000|{hex(get_file_size(params_bin))}|1"
    # ssb
    ssb_bin = os.path.join(fwpkg_dir, "ssb_sign.bin")
    ssb_bx = ssb_bin + f"|0x202000|{hex(get_file_size(ssb_bin))}|1"
    # flash boot
    flashboot_bin = os.path.join(fwpkg_dir, "flashboot_sign.bin")
    flashboot_bx = flashboot_bin + f"|0x220000|{hex(get_file_size(flashboot_bin))}|1"
    # flash boot backup
    flashboot_backup_bin = os.path.join(fwpkg_dir, "flashboot_backup_sign.bin")
    flashboot_backup_bx = flashboot_backup_bin + f"|0x210000|{hex(get_file_size(flashboot_backup_bin))}|1"
    # nv
    nv_bin = os.path.join(fwpkg_dir, "ws63_all_nv.bin")
    nv_bx = nv_bin + f"|0x5FC000|0x4000|1"
    # nv backup
    nv_backup_bin = os.path.join(fwpkg_dir, "ws63_all_nv_factory.bin")
    nv_backup_bx = nv_backup_bin + f"|0x20C000|0x4000|1"
    # app
    app_bin = os.path.join(fwpkg_dir, "ws63-liteos-app_sign.bin")
    app_bx = app_bin + f"|0x230000|{hex(get_file_size(app_bin))}|1"
    # mfg
    mfg_bin = os.path.join(fwpkg_dir, "ws63-liteos-mfg_sign.bin")

    packet_post_agvs = list()
    packet_post_agvs.append(loadboot_bx)
    packet_post_agvs.append(params_bx)
    packet_post_agvs.append(ssb_bx)
    packet_post_agvs.append(flashboot_bx)
    packet_post_agvs.append(flashboot_backup_bx)
    packet_post_agvs.append(nv_bx)
    if "PACKET_NV_FACTORY" in extr_defines:
        print("nv factory pack")
        packet_post_agvs.append(nv_backup_bx)
    packet_post_agvs.append(app_bx)
    if os.path.exists(mfg_bin):
        mfg_bx = mfg_bin + f"|0x470000|{hex(0x183000)}|1"
        packet_post_agvs.append(mfg_bx)
    packet_post_agvs.append(efuse_bx)
    fpga_fwpkg = os.path.join(fwpkg_dir, f"ws63-liteos-app_all.fwpkg")
    packet_bin(fpga_fwpkg, packet_post_agvs)

    packet_post_agvs = list()
    packet_post_agvs.append(loadboot_bx)
    packet_post_agvs.append(app_bx)
    fpga_loadapp_only_fwpkg = os.path.join(fwpkg_dir, f"ws63-liteos-app_only.fwpkg")
    packet_bin(fpga_loadapp_only_fwpkg, packet_post_agvs)

if __name__ == '__main__':
    make_all_in_one_packet(sys.argv)