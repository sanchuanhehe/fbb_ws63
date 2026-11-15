/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: Status Binding Interface Implementation
 *
 * History:
 * 2024-6-21, Create file.
 */
#include <math.h>
#include "model/generic_server.h"
#include "model/lightness_server.h"
#include "model/light_ctl_server.h"
#include "state_transition.h"
#include "state_binding.h"

#define INT16_NEG_MAX 32768

static int32_t lightness_linear_ceiling(float num)
{
    int32_t inum;

    inum = (int32_t)num;
    if (num == (float)inum) {
        return inum;
    }

    return inum + 1;
}

static uint16_t lightness_actual_to_lightness_linear(uint16_t val)
{
    float tmp;

    tmp = ((float)val / UINT16_MAX);

    return (uint16_t)lightness_linear_ceiling(UINT16_MAX * tmp * tmp);
}

static uint16_t lightness_linear_to_lightness_actual(uint16_t val)
{
    return (uint16_t)(UINT16_MAX * sqrt(((float)val / UINT16_MAX)));
}

static uint16_t level_to_light_ctl_temp(int16_t level, ble_mesh_light_ctl_temp_srv_t *light_ctl_temp_svr_usr)
{
    uint16_t tmp;
    float diff;

    /* Mesh Model Specification 6.1.3.1.1 1st formula start */
    diff = (float)(light_ctl_temp_svr_usr->state->temp_range_max - light_ctl_temp_svr_usr->state->temp_range_min) /
        UINT16_MAX;

    tmp = (uint16_t)((level + INT16_NEG_MAX) * diff);

    return (light_ctl_temp_svr_usr->state->temp_range_min + tmp);

    /* 6.1.3.1.1 1st formula end */
}

static void light_ctl_temp_bind_gen_level(int16_t gen_level, struct bt_mesh_model *gen_level_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(gen_level_model);
    if (!elem) {
        return;
    }

    struct bt_mesh_model *light_ctl_temp_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_LIGHT_CTL_TEMP_SRV);
    if (light_ctl_temp_model && light_ctl_temp_model->user_data) {
        ble_mesh_light_ctl_temp_srv_t *light_ctl_temp_svr_usr =
            (ble_mesh_light_ctl_temp_srv_t *)light_ctl_temp_model->user_data;
        if (light_ctl_temp_svr_usr->state) {
            light_ctl_temp_svr_usr->state->temp = level_to_light_ctl_temp(gen_level, light_ctl_temp_svr_usr);
            light_ctl_temp_svr_usr->state->target_temp = light_ctl_temp_svr_usr->state->temp;
        }
        bt_mesh_server_stop_transition(&light_ctl_temp_svr_usr->transition);
    }
}

static void light_ctl_temp_bind_gen_powerup(uint8_t onpowerup)
{
    if (light_ctl_temp_svr_usr_data) {
        if ((onpowerup == 0 || onpowerup == 1) && light_ctl_temp_svr_usr_data->state) {
            light_ctl_temp_svr_usr_data->state->temp = light_ctl_temp_svr_usr_data->state->temp_def;
            light_ctl_temp_svr_usr_data->state->target_temp = light_ctl_temp_svr_usr_data->state->temp;
        }

        bt_mesh_server_stop_transition(&light_ctl_temp_svr_usr_data->transition);
    }
}

static void light_ctl_bind_light_lightness(uint16_t light_actual)
{
    if (light_ctl_svr_usr_data && light_ctl_svr_usr_data->state) {
        light_ctl_svr_usr_data->state->lightness = light_actual;
        light_ctl_svr_usr_data->state->target_lightness = light_actual;
        bt_mesh_server_stop_transition(&light_ctl_svr_usr_data->transition);
    }
}

static void bind_gen_level(uint16_t state, struct bt_mesh_model *gen_level_model)
{
    if (gen_level_model) {
        int16_t gen_level = state - INT16_NEG_MAX;
        struct bt_mesh_gen_level_srv *level_srv = gen_level_model->user_data;
        bt_mesh_server_stop_transition(&level_srv->transition);
        level_srv->level_state.current_level = gen_level;
        level_srv->level_state.target_level = gen_level;
        if (level_srv->set) {
            level_srv->set(gen_level_model, &level_srv->level_state);
        }
    }
}

static void onoff_bind_lightness(uint8_t onoff, struct bt_mesh_model *lightness_model,
    struct bt_mesh_model *gen_level_model)
{
    if (lightness_model) {
        uint16_t light_actual;
        uint16_t light_linear;
        struct bt_mesh_light_lightness_srv *light_srv = lightness_model->user_data;
        bt_mesh_server_stop_transition(&light_srv->actual_transition);
        bt_mesh_server_stop_transition(&light_srv->linear_transition);
        if (onoff == 0) {
            light_actual = 0;
        } else if (onoff == 1 &&
            light_srv->light_state->def == 0) { // 当onoff=1且lgiht_default=0时，light_actual=lgiht_last
            light_actual = light_srv->light_state->last;
        } else { // 当onoff=1且light_default!=0时，light_actual=light_default
            light_actual = light_srv->light_state->def;
            light_srv->light_state->last = light_actual;
        }
        light_srv->light_state->actual = light_actual;
        light_srv->light_state->target_actual = light_actual;
        light_linear = lightness_actual_to_lightness_linear(light_actual);
        light_srv->light_state->linear = light_linear;
        light_srv->light_state->target_linear = light_linear;
        if (light_srv->set) {
            light_srv->set(lightness_model, light_srv->light_state);
        }

        light_ctl_bind_light_lightness(light_actual);

        bind_gen_level(light_actual, gen_level_model);
    }
}

static void onoff_bind_power_level(uint8_t onoff, struct bt_mesh_model *power_level_model,
    struct bt_mesh_model *gen_level_model)
{
    if (power_level_model) {
        uint16_t pwr_level = 0;
        struct bt_mesh_gen_power_level_srv *pwr_lev_srv = power_level_model->user_data;
        bt_mesh_server_stop_transition(&pwr_lev_srv->transition);
        if (onoff == 0) {
            pwr_level = 0;
        } else if (onoff == 1 && pwr_lev_srv->pwr_level_state->default_level ==
            0) { // 当onoff=1且gen_power_default=0时，gen_power_actual=gen_power_last
            pwr_level = pwr_lev_srv->pwr_level_state->last_level;
        } else { // 当onoff=1且gen_power_default!=0时，gen_power_actual=gen_power_default
            pwr_level = pwr_lev_srv->pwr_level_state->default_level;
            pwr_lev_srv->pwr_level_state->last_level = pwr_level;
        }
        pwr_lev_srv->pwr_level_state->current_level = pwr_level;
        pwr_lev_srv->pwr_level_state->target_level = pwr_level;
        if (pwr_lev_srv->set) {
            pwr_lev_srv->set(power_level_model, pwr_lev_srv->pwr_level_state);
        }

        bind_gen_level(pwr_level, gen_level_model);
    }
}

void gen_onoff_bind_state(uint8_t onoff, struct bt_mesh_model *onoff_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(onoff_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *lightness_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    struct bt_mesh_model *power_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV);
    struct bt_mesh_model *gen_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);

    onoff_bind_lightness(onoff, lightness_model, gen_level_model);

    onoff_bind_power_level(onoff, power_level_model, gen_level_model);
}

static void bind_gen_onoff(uint16_t state, struct bt_mesh_model *gen_onoff_model)
{
    if (gen_onoff_model) {
        uint8_t onoff = 1;
        struct bt_mesh_gen_onoff_srv *onoff_srv = gen_onoff_model->user_data;
        bt_mesh_server_stop_transition(&onoff_srv->transition);
        if (state == 0) {
            onoff = 0;
        }
        onoff_srv->onoff = onoff;
        onoff_srv->target_onoff = onoff;
        if (onoff_srv->set) {
            onoff_srv->set(gen_onoff_model, onoff);
        }
    }
}

static void gen_level_bind_lightness(int16_t gen_level, struct bt_mesh_model *lightness_model,
    struct bt_mesh_model *gen_onoff_model)
{
    if (lightness_model) {
        uint16_t light_actual;
        uint16_t light_linear;
        struct bt_mesh_light_lightness_srv *light_srv = lightness_model->user_data;
        bt_mesh_server_stop_transition(&light_srv->actual_transition);
        bt_mesh_server_stop_transition(&light_srv->linear_transition);
        light_actual = gen_level + INT16_NEG_MAX;
        if (light_actual > light_srv->light_state->light_range_max) {
            light_actual = light_srv->light_state->light_range_max;
        }
        light_srv->light_state->actual = light_actual;
        light_srv->light_state->target_actual = light_actual;
        if (light_actual != 0) {
            light_srv->light_state->last = light_actual;
        }
        light_linear = lightness_actual_to_lightness_linear(light_actual);
        light_srv->light_state->linear = light_linear;
        light_srv->light_state->target_linear = light_linear;
        if (light_srv->set) {
            light_srv->set(lightness_model, light_srv->light_state);
        }

        light_ctl_bind_light_lightness(light_actual);

        bind_gen_onoff(light_actual, gen_onoff_model);
    }
}

static void gen_level_bind_power_level(int16_t gen_level, struct bt_mesh_model *power_level_model,
    struct bt_mesh_model *gen_onoff_model)
{
    if (power_level_model) {
        uint16_t pwr_level = 0;
        struct bt_mesh_gen_power_level_srv *pwr_lev_srv = power_level_model->user_data;
        bt_mesh_server_stop_transition(&pwr_lev_srv->transition);
        pwr_level = gen_level + INT16_NEG_MAX;
        if (pwr_level > pwr_lev_srv->pwr_level_state->max_level) {
            pwr_level = pwr_lev_srv->pwr_level_state->max_level;
        } else if (pwr_level < pwr_lev_srv->pwr_level_state->min_level) {
            pwr_level = pwr_lev_srv->pwr_level_state->min_level;
        }
        pwr_lev_srv->pwr_level_state->current_level = pwr_level;
        pwr_lev_srv->pwr_level_state->target_level = pwr_level;
        if (pwr_level != 0) {
            pwr_lev_srv->pwr_level_state->last_level = pwr_level;
        }

        if (pwr_lev_srv->set) {
            pwr_lev_srv->set(power_level_model, pwr_lev_srv->pwr_level_state);
        }

        bind_gen_onoff(pwr_level, gen_onoff_model);
    }
}

void gen_level_bind_state(int16_t gen_level, struct bt_mesh_model *gen_level_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(gen_level_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *lightness_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    struct bt_mesh_model *power_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV);
    struct bt_mesh_model *gen_onoff_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);

    light_ctl_temp_bind_gen_level(gen_level, gen_level_model);

    gen_level_bind_lightness(gen_level, lightness_model, gen_onoff_model);

    gen_level_bind_power_level(gen_level, power_level_model, gen_onoff_model);
}

static void gen_onpowerup_bind_lightness(uint8_t onpowerup, struct bt_mesh_model *lightness_model)
{
    light_ctl_temp_bind_gen_powerup(onpowerup);

    if (lightness_model) {
        struct bt_mesh_light_lightness_srv *light_srv = lightness_model->user_data;
        bt_mesh_server_stop_transition(&light_srv->actual_transition);
        bt_mesh_server_stop_transition(&light_srv->linear_transition);
        uint16_t lightness_actual = light_srv->light_state->actual;
        if (onpowerup == 0) {
            lightness_actual = 0;
        } else if (onpowerup == 1 && light_srv->light_state->def != 0) {
            lightness_actual = light_srv->light_state->def;
        } else if (onpowerup == 1 && light_srv->light_state->def == 0) {
            lightness_actual = light_srv->light_state->last;
        } else if (light_srv->light_state->actual != light_srv->light_state->target_actual) {
            lightness_actual = light_srv->light_state->target_actual;
        }
        if (lightness_actual != light_srv->light_state->actual) {
            light_srv->light_state->actual = lightness_actual;
            light_srv->light_state->target_actual = lightness_actual;
            lightness_actual_bind_state(lightness_actual, lightness_model);
            if (light_srv->set) {
                light_srv->set(light_srv->model, light_srv->light_state);
            }
        }

        light_ctl_bind_light_lightness(lightness_actual);
    }
}

static void gen_onpowerup_bind_power_level(uint8_t onpowerup, struct bt_mesh_model *power_level_model)
{
    if (power_level_model) {
        uint16_t pwr_level = 0;
        struct bt_mesh_gen_power_level_srv *pwr_lev_srv = power_level_model->user_data;
        bt_mesh_server_stop_transition(&pwr_lev_srv->transition);
        if (onpowerup == 0) {
            pwr_level = 0;
        } else if (onpowerup == 1 && pwr_lev_srv->pwr_level_state->default_level != 0) {
            pwr_level = pwr_lev_srv->pwr_level_state->default_level;
        } else if (onpowerup == 1 && pwr_lev_srv->pwr_level_state->default_level == 0) {
            pwr_level = pwr_lev_srv->pwr_level_state->last_level;
        } else {
            pwr_level = pwr_lev_srv->pwr_level_state->current_level;
        }
        if (pwr_level != pwr_lev_srv->pwr_level_state->current_level) {
            pwr_lev_srv->pwr_level_state->current_level = pwr_level;
            pwr_lev_srv->pwr_level_state->target_level = pwr_level;
            gen_power_level_bind_state(pwr_level, power_level_model);
            if (pwr_lev_srv->set) {
                pwr_lev_srv->set(pwr_lev_srv->model, pwr_lev_srv->pwr_level_state);
            }
        }
    }
}

void gen_onpowerup_bind_state(uint8_t onpowerup, struct bt_mesh_model *gen_power_onoff_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(gen_power_onoff_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *lightness_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_LIGHT_LIGHTNESS_SRV);
    struct bt_mesh_model *power_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_POWER_LEVEL_SRV);

    gen_onpowerup_bind_lightness(onpowerup, lightness_model);

    gen_onpowerup_bind_power_level(onpowerup, power_level_model);
}

void gen_power_level_bind_state(uint16_t gen_pwr_level, struct bt_mesh_model *gen_pwr_level_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(gen_pwr_level_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *gen_onoff_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    struct bt_mesh_model *gen_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);

    if (gen_pwr_level != 0) {
        struct bt_mesh_gen_power_level_srv *pwr_srv = gen_pwr_level_model->user_data;
        pwr_srv->pwr_level_state->last_level = gen_pwr_level;
    }

    bind_gen_onoff(gen_pwr_level, gen_onoff_model);

    bind_gen_level(gen_pwr_level, gen_level_model);
}

void lightness_actual_bind_state(uint16_t lightness_actual, struct bt_mesh_model *lightness_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(lightness_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *gen_onoff_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    struct bt_mesh_model *gen_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);

    light_ctl_bind_light_lightness(lightness_actual);

    uint16_t light_linear;
    struct bt_mesh_light_lightness_srv *light_srv = lightness_model->user_data;
    if (lightness_actual != 0) {
        light_srv->light_state->last = lightness_actual;
    }
    light_linear = lightness_actual_to_lightness_linear(lightness_actual);
    light_srv->light_state->linear = light_linear;
    light_srv->light_state->target_linear = light_linear;

    bind_gen_onoff(lightness_actual, gen_onoff_model);

    bind_gen_level(lightness_actual, gen_level_model);
}

void lightness_linear_bind_state(uint16_t lightness_linear, struct bt_mesh_model *lightness_model)
{
    struct bt_mesh_elem *elem = bt_mesh_model_elem(lightness_model);
    if (!elem) {
        return;
    }
    struct bt_mesh_model *gen_onoff_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_ONOFF_SRV);
    struct bt_mesh_model *gen_level_model = bt_mesh_model_find(elem, BT_MESH_MODEL_ID_GEN_LEVEL_SRV);

    struct bt_mesh_light_lightness_srv *light_srv = lightness_model->user_data;
    bt_mesh_server_stop_transition(&light_srv->actual_transition);
    uint16_t lightness_actual = lightness_linear_to_lightness_actual(lightness_linear);
    light_srv->light_state->actual = lightness_actual;
    light_srv->light_state->target_actual = lightness_actual;
    if (lightness_actual != 0) {
        light_srv->light_state->last = lightness_actual;
    }

    light_ctl_bind_light_lightness(lightness_actual);

    bind_gen_onoff(lightness_actual, gen_onoff_model);

    bind_gen_level(lightness_actual, gen_level_model);
}

static int16_t light_ctl_temp_to_level(uint16_t temp)
{
    float tmp;

    /* Mesh Model Specification 6.1.3.1.1 2nd formula start */

    tmp = (temp - light_ctl_temp_svr_usr_data->state->temp_range_min) * UINT16_MAX;

    tmp =
        tmp / (light_ctl_temp_svr_usr_data->state->temp_range_max - light_ctl_temp_svr_usr_data->state->temp_range_min);

    return (int16_t)(tmp - INT16_NEG_MAX);

    /* 6.1.3.1.1 2nd formula end */
}

static void gen_level_bind_light_temp(uint16_t temp)
{
    struct bt_mesh_model *model =
        bt_mesh_model_find(bt_mesh_model_elem(light_ctl_temp_svr_usr_data->model), BT_MESH_MODEL_ID_GEN_LEVEL_SRV);

    if (model == NULL) {
        BT_ERR("not find gen level model in temp elem!");
        return;
    }

    struct bt_mesh_gen_level_srv *level_srv = (struct bt_mesh_gen_level_srv *)model->user_data;
    level_srv->level_state.current_level = light_ctl_temp_to_level(temp);
    bt_mesh_server_stop_transition(&level_srv->transition);
}

void light_ctl_light_update_state_binding(uint16_t lightness)
{
    if (!light_lightness_srv) {
        BT_ERR("not find light lightness model");
        return;
    }

    light_lightness_srv->light_state->actual = lightness;
    light_lightness_srv->light_state->target_actual = lightness;
    bt_mesh_server_stop_transition(&light_lightness_srv->actual_transition);
    light_lightness_srv->light_state->linear = lightness_actual_to_lightness_linear(lightness);
    light_lightness_srv->light_state->target_linear = light_lightness_srv->light_state->linear;
    bt_mesh_server_stop_transition(&light_lightness_srv->linear_transition);

    if (lightness != 0) {
        light_lightness_srv->light_state->last = lightness;
    }

    if (gen_onoff_srv) {
        if (lightness != 0) {
            gen_onoff_srv->onoff = STATE_ON;
        } else {
            gen_onoff_srv->onoff = STATE_OFF;
        }

        gen_onoff_srv->target_onoff = gen_onoff_srv->onoff;
        bt_mesh_server_stop_transition(&gen_onoff_srv->transition);
    }

    if (gen_level_srv) {
        gen_level_srv->level_state.current_level = lightness - INT16_NEG_MAX;
        gen_level_srv->level_state.target_level = lightness - INT16_NEG_MAX;
        bt_mesh_server_stop_transition(&gen_level_srv->transition);
    }
}

void light_ctl_temp_update_state_binding(uint16_t temp)
{
    gen_level_bind_light_temp(temp);
}
