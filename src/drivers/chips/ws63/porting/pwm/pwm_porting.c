/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "chip_core_irq.h"
#include "soc_osal.h"
#include "common_def.h"
#include "hal_pwm_v151.h"
#include "platform_core.h"
#include "chip_io.h"
#include "soc_porting.h"
#include "pwm_porting.h"

#define BUS_CLOCK_TIME_40M      40000000UL
#define BIT_WIDTH_LIMIT         0xFFFF
#define CLDO_CRG_CLK_SEL        0x44001134
#define PWM_CKSEL_BIT           7

#define CLDO_SUB_CRG_CKEN_CTL0   0x44001100
#define CLDO_CRG_DIV_CTL3        0x44001114
#define CLDO_CRG_DIV_CTL4        0x44001118
#define CLDO_CRG_DIV_CTL5        0x4400111C

#define PWM_BUS_CKEN             2
#define PWM_CHANNEL_CKEN_LEN     9

#define PWM0_LOAD_DIV_EN         20
#define PWM0_DIV1_CFG            16
#define PWM0_DIV1_CFG_LEN        4
#define PWM1_LOAD_DIV_EN         30
#define PWM1_DIV1_CFG            26
#define PWM1_DIV1_CFG_LEN        4
#define PWM2_LOAD_DIV_EN         8
#define PWM2_DIV1_CFG            4
#define PWM2_DIV1_CFG_LEN        4
#define PWM3_LOAD_DIV_EN         18
#define PWM3_DIV1_CFG            14
#define PWM3_DIV1_CFG_LEN        4
#define PWM4_LOAD_DIV_EN         28
#define PWM4_DIV1_CFG            24
#define PWM4_DIV1_CFG_LEN        4
#define PWM5_LOAD_DIV_EN         8
#define PWM5_DIV1_CFG            4
#define PWM5_DIV1_CFG_LEN        4
#define PWM6_LOAD_DIV_EN         18
#define PWM6_DIV1_CFG            14
#define PWM6_DIV1_CFG_LEN        4
#define PWM7_LOAD_DIV_EN         28
#define PWM7_DIV1_CFG            24
#define PWM7_DIV1_CFG_LEN        4

#define PWM_DIV_6                6
#define PWM_DIV_10               10

uintptr_t g_pwm_base_addr =  (uintptr_t)PWM_0_BASE;

uintptr_t pwm_porting_base_addr_get(void)
{
    return g_pwm_base_addr;
}

static int pwm_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);
    hal_pwm_v151_irq_handler();
    return 0;
}


void pwm_port_register_hal_funcs(void)
{
    hal_pwm_register_funcs(hal_pwm_v151_funcs_get());
}

void pwm_port_unregister_hal_funcs(void)
{
    hal_pwm_unregister_funcs();
}

void pwm_port_clock_enable(bool on)
{
    if (on == true) {
        uint32_t div_cfg = PWM_DIV_6;
#ifdef CONFIG_HIGH_FREQUENCY
        uapi_reg_setbit(CLDO_CRG_CLK_SEL, PWM_CKSEL_BIT);
#elif defined(CONFIG_LOW_FREQUENCY)
        uapi_reg_clrbit(CLDO_CRG_CLK_SEL, PWM_CKSEL_BIT);
        if (get_tcxo_freq() == CLK24M_TCXO) {
            div_cfg = PWM_DIV_6;
        } else {
            div_cfg = PWM_DIV_10;
        }
#endif
        reg32_setbits(CLDO_SUB_CRG_CKEN_CTL0, PWM_BUS_CKEN, PWM_CHANNEL_CKEN_LEN, 0x1FF);

        reg32_clrbit(CLDO_CRG_DIV_CTL3, PWM0_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL3, PWM0_DIV1_CFG, PWM0_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL3, PWM0_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL3, PWM1_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL3, PWM1_DIV1_CFG, PWM1_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL3, PWM1_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL4, PWM2_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL4, PWM2_DIV1_CFG, PWM2_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL4, PWM2_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL4, PWM3_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL4, PWM3_DIV1_CFG, PWM3_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL4, PWM3_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL4, PWM4_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL4, PWM4_DIV1_CFG, PWM4_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL4, PWM4_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL5, PWM5_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL5, PWM5_DIV1_CFG, PWM5_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL5, PWM5_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL5, PWM6_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL5, PWM6_DIV1_CFG, PWM6_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL5, PWM6_LOAD_DIV_EN);

        reg32_clrbit(CLDO_CRG_DIV_CTL5, PWM7_LOAD_DIV_EN);
        reg32_setbits(CLDO_CRG_DIV_CTL5, PWM7_DIV1_CFG, PWM7_DIV1_CFG_LEN, div_cfg);
        reg32_setbit(CLDO_CRG_DIV_CTL5, PWM7_LOAD_DIV_EN);
    } else {
        reg32_clrbits(CLDO_SUB_CRG_CKEN_CTL0, PWM_BUS_CKEN, PWM_CHANNEL_CKEN_LEN);
    }
}

void pwm_port_register_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_request((uintptr_t)PWM_ABNOR_IRQN, (osal_irq_handler)pwm_handler, NULL, NULL, NULL);

    osal_irq_enable((uintptr_t)PWM_ABNOR_IRQN);
}

void pwm_port_unregister_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_disable((uintptr_t)PWM_ABNOR_IRQN);
    osal_irq_disable((uintptr_t)PWM_CFG_IRQN);
    osal_irq_free((uintptr_t)PWM_ABNOR_IRQN, NULL);
    osal_irq_free((uintptr_t)PWM_CFG_IRQN, NULL);
}

void pwm_irq_lock(uint8_t channel)
{
    unused(channel);
    osal_irq_lock();
}

void pwm_irq_unlock(uint8_t channel)
{
    unused(channel);
    osal_irq_unlock();
}

uint32_t pwm_port_get_clock_value(pwm_channel_t channel)
{
    if (channel >= PWM_MAX_NUMBER) {
        return 0;
    }
    return BUS_CLOCK_TIME_40M;
}

errcode_t pwm_port_param_check(const pwm_config_t *cfg)
{
    if ((cfg->low_time + cfg->high_time > BIT_WIDTH_LIMIT) || (cfg->offset_time > cfg->low_time)) {
        return ERRCODE_PWM_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}