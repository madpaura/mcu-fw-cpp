# Build Instructions

This document provides a summary of how to build the Caliptra MCU Software C++ port.

## Prerequisites

Before building, ensure you have the following tools installed:

- **CMake** (version 3.16 or higher)
- **GCC/Clang** with C++20 support
- **RISC-V Toolchain** (GCC 15+ with prefix `riscv32-unknown-elf-`)
- **Rust / Cargo** (latest stable version)

### RISC-V Toolchain Installation

The project expects the RISC-V 32-bit bare-metal toolchain at `/opt/riscv/bin`. To install:

```bash
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv32imc_zicsr_zbkc --with-abi=ilp32
sudo make -j$(nproc)
```

### Building Rust Dependencies

Before building the C++ ROM, you must build the Rust-based emulator and Caliptra components from the repository root:

```bash
# From the repo root (caliptra-mcu-sw/)
cargo build                  # Builds the emulator binary → target/debug/emulator
cargo xtask runtime run --help  # Shows all runtime/emulator options
```

## Building the Project

### Host Build

To build for the host system (useful for testing and development):

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### RISC-V Cross-Compilation

To build for the VeeR EL2 RISC-V target:

```bash
mkdir build-riscv && cd build-riscv
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv32-toolchain.cmake \
      -DBUILD_RISCV32=ON \
      ..
make -j$(nproc)
```

## Build Output

After building, the following artifacts are generated:

- **ROM Binary**: `platforms/emulator/rom/mcu-rom-emulator.bin` (Bootable ROM image)
- **ROM ELF**: `platforms/emulator/rom/mcu-rom-emulator.elf` (ELF for debugging)
- **Static Libraries**: `runtime/kernel/*/lib*.a` (Kernel component libraries)

For more detailed information, refer to the [README.md](../README.md) file.