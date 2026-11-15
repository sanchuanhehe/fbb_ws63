/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 *
 * Description: Multi-precision integer implementation. \n
 *
 * History: \n
 * 2024-01-27, Create file. \n
 */

#include "app_call.h"
#include "hilink_sal_mpi.h"

HiLinkMpi HILINK_SAL_MpiInit(void)
{
    app_call0(APP_CALL_HILINK_SAL_MPI_INIT, HILINK_SAL_MpiInit, HiLinkMpi);
    return NULL;
}

void HILINK_SAL_MpiFree(HiLinkMpi mpi)
{
    app_call1_ret_void(APP_CALL_HILINK_SAL_MPI_FREE, HILINK_SAL_MpiFree, HiLinkMpi, mpi);
}

int HILINK_SAL_MpiExpMod(HiLinkMpi x, HiLinkMpi a, HiLinkMpi e, HiLinkMpi n)
{
    app_call4(APP_CALL_HILINK_SAL_MPI_EXP_MOD, HILINK_SAL_MpiExpMod, int,
        HiLinkMpi, x, HiLinkMpi, a, HiLinkMpi, e, HiLinkMpi, n);
    return 0;
}

int HILINK_SAL_MpiCmpInt(HiLinkMpi x, int64_t z)
{
    app_call2(APP_CALL_HILINK_SAL_MPI_CMP_INT, HILINK_SAL_MpiCmpInt, int, HiLinkMpi, x, int64_t, z);
    return 0;
}

int HILINK_SAL_MpiSubInt(HiLinkMpi x, HiLinkMpi a, int64_t b)
{
    app_call3(APP_CALL_HILINK_SAL_MPI_SUB_INT, HILINK_SAL_MpiSubInt, int, HiLinkMpi, x, HiLinkMpi, a, int64_t, b);
    return 0;
}

int HILINK_SAL_MpiCmpMpi(HiLinkMpi x, HiLinkMpi y)
{
    app_call2(APP_CALL_HILINK_SAL_MPI_CMP_MPI, HILINK_SAL_MpiCmpMpi, int, HiLinkMpi, x, HiLinkMpi, y);
    return 0;
}

int HILINK_SAL_MpiReadString(HiLinkMpi mpi, unsigned char radix, const char *s)
{
    app_call3(APP_CALL_HILINK_SAL_MPI_READ_STRING, HILINK_SAL_MpiReadString, int,
        HiLinkMpi, mpi, unsigned char, radix, const char *, s);
    return 0;
}

int HILINK_SAL_MpiWriteString(HiLinkMpi mpi, unsigned int radix, char *buf, unsigned int *bufLen)
{
    app_call4(APP_CALL_HILINK_SAL_MPI_WRITE_STRING, HILINK_SAL_MpiWriteString, int,
        HiLinkMpi, mpi, unsigned int, radix, char *, buf, unsigned int *, bufLen);
    return 0;
}

int HILINK_SAL_MpiReadBinary(HiLinkMpi mpi, const unsigned char *buf, unsigned int bufLen)
{
    app_call3(APP_CALL_HILINK_SAL_MPI_READ_BINARY, HILINK_SAL_MpiReadBinary, int,
        HiLinkMpi, mpi, const unsigned char *, buf, unsigned int, bufLen);
    return 0;
}

int HILINK_SAL_MpiWriteBinary(HiLinkMpi mpi, unsigned char *buf, unsigned int bufLen)
{
    app_call3(APP_CALL_HILINK_SAL_MPI_WRITE_BINARY, HILINK_SAL_MpiWriteBinary, int,
        HiLinkMpi, mpi, unsigned char *, buf, unsigned int, bufLen);
    return 0;
}