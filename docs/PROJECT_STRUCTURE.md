# Project Structure

This document provides an overview of the directory layout and organization of the Caliptra MCU Software C++ port repository.

## Repository Root

```
mcu-fw-cpp/
├── CMakeLists.txt              # Root build configuration
├── cmake/                      # CMake modules and toolchain files
├── docs/                       # Documentation files
├── include/                    # Shared headers and common utilities
├── platforms/                  # Platform-specific support code
├── rom/                        # ROM implementation (secure boot)
├── romtime/                    # Shared code between ROM and runtime
├── runtime/                    # Runtime kernel implementation
└── README.md                   # Main project documentation
```

## Directory Descriptions

### `cmake/`
Contains CMake build system configuration files, including the RISC-V cross-compilation toolchain definition.

### `docs/`
Documentation files that help developers understand and contribute to the project.

### `include/`
Contains shared headers and common utilities used throughout the codebase:
- `common/` - Basic type aliases, error handling (`Result<T,E>`), bitflags, and static reference wrapper

### `platforms/`
Platform-specific support code:
- `emulator/` - Support for running the firmware in the Caliptra emulator environment

### `rom/`
The ROM module implements the secure boot process with these components:
- `include/rom/` - ROM-specific headers (boot status, cold/warm boot, lifecycle, OTP, recovery)
- `src/` - ROM implementation code

### `romtime/`
Shared code used by both ROM and runtime environments:
- `include/romtime/` - Headers for shared functionality
- `src/` - Implementation of shared components

### `runtime/`
The runtime kernel that provides the operating environment after boot:
- `kernel/` - The main kernel implementation with:
  - `capsules/` - High-level system abstractions
  - `components/` - Mid-level system services
  - `drivers/` - Low-level hardware drivers
  - `veer/` - VeeR EL2 chip support (interrupt controller, etc.)

## Build System

The project uses CMake as its build system with support for both host development and RISC-V cross-compilation targets. The build process generates:
- ROM binaries for secure boot
- Runtime kernel libraries
- Emulator-compatible firmware images

## Key Components

1. **ROM** - Implements the secure boot chain including cold boot, warm boot, and recovery paths
2. **Runtime Kernel** - Provides the operating environment with capsules, components, and drivers
3. **ROMTime** - Shared utilities between ROM and runtime environments
4. **Common Utilities** - Basic types, error handling, and helper functions

This structure supports the development of a modern C++20 bare-metal firmware targeting the VeeR EL2 RISC-V core while maintaining embedded system constraints like static memory allocation and no standard library dependencies.