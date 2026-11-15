#===============================================================================
# @brief cmake file — BLE HID Button sample
#===============================================================================
set(SOURCES "${SOURCES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ble_hid_btn_sample.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ble_hid_btn.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ble_hid_adv.c"
    PARENT_SCOPE)

set(PRIVATE_HEADER "${PRIVATE_HEADER}"
    "${CMAKE_CURRENT_SOURCE_DIR}/inc"
    PARENT_SCOPE)