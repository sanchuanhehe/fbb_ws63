/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: define the data store address of curve algorithm in the PKE DRAM.
 *
 * Create: 2023-05-30
*/

#include "rom_lib.h"

/* store modulus */
const td_u32 curve_addr_m = 0;
/* store results */
const td_u32 curve_addr_x2 = 2;
const td_u32 curve_addr_z2 = 4;
/* store point coordinate temporarily */
const td_u32 curve_addr_x3 = 6;
const td_u32 curve_addr_z3 = 8;
const td_u32 curve_addr_x1 = 10;

/* store temporary variable */
const td_u32 curve_addr_t0 = 12;
const td_u32 curve_addr_t1 = 14;
const td_u32 curve_addr_t2 = 16;
const td_u32 curve_addr_t3 = 18;
const td_u32 curve_addr_t4 = 20;
const td_u32 curve_addr_t5 = 22;
const td_u32 curve_addr_t6 = 24;
const td_u32 curve_addr_tp = 26;

/* store the const value */
const td_u32 curve_addr_p = 28;
const td_u32 curve_addr_rrp = 30;
const td_u32 curve_addr_n = 32;
const td_u32 curve_addr_rrn = 34;
const td_u32 curve_addr_mont_a = 36;
const td_u32 curve_addr_mont_a24 = 38;
const td_u32 curve_addr_mont_1_p = 40;
const td_u32 curve_addr_mont_1_n = 42;
const td_u32 curve_addr_const_1 = 44;
const td_u32 curve_addr_const_0 = 46;
