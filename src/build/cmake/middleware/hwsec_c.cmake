#===============================================================================
# @brief    cmake file
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "hwsec_c")

set(HWSEC_C_DIR ${ROOT_DIR}/middleware/utils/hwsec_c)
    set(SOURCES
        ${HWSEC_C_DIR}/src/memcpy_s.c
        ${HWSEC_C_DIR}/src/memmove_s.c
        ${HWSEC_C_DIR}/src/memset_s.c
        ${HWSEC_C_DIR}/src/securecutil.c
        ${HWSEC_C_DIR}/src/secureprintoutput_a.c
        ${HWSEC_C_DIR}/src/snprintf_s.c
        ${HWSEC_C_DIR}/src/sprintf_s.c
        ${HWSEC_C_DIR}/src/strcat_s.c
        ${HWSEC_C_DIR}/src/strcpy_s.c
        ${HWSEC_C_DIR}/src/strncat_s.c
        ${HWSEC_C_DIR}/src/strncpy_s.c
        ${HWSEC_C_DIR}/src/vsnprintf_s.c
        ${HWSEC_C_DIR}/src/vsprintf_s.c
        ${HWSEC_C_DIR}/src/strtok_s.c
    )



set(PUBLIC_HEADER
    ${HWSEC_C_DIR}/include
)

set(PRIVATE_HEADER
    ${HWSEC_C_DIR}/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()


set(COMPONENT_NAME "hwsec_c_sscanf_s")
set(SOURCES
    ${HWSEC_C_DIR}/src/sscanf_s.c
    ${HWSEC_C_DIR}/src/vsscanf_s.c
    ${HWSEC_C_DIR}/src/secureinput_a.c
)

set(PUBLIC_HEADER
    ${HWSEC_C_DIR}/include
)

set(PRIVATE_HEADER
    ${HWSEC_C_DIR}/src
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
