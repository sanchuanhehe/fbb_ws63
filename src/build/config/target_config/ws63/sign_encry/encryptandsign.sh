#!/bin/sh

CUR_DIR=$(cd $(dirname $0);pwd)

# 签名工具
SIGN_TOOL=${CUR_DIR}/sign_tool_pltuni
PACK_TOOL=${CUR_DIR}/packet_allinone.py
EFUSE_TOOL=${CUR_DIR}/efuse_cfg_gen.py

# 默认入参
INPUT_FILE=${1-"./input/pktbin.zip"}
OUT_PATH=${2-"./output"}
IMG_TYPE=${3-"all"}
EXTERN_PARAM=${4-"none"}

# 配置文件
TMP_DIR="${OUT_PATH}/tmp"
CONF_DIR="${CUR_DIR}/config"
SIGN_CONF_DIR="${CUR_DIR}/config/sign"

set_opt()
{
    local tmp=$2
    tmp=${tmp//\//\\\/}
    sed -r -i "s/$1=.*/$1=$tmp/g" $3
}

update_keyfile_path()
{
    local file=$1
    local root_keyfile=`sed '/^RootKeyFile=/!d;s/.*=//' ${file}`
    local sub_keyfile=`sed '/^SubKeyFile=/!d;s/.*=//' ${file}`
    set_opt RootKeyFile ${SIGN_CONF_DIR}/${root_keyfile} $file
    set_opt SubKeyFile ${SIGN_CONF_DIR}/${sub_keyfile} $file
}

# 镜像签名
sign_image()
{
    local config_name=$1
    local image_name=$2
    local src_config=${SIGN_CONF_DIR}/${config_name}
    local dst_config=${TMP_DIR}/${config_name}
    local src_image=${TMP_DIR}/${image_name}
    local dst_image=${TMP_DIR}/`basename ${image_name} .bin`_sign.bin
    local tmp_image=${TMP_DIR}/${image_name}_64B
    local image_size=`wc -c < ${src_image}`
    local cnt=`expr ${image_size} / 64 + 1`

    echo "Start sign ${src_image} by ${config} ${image_size}"
    # 1.镜像64bye对齐
    dd if=${src_image} of=${tmp_image} bs=64c seek=0 count=${cnt} conv=sync
    mv ${tmp_image} ${src_image}
    # 2.准备签名配置文件
    cp ${src_config} ${dst_config}
    set_opt SrcFile ${src_image} ${dst_config}
    set_opt DstFile ${dst_image} ${dst_config}
    update_keyfile_path ${dst_config}

    ${SIGN_TOOL} 0 ${dst_config}
    rm -rf ${src_image}
    echo "Sign ${src_image} successful!!!"
}

# 制作根公钥
make_root_puk()
{
    local config_name=$1
    local puk_name=$2
    local image_name=$3
    local src_config=${SIGN_CONF_DIR}/${config_name}
    local dst_config=${TMP_DIR}/${config_name}
    local puk_image=${TMP_DIR}/${puk_name}

    # 1.制作根公钥
    cp ${src_config} ${dst_config}
    set_opt DstFile ${puk_image} ${dst_config}
    set_opt SrcFile ${SIGN_CONF_DIR}/ec_bp256_oem_root_private_key.pem ${dst_config}
    $SIGN_TOOL 1 ${dst_config}
    mv ${TMP_DIR}/root_pubk.bin.hash ${CONF_DIR}/sign/root_pubk.bin.hash
    # 2.将根公钥放到指定镜像头中
    cat ${puk_image} ${TMP_DIR}/${image_name} > ${TMP_DIR}/root_`basename ${image_name}`
    rm -f ${TMP_DIR}/${image_name}
    rm -f ${puk_image}
}

make_burn_image()
{
    local src_image=$1
    local dst_image=$2
    cp ${TMP_DIR}/${src_image} ${OUT_PATH}/${dst_image}
}

get_ota_images()
{
    local FILE_DIR=${TMP_DIR}
    local FILES=$(ls $FILE_DIR)
    for filename in $FILES
    do
        case $filename in
            *"test"*)
                image[IDX++]="test"
                ;;
            *"app"*)
                image[IDX++]="app"
                ;;
            *"boot"*)
                image[IDX++]="boot"
                ;;
            *"nv"*)
                image[IDX++]="nv"
                ;;
            *)
                ;;
        esac
    done
    OTA_IMAGES=$(IFS=,; echo "${image[*]}")
    echo $OTA_IMAGES
}

# 入参检查
check_input()
{
    echo "INPUT_FILE: ${INPUT_FILE}"
    echo "OUT_PATH: ${OUT_PATH}"
    echo "IMG_TYPE: ${IMG_TYPE}"

    # 入参判断
    if [ ! -e ${INPUT_FILE} ]
    then
        echo "The input file ${INPUT_FILE} not exist, and exit!!!"
        exit 1
    fi

    if [ ! -d ${OUT_PATH} ]
    then
        echo "The output path ${OUT_PATH} not exist, and exit!!!"
        exit 1
    fi
}

#################### Main ####################

check_input
rm -rf $TMP_DIR
mkdir $TMP_DIR
case ${IMG_TYPE} in
    *"ota"*)
        cp ${INPUT_FILE} ${OUT_PATH}/all.zip
        tar -vxf ${OUT_PATH}/all.zip -C ${TMP_DIR}
        rm ${OUT_PATH}/all.zip
        OTA_IMAGES=`get_ota_images`
        if [[ $OTA_IMAGES == *"app"* ]]; then
            sign_image liteos_app_bin_ecc_ota.cfg ws63-liteos-app.bin
        fi
        if [[ $OTA_IMAGES == *"boot"* ]]; then
            sign_image flash_bin_ecc.cfg flashboot.bin
        fi
        python3 ${CUR_DIR}/build_ota_pkt.py --path=${TMP_DIR},${OUT_PATH} --pkt=$OTA_IMAGES
        ;;
    *"all"*)
        cp ${INPUT_FILE} ${OUT_PATH}/all.zip
        tar -xf ${OUT_PATH}/all.zip --strip-components 2 -C ${TMP_DIR}
        rm ${OUT_PATH}/all.zip
        # loaderboot
        sign_image loaderboot_bin_ecc.cfg loaderboot.bin
        make_root_puk root_pubk.cfg root_pubk.bin loaderboot_sign.bin
        # param
        sign_image param_bin_ecc.cfg params.bin
        make_root_puk root_pubk.cfg root_pubk.bin params_sign.bin
        # ssb
        sign_image ssb_bin_ecc.cfg ssb.bin
        # flashboot
        cp ${TMP_DIR}/flashboot.bin ${TMP_DIR}/flashboot_backup.bin
        sign_image flash_bin_ecc.cfg flashboot.bin
        # flashboot backup
        sign_image flash_backup_bin_ecc.cfg flashboot_backup.bin
        # app
        sign_image liteos_app_bin_ecc.cfg ws63-liteos-app.bin
        # mfg
        if [ -d "${TMP_DIR}/ws63-liteos-mfg.bin" ];then
            sign_image liteos_mfg_bin_ecc.cfg ws63-liteos-mfg.bin
        fi
        # efuse
        $EFUSE_TOOL ${TMP_DIR}/efuse.csv ${TMP_DIR}/efuse_cfg.bin
        # 打包
        python3 $PACK_TOOL $TMP_DIR $EXTERN_PARAM
        # 整包fwpkg镜像加密 & 模块烧写文件
        make_burn_image ws63-liteos-app_all.fwpkg ws63-liteos-app_encry.fwpkg
        ;;
    *)
        echo "Unknown image type: ${IMG_TYPE}"
        exit 2
        ;;
esac
rm -rf $TMP_DIR
