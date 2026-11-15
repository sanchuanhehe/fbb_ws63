/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides i2s port \n
 *
 * History: \n
 * 2023-03-10， Create file. \n
 */

#include "common_def.h"
#include "hal_sio_v151.h"
#include "oal_interface.h"
#include "soc_osal.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "chip_core_irq.h"
#include "sio_porting.h"
#if defined(CONFIG_I2S_SUPPORT_DMA)
#include "dma_porting.h"
#endif
#include "debug_print.h"

#define FREQ_OD_NEED                    32
#define I2S_MCLK_DIV                    2
#define I2S_MCLK_RATE                   12288
#define I2S_BUS_0_REAL_BASE             (I2S_BUS_0_BASE_ADDR - 0x3c)
#define I2S_MERGE_TX_DATA_ADDR          (I2S_BUS_0_REAL_BASE + 0xc0)
#define I2S_MERGE_RX_DATA_ADDR          (I2S_BUS_0_REAL_BASE + 0xa0)
#define I2S_TX_SPLIT_LEFT_DATA_ADDR     (I2S_BUS_0_REAL_BASE + 0x4c)
#define I2S_TX_SPLIT_RIGHT_DATA_ADDR    (I2S_BUS_0_REAL_BASE + 0x50)
#define I2S_RX_SPLIT_LEFT_DATA_ADDR     (I2S_BUS_0_REAL_BASE + 0x54)
#define I2S_RX_SPLIT_RIGHT_DATA_ADDR    (I2S_BUS_0_REAL_BASE + 0x58)
#define HAL_I2S_CLK_CG_ADDR 0x44001100
#define CLDO_CRG_RST_I2S_DIV_CFG_ADDR 0x44001144
#define CMU_NEW_CFG0 0x400034a0
#define HAL_I2S_CLK_CG_ON 0xffffffff
#define I2S_CLKEN_BIT                   12
#define I2S_BUS_CLKEN_BIT               11
#define CMU_DIV_AD_RSTN_SYNC_BIT        0

static uintptr_t g_sio_base_addrs[I2S_MAX_NUMBER] = {
    (uintptr_t)I2S_BUS_0_BASE_ADDR,
};

typedef struct sio_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
}sio_interrupt_t;

static const sio_interrupt_t g_sio_interrupt_lines[I2S_MAX_NUMBER] = {
    { I2S_IRQN, (osal_irq_handler)irq_sio0_handler },
};

uintptr_t sio_porting_base_addr_get(sio_bus_t bus)
{
    return g_sio_base_addrs[bus];
}

void sio_porting_register_hal_funcs(sio_bus_t bus)
{
    hal_sio_register_funcs(bus, hal_sio_v151_funcs_get());
}

void sio_porting_unregister_hal_funcs(sio_bus_t bus)
{
    hal_sio_unregister_funcs(bus);
}

void sio_porting_register_irq(sio_bus_t bus)
{
    osal_irq_request(g_sio_interrupt_lines[bus].irq_num, g_sio_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
    osal_irq_enable(g_sio_interrupt_lines[bus].irq_num);
}

void sio_porting_unregister_irq(sio_bus_t bus)
{
    osal_irq_disable(g_sio_interrupt_lines[bus].irq_num);
}

void irq_sio0_handler(void)
{
    hal_sio_v151_irq_handler(SIO_BUS_0);
}

void sio_porting_clock_enable(bool enable)
{
    if (enable) {
        uapi_reg_setbit(CMU_NEW_CFG0, CMU_DIV_AD_RSTN_SYNC_BIT);
        uapi_reg_setbit(HAL_I2S_CLK_CG_ADDR, I2S_CLKEN_BIT);
        uapi_reg_setbit(HAL_I2S_CLK_CG_ADDR, I2S_BUS_CLKEN_BIT);
    } else {
        uapi_reg_clrbit(CMU_NEW_CFG0, CMU_DIV_AD_RSTN_SYNC_BIT);
        uapi_reg_clrbit(HAL_I2S_CLK_CG_ADDR, I2S_CLKEN_BIT);
        uapi_reg_clrbit(HAL_I2S_CLK_CG_ADDR, I2S_BUS_CLKEN_BIT);
    }
}

void sio_porting_i2s_pinmux(void)
{
    uapi_pin_set_mode(S_MGPIO9, PIN_MODE_4);
    uapi_pin_set_mode(S_MGPIO10, PIN_MODE_4);
    uapi_pin_set_mode(S_MGPIO11, PIN_MODE_4);
    uapi_pin_set_mode(S_MGPIO12, PIN_MODE_4);
    uapi_pin_set_pull(S_MGPIO11, PIN_PULL_TYPE_DISABLE);
    uapi_pin_set_pull(S_MGPIO12, PIN_PULL_TYPE_DISABLE);
    return;
}

uintptr_t i2s_porting_tx_merge_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_MERGE_TX_DATA_ADDR;
}

uintptr_t i2s_porting_rx_merge_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_MERGE_RX_DATA_ADDR;
}

uintptr_t i2s_porting_tx_left_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_TX_SPLIT_LEFT_DATA_ADDR;
}

uintptr_t i2s_porting_tx_right_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_TX_SPLIT_RIGHT_DATA_ADDR;
}

uintptr_t i2s_porting_rx_left_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_RX_SPLIT_LEFT_DATA_ADDR;
}

uintptr_t i2s_porting_rx_right_data_addr_get(sio_bus_t bus)
{
    unused(bus);
    return I2S_RX_SPLIT_RIGHT_DATA_ADDR;
}

uint32_t sio_porting_get_bclk_div_num(uint8_t data_width, uint32_t ch)
{
    uint32_t bclk_div_num, freq_of_need;
    float middle_div;
    float s_clk = I2S_MCLK_RATE;
    freq_of_need = FREQ_OF_NEED;
    middle_div = (float)s_clk / (freq_of_need * data_width * ch);
    if ((uint32_t)(middle_div * I2S_PARAM + 1) == ((uint32_t)middle_div * I2S_PARAM + 1)) {
        bclk_div_num = (uint32_t)middle_div;
    } else {
        bclk_div_num = (uint32_t)middle_div + 1;
    }
    return bclk_div_num;
}

bool sio_porting_check_standard_sample_rate(uint32_t fs)
{
    uint32_t i;
    const uint32_t fs_table[] = {
        I2S_SAMPLE_RATE_8K,
        I2S_SAMPLE_RATE_11K,
        I2S_SAMPLE_RATE_12K,
        I2S_SAMPLE_RATE_16K,
        I2S_SAMPLE_RATE_22K,
        I2S_SAMPLE_RATE_24K,
        I2S_SAMPLE_RATE_32K,
        I2S_SAMPLE_RATE_44K,
        I2S_SAMPLE_RATE_48K,
        I2S_SAMPLE_RATE_88K,
        I2S_SAMPLE_RATE_96K,
        I2S_SAMPLE_RATE_176K,
        I2S_SAMPLE_RATE_192K,
    };

    for (i = 0; i < sizeof(fs_table) / sizeof(fs_table[0]); i++) {
        if (fs_table[i] == fs) {
            return true;
        }
    }

    return false;
}

struct sample_rate_info {
    uint32_t sample_rate;
    uint32_t mclk_rate;
    uint32_t mclk_div_num;
};

uint32_t sio_porting_get_bclk_div(uint8_t data_width, uint32_t ch, uint32_t sample_rate_index)
{
    const struct sample_rate_info sample_rate_information[] = {
        {8000, 12288000, 0x1A36E3},
        {11025, 2822400, 0x60568},
        {12000, 12288000, 0x1A36E3},
        {16000, 12288000, 0x1A36E3},
        {22050, 5644800, 0xC0AD0},
        {24000, 12288000, 0x1A36E3},
        {32000, 12288000, 0x1A36E3},
        {44100, 11289600, 0x1815A0},
        {48000, 12288000, 0x1A36E3},
        {88200, 22579200, 0x302B41},
        {96000, 12288000, 0x1A36E3},
        {176400, 45158400, 0x605682},
        {192000, 12288000, 0x1A36E3},
    };
    
    uint32_t sample_rate, mclk, mclk_div_num, bclk_div_num;
    float middle_div;
    mclk = sample_rate_information[sample_rate_index].mclk_rate;
    sample_rate = sample_rate_information[sample_rate_index].sample_rate;
    mclk_div_num = sample_rate_information[sample_rate_index].mclk_div_num;

    reg32(CLDO_CRG_RST_I2S_DIV_CFG_ADDR) = mclk_div_num;
    middle_div = (float)mclk / (sample_rate * data_width * ch);
    if ((uint32_t)(middle_div * I2S_PARAM + 1) == ((uint32_t)middle_div * I2S_PARAM + 1)) {
        bclk_div_num = (uint32_t)middle_div;
    } else {
        bclk_div_num = (uint32_t)middle_div + 1;
    }

    return bclk_div_num;
}

#if defined(CONFIG_I2S_SUPPORT_DMA)
uint32_t i2s_port_get_dma_trans_src_handshaking(sio_bus_t bus)
{
    unused(bus);
    return HAL_DMA_HANDSHAKING_I2S_RX;
}

uint32_t i2s_port_get_dma_trans_dest_handshaking(sio_bus_t bus)
{
    unused(bus);
    return HAL_DMA_HANDSHAKING_I2S_TX;
}
#endif

uint32_t sio_porting_get_mclk(void)
{
    return I2S_MCLK_RATE;
}