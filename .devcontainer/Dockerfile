FROM ubuntu:24.04

# 设置非交互模式以避免时间区选择等提示
ENV DEBIAN_FRONTEND=noninteractive

# 更新系统并安装必要的工具和依赖
RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    cmake \
    python3 \
    python3-pip \
    python3-setuptools \
    wget \
    build-essential \
    sudo \
    curl \
    git \
    bash \
    ninja-build \
    unzip \
    doxygen \
    clang-format

# 配置shell为bash
RUN echo "dash dash/sh boolean false" | debconf-set-selections && \
    dpkg-reconfigure -p critical dash

# 从 LLVM 官方下载最新的 clangd
RUN CLANGD_VERSION=18.1.3 && \
    wget https://github.com/clangd/clangd/releases/download/$CLANGD_VERSION/clangd-linux-$CLANGD_VERSION.zip && \
    unzip clangd-linux-$CLANGD_VERSION.zip && \
    cp -r clangd_$CLANGD_VERSION/bin/* /usr/local/bin/ && \
    cp -r clangd_$CLANGD_VERSION/lib/* /usr/local/lib/ && \
    rm -rf clangd-linux-$CLANGD_VERSION.zip clangd_$CLANGD_VERSION

# 安装 Python 依赖
RUN pip3 install "pycparser>=2.21" pyyaml "kconfiglib>=14.1.0" --break-system-packages

# 设置环境变量export LC_ALL=C.UTF-8
ENV LC_ALL=C.UTF-8

# 设置passwordless sudo
RUN echo "ALL ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# 如果已存在 UID 1000 用户，则改名为 vscode，否则新建
RUN if id -u 1000 >/dev/null 2>&1; then \
      usermod -l vscode -d /home/vscode -m $(getent passwd 1000 | cut -d: -f1) && \
      usermod -aG sudo vscode && \
      echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers; \
    else \
      useradd -m -s /bin/bash -u 1000 vscode && \
      echo "vscode ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers; \
    fi

USER vscode