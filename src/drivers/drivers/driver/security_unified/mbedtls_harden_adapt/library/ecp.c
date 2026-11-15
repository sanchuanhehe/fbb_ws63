/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: mbedtls harden adapt source file.
 * Create: 2025-12-03
*/

#include "common.h"

#if defined(MBEDTLS_ECP_C)

#include "mbedtls/ecp.h"
#include "mbedtls/error.h"
#include "mbedtls/platform_util.h"

#include "mbedtls/bignum.h"

#include "dfx.h"
#include "securec.h"
#include "mbedtls_harden_adapt_api.h"

/* Parameter validation macros based on platform_util.h */
#define ECP_VALIDATE_RET( cond )    \
    MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_ECP_BAD_INPUT_DATA)
#define ECP_VALIDATE( cond )        \
    MBEDTLS_INTERNAL_VALIDATE(cond)

#define MAX_ECC_LEN     72
#define MBEDTLS_ECP_DP_SECP224R1_KLEN     28
#define MBEDTLS_ECP_DP_SECP521R1_KLEN     66
/*
 * Restartable multiplication R = m * P
 */
#if defined(MBEDTLS_ECP_MUL_ALT)
typedef struct {
    mbedtls_ecp_group_id id;
    mbedtls_alt_ecp_curve_type curve_type;
    unsigned int klen;
} inner_ecp_curve_item;

static const inner_ecp_curve_item g_curve_item[] = {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_SECP192R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P192R, .klen = 24,
    },
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_SECP224R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P224R, .klen = 32,
    },
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_SECP256R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P256R, .klen = 32,
    },
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_SECP384R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P384R, .klen = 48,
    },
#endif
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_SECP521R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P521R, .klen = 72,
    },
#endif
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_BP256R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P256, .klen = 32,
    },
#endif
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_BP384R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P384, .klen = 48,
    },
#endif
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_BP512R1, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC5639_P512, .klen = 64,
    },
#endif
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_CURVE25519, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC7748, .klen = 32,
    },
#endif
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
    {
        .id = MBEDTLS_ECP_DP_CURVE448, .curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_RFC7748_448, .klen = 56,
    },
#endif
};

static void inner_get_curve_type_and_klen(mbedtls_ecp_group_id id,
    mbedtls_alt_ecp_curve_type *curve_type, unsigned int *klen)
{
    unsigned int i;
    *curve_type = MBEDTLS_ALT_ECP_CURVE_TYPE_INVALID;
    *klen = 0;
    for (i = 0; i < sizeof(g_curve_item) / sizeof(g_curve_item[0]); i++) {
        if (g_curve_item[i].id == id) {
            *curve_type = g_curve_item[i].curve_type;
            *klen = g_curve_item[i].klen;
            return;
        }
    }
}

int mbedtls_ecp_mul_restartable(mbedtls_ecp_group *grp, mbedtls_ecp_point *R,
    const mbedtls_mpi *m, const mbedtls_ecp_point *P,
    int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
    mbedtls_ecp_restart_ctx *rs_ctx)
{
    int ret;
    unsigned char point_buf[MAX_ECC_LEN * 2 + 1];   // 2: for x and y.
    unsigned char m_buf[MAX_ECC_LEN];
    unsigned char point_buf_x[MAX_ECC_LEN] = {0};
    unsigned char point_buf_y[MAX_ECC_LEN] = {0};
    mbedtls_alt_ecp_curve_type curve_type;
    unsigned int klen;
    unsigned int mbedtls_klen;
    size_t out_len = sizeof(point_buf);
    mbedtls_alt_ecp_data k_data;
    mbedtls_alt_ecp_point p_point;
    mbedtls_alt_ecp_point r_point;

    (void)p_rng;
    (void)rs_ctx;

    ECP_VALIDATE_RET(grp != NULL);
    ECP_VALIDATE_RET(R   != NULL);
    ECP_VALIDATE_RET(m   != NULL);
    ECP_VALIDATE_RET(P   != NULL);

    if (f_rng == NULL) {
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }

    inner_get_curve_type_and_klen(grp->id, &curve_type, &klen);
    if (klen == 0) {
        mbedtls_printf("invalid curve type\n");
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
    }
    if (curve_type == MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P521R) {
        mbedtls_klen = MBEDTLS_ECP_DP_SECP521R1_KLEN;
    } else if (curve_type == MBEDTLS_ALT_ECP_CURVE_TYPE_FIPS_P224R) {
        mbedtls_klen = MBEDTLS_ECP_DP_SECP224R1_KLEN;
    } else {
        mbedtls_klen = klen;
    }

    mbedtls_dump_mpi("input m", m);
    ret = mbedtls_mpi_write_binary(m, m_buf, klen);
    if (ret != 0) {
        mbedtls_printf("mbedtls_mpi_write_binary failed\n");
        goto exit_clean;
    }

    mbedtls_dump_point("input P", P);
    ret = mbedtls_ecp_point_write_binary(grp, P, MBEDTLS_ECP_PF_UNCOMPRESSED, &out_len, point_buf, sizeof(point_buf));
    mbedtls_dump_hex("point_buf of P", point_buf, out_len);
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp_point_write_binary failed\n");
        goto exit_clean;
    }

    k_data.data = m_buf;
    k_data.length = klen;
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_MONTGOMERY) {
        /* curve25519 & curve448 */
        p_point.x = (unsigned char *)(point_buf);
        p_point.y = NULL;
        p_point.length = klen;
        r_point.x = (unsigned char *)(point_buf);
        r_point.y = NULL;
        r_point.length = klen;
    } else {
        ret = memcpy_s(point_buf_x + (klen - mbedtls_klen), mbedtls_klen, point_buf + 1, mbedtls_klen);
        if (ret != EOK) {
            mbedtls_printf("memcpy_s failed\n");
            goto exit_clean;
        }
        ret = memcpy_s(point_buf_y + (klen - mbedtls_klen), mbedtls_klen, point_buf + 1 + mbedtls_klen, mbedtls_klen);
        if (ret != EOK) {
            mbedtls_printf("memcpy_s failed\n");
            goto exit_clean;
        }
        p_point.x = (unsigned char *)(point_buf_x);
        p_point.y = (unsigned char *)(point_buf_y);
        p_point.length = klen;
        r_point.x = (unsigned char *)(point_buf_x);
        r_point.y = (unsigned char *)(point_buf_y);
        r_point.length = klen;
    }

    mbedtls_dump_hex("input k_data", k_data.data, k_data.length);
    mbedtls_dump_hex("input p_point.x", p_point.x, p_point.length);
    mbedtls_dump_hex("input p_point.y", p_point.y, p_point.length);
    ret = mbedtls_alt_ecp_mul(curve_type, &k_data, &p_point, &r_point);
    mbedtls_dump_hex("outputput r_point.x", r_point.x, r_point.length);
    mbedtls_dump_hex("outputput r_point.y", r_point.y, r_point.length);
    if (ret != 0) {
        mbedtls_printf("mbedtls_alt_ecp_mul failed, ret = 0x%x\n", ret);
        goto exit_clean;
    }
    if (mbedtls_ecp_get_type(grp) == MBEDTLS_ECP_TYPE_SHORT_WEIERSTRASS) {
        /* need to remove lead zero. */
        ret = memcpy_s(point_buf + 1, mbedtls_klen, point_buf_x + (klen - mbedtls_klen), mbedtls_klen);
        if (ret != 0) {
            mbedtls_printf("memcpy_s failed\n");
            goto exit_clean;
        }
        /* need to remove lead zero. */
        ret = memcpy_s(point_buf + 1 + mbedtls_klen, mbedtls_klen, point_buf_y + (klen - mbedtls_klen), mbedtls_klen);
        if (ret != 0) {
            mbedtls_printf("memcpy_s failed\n");
            goto exit_clean;
        }
    }
    mbedtls_dump_hex("point_buf of R", point_buf, out_len);
    ret = mbedtls_ecp_point_read_binary(grp, R, point_buf, out_len);
    mbedtls_dump_point("output R", R);
    if (ret != 0) {
        mbedtls_printf("mbedtls_ecp_point_read_binary failed\n");
        goto exit_clean;
    }

    ret = 0;
exit_clean:
    (void)memset_s(m_buf, sizeof(m_buf), 0, sizeof(m_buf));
    (void)memset_s(point_buf, sizeof(point_buf), 0, sizeof(point_buf));
    (void)memset_s(point_buf_x, sizeof(point_buf_x), 0, sizeof(point_buf_x));
    (void)memset_s(point_buf_y, sizeof(point_buf_y), 0, sizeof(point_buf_y));
    return ret;
}
#endif

#endif