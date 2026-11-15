#ifndef _WAL_NETLINK_H
#define _WAL_NETLINK_H

/*****************************************************************************
  1 ����ͷ�ļ�����
*****************************************************************************/
#include "securec.h"
#include "soc_osal.h"
#include "osal_types.h"
#include "oam_ext_if.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
/*****************************************************************************
  2 �궨��
*****************************************************************************/
#ifdef _PRE_SYSCHANNEL_HDR_OPT
#define MAX_USER_DATA_LEN               412     /* С�����ֵ�����ݱ���ͨ�������ȼ�ͨ������ */
#else
#define MAX_USER_DATA_LEN               384
#endif
#define MAX_USER_LONG_DATA_LEN          1500    /* ��Ϣ��󳤶� */
/*****************************************************************************
  3 ��������
*****************************************************************************/
osal_s32 oal_netlink_init(osal_void);

osal_s32 oal_netlink_deinit(osal_void);

osal_u32 oal_send_user_msg(osal_u8 *buf, osal_s32 len);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
