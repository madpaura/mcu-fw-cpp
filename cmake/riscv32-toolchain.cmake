# RISC-V 32-bit Bare Metal Toolchain File
# For Caliptra MCU SW C++ Port
#
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/riscv32-toolchain.cmake ..
#
# Prerequisites:
#   Install riscv32-unknown-elf-gcc toolchain from:
#   - https://github.com/riscv-collab/riscv-gnu-toolchain
#   - Or use prebuilt: https://github.com/riscv-collab/riscv-gnu-toolchain/releases
#
# Set RISCV_TOOLCHAIN_PATH environment variable or edit this file

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Try to find toolchain prefix
if(DEFINED ENV{RISCV_TOOLCHAIN_PATH})
    set(RISCV_TOOLCHAIN_PATH "$ENV{RISCV_TOOLCHAIN_PATH}")
    set(CROSS_COMPILE "${RISCV_TOOLCHAIN_PATH}/bin/riscv32-unknown-elf-")
else()
    # Try common locations
    find_program(RISCV_GCC riscv32-unknown-elf-gcc 
        PATHS 
            /opt/riscv/bin
            /usr/local/bin
            /usr/bin
            $ENV{HOME}/.local/bin
    )
    if(RISCV_GCC)
        get_filename_component(RISCV_BIN_DIR ${RISCV_GCC} DIRECTORY)
        set(CROSS_COMPILE "${RISCV_BIN_DIR}/riscv32-unknown-elf-")
    else()
        set(CROSS_COMPILE "riscv32-unknown-elf-")
    endif()
endif()

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++)
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc)
set(CMAKE_AR ${CROSS_COMPILE}ar)
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy)
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump)
set(CMAKE_SIZE ${CROSS_COMPILE}size)

# Disable compiler checks (cross-compilation)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Architecture flags for VeeR EL2 (RV32IMC with extensions)
# - Zicsr: Control and status register instructions
# - Zbkc: Carryless multiply for CRC
set(CMAKE_C_FLAGS_INIT "-march=rv32imc_zicsr_zbkc -mabi=ilp32")
set(CMAKE_CXX_FLAGS_INIT "-march=rv32imc_zicsr_zbkc -mabi=ilp32")
set(CMAKE_ASM_FLAGS_INIT "-march=rv32imc_zicsr_zbkc -mabi=ilp32")

# No standard libraries for bare metal
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostdlib -nostartfiles")

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Define for code
add_compile_definitions(TARGET_RISCV32)
