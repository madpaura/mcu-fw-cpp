# Licensed under the Apache-2.0 license
# CMakeLists.txt for capsules library

add_library(capsules STATIC
    src/capsules.cpp
    src/flash_partition.cpp
    src/mailbox.cpp
    src/mbox_sram.cpp
    src/mci.cpp
    src/mcu_mbox.cpp
    src/system.cpp
)

target_include_directories(capsules PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../romtime/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../rom/include
)

target_link_libraries(capsules PUBLIC romtime rom)

target_compile_features(capsules PUBLIC cxx_std_17)
