/*++

Licensed under the Apache-2.0 license.

File Name:

    memory.cpp

Abstract:

    Flash memory implementation.

--*/

#include "rom/flash/memory.hpp"

namespace caliptra {
namespace rom {
namespace flash {

FlashMemory::FlashMemory(FlashHil* hil, std::uint32_t base_address, std::uint32_t size)
    : hil_(hil)
    , base_address_(base_address)
    , size_(size) {}

error::McuResult<std::uint32_t> FlashMemory::read_word(std::uint32_t offset) {
    if (offset + 4 > size_) {
        return error::McuResult<std::uint32_t>::Err(error::McuError::FLASH_INVALID_ADDRESS);
    }

    std::uint8_t buffer[4];
    auto result = hil_->read(base_address_ + offset, std::span<std::uint8_t>(buffer, 4));
    if (result.is_err()) {
        return error::McuResult<std::uint32_t>::Err(result.error());
    }

    // Little-endian word assembly
    std::uint32_t word = 
        static_cast<std::uint32_t>(buffer[0]) |
        (static_cast<std::uint32_t>(buffer[1]) << 8) |
        (static_cast<std::uint32_t>(buffer[2]) << 16) |
        (static_cast<std::uint32_t>(buffer[3]) << 24);

    return error::McuResult<std::uint32_t>::Ok(word);
}

error::McuResult<std::size_t> FlashMemory::read(std::uint32_t offset, 
                                                 std::uint8_t* buffer, 
                                                 std::size_t length) {
    if (offset + length > size_) {
        return error::McuResult<std::size_t>::Err(error::McuError::FLASH_INVALID_ADDRESS);
    }

    return hil_->read(base_address_ + offset, std::span<std::uint8_t>(buffer, length));
}

bool FlashMemory::contains(std::uint32_t address) const {
    return address >= base_address_ && address < base_address_ + size_;
}

std::uint32_t FlashMemory::to_flash_offset(std::uint32_t address) const {
    return address - base_address_;
}

} // namespace flash
} // namespace rom
} // namespace caliptra
