/*++

Licensed under the Apache-2.0 license.

File Name:

    platform_host.cpp

Abstract:

    Host platform-specific implementations.

--*/

#include "romtime/platform.hpp"

namespace caliptra {
namespace romtime {

std::uint8_t platform_crc8(std::uint8_t crc, std::uint8_t data) {
    // CRC-8 with polynomial x^8 + x^2 + x^1 + 1
    constexpr std::uint8_t polynomial = 0x07;
    
    crc ^= data;
    for (int i = 0; i < 8; ++i) {
        if ((crc & 0x80) != 0) {
            crc = static_cast<std::uint8_t>((crc << 1) ^ polynomial);
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

std::uint64_t platform_mcycle() {
    // Host placeholder - return 0
    return 0;
}

} // namespace romtime
} // namespace caliptra
