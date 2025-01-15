#!/usr/bin/env python3
# encoding=utf-8

import os
import sys
import argparse

g_root = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(g_root))
from build_ota_img import begin


class upg_base_info:
    def __init__(self, conf):
        path_str = conf.path.split(",")
        input_path = path_str[0]
        output_path = path_str[1]
        
        self.root_path = g_root
        # 升级包结构配置文件
        self.fota_format_path = os.path.join(self.root_path, "config", "ota")
        # 升级配置文件模板
        self.temp_fota_cfg = os.path.join(self.root_path, "config", "ota", "fota_template.cfg")
        # 升级配置文件
        self.fota_cfg = os.path.join(self.root_path, input_path, "fota.cfg")
        # 镜像输入路径
        self.output = os.path.join(self.root_path, input_path)
        # 升级镜像包输出路径
        self.upg_output = os.path.join(self.root_path, output_path)
        # 升级制作临时文件输出路径
        self.temp_dir = os.path.join(self.upg_output, "tmp_ota_cfg")
        print("\ninput_dir:", self.output)
        print("output_dir:", self.upg_output)
        # 产品镜像路径
        self.flashboot = os.path.join(self.output, "flashboot_sign.bin")
        self.app_bin = os.path.join(self.output, "ws63-liteos-app_sign.bin")
        self.test_bin = os.path.join(self.output, "ws63-liteos-testsuite_sign.bin")
        self.nv_bin = os.path.join(self.output, "ws63_all_nv.bin")

        self.flashboot_old_bin = os.path.join(self.output, "flashboot_sign.bin")
        self.app_old_bin = os.path.join(self.output, "ws63-liteos-app_sign.bin")
        self.test_old_bin = os.path.join(self.output, "ws63-liteos-testsuite_sign.bin")
        self.nv_old_bin = os.path.join(self.output, "ws63_all_nv.bin")

def get_new_image(input,info):
    image_list = []
    if 'app' in input:
        image_list.append("=".join([info.app_bin, "application"]))
    if 'test' in input:
        image_list.append("=".join([info.test_bin, "application"]))
    if 'boot' in input:
        image_list.append("=".join([info.flashboot, "flashboot"]))
    if 'nv' in input:
        image_list.append("=".join([info.nv_bin, "nv"]))

    new_image = "|".join(image_list)
    return new_image


def get_old_image(input,info):
    image_list = []
    if 'app' in input:
        image_list.append("=".join([info.app_old_bin, "application"]))
    if 'test' in input:
        image_list.append("=".join([info.test_old_bin, "application"]))
    if 'boot' in input:
        image_list.append("=".join([info.flashboot_old_bin, "flashboot"]))
    if 'nv' in input:
        image_list.append("=".join([info.nv_old_bin, "nv"]))

    old_image = "|".join(image_list)
    return old_image

def get_parameters():
    parser = argparse.ArgumentParser()

    parser.add_argument('--pkt', type=str, default = 'app',
                        help='需要生成的镜像,包括: app,boot,nv')

    parser.add_argument('--path', type=str, default = '../input',
                        help='待打包镜像路径 and 输出路径: ../input,../output')

    config = parser.parse_args()
    return config

if __name__ == '__main__':
    conf = get_parameters()
    info = upg_base_info(conf)
    input = conf.pkt.split(",")
    conf.app_name        = "update_encry"
    conf.upg_format_path = info.fota_format_path
    conf.base            = info.fota_cfg
    conf.temp_base       = info.temp_fota_cfg
    conf.temp_dir        = info.temp_dir
    conf.new_images      = get_new_image(input,info)
    conf.old_images      = get_old_image(input,info)
    conf.output_dir      = info.upg_output
    conf.type            = 0
    begin(conf)
