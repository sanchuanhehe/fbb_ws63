/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: hcc adapt bus completion.
 * Author: CompanyName
 * Create: 2021-05-13
 */

#include "soc_osal.h"
#include "osal_def.h"
#include "osal_list.h"
#include "securec.h"
#include "common_def.h"

#include "hcc_bus_types.h"
#include "hcc_comm.h"
#include "hcc_cfg_comm.h"
#include "hcc_if.h"
#include "hcc.h"
#include "hcc_channel.h"
#include "hcc_service.h"
#ifdef CONFIG_HCC_SUPPORT_IPC
#include "hcc_ipc_adapt.h"
#endif

#ifdef CONFIG_HCC_SUPPORT_SDIO
#include "hcc_adapt_sdio.h"
#endif

#ifdef CONFIG_HCC_SUPPORT_USB
#include "hcc_adapt_usb.h"
#endif

#ifdef CONFIG_HCC_SUPPORT_SPI
#include "hcc_adapt_spi.h"
#endif

#include "hcc_bus.h"

#undef THIS_FILE_ID
#define THIS_FILE_ID DIAG_FILE_ID_HCC_COMM_HCC_BUS_C

#undef THIS_MOD_ID
#define THIS_MOD_ID LOG_PFMODULE

STATIC bus_load_unload g_bus_load_func[HCC_BUS_BUTT] = {
#ifdef CONFIG_HCC_SUPPORT_IPC
    [HCC_BUS_IPC]  = { hcc_adapt_ipc_load, hcc_adapt_ipc_unload },
#endif

#ifdef CONFIG_HCC_SUPPORT_SDIO
    [HCC_BUS_SDIO] = { hcc_adapt_sdio_load, hcc_adapt_sdio_unload },
#endif

#ifdef CONFIG_HCC_SUPPORT_USB
    [HCC_BUS_USB]  = { hcc_adapt_usb_load, hcc_adapt_usb_unload },
#endif

#ifdef CONFIG_HCC_SUPPORT_SPI
    [HCC_BUS_SPI]  = { hcc_adapt_spi_load, hcc_adapt_spi_unload },
#endif
};

td_s32 hcc_set_bus_func(td_u8 bus_type, bus_load_unload *bus_load_opt)
{
    if (bus_type >= HCC_BUS_BUTT || bus_load_opt == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    g_bus_load_func[bus_type].load = bus_load_opt->load;
    g_bus_load_func[bus_type].unload = bus_load_opt->unload;
    return EXT_ERR_SUCCESS;
}

td_void hcc_force_update_queue_id(hcc_bus *bus, td_u8 value)
{
    if (bus == TD_NULL_PTR) {
        return;
    }
    bus->force_update_queue_id = value;
}

td_s32 hcc_bus_power_action(hcc_bus *bus, hcc_bus_power_action_type action)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->power_action != TD_NULL) {
            return bus_ops->power_action(bus, action);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

td_s32 hcc_bus_sleep_request(hcc_bus *bus)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->sleep_request != TD_NULL) {
            return bus_ops->sleep_request(bus);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

td_s32 hcc_bus_wakeup_request(hcc_bus *bus)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->wakeup_request != TD_NULL) {
            return bus_ops->wakeup_request(bus);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

hcc_bus *hcc_get_channel_bus(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc != TD_NULL) {
        return hcc->bus;
    }
    return TD_NULL;
}

bus_dev_ops *hcc_get_bus_ops(hcc_bus *bus)
{
    if (bus == TD_NULL) {
        return TD_NULL;
    }
    return bus->bus_ops;
}

td_void hcc_set_bus_ops(hcc_bus *bus, bus_dev_ops *dev_ops)
{
    if (bus == TD_NULL || dev_ops == TD_NULL) {
        return;
    }
    bus->bus_ops = dev_ops;
}

td_u32 hcc_get_pkt_max_len(td_u8 chl)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    return (bus == TD_NULL ? 0 : bus->max_trans_size);
}

ext_errno hcc_send_message(td_u8 chl, hcc_tx_msg_type msg_id, td_u8 rsv)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    uapi_unused(rsv);

    if (bus_ops == TD_NULL || msg_id >= HCC_TX_MAX_MESSAGE) {
        hcc_printf_err_log("hcc send msg-%d\r\n", msg_id);
        return EXT_ERR_HCC_PARAM_ERR;
    }
    if (bus_ops->send_and_clear_msg != TD_NULL) {
        return bus_ops->send_and_clear_msg(bus, msg_id);
    }
    return EXT_ERR_HCC_BUS_ERR;
}

td_u32 hcc_bus_call_rx_message(hcc_bus *bus, hcc_rx_msg_type msg)
{
    td_u32 ret = EXT_ERR_FAILURE;
    if (msg >= HCC_RX_MAX_MESSAGE) {
        hcc_printf_err_log("hcc bus msg cb-%d\r\n", msg);
        return ret;
    }
    osal_atomic_inc(&bus->msg[msg].count);
    bus->last_msg = msg;

    if (bus->msg[msg].msg_rx != TD_NULL) {
        ret = bus->msg[msg].msg_rx(bus->msg[msg].data);
    } else {
        hcc_printf_err_log("hcc msg not registered, msg -%d\r\n", msg);
    }

    return ret;
}

hcc_bus *hcc_alloc_bus(td_void)
{
    hcc_bus *bus = (hcc_bus *)osal_kzalloc(sizeof(hcc_bus), OSAL_GFP_KERNEL);
    if (bus == TD_NULL) {
        hcc_printf_err_log("hcc bus malloc:%d err\r\n", sizeof(hcc_bus));
        return TD_NULL;
    }
    // hcc bus中默认初始化的值需要在调用该接口以后赋值，如对齐长度
    return bus;
}

td_void hcc_free_bus(hcc_bus *bus)
{
    if (bus != TD_NULL) {
        osal_kfree(bus);
    }
}

td_u32 hcc_bus_load(hcc_bus_type bus_type, hcc_handler *hcc)
{
    hcc_bus *bus = TD_NULL;

    hcc_printf("[success] - load bus: %d\r\n", bus_type);
    if (bus_type >= HCC_BUS_BUTT) {
        return EXT_ERR_FAILURE;
    }
    if (g_bus_load_func[bus_type].load == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    bus = g_bus_load_func[bus_type].load(hcc);
    if (bus == TD_NULL) {
        return EXT_ERR_FAILURE;
    }
    bus->hcc = hcc;
    hcc->bus = bus;
    return EXT_ERR_SUCCESS;
}

td_void hcc_bus_unload(TD_CONST hcc_bus *bus)
{
    if (bus != TD_NULL && bus->bus_type < HCC_BUS_BUTT) {
        return g_bus_load_func[bus->bus_type].unload();
    }
}

td_u32 hcc_bus_tx_proc(hcc_bus *bus, hcc_trans_queue *queue, td_u16 *remain_pkt_nums)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->tx_proc != TD_NULL) {
            return bus_ops->tx_proc(bus, queue, remain_pkt_nums);
        }
    }
    hcc_printf_err_log("hcc bus ops tx proc is null\r\n");
    return EXT_ERR_HCC_BUS_ERR;
}

td_bool hcc_bus_is_busy(hcc_bus *bus, hcc_queue_dir dir)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->is_busy != TD_NULL) {
            return bus_ops->is_busy(dir);
        }
    }
    return TD_FALSE;
}

td_void hcc_bus_update_credit(hcc_bus *bus, td_u32 credit)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->update_credit != TD_NULL) {
            bus_ops->update_credit(bus, credit);
        }
    }
}

td_s32 hcc_bus_get_credit(hcc_bus *bus, td_u32 *credit)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL && credit != TD_NULL) {
        if (bus_ops->get_credit != TD_NULL) {
            return bus_ops->get_credit(bus, credit);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

td_void hcc_set_tx_sched_count(td_u8 chl, td_u8 count)
{
    hcc_bus *bus = hcc_get_channel_bus(chl);
    if (bus != TD_NULL) {
        bus->tx_sched_count = count;
    }
}

td_u32 hcc_get_channel_align_len(td_u8 chl)
{
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL) {
        return HCC_DEFAULT_ALIGN_LEN;
    }

    if (hcc->bus == TD_NULL) {
        return HCC_DEFAULT_ALIGN_LEN;
    }
    return hcc->bus->len_align;
}

td_s32 hcc_bus_reinit(hcc_bus *bus)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->reinit != TD_NULL) {
            return bus_ops->reinit(bus);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}


#ifdef CONFIG_HCC_SUPPORT_REG_OPT
ext_errno hcc_read_reg(td_u8 chl, td_u32 addr, td_u32 *value)
{
    bus_dev_ops *bus_ops = TD_NULL;
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL || hcc->hcc_state == HCC_BUS_FORBID) {
        return EXT_ERR_HCC_BUS_ERR;
    }
    bus_ops = hcc_get_bus_ops(hcc->bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->read_reg != TD_NULL) {
            return bus_ops->read_reg(addr, value);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

ext_errno hcc_write_reg(td_u8 chl, td_u32 addr, td_u32 value)
{
    bus_dev_ops *bus_ops = TD_NULL;
    hcc_handler *hcc = hcc_get_handler(chl);
    if (hcc == TD_NULL || hcc->hcc_state == HCC_BUS_FORBID) {
        return EXT_ERR_HCC_BUS_ERR;
    }
    bus_ops = hcc_get_bus_ops(hcc->bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->write_reg != TD_NULL) {
            return bus_ops->write_reg(addr, value);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}
#endif

#ifdef CONFIG_HCC_SUPPORT_PATCH_OPT
td_s32 hcc_bus_patch_read(hcc_bus *bus, td_u8 *buff, td_s32 len, td_u32 timeout)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->patch_read != TD_NULL) {
            return bus_ops->patch_read(bus, buff, len, timeout);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}

td_s32 hcc_bus_patch_write(hcc_bus *bus, td_u8 *buff, td_s32 len)
{
    bus_dev_ops *bus_ops = hcc_get_bus_ops(bus);
    if (bus_ops != TD_NULL) {
        if (bus_ops->patch_write != TD_NULL) {
            return bus_ops->patch_write(bus, buff, len);
        }
    }
    return EXT_ERR_HCC_BUS_ERR;
}
#endif
osal_module_export(hcc_send_message);
osal_module_export(hcc_read_reg);
osal_module_export(hcc_write_reg);
osal_module_export(hcc_get_pkt_max_len);
osal_module_export(hcc_get_channel_align_len);
osal_module_export(hcc_set_tx_sched_count);
