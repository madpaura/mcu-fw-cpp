# Caliptra MCU Software - C++ Port

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://isocpp.org/std/the-standard)

A modern C++20 port of the Caliptra MCU firmware, targeting the VeeR EL2 RISC-V core. This project provides ROM, runtime kernel, and hardware abstraction layers for secure boot and system management.

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Building](#building)
  - [Host Build](#host-build)
  - [RISC-V Cross-Compilation](#risc-v-cross-compilation)
- [Project Structure](#project-structure)
- [Core Components](#core-components)
  - [ROM](#rom)
  - [Runtime Kernel](#runtime-kernel)
  - [ROMTime](#romtime)
  - [Common Utilities](#common-utilities)
- [Running](#running)
  - [Emulator](#emulator)
  - [Debugging](#debugging)
- [Configuration Options](#configuration-options)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

Caliptra MCU SW C++ is a bare-metal firmware implementation designed for secure microcontroller environments. The codebase follows modern C++ idioms while maintaining the constraints required for embedded systems:

- **No exceptions** (`-fno-exceptions`)
- **No RTTI** (`-fno-rtti`)
- **No standard library** (for RISC-V targets)
- **Static memory allocation**
- **Rust-inspired error handling** with `Result<T, E>` types

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        Applications                             │
├─────────────────────────────────────────────────────────────────┤
│                     Runtime Kernel                              │
│  ┌───────────┐  ┌────────────┐  ┌─────────┐  ┌───────────────┐ │
│  │ Capsules  │  │ Components │  │ Drivers │  │  VeeR Support │ │
│  └───────────┘  └────────────┘  └─────────┘  └───────────────┘ │
├─────────────────────────────────────────────────────────────────┤
│                         ROMTime                                 │
│        (Shared code between ROM and Runtime)                    │
├─────────────────────────────────────────────────────────────────┤
│                           ROM                                   │
│  ┌────────────┐  ┌───────────┐  ┌────────────┐  ┌────────────┐ │
│  │ Cold Boot  │  │ Warm Boot │  │  Recovery  │  │ Lifecycle  │ │
│  └────────────┘  └───────────┘  └────────────┘  └────────────┘ │
├─────────────────────────────────────────────────────────────────┤
│                      Hardware (VeeR EL2)                        │
└─────────────────────────────────────────────────────────────────┘
```

## Features

- **Secure Boot Chain**: Cold boot, warm boot, and recovery boot paths
- **Hardware Abstraction**: Drivers for Flash, I3C, MCU Mailbox, DMA, and DoE
- **Mailbox Communication**: Caliptra and MCU mailbox interfaces
- **Lifecycle Management**: Device lifecycle state machine
- **OTP Support**: One-Time Programmable memory access
- **Image Verification**: Firmware image validation and verification
- **Interrupt Handling**: VeeR interrupt controller support
- **MCTP Protocol**: Management Component Transport Protocol support

---

## Prerequisites

### Required Tools

| Tool             | Minimum Version | Purpose           |
| ---------------- | --------------- | ----------------- |
| CMake            | 3.16+           | Build system      |
| GCC/Clang        | C++20 support   | Host compiler     |
| RISC-V Toolchain | GCC 12+         | Cross-compilation |

### RISC-V Toolchain Installation

The project expects the RISC-V toolchain at `/opt/riscv/bin`. Install using:

```bash
# Ubuntu/Debian
sudo apt-get install gcc-riscv64-unknown-elf

# Or build from source (recommended for RV32IMC support)
git clone https://github.com/riscv-collab/riscv-gnu-toolchain
cd riscv-gnu-toolchain
./configure --prefix=/opt/riscv --with-arch=rv32imc --with-abi=ilp32
make -j$(nproc)
```

---

## Building

### Host Build

Build for the host system (for testing and development):

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### RISC-V Cross-Compilation

Build for the VeeR EL2 RISC-V target:

```bash
mkdir build-riscv && cd build-riscv
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv32-toolchain.cmake \
      -DBUILD_RISCV32=ON \
      ..
make -j$(nproc)
```

### Build Output

After building, the following artifacts are generated:

| Artifact         | Path                                          | Description                |
| ---------------- | --------------------------------------------- | -------------------------- |
| ROM Binary       | `platforms/emulator/rom/mcu-rom-emulator.bin` | Bootable ROM image         |
| ROM ELF          | `platforms/emulator/rom/mcu-rom-emulator.elf` | ELF for debugging          |
| Static Libraries | `runtime/kernel/*/lib*.a`                     | Kernel component libraries |

---

## Project Structure

```
mcu-fw-cpp/
├── CMakeLists.txt              # Root build configuration
├── cmake/
│   └── riscv32-toolchain.cmake # RISC-V cross-compilation toolchain
├── include/
│   └── common/                 # Shared headers
│       ├── bitflags.hpp        # Type-safe bitflags
│       ├── error.hpp           # Error codes and McuResult type
│       ├── static_ref.hpp      # Static reference wrapper
│       └── types.hpp           # Basic type aliases and Result<T,E>
├── rom/
│   ├── include/rom/            # ROM-specific headers
│   │   ├── boot_status.hpp     # Boot status definitions
│   │   ├── cold_boot.hpp       # Cold boot implementation
│   │   ├── fuses.hpp           # Fuse reading interface
│   │   ├── lifecycle.hpp       # Lifecycle state machine
│   │   ├── otp.hpp             # OTP access interface
│   │   ├── recovery.hpp        # Recovery boot path
│   │   ├── rom.hpp             # Main ROM entry
│   │   └── warm_boot.hpp       # Warm boot implementation
│   └── src/                    # ROM implementation
├── romtime/
│   ├── include/romtime/        # Shared ROM/runtime headers
│   └── src/                    # Shared implementations
├── runtime/
│   └── kernel/
│       ├── capsules/           # System capsules (high-level abstractions)
│       │   └── include/capsules/
│       │       ├── flash_partition.hpp
│       │       ├── mailbox.hpp
│       │       ├── mbox_sram.hpp
│       │       ├── mci.hpp
│       │       ├── mcu_mbox.hpp
│       │       └── system.hpp
│       ├── components/         # System components (mid-level)
│       │   └── src/
│       │       ├── dma.cpp
│       │       ├── doe.cpp
│       │       ├── mctp_driver.cpp
│       │       └── ...
│       ├── drivers/            # Hardware drivers (low-level)
│       │   ├── doe/            # Data Object Exchange driver
│       │   ├── flash/          # Flash memory driver
│       │   ├── i3c/            # I3C bus driver
│       │   └── mcu_mbox/       # MCU mailbox driver
│       └── veer/               # VeeR EL2 chip support
│           └── include/veer/
│               └── veer.hpp    # Interrupt controller & chip config
└── platforms/
    └── emulator/               # Emulator platform support
        └── rom/
            └── rom.ld          # Linker script
```

---

## Core Components

### ROM

The ROM module implements the secure boot process:

| Component        | Description                           |
| ---------------- | ------------------------------------- |
| `cold_boot`      | Initial boot from power-on reset      |
| `warm_boot`      | Boot after sleep/hibernate            |
| `recovery`       | Recovery mode boot path               |
| `lifecycle`      | Device lifecycle state management     |
| `otp`            | One-Time Programmable memory access   |
| `fuses`          | Hardware fuse reading                 |
| `image_verifier` | Firmware image signature verification |

### Runtime Kernel

The runtime kernel provides the operating environment after boot:

#### Capsules (High-Level Abstractions)
- `FlashPartition` - Flash memory partition management
- `Mailbox` - Caliptra mailbox interface
- `McuMbox` - MCU-specific mailbox
- `MboxSram` - Mailbox SRAM access
- `Mci` - Memory Controller Interface
- `System` - System-level operations

#### Components (Mid-Level Services)
- `DMA` - Direct Memory Access controller
- `DoE` - Data Object Exchange
- `MctpDriver` - MCTP protocol driver
- `MockMctp` / `MuxMctp` - MCTP variants

#### Drivers (Hardware Abstraction)
- `Flash` - Flash memory operations (read/write/erase)
- `I3C` - I3C bus communication
- `McuMbox` - MCU mailbox hardware driver
- `DoE` - DoE hardware driver

#### VeeR Support
- `InterruptController` - Interrupt enable/disable/acknowledge
- `Chip` - Chip-level configuration, watchdog

### ROMTime

Shared code between ROM and runtime environments, including common utilities and interfaces that both stages require.

### Common Utilities

Located in `include/common/`:

```cpp
// Result type (Rust-inspired error handling)
caliptra::common::Result<T, E>  // Success or error
caliptra::error::McuResult<T>   // MCU-specific result with McuError

// Type aliases
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
// ... etc

// Bitflags for type-safe flag operations
BITFLAGS(MyFlags, uint32_t) { FLAG_A = 0x01, FLAG_B = 0x02 };
```

---

## Running

### Emulator

Run the firmware in the Caliptra emulator:

```bash
./target/debug/emulator \
  --rom mcu-fw-cpp/build-riscv/platforms/emulator/rom/mcu-rom-emulator.bin \
  --firmware target/riscv32imc-unknown-none-elf/release/runtime-emulator.bin \
  --caliptra-rom target/caliptra-rom.bin \
  --caliptra-firmware target/caliptra-fw-bundle.bin \
  --soc-manifest target/soc-manifest \
  --rom-offset 0x80000000 \
  --rom-size 0x10000 \
  --dccm-offset 0x50000000 \
  --dccm-size 0x4000 \
  --sram-offset 0x40000000 \
  --sram-size 0x80000 \
  --no-stdin-uart 2>&1
```

### Debugging

Debug using GDB with the emulator's GDB stub:

```bash
# Start emulator with GDB server (port 3333)
# Then connect with GDB:

gdb-multiarch build-riscv/platforms/emulator/rom/mcu-rom-emulator.elf \
  -ex "set architecture riscv:rv32" \
  -ex "target remote localhost:3333" \
  -ex "info registers" \
  -ex "x/10i \$pc"
```

#### Useful GDB Commands

```gdb
# View memory
x/32xw 0x40000000          # View 32 words at SRAM base

# Breakpoints
break caliptra::rom::cold_boot
break *0x80000100          # Break at address

# Step through code
stepi                      # Single instruction
nexti                      # Next instruction (skip calls)
continue                   # Resume execution

# Examine state
info registers             # All registers
p/x $pc                    # Program counter
p/x $sp                    # Stack pointer
```

---

## Configuration Options

CMake options available during configuration:

| Option             | Default | Description                     |
| ------------------ | ------- | ------------------------------- |
| `BUILD_RISCV32`    | `OFF`   | Build for RISC-V 32-bit target  |
| `BUILD_TESTS`      | `OFF`   | Build unit tests                |
| `ENABLE_HW_2_1`    | `OFF`   | Enable HW 2.1 specific features |
| `ENABLE_CORE_TEST` | `OFF`   | Enable core test features       |

### Example Configuration

```bash
cmake -DBUILD_RISCV32=ON \
      -DENABLE_HW_2_1=ON \
      -DCMAKE_BUILD_TYPE=Release \
      ..
```

---

## Error Handling

The codebase uses Rust-inspired `Result` types for error handling:

```cpp
#include <common/error.hpp>

using namespace caliptra::error;

McuResult<uint32_t> read_register(uint32_t addr) {
    if (addr < VALID_BASE || addr > VALID_END) {
        return McuResult<uint32_t>::Err(McuError::INVALID_PARAMETER);
    }
    uint32_t value = *reinterpret_cast<volatile uint32_t*>(addr);
    return McuResult<uint32_t>::Ok(value);
}

// Usage
auto result = read_register(0x40000000);
if (result.is_ok()) {
    uint32_t val = std::move(result).unwrap();
} else {
    // Handle McuError
    McuError err = result.error();
}
```

---

## Memory Map

| Region | Start Address | Size   | Description                 |
| ------ | ------------- | ------ | --------------------------- |
| ROM    | `0x80000000`  | 64 KB  | Boot ROM                    |
| SRAM   | `0x40000000`  | 512 KB | Main memory                 |
| DCCM   | `0x50000000`  | 16 KB  | Data Closely Coupled Memory |

---

## Contributing

1. Follow the existing code style (C++20, no exceptions/RTTI)
2. Use the `caliptra::` namespace hierarchy
3. Document public APIs with Doxygen-style comments
4. Ensure builds pass for both host and RISC-V targets

---

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE) for details.

---

## Related Projects

- [Caliptra RTL](https://github.com/chipsalliance/caliptra-rtl) - Hardware design
- [Caliptra SW](https://github.com/chipsalliance/caliptra-sw) - Original Rust implementation
- [VeeR EL2](https://github.com/chipsalliance/Cores-VeeR-EL2) - RISC-V core
