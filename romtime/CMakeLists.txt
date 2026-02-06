# Licensed under the Apache-2.0 license
# CMakeLists.txt for romtime library

add_library(romtime STATIC
    src/romtime.cpp
    src/mci.cpp
    src/soc_manager.cpp
    src/static_ref.cpp
)

target_include_directories(romtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_features(romtime PUBLIC cxx_std_17)

# Platform-specific sources
if(BUILD_RISCV32)
    target_sources(romtime PRIVATE
        src/platform_riscv32.cpp
    )
else()
    target_sources(romtime PRIVATE
        src/platform_host.cpp
    )
endif()
