#!/bin/bash
BUILD_SOURCE=$1
CODEHUB_URL=$2
CODEHUB_BRANCH=$3

if [ -z "${DOCS_DIR}" ]; then
    DOCS_DIR="docs"
fi


function install_py_libs() {
    # 安装构建依赖
    # requirements_txt：流水线自定义参数
    txt_file=requirements.txt
    if [ ! -f "${txt_file}" ]; then
        echo "missing ${txt_file}"
        exit 1
    fi
    pip install -r $txt_file -i ${PIP_URL} --trusted-host ${PIP_HOST}
}


function check_success() {
    if [ $? -eq 0 ]; then
        echo "$1 run success!"
    else
        echo "$1 run failed!"
        exit 1
    fi
}


function sphinx_build_and_obs_upload() {
    # sphinx构建+obs上传
    src_dir=$1  # 文档路径
    out_dir=$2  # 构建输出路径，同事也是OBS的objectKey
    if [ -f "${out_dir}" ]; then
        rm -rf ${out_dir}
    fi
    echo "build doc source: ${src_dir}, build output dir(OBS objectKey): ${out_dir}"
    export PATH=$HOME/.local/bin:$PATH
    sphinx-build -b html ${src_dir} ${out_dir}
    check_success "sphinx-build html"

    ls -alh
    # 默认上传
    if [ "${OBS_UPLOAD}" != "false" ]; then
        echo "start to upload file to OBS..."
        python3 obs_upload.py -ak ${OBS_AK} -sk ${OBS_SK} -bucket ${OBS_BUCKET} -server ${OBS_SERVER} -src ${out_dir} -key ${out_dir}
        check_success "OBS upload"
    fi
}


function docs_build_start() {
    repo_path=$1
    repo_branch=$2
    chip=$3

    key_prefix="${OBS_ROOT_KEY}/${repo_path}"
    if [ -n "${chip}" ]; then
        # 如 bs2x/docs/en
        dir_path_prefix="${chip}/${DOCS_DIR}"
        key_suffix="${repo_branch}/${chip}"
    else
        dir_path_prefix=$DOCS_DIR
        key_suffix=$repo_branch
    fi

    has_language="false"
    en="en"
    if [ -d "${en}" ]; then
        echo "start to build docs, language: ${en}"
        has_language="true"
        obs_object_key="${key_prefix}/${en}/${key_suffix}"
        sphinx_build_and_obs_upload ${en} $obs_object_key
    else
        echo "path not exist: ${en}, ignore to build en docs"
    fi

    zh_CN='zh-CN'
    if [ -d "${zh_CN}" ]; then
        echo "start to build docs, language: ${zh_CN}"
        has_language="true"
        obs_object_key="${key_prefix}/${zh_CN}/${key_suffix}"
        sphinx_build_and_obs_upload ${zh_CN} $obs_object_key
    else
        echo "path not exist: ${zh_CN}, ignore to build zh-CN docs"
    fi

    if [ "${has_language}" == "false" ]; then
        echo "start to build docs, language docs dir not exist"
        obs_object_key="${key_prefix}/${key_suffix}"
        sphinx_build_and_obs_upload . $obs_object_key
    fi
}


function to_html_and_upload() {
    install_py_libs
    repo_path=$1
    repo_branch=$2

    # 文档路径，默认都是仓目录下面的docs文件夹，还需要适配是否存在中英文文件夹
    if [ -z "${MULTI_CHIP}" ]; then
        # 单仓芯片
        docs_build_start $repo_path $repo_branch
    else
        # 多仓芯片
        for chip in ${MULTI_CHIP}; do
            docs_build_start $repo_path $repo_branch $chip
        done
    fi
}

# gitee构建
function gitee_build() {
    echo "build env: gitee"
    # GIT_TAG空则是分支构建
    if [ -z "${GIT_TAG}" ]; then
        branch=$codeBranch
    else
        branch=$GIT_TAG
    fi
    export REPO_BRANCH=$branch  # 声明分支构建时使用
    echo "组织：${REPO_NAMESPACE}，仓名：${REPO_PATH}，分支/tag ${branch}"
    to_html_and_upload $REPO_PATH $branch
}


function codehub_build() {
    echo "build env: codehub"
    export REPO_NAMESPACE=$CODEHUB_NAMESPACE
    export REPO_PATH=$CODEHUB_PATH
    export REPO_HOST=$CODEHUB_HOST
    export REPO_BRANCH=$CODEHUB_BRANCH
    to_html_and_upload $REPO_PATH $CODEHUB_BRANCH
}


function local_build() {
    echo "build env: local"
}


function main() {
    if [ -z "${BUILD_SOURCE}" ]; then
        if [ "${MAKE_INDEX}" == "yes" ]; then
            echo "generate doc index.rst"
            python3 make_index.py
            echo "rst file generate done."
        fi
        gitee_build
    elif [ "${BUILD_SOURCE}" == "jenkins" ]; then
        codehub_build
    else
        local_build
    fi
}

main
