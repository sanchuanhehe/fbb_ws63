#!/usr/bin/env python3
# coding=utf-8

import csv
import struct
import hashlib
import os
import sys
import re
import csv
from sys import version_info

number = 0
value_len = 0
buf = b''
csv_dir = os.path.split(os.path.realpath(__file__))[0]
csv_path = sys.argv[1]         # 输入efuse.scv文件
bin_path = sys.argv[2]         # 输出efuse_cfg.bin烧写文件

def str_to_hex(s):
    return ' '.join([hex(ord(c)).replace('0x', '') for c in s])

def print_bytes(bytes):
    if version_info.major == 3:
        c = bytes.hex()
        print(c)
    else:
        c = bytes.encode('hex')
        print(c)

def get_flash_key_param():
    cfg_file = os.path.join(csv_dir, 'config', 'sign', 'encry_config.cfg')
    file1 = open(cfg_file)
    omrk1=""
    for line in file1:
        rs = re.search("Omrk1=", line)
        if rs:
            omrk1 = line[6:-1]
    if omrk1 == "":
        raise Exception("ERR: get_flash_key_param err, omrk1 is null", cfg_file)
    result = ""
    for i in range(0, len(omrk1), 4):
        result += (" " + omrk1[i:i+4])
    line = ''.join(result[1:].split(" ")[::-1])
    result = ""
    for i in range(0, len(line), 8):
        result += (" 0x" + line[i+4:i+8] + line[i:i+4])
    file1.close()
    return result[1:]

def get_root_key_hash():
    cfg_file = os.path.join(csv_dir, 'config', 'sign', 'root_pubk.bin.hash')
    file1 = open(cfg_file)
    line = file1.readline()
    if line == "":
        raise Exception("ERR: get_root_key_hash err, hash is null", cfg_file)
    result = ""
    for i in range(0, len(line), 8):
        result += (" 0x" + line[i+6:i+8] + line[i+4:i+6] + line[i+2:i+4] + line[i:i+2])
    file1.close()
    return result[1:]

def del_old_csv_key():
    with open(csv_path, 'r') as file1:
        lines = list(csv.reader(file1))
        for line in lines[::-1]:
            if 'sec_verify_enable' in line:
                lines.remove(line)
            elif 'otp_oem_mrk' in line:
                lines.remove(line)
            elif 'root_key_hash' in line:
                lines.remove(line)
    with open(csv_path, 'w') as file2:
        writer = csv.writer(file2)
        writer.writerows(lines)

def read_cfg_value(cfg_file, key):
    with open(cfg_file) as f:
        for line in f:
            if not line.startswith('#') and '=' in line:
                k, v = line.split('=')
                if k.strip() == key:
                    return v.strip()
    return None

def set_new_csv_key():
    efuse_file = open(csv_path, 'a+', newline='')
    cfg_file = os.path.join(csv_dir, 'config', 'sign', 'root_pubk.bin.hash')
    # 安全启动开关&根公钥hash
    if os.path.exists(cfg_file):
        sec_en = "\n1,sec_verify_enable,960,1,0x1,PG0"
        root_key_hash = "\n1,root_key_hash,672,256,", get_root_key_hash() ,",PG5"
        efuse_file.writelines(sec_en)
        efuse_file.writelines(root_key_hash)

    cfg_file = os.path.join(csv_dir, 'config', 'sign', 'liteos_app_bin_ecc.cfg')
    encry_en = read_cfg_value(cfg_file, 'SignSuite')
    PlainKey = read_cfg_value(cfg_file, 'PlainKey')
    demo = '8ABDA082DB74753577FF2D1E7D79DAC7'
    # flash加密秘钥
    if encry_en == '1' and PlainKey != demo:
        flash_key = "\n1,otp_oem_mrk,352,128,", get_flash_key_param() ,",PG3"
        efuse_file.writelines(flash_key)
    efuse_file.close()

def del_null_rows():
    with open(csv_path, 'r') as file1:
        reader = csv.reader(file1)
        rows = [row for row in reader]
        rows = [row for row in rows if any(row)]
    with open(csv_path, 'w', newline='') as file2:
        writer = csv.writer(file2)
        writer.writerows(rows)

def set_key_to_efuse():
    if os.path.exists(csv_path):
        del_old_csv_key()
        set_new_csv_key()
        del_null_rows()

# 配置秘钥到efuse.csv文件中
set_key_to_efuse()

# 用reader读取csv文件
#Use the reader to read the CSV file.
with open(csv_path, 'r') as csvFile:
    reader = csv.reader(csvFile)
    for line in reader:
        if line and line[0] == "1":
            size = int(line[3])
            if (size <= 32):
                value_len = 4
            elif (size <= 64):
                value_len = 8
            else:
                value_len = size // 8
            result = struct.pack('BBHHH', 0, 8, int(line[2]), size, value_len)
            value_str = line[4]
            value_list = value_str.split(" ")
            value_struct = b''
            for i in range(value_len // 4):
                value = int(value_list[i], 16)
                value_struct = value_struct + struct.pack('I', value)
            print_bytes(value_struct)
            buf = buf + result + value_struct
            number = number + 1
header = struct.pack('BBHIII', 0, 48, number, len(buf) + 48, 0, 0)
data = header + buf
data_len = len(data)
print("data size: ", data_len)
if data_len % 64 != 0:
    max_size = int((data_len / 64)+1) * 64
    if data_len < max_size:
        data = data + bytes([0] * int(max_size - data_len))
print("finally size: ", len(data))

hash = hashlib.sha256(data).digest()
bin_data = hash + data
#print(bin_data)

with open(bin_path, 'wb+') as f:
    f.write(bin_data)
