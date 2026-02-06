/*++

Licensed under the Apache-2.0 license.

File Name:

    platform_riscv32.cpp

Abstract:

    RISC-V 32-bit platform-specific implementations.

--*/

#include "romtime/platform.hpp"

#ifdef TARGET_RISCV32

namespace caliptra {
namespace romtime {

std::uint8_t platform_crc8(std::uint8_t crc, std::uint8_t data) {
    // CRC-8 with polynomial x^8 + x^2 + x^1 + 1
    // Using RISC-V carry-less multiply instruction for optimization
    constexpr std::uint32_t polynomial = 0x07;
    
    std::uint32_t crc_val = static_cast<std::uint32_t>(crc ^ data);
    std::uint32_t a, b, tmp;

    // Note: This requires Zbc extension (carry-less multiplication)
    // clmul instruction performs carry-less multiplication
    asm volatile(
        "clmul %[a], %[crc], %[poly]\n\t"
        "srli %[tmp], %[a], 8\n\t"
        "clmul %[b], %[tmp], %[poly]"
        : [a] "=r" (a), [b] "=r" (b), [tmp] "=r" (tmp)
        : [crc] "r" (crc_val), [poly] "r" (polynomial)
    );

    return static_cast<std::uint8_t>(a ^ b);
}

std::uint64_t platform_mcycle() {
    std::uint32_t mcycle_low, mcycle_high;
    
    // Read mcycle and mcycleh CSRs
    asm volatile(
        "csrr %[low], mcycle\n\t"
        "csrr %[high], mcycleh"
        : [low] "=r" (mcycle_low), [high] "=r" (mcycle_high)
    );

    return (static_cast<std::uint64_t>(mcycle_high) << 32) | 
           static_cast<std::uint64_t>(mcycle_low);
}

std::uint32_t read_csr_mcause() {
    std::uint32_t value;
    asm volatile("csrr %0, mcause" : "=r" (value));
    return value;
}

std::uint32_t read_csr_mepc() {
    std::uint32_t value;
    asm volatile("csrr %0, mepc" : "=r" (value));
    return value;
}

std::uint32_t read_csr_mscratch() {
    std::uint32_t value;
    asm volatile("csrr %0, mscratch" : "=r" (value));
    return value;
}

} // namespace romtime
} // namespace caliptra

#endif // TARGET_RISCV32
