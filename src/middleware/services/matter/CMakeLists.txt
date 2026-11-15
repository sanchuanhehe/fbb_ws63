set(COMPONENT_NAME "matter")

set(target ws63)
set(chip_dir ${CMAKE_CURRENT_SOURCE_DIR})
set(base_dir ${ROOT_DIR})
set(config_dir ${CMAKE_BINARY_DIR}/config)
set(output_dir ${CMAKE_BINARY_DIR}/matter_build)
set(lib_dir ${output_dir}/out/${target}/lib)
set(toolchain ${COMPILER_ROOT})
set(toolchain_prefix riscv32-linux-musl-)

if(EXISTS ${chip_dir}/connectedhomeip)
message(STATUS "exist connectedhomeip")

include(ExternalProject)
ExternalProject_Add(matter_project
	SOURCE_DIR ${chip_dir}
	CONFIGURE_COMMAND ""
	BUILD_IN_SOURCE 1
	BUILD_COMMAND
		make TARGET=${target}
		TOOLCHAIN_PATH=${toolchain}
		TOOLCHAIN_PREFIX=${toolchain_prefix}
		CONFIG_DIR=${config_dir}
		BASEDIR=${base_dir}
		OUTPUT_DIR=${output_dir}
		-f ${chip_dir}/libMatter.mk
	BUILD_BYPRODUCTS ${lib_dir}/libMatterApp.a
	BUILD_ALWAYS 1
	INSTALL_COMMAND ""
	)

add_library(lib_matter STATIC IMPORTED GLOBAL)
add_dependencies(lib_matter matter_project)
set_target_properties(lib_matter PROPERTIES IMPORTED_LOCATION ${lib_dir}/libMatterApp.a)

endif()

set(LIBS lib_matter)

set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

build_component()
