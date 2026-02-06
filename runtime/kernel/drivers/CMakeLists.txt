# Licensed under the Apache-2.0 license
# CMakeLists.txt for drivers library

add_library(drivers STATIC
    doe/doe.cpp
    flash/flash.cpp
    i3c/i3c.cpp
    mcu_mbox/mcu_mbox.cpp
)

target_include_directories(drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../romtime/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../rom/include
)

target_link_libraries(drivers PUBLIC romtime rom)

target_compile_features(drivers PUBLIC cxx_std_17)
