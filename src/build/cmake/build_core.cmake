#===============================================================================
# @brief    build core - shared by in-tree root CMakeLists and out-of-tree
#           project.cmake. Provides prologue / epilogue macros that bracket
#           the literal project() call required by CMake.
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2026. All rights reserved.
#===============================================================================

# Caller contract:
#   - cmake_minimum_required already called
#   - ROOT_DIR already defined (in-tree: PROJECT_SOURCE_DIR; out-of-tree: project.cmake derives it)
#   - CHIP, BIN_NAME, CCFLAGS, BUILD_TYPE etc. already passed via -D
#
# Usage (in caller's CMakeLists.txt):
#   include("${ROOT_DIR}/build/cmake/build_core.cmake")
#   cfbb_build_prologue()
#   project(${CHIP}_CFBB C ASM CXX)
#   cfbb_build_epilogue()

function(_fbb_link_sdk_interfaces external_target)
    if(NOT TARGET "${external_target}")
        return()
    endif()

    get_target_property(_fbb_external_type "${external_target}" TYPE)
    get_target_property(_fbb_external_imported "${external_target}" IMPORTED)
    if(_fbb_external_type STREQUAL "INTERFACE_LIBRARY" OR
            _fbb_external_imported)
        target_link_libraries("${external_target}" INTERFACE ${ARGN})
    else()
        target_link_libraries("${external_target}" PRIVATE ${ARGN})
    endif()
endfunction()

# --------------------------------------------------------------------
# Prologue - runs before project(); sets up compile env, platform name,
# kconfig helpers, and includes the foundational cmake modules.
# --------------------------------------------------------------------
macro(cfbb_build_prologue)
    set(CMAKE_SYSTEM_NAME "Generic")

    if(DEFINED ENV{build_ws63_sdk_open})
        set(build_lib true)
    else()
        set(build_lib false)
    endif()

    if(NOT DEFINED CHIP)
        message(FATAL_ERROR "Chip is not defined ")
    endif()

    if(NOT DEFINED ROOT_DIR)
        message(FATAL_ERROR "ROOT_DIR must be defined before cfbb_build_prologue()")
    endif()
    # Windows shells may pass backslashes; CMake parses them as escape
    file(TO_CMAKE_PATH "${ROOT_DIR}" ROOT_DIR)

    set(Python3_EXECUTABLE ${PY_PATH})
    find_program(CCACHE_FOUND ccache)
    if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    endif(CCACHE_FOUND)

    # default hso module_name & AUTO_DEF_FILE_ID & AUTO_DEF_MODULE_ID, redefine these var in sub cmake file will
    # override these value just in sub directory but not others
    set(MODULE_NAME "pf")
    set(AUTO_DEF_FILE_ID TRUE)
    set(AUTO_DEF_MODULE_ID TRUE)

    set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
    set(CMAKE_DIR "${ROOT_DIR}/build/cmake")
    set(BIN_DIR   "${ROOT_DIR}/interim_binary")

    include("${CMAKE_DIR}/build_function.cmake")
    include("${CMAKE_DIR}/global_variable.cmake")
    include("${CMAKE_DIR}/build_script.cmake")
    include("${CMAKE_DIR}/build_command.cmake")
    include("${CMAKE_DIR}/build_hso_database.cmake")
    include("${CMAKE_DIR}/build_component.cmake")
    include("${CMAKE_DIR}/build_sdk.cmake")
    include_directories(${ROOT_DIR}/kernel/liteos/liteos_v208.5.0/Huawei_LiteOS/targets/ws63/include)

    if(EXISTS "${ROOT_DIR}/kernel/liteos/liteos_v208.5.0/${TARGET_COMMAND}")
        set(build_ws63_sdk_open true)
    endif ()

    # UT / FUZZ early-exit paths: returning from a macro returns from caller's
    # scope (root CMakeLists.txt) — so project() and epilogue are skipped.
    if(${BUILD_TYPE} STREQUAL "UT")
        KCONFIG_GET_PARAMS("${ROOT_DIR}/build/menuconfig/test/platform/ut.config")
        include("${CMAKE_DIR}/build_ut.cmake")
        return()
    endif()

    if(${BUILD_TYPE} STREQUAL "FUZZ")
        KCONFIG_GET_PARAMS("${ROOT_DIR}/build/menuconfig/test/platform/ut.config")
        include("${CMAKE_DIR}/build_fuzz.cmake")
        return()
    endif()
endmacro()

# --------------------------------------------------------------------
# Epilogue - runs after project(); brings in all components, the linker,
# bin/elf post-processing, signing, fwpkg packaging.
# --------------------------------------------------------------------
macro(cfbb_build_epilogue)
    set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")
    set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "" CACHE INTERNAL "Clear C implicit libs")

    if(EXISTS ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds)
        file(REMOVE ${CMAKE_BINARY_DIR}/auto_gen_libfunc.lds)
    endif()

    set(TARGET_COMPONENT "${RAM_COMPONENT}" "${ROM_COMPONENT}")
    set(TARGET_NAME ${BIN_NAME})
    file(WRITE ${PROJECT_BINARY_DIR}/temp/__null___.c "int __null___(void) {return 0;}")
    add_executable(${BIN_NAME} ${PROJECT_BINARY_DIR}/temp/__null___.c)
    set_target_properties(${BIN_NAME} PROPERTIES RUNTIME_OUTPUT_NAME ${BIN_NAME}.elf)
    target_compile_options(${BIN_NAME} PRIVATE "${CCFLAGS}")

    include("${CMAKE_DIR}/build_rom_callback.cmake")
    if(${BUILD_ROM_CALLBACK})
        build_rom_callback()
    endif()

    if(NOT "$ENV{FBB_KCONFIG_CONFIG}" STREQUAL "" AND
            "$ENV{FBB_PROJECT_TARGET}" STREQUAL "${BIN_NAME}")
        file(TO_CMAKE_PATH "$ENV{FBB_KCONFIG_CONFIG}" KCONFIG_PATH)
    else()
        set(KCONFIG_PATH "${ROOT_DIR}/build/config/target_config/${CHIP}/menuconfig/${CORE}/${BUILD_TARGET_NAME}.config")
    endif()
    if(EXISTS "${KCONFIG_PATH}")
        KCONFIG_GET_PARAMS("${KCONFIG_PATH}")
        set(USE_KCONFIG True)
    endif()

    # SDK component tree: in-tree these are relative add_subdirectory() and
    # work because CMAKE_CURRENT_SOURCE_DIR == ROOT_DIR. Out-of-tree they
    # would silently no-op, so use the ROOT_DIR-anchored form unconditionally.
    foreach(_comp application bt bootloader kernel drivers middleware
                  open_source protocol test include vendor)
        if(EXISTS "${ROOT_DIR}/${_comp}/CMakeLists.txt")
            add_subdirectory("${ROOT_DIR}/${_comp}" "${CMAKE_BINARY_DIR}/${_comp}")
        endif()
    endforeach()

    if("$ENV{FBB_PROJECT_TARGET}" STREQUAL "" OR "$ENV{FBB_PROJECT_TARGET}" STREQUAL "${BIN_NAME}")
    if(NOT "$ENV{FBB_COMPONENTS_CMAKE}" STREQUAL "")
        if(NOT EXISTS "$ENV{FBB_COMPONENTS_CMAKE}")
            message(FATAL_ERROR
                "FBB_COMPONENTS_CMAKE was provided but does not exist: "
                "$ENV{FBB_COMPONENTS_CMAKE}")
        endif()
        file(TO_CMAKE_PATH "$ENV{FBB_PROJECT_DIR}" FBB_PROJECT_DIR)
        include("$ENV{FBB_COMPONENTS_CMAKE}")
        # External targets are generated outside build_component(), so give
        # them the public interfaces of the SDK components enabled for this
        # image. Rebuild the list in this directory: TARGETS_INTERFACES is set
        # inside component subdirectories and is therefore not a reliable
        # image-wide closure here. External targets are linked explicitly and
        # intentionally stay outside RAM_COMPONENT.
        set(_fbb_sdk_interfaces)
        foreach(_fbb_sdk_component IN LISTS RAM_COMPONENT ROM_COMPONENT)
            if(TARGET "${_fbb_sdk_component}_interface")
                list(APPEND _fbb_sdk_interfaces
                     "${_fbb_sdk_component}_interface")
            endif()
        endforeach()
        foreach(_fbb_external_target IN LISTS FBB_EXTERNAL_COMPONENT_TARGETS)
            _fbb_link_sdk_interfaces(
                "${_fbb_external_target}" ${_fbb_sdk_interfaces})
        endforeach()
    endif()

    # Out-of-tree projects additionally register their own main target. The
    # directory scan remains a v1-only fallback when no exact plan exists.
    if(FBB_OUT_OF_TREE)
        if("$ENV{FBB_COMPONENTS_CMAKE}" STREQUAL "")
            set(_FBB_LEGACY_COMPONENT_SCAN TRUE)
        endif()
        if(EXISTS "${CMAKE_SOURCE_DIR}/main/CMakeLists.txt")
            list(APPEND RAM_COMPONENT "${FBB_PROJECT_COMPONENT_NAME}")
            add_subdirectory("${CMAKE_SOURCE_DIR}/main"
                             "${CMAKE_BINARY_DIR}/_fbb_project_main")
            if(TARGET "${FBB_PROJECT_COMPONENT_NAME}" AND
                    FBB_EXTERNAL_COMPONENT_TARGETS)
                target_link_libraries(
                    ${FBB_PROJECT_COMPONENT_NAME} PRIVATE
                    ${FBB_EXTERNAL_COMPONENT_TARGETS})
            endif()
        endif()
        if(_FBB_LEGACY_COMPONENT_SCAN)
            # v1 compatibility only: legacy projects had no Lockfile plan.
            file(GLOB _proj_comps RELATIVE "${CMAKE_SOURCE_DIR}/components"
                 "${CMAKE_SOURCE_DIR}/components/*")
            foreach(_pc ${_proj_comps})
                if(EXISTS "${CMAKE_SOURCE_DIR}/components/${_pc}/CMakeLists.txt")
                    list(APPEND RAM_COMPONENT "${_pc}")
                    add_subdirectory("${CMAKE_SOURCE_DIR}/components/${_pc}"
                                     "${CMAKE_BINARY_DIR}/_fbb_project_${_pc}")
                endif()
            endforeach()
        endif()
    endif()
    if(FBB_EXTERNAL_COMPONENT_TARGETS)
        # Keep external static libraries after the target that references them;
        # their public interfaces were registered from the exact plan.
        target_link_libraries(
            ${TARGET_NAME} PRIVATE ${FBB_EXTERNAL_COMPONENT_TARGETS})
    endif()
    if(FBB_EXTERNAL_BOOTSTRAP_SOURCE)
        # Bootstrap owns the single strong external_board_init definition.
        # Compile it only into the final firmware target.
        target_sources(
            ${TARGET_NAME} PRIVATE "${FBB_EXTERNAL_BOOTSTRAP_SOURCE}")
    endif()
    endif() # FBB project image
    # Refresh the linker's notion of all components after project additions.
    set(TARGET_COMPONENT "${RAM_COMPONENT}" "${ROM_COMPONENT}")

    include("${CMAKE_DIR}/open_source.cmake")
    include("${CMAKE_DIR}/middleware/hwsec_c.cmake")
    include("${CMAKE_DIR}/build_linker.cmake")

    if (NOT DEFINED ROM_COMPONENT)
    add_custom_target(GENERAT_BIN ALL
        COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -R .fls_loader_ram -R .logstr -R .ARM -R .ARM ${BIN_NAME}.elf ${BIN_NAME}.bin
        COMMENT "post_build:gen bin file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
    else()
    add_custom_target(GENERAT_BIN ALL
        COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -R .logstr -R .ARM.exidx -R .ARM.extab -R .*_romtext ${TARGET_NAME}.elf ${TARGET_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} --gap-fill 0xFF -O binary -j .*_romtext ${TARGET_NAME}.elf ${TARGET_NAME}_rom.bin
        COMMENT "post_build:gen rom and ram bin file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
    endif()

    if(DEFINED CONFIG_SFC_SUPPORT_RWE_INDEPENDENT)
    add_custom_target(GENERAT_FLASH_DRIVER_BIN ALL
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=0x20 --srec-forceS3 -S -j .fls_loader_ram ${BIN_NAME}.elf BOOTLOADERFlsDrv.signed.s19
        COMMENT "post_build:gen flash driver bin file"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS ${TARGET_NAME}
    )
    endif()

    include("${CMAKE_DIR}/build_ssb.cmake")
    include("${CMAKE_DIR}/build_elf_info.cmake")
    include("${CMAKE_DIR}/build_sign.cmake")
    include("${CMAKE_DIR}/build_nv_bin.cmake")
    include("${CMAKE_DIR}/build_partition_bin.cmake")
    include("${CMAKE_DIR}/build_boot_bin_cp.cmake")

    create_hso_db()
    generate_project_file()
endmacro()
