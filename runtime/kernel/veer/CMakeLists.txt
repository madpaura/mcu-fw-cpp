# Licensed under the Apache-2.0 license
# CMakeLists.txt for veer library

add_library(veer STATIC
    src/veer.cpp
    src/chip.cpp
    src/interrupt.cpp
)

target_include_directories(veer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../romtime/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../../rom/include
)

target_link_libraries(veer PUBLIC romtime rom)

target_compile_features(veer PUBLIC cxx_std_17)
