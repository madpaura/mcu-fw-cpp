/*++

Licensed under the Apache-2.0 license.

File Name:

    platform.hpp

Abstract:

    Platform-specific declarations.

--*/

#pragma once

#include <cstdint>

namespace caliptra {
namespace romtime {

/**
 * Platform-specific CRC-8 implementation
 */
std::uint8_t platform_crc8(std::uint8_t crc, std::uint8_t data);

/**
 * Platform-specific cycle counter
 */
std::uint64_t platform_mcycle();

#ifdef TARGET_RISCV32

/**
 * Read RISC-V CSR
 */
std::uint32_t read_csr_mcause();
std::uint32_t read_csr_mepc();
std::uint32_t read_csr_mscratch();

#endif

} // namespace romtime
} // namespace caliptra
