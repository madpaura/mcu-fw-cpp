# Licensed under the Apache-2.0 license
# CMakeLists.txt for rom library

add_library(rom STATIC
    src/rom.cpp
    src/boot_status.cpp
    src/cold_boot.cpp
    src/warm_boot.cpp
    src/fw_boot.cpp
    src/fw_hitless_update.cpp
    src/device_ownership_transfer.cpp
    src/fuses.cpp
    src/fuse_layout.cpp
    src/lifecycle.cpp
    src/otp.cpp
    src/rom_env.cpp
    src/i3c.cpp
    src/recovery.cpp
    src/image_verifier.cpp
    src/flash/flash_partition.cpp
    src/flash/hil.cpp
    src/flash/memory.cpp
)

target_include_directories(rom PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../romtime/include
)

target_link_libraries(rom PUBLIC romtime)

target_compile_features(rom PUBLIC cxx_std_17)

# Feature flags
if(ENABLE_HW_2_1)
    target_compile_definitions(rom PRIVATE HW_2_1)
endif()

if(ENABLE_CORE_TEST)
    target_compile_definitions(rom PRIVATE CORE_TEST)
endif()
