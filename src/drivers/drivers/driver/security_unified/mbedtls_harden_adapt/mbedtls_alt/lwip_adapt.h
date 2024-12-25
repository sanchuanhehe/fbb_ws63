/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: mbedtls alt lwip adapt header.
 *
 * Create: 2024-07-16
*/

#ifndef LWIP_ADAPT_H
#define LWIP_ADAPT_H

#if defined(WS_IOT_LWIP_C)
#include "lwip/sockets.h"

#undef _GNU_SOURCE
#undef IFNAMSIZ

#define read(fd, buf, count)    lwip_read(fd, buf, count)
#define write(fd, buf, count)   lwip_write(fd, buf, count)

#define select(nfds, readfds, writefds, exceptfds, timeout) \
    lwip_select(nfds, readfds, writefds, exceptfds, timeout)

#define fcntl(fd, cmd, ...)     lwip_fcntl(fd, cmd, 0)
#define close(fd)               lwip_close(fd)
#endif

#endif