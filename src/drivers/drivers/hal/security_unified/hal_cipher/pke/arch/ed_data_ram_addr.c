/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: define the data store address of Edward algorithm in the PKE DRAM.
 *
 * Create: 2023-05-30
*/

#include "rom_lib.h"

/* support ed25519 and ed448, which needs 448bit length, so needs to occupy 2 blocks. */

/* store modulus */
const td_u32 ed_addr_m = 0;
/* store the result for point mulplication. */
const td_u32 ed_addr_cx = 2;
const td_u32 ed_addr_cy = 4;
const td_u32 ed_addr_cz = 6;
const td_u32 ed_addr_ct = 8;
/* store point coordinate for temporary */
const td_u32 ed_addr_ax = 10;
const td_u32 ed_addr_ay = 12;
const td_u32 ed_addr_az = 14;
const td_u32 ed_addr_at = 16;
/* store the input point */
const td_u32 ed_addr_px = 18;
const td_u32 ed_addr_py = 20;
const td_u32 ed_addr_pz = 22;
const td_u32 ed_addr_pt = 24;
/* store the input point */
const td_u32 ed_addr_gx = 26;
const td_u32 ed_addr_gy = 28;
const td_u32 ed_addr_gz = 30;
const td_u32 ed_addr_gt = 32;

/* store the tmeporary variable */
const td_u32 ed_addr_tp = 34;
const td_u32 ed_addr_t0 = 36;
const td_u32 ed_addr_t1 = 38;
const td_u32 ed_addr_t2 = 40;
const td_u32 ed_addr_t3 = 42;
const td_u32 ed_addr_t4 = 44;

/* store the const value */
const td_u32 ed_addr_p = 46;
const td_u32 ed_addr_rrp = 48;
const td_u32 ed_addr_n = 50;
const td_u32 ed_addr_rrn = 52;
const td_u32 ed_addr_mont_d = 54;
const td_u32 ed_addr_mont_1_p = 56;
const td_u32 ed_addr_mont_1_n = 58;
const td_u32 ed_addr_const_1 = 60;
const td_u32 ed_addr_const_0 = 62;
const td_u32 ed_addr_sqrt_m1 = 64;

/* store intermediate variable for sign */
const td_u32 ed_addr_r = 66;
const td_u32 ed_addr_h = 68;
/* store private key for sign */
const td_u32 ed_addr_sk = 70;
/* store result for signature */
const td_u32 ed_addr_s = 72;

/* store temporary variable for verify */
const td_u32 ed_addr_u = 74;
const td_u32 ed_addr_v = 76;

const td_u32 ed_addr_tx0 = 78;
const td_u32 ed_addr_tx1 = 80;
const td_u32 ed_addr_ty = 82;
const td_u32 ed_addr_bx = 84;
const td_u32 ed_addr_by = 86;
const td_u32 ed_addr_rx = 88;
const td_u32 ed_addr_ry = 90;
