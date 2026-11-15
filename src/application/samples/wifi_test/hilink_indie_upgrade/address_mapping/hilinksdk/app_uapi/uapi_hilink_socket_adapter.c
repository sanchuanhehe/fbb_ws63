/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Implementation of the Network Socket Interface at the System Adaptation Layer. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_socket_adapter.h"

int HILINK_GetAddrInfo(const char *nodename, const char *servname,
    const HiLinkAddrInfo *hints, HiLinkAddrInfo **result)
{
    app_call4(APP_CALL_HILINK_GET_ADDR_INFO, HILINK_GetAddrInfo, int, const char *, nodename, const char *, servname,
        const HiLinkAddrInfo *, hints, HiLinkAddrInfo **, result);
    return 0;
}

void HILINK_FreeAddrInfo(HiLinkAddrInfo *addrInfo)
{
    app_call1_ret_void(APP_CALL_HILINK_FREE_ADDR_INFO, HILINK_FreeAddrInfo, HiLinkAddrInfo *, addrInfo);
}

int HILINK_Socket(HiLinkSocketDomain domain, HiLinkSocketType type, HiLinkSocketProto proto)
{
    app_call3(APP_CALL_HILINK_SOCKET, HILINK_Socket, int,
        HiLinkSocketDomain, domain, HiLinkSocketType, type, HiLinkSocketProto, proto);
    return 0;
}

void HILINK_Close(int fd)
{
    app_call1_ret_void(APP_CALL_HILINK_CLOSE, HILINK_Close, int, fd);
}

int HILINK_SetSocketOpt(int fd, HiLinkSocketOption option, const void *value, unsigned int len)
{
    app_call4(APP_CALL_HILINK_SET_SOCKET_OPT, HILINK_SetSocketOpt, int, int, fd, HiLinkSocketOption, option,
        const void *, value, unsigned int, len);
    return 0;
}

int HILINK_Bind(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    app_call3(APP_CALL_HILINK_BIND, HILINK_Bind, int, int, fd, const HiLinkSockaddr *, addr, unsigned int, addrLen);
    return 0;
}

int HILINK_Connect(int fd, const HiLinkSockaddr *addr, unsigned int addrLen)
{
    app_call3(APP_CALL_HILINK_CONNECT, HILINK_Connect, int,
        int, fd, const HiLinkSockaddr *, addr, unsigned int, addrLen);
    return 0;
}

int HILINK_Recv(int fd, unsigned char *buf, unsigned int len)
{
    app_call3(APP_CALL_HILINK_RECV, HILINK_Recv, int, int, fd, unsigned char *, buf, unsigned int, len);
    return 0;
}

int HILINK_Send(int fd, const unsigned char *buf, unsigned int len)
{
    app_call3(APP_CALL_HILINK_SEND, HILINK_Send, int, int, fd, const unsigned char *, buf, unsigned int, len);
    return 0;
}

int HILINK_RecvFrom(int fd, unsigned char *buf, unsigned int len,
    HiLinkSockaddr *from, unsigned int *fromLen)
{
    app_call5(APP_CALL_HILINK_RECV_FROM, HILINK_RecvFrom, int, int, fd, unsigned char *, buf, unsigned int, len,
        HiLinkSockaddr *, from, unsigned int *, fromLen);
    return 0;
}

int HILINK_SendTo(int fd, const unsigned char *buf, unsigned int len,
    const HiLinkSockaddr *to, unsigned int toLen)
{
    app_call5(APP_CALL_HILINK_SEND_TO, HILINK_SendTo, int, int, fd, const unsigned char *, buf, unsigned int, len,
        const HiLinkSockaddr *, to, unsigned int, toLen);
    return 0;
}

int HILINK_Select(HiLinkFdSet *readSet, HiLinkFdSet *writeSet, HiLinkFdSet *exceptSet, unsigned int ms)
{
    app_call4(APP_CALL_HILINK_SELECT, HILINK_Select, int, HiLinkFdSet *, readSet,
        HiLinkFdSet *, writeSet, HiLinkFdSet *, exceptSet, unsigned int, ms);
    return 0;
}

int HILINK_GetSocketErrno(int fd)
{
    app_call1(APP_CALL_HILINK_GET_SOCKET_ERRNO, HILINK_GetSocketErrno, int, int, fd);
    return 0;
}

unsigned int HILINK_Htonl(unsigned int hl)
{
    app_call1(APP_CALL_HILINK_HTONL, HILINK_Htonl, unsigned int, unsigned int, hl);
    return 0;
}

unsigned int HILINK_Ntohl(unsigned int nl)
{
    app_call1(APP_CALL_HILINK_NTOHL, HILINK_Ntohl, unsigned int, unsigned int, nl);
    return 0;
}

unsigned short HILINK_Htons(unsigned short hs)
{
    app_call1(APP_CALL_HILINK_HTONS, HILINK_Htons, unsigned short, unsigned short, hs);
    return 0;
}

unsigned short HILINK_Ntohs(unsigned short ns)
{
    app_call1(APP_CALL_HILINK_NTOHS, HILINK_Ntohs, unsigned short, unsigned short, ns);
    return 0;
}

unsigned int HILINK_InetAton(const char *ip, unsigned int *addr)
{
    app_call2(APP_CALL_HILINK_INET_ATON, HILINK_InetAton, unsigned int, const char *, ip, unsigned int *, addr);
    return 0;
}

unsigned int HILINK_InetAddr(const char *ip)
{
    app_call1(APP_CALL_HILINK_INET_ADDR, HILINK_InetAddr, unsigned int, const char *, ip);
    return 0;
}

const char *HILINK_InetNtoa(unsigned int addr, char *buf, unsigned int buflen)
{
    app_call3(APP_CALL_HILINK_INET_NTOA, HILINK_InetNtoa, const char *,
        unsigned int, addr, char *, buf, unsigned int, buflen);
    return NULL;
}