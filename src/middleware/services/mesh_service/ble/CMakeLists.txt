#===============================================================================
# @brief mesh service cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "ble_mesh")
set(MODULE_NAME "mesh")
set(AUTO_DEF_FILE_ID TRUE)

add_subdirectory(mesh_adapter)

set(OPENSOURCE_DIR
    ${ROOT_DIR}/open_source/
    CACHE PATH "open_source dir"
)

set(BLE_MESH_DIR
    ${ROOT_DIR}/middleware/services/mesh_service/ble/
    CACHE PATH "ble mesh dir"
)

set(NIMBLE_DIR
    ${OPENSOURCE_DIR}/mynewt-nimble
)

set(PORTING_SRC
    ${NIMBLE_DIR}/porting/nimble/src/os_mempool.c
    ${NIMBLE_DIR}/porting/nimble/src/endian.c
    ${NIMBLE_DIR}/porting/nimble/src/mem.c
    ${NIMBLE_DIR}/porting/nimble/src/os_mbuf.c
    ${NIMBLE_DIR}/porting/nimble/src/os_msys_init.c
    ${NIMBLE_DIR}/porting/nimble/src/nimble_port.c
    ${NIMBLE_DIR}/porting/nimble/src/os_cputime.c
    ${NIMBLE_DIR}/porting/nimble/src/os_cputime_pwr2.c
    ${NIMBLE_DIR}/nimble/host/src/ble_sm_alg.c
    ${NIMBLE_DIR}/nimble/host/src/ble_hs_log.c
)

set(COMPONENT_SRC
    ${NIMBLE_DIR}/nimble/host/mesh/src/access.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/adv_ext.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/adv_legacy.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/adv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/aes-ccm.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/app_keys.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/beacon.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/priv_beacon.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/cdb.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/cfg_cli.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/cfg_srv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/cfg.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/crypto.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/friend.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/glue.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/health_cli.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/health_srv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/heartbeat.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/light_model.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/lpn.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/mesh.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/app_keys.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/msg.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/net.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/pb_adv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/pb_gatt_srv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/pb_gatt.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/prov_device.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/prov.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/provisioner.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/proxy_msg.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/proxy_srv.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/rpl.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/settings.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/subnet.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/testing.c
    ${NIMBLE_DIR}/nimble/host/mesh/src/transport.c

    
    ${NIMBLE_DIR}/nimble/transport/src/monitor.c
    ${NIMBLE_DIR}/nimble/transport/src/transport.c

    ${NIMBLE_DIR}/ext/tinycrypt/src/aes_decrypt.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/aes_encrypt.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/cbc_mode.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ccm_mode.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/cmac_mode.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ctr_mode.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ctr_prng.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/utils.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ecc.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ecc_dh.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ecc_dsa.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/ecc_platform_specific.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/hmac_prng.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/hmac.c
    ${NIMBLE_DIR}/ext/tinycrypt/src/sha256.c
)

set(
    HEADER_LIST 
    ${ROOT_DIR}/include
    ${NIMBLE_DIR}/nimble/host/mesh/include
    ${NIMBLE_DIR}/nimble/include
    ${NIMBLE_DIR}/nimble/host/include
    ${NIMBLE_DIR}/nimble/transport/include
    ${NIMBLE_DIR}/nimble/host/mesh/src
    ${NIMBLE_DIR}/porting/npl/mynewt/include/nimble
    ${NIMBLE_DIR}/porting/nimble/include
    ${NIMBLE_DIR}/porting/nimble/include/sysinit
    ${NIMBLE_DIR}/ext/tinycrypt/include
    ${NIMBLE_DIR}/nimble/host/include/host
    ${NIMBLE_DIR}/nimble/host/src
    ${NIMBLE_DIR}/nimble/host/services/gatt/include
)

set(
    PUBLIC_HEADER_LIST
    ${BLE_MESH_DIR}/mesh_adapter/include/utils
    ${BLE_MESH_DIR}/mesh_adapter/include/model
    ${BLE_MESH_DIR}/mesh_adapter/mesh_porting/include
)

set(COMPONENT_PUBLIC_CCFLAGS
)
 
set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-label
    -Wno-unused-function
    -Wno-unused-but-set-variable
    -Wno-sign-compare
    -Wno-incompatible-pointer-types
    -Wno-implicit-function-declaration
    -Wno-builtin-declaration-mismatch
)

set(
    SOURCES_LIST
    ${PORTING_SRC}
    ${COMPONENT_SRC}
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

set(SOURCES "${SOURCES}" ${SOURCES_LIST})
set(PRIVATE_HEADER "${PRIVATE_HEADER}" ${HEADER_LIST})
set(PUBLIC_HEADER "${PUBLIC_HEADER}" ${PUBLIC_HEADER_LIST})

build_component()
