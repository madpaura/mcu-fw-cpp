# Licensed under the Apache-2.0 license
# Main CMakeLists.txt for Caliptra MCU SW C++ Port

cmake_minimum_required(VERSION 3.16)
project(caliptra_mcu_sw_cpp VERSION 1.0.0 LANGUAGES CXX C ASM)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_RISCV32 "Build for RISC-V 32-bit target" OFF)
option(BUILD_TESTS "Build unit tests" OFF)
option(ENABLE_HW_2_1 "Enable HW 2.1 features" OFF)
option(ENABLE_CORE_TEST "Enable core test features" OFF)

# Common compile options
add_compile_options(
    -Wall
    -Wextra
    -Werror
    -fno-exceptions
    -fno-rtti
)

# Platform-specific settings
if(BUILD_RISCV32)
    set(CMAKE_SYSTEM_NAME Generic)
    set(CMAKE_SYSTEM_PROCESSOR riscv32)
    add_compile_definitions(TARGET_RISCV32)
    # Disable libstdc++ assertions (bounds checking in std::array, std::optional etc.)
    # that pull in __glibcxx_assert_fail at link time in GCC 15+
    add_compile_definitions(_GLIBCXX_ASSERTIONS=0)
    # VeeR EL2 with Zicsr (CSR access) and Zbkc (carryless multiply for CRC)
    add_compile_options(
        -march=rv32imc_zicsr_zbkc
        -mabi=ilp32
        -nostdlib
        -ffreestanding
    )
else()
    add_compile_definitions(TARGET_HOST)
endif()

# Feature flags
if(ENABLE_HW_2_1)
    add_compile_definitions(HW_2_1)
endif()

if(ENABLE_CORE_TEST)
    add_compile_definitions(CORE_TEST)
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Subdirectories
add_subdirectory(romtime)
add_subdirectory(rom)
add_subdirectory(runtime)
add_subdirectory(platforms)

# Install targets
install(DIRECTORY include/ DESTINATION include)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/caliptra_mcu_sw_cpp-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
