FROM ubuntu:24.04

# 设置非交互模式以避免时间区选择等提示
ENV DEBIAN_FRONTEND=noninteractive

# 创建工作目录
WORKDIR /workspaces

# 复制配置脚本
COPY src/config.sh /tmp/config.sh

RUN apt install sudo

# 运行配置脚本进行环境初始化
RUN chmod +x /tmp/config.sh && \
    cd /tmp && \
    ./config.sh

# 配置shell为bash
RUN echo "dash dash/sh boolean false" | debconf-set-selections && \
    dpkg-reconfigure -p critical dash

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