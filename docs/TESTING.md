# Testing

This document summarizes how to build and exercise tests for the Caliptra
MCU C++ Port. It complements the [README](../README.md), which covers
general build and run instructions.

## Table of Contents

- [Test-Related CMake Options](#test-related-cmake-options)
- [Host Build Testing](#host-build-testing)
- [RISC-V Cross-Compilation Testing](#risc-v-cross-compilation-testing)
- [Emulator-Based Testing](#emulator-based-testing)
- [Testing Considerations](#testing-considerations)

---

## Test-Related CMake Options

Two CMake options control test-related behavior:

| Option             | Default | Description                     |
| ------------------ | ------- | -------------------------------- |
| `BUILD_TESTS`      | `OFF`   | Build unit tests                 |
| `ENABLE_CORE_TEST` | `OFF`   | Enable core test features        |

`ENABLE_CORE_TEST` defines the `CORE_TEST` preprocessor macro (see
`CMakeLists.txt` and `rom/CMakeLists.txt`), which is compiled into the
`rom` library. Use it to gate any core-specific test hooks or diagnostics
in ROM source code.

`BUILD_TESTS` is reserved for enabling a unit test target as the project's
test suite grows. Pass it alongside the other options when configuring
either the host or RISC-V build:

```bash
cmake -DBUILD_TESTS=ON -DENABLE_CORE_TEST=ON ..
```

---

## Host Build Testing

The host build (x86_64) is the fastest way to validate that the codebase
compiles cleanly and to iterate during development, since it avoids the
RISC-V toolchain and emulator round-trip.

```bash
mkdir build && cd build
cmake -DBUILD_TESTS=ON -DENABLE_CORE_TEST=ON ..
make -j$(nproc)
```

This targets `TARGET_HOST` (see `CMakeLists.txt`) and links against the
host's standard library, so it is well suited for quick compilation checks,
static analysis, and any host-runnable unit tests as they are added.

---

## RISC-V Cross-Compilation Testing

Functional and hardware-facing behavior must ultimately be validated on the
VeeR EL2 RISC-V target, since the host build does not exercise the
bare-metal code paths (`-nostdlib`, `-ffreestanding`, no C++ runtime).

```bash
mkdir build-riscv && cd build-riscv
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv32-toolchain.cmake \
      -DBUILD_RISCV32=ON \
      -DBUILD_TESTS=ON \
      -DENABLE_CORE_TEST=ON \
      ..
make -j$(nproc)
```

This produces `platforms/emulator/rom/mcu-rom-emulator.bin` (and the
matching `.elf` for debugging), which is the artifact exercised by the
emulator. The RISC-V toolchain must be installed at `/opt/riscv/bin` with
the `riscv32-unknown-elf-` prefix; see the [README's Prerequisites
section](../README.md#prerequisites) for toolchain build instructions.

---

## Emulator-Based Testing

Since there is no hardware target available in CI or local development,
the Caliptra emulator is the primary way to test end-to-end ROM behavior.
This requires the Rust-based emulator and Caliptra components to be built
first from the repo root (see the README's [Building Rust
Dependencies](../README.md#building-rust-dependencies-emulator--caliptra)
section):

```bash
# From the repo root: caliptra-mcu-sw/
cargo build
```

Then run the compiled RISC-V ROM binary against the emulator:

```bash
# From the repo root: caliptra-mcu-sw/
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
  --no-stdin-uart
```

A successful test run prints the ROM's boot log to the console, e.g.:

```
[mcu-rom] Hello from CPP ROM
[mcu-rom] Device lifecycle: Unprovisioned
[mcu-rom] MCI generic input wires[0]: 00000000
[mcu-rom] MCI generic input wires[1]: 00000000
[mcu-rom] MCI RESET_REASON: 0x00000000
[mcu-rom] Cold boot detected
[ROM] Cold boot flow started

Running Caliptra ROM ...
...
```

The Caliptra ROM stage that follows performs its own Known Answer Tests
(KATs) as part of the boot sequence; a clean run through this stage is a
strong signal that the C++ ROM correctly initialized memory, mailboxes,
and lifecycle state before handing off.

For instruction-level debugging of a failing or unexpected run, attach GDB
to the emulator's GDB stub as described in the README's [Debugging
section](../README.md#debugging).

---

## Testing Considerations

- **No exceptions, no RTTI, no standard library on RISC-V.** Any test
  harness added for the RISC-V target must work within these constraints
  (`-fno-exceptions -fno-rtti -nostdlib -ffreestanding`); host-only test
  frameworks that assume libstdc++/exceptions should be gated behind
  `TARGET_HOST` rather than run against RISC-V builds.
- **GCC 15+ workaround.** `_GLIBCXX_ASSERTIONS=0` is defined for the
  RISC-V build to avoid pulling in `std::__glibcxx_assert_fail()` from
  `std::array`, `std::optional`, and `std::span` at link time. Keep this in
  mind when debugging link errors on a freshly built RISC-V toolchain; see
  the README's [Toolchain Notes](../README.md#toolchain-notes) for details.
- **Host vs. target divergence.** Passing on the host build only confirms
  the code compiles and runs under a hosted C++ runtime; it does not
  validate bare-metal behavior (memory-mapped I/O, linker script layout,
  lack of a C runtime). Treat host builds as a fast pre-check and the
  emulator run as the source of truth.
- **Emulator memory map must match.** The addresses passed to the emulator
  (ROM, SRAM, DCCM, and peripheral base addresses) must match
  `platforms/emulator/config/src/lib.rs` in the Rust emulator and the
  initialization in `rom/src/rom_env.cpp`. A mismatch here manifests as
  silent hangs or faults rather than a build failure, so verify the memory
  map first when a test run misbehaves.
- **Build both targets before submitting changes.** As noted in the
  README's [Contributing](../README.md#contributing) section, ensure
  builds pass for both the host and RISC-V targets before considering a
  change complete.
