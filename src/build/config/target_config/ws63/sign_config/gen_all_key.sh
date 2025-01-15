#!/bin/bash

if [ $# -eq 0 ]; then
    param="all"
else
    param=$1
fi

if [ -f "ec_bp256_oem_root_private_key.pem" ]; then
    echo "WARNING: The old keys will be overwritten, and new keys can not work on old devices. Continue? (Y/N)"
    read confirm
    if [[ $confirm != Y ]] && [[ $confirm != y ]]; then
        exit
    fi
fi

# =======================安全启动： 随机生成签名私钥 & 根公钥 ==============================================
if [[ $param == sec ]] || [[ $param == all ]]; then
    # 生成根私钥/公钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_oem_root_private_key.pem
    openssl ec -in ec_bp256_oem_root_private_key.pem -pubout -out ec_bp256_oem_root_public_key.pem
    # 生成ssb私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_ssb_private_key.pem
    # 生成flashboot私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_flash_private_key.pem
    # 生成flashboot_backup私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_flashboot_backup_private_key.pem
    # 生成loaderboot私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_loaderboot_private_key.pem
    # 生成param私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_param_private_key.pem
    # 生成app私钥
    openssl ecparam -genkey -name brainpoolP256r1 -out ec_bp256_app_private_key.pem
fi

# =======================Flash在线解密： 随机生成派生参数, 派生flash秘钥 ======================================
if [[ $param == encry ]] || [[ $param == all ]]; then
    ENCRY_CONF=liteos_app_bin_ecc.cfg
    OTA_CONF=../fota/fota.cfg
    MAKE_KEY_CONF=encry_config.cfg
    MAKE_KEY_TOOL=./tool_security
    random_num() { < /dev/urandom tr -dc A-F0-9 | head -c $1; echo;}
    set_opt() { sed -r -i "s/$1=.*/$1=$2/g" $3; }

    # # 1. 随机生成秘钥派生参数
    PARAM_OMRK1=`random_num 32`
    PARAM_OKPSALT=`random_num 64`
    PARAM_OWNERID=0

    echo "Omrk1  =$PARAM_OMRK1"
    echo "OkpSalt=${PARAM_OKPSALT:0:56}"
    echo "OwnerId=$PARAM_OWNERID"

    # # 2.配置派生参数到配置文件encry_config.cfg, 该文件为flash加密秘钥派生参数配置文件
    set_opt Omrk1 $PARAM_OMRK1 $MAKE_KEY_CONF
    set_opt OkpSalt ${PARAM_OKPSALT:0:56} $MAKE_KEY_CONF
    set_opt OwnerId $PARAM_OWNERID $MAKE_KEY_CONF

    # # 3.使用配置文件派生flash秘钥odrk  & 配置秘钥到配置文件liteos_app_bin_ecc.cfg
    PARAM_ENCRY_IV=`random_num 32`
    PARAM_ENCRY_KEY=`$MAKE_KEY_TOOL 0 $MAKE_KEY_CONF | tr -cd "[0-9A-Z]"`
    set_opt PlainKey ${PARAM_ENCRY_KEY:5:32} $ENCRY_CONF
    echo "odrk:${PARAM_ENCRY_KEY:5}"
    set_opt Iv $PARAM_ENCRY_IV $ENCRY_CONF
    set_opt ProtectionKeyL1 ${PARAM_OKPSALT:0:32} $ENCRY_CONF
    set_opt ProtectionKeyL2 ${PARAM_OKPSALT:32:64} $ENCRY_CONF

    # # 4.使用配置文件派生ota升级加密秘钥abrk & 配置秘钥到配置文件fota.cfg
    PARAM_ENCRY_KEY=`$MAKE_KEY_TOOL 1 $MAKE_KEY_CONF | tr -cd "[0-9A-Z]"`
    set_opt PlainKey ${PARAM_ENCRY_KEY:5:32} $OTA_CONF
    echo "abrk:${PARAM_ENCRY_KEY:5}"
    set_opt Iv $PARAM_ENCRY_IV $OTA_CONF
    set_opt ProtectionKeyL1 ${PARAM_OKPSALT:0:32} $OTA_CONF
    set_opt ProtectionKeyL2 ${PARAM_OKPSALT:32:64} $OTA_CONF
fi