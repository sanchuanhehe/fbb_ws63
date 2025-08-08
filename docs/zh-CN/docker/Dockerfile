FROM ohos_ws63:v1.0.0

ENV VER     1.0
ENV DEBIAN_FRONTEND=noninteractive

#set shell 
RUN echo "==> Set bash as default shell..."                     && \
    echo "dash dash/sh boolean false" | debconf-set-selections  && \
    DEBIAN_FRONTEND=noninteractive dpkg-reconfigure dash

#set apt
RUN echo "==> Update apt source..." \
    && mv /etc/apt/sources.list /etc/apt/sources.list.bak

COPY etc/apt/sources.list /etc/apt

RUN echo "apt update" \
    && apt-get update \
    && apt-get upgrade -y

#set shell
RUN echo "==> Set shell..."                                     && \
    apt-get install -y language-pack-zh-hans language-pack-zh-hans-base  && \
    echo "LANG=zh_CN.UTF-8" >> /etc/bash.bashrc                 && \
    echo "LANGUAGE=zh_CN:zh:en_US:en" >> /etc/bash.bashrc       && \
    echo "LC_ALL=zh_CN.UTF-8" >> /etc/bash.bashrc               && \
    apt-get install -y tzdata                                   && \
    ln -fs /usr/share/zoneinfo/Asia/Shanghai /etc/localtime

#apt install
RUN echo "==> Install gcc and utils..." \
    && apt-get install -y gcc \
    && apt-get install -y build-essential gcc g++ make zlib* libffi-dev e2fsprogs pkg-config flex bison perl libterm-readkey-perl bc openssl libssl-dev libelf-dev libc6-dev binutils binutils-dev libdwarf-dev u-boot-tools mtd-utils gcc-arm-linux-gnueabi cpio device-tree-compiler \
    && apt-get install -y build-essential gcc g++ make zlib* libffi-dev \
    && apt-get install -y dosfstools mtools mtd-utils default-jre default-jdk \
    && apt-get install -y sudo openssh-client openssh-server vim iputils-ping net-tools apt-utils less curl wget libncurses5 ccache rsync sudo


#python
RUN echo "python" \ 
    && update-alternatives --install /usr/bin/python python /usr/bin/python3.8 1 \
    && update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1

#install pycparser cmake
RUN echo "pip install pycparser and cmake" \
     && python3 -m pip install -i https://repo.huaweicloud.com/repository/pypi/simple pycparser \
     && python3 -m pip install -i https://repo.huaweicloud.com/repository/pypi/simple cmake

WORKDIR /home