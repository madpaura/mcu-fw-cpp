# Licensed under the Apache-2.0 license
# CMakeLists.txt for components library

add_library(components STATIC
    src/components.cpp
    src/dma.cpp
    src/doe.cpp
    src/flash_partition.cpp
    src/mailbox.cpp
    src/mbox_sram.cpp
    src/mci.cpp
    src/mctp_driver.cpp
    src/mcu_mbox.cpp
    src/mock_mctp.cpp
    src/mux_mctp.cpp
    src/system.cpp
)

target_include_directories(components PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../romtime/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../rom/include
)

target_link_libraries(components PUBLIC romtime rom)

target_compile_features(components PUBLIC cxx_std_17)
