/*++

Licensed under the Apache-2.0 license.

File Name:

    memory.hpp

Abstract:

    Flash memory mapping utilities.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include "common/error.hpp"
#include "rom/flash/hil.hpp"

namespace caliptra {
namespace rom {
namespace flash {

/**
 * Memory-mapped flash region
 */
class FlashMemory {
public:
    /**
     * Construct with flash HIL and address mapping
     */
    FlashMemory(FlashHil* hil, std::uint32_t base_address, std::uint32_t size);

    virtual ~FlashMemory() = default;

    /**
     * Get base address
     */
    std::uint32_t base_address() const { return base_address_; }

    /**
     * Get size
     */
    std::uint32_t size() const { return size_; }

    /**
     * Read a word from flash
     */
    error::McuResult<std::uint32_t> read_word(std::uint32_t offset);

    /**
     * Read bytes from flash
     */
    error::McuResult<std::size_t> read(std::uint32_t offset, 
                                       std::uint8_t* buffer, 
                                       std::size_t length);

    /**
     * Check if address is within this region
     */
    bool contains(std::uint32_t address) const;

    /**
     * Translate address to flash offset
     */
    std::uint32_t to_flash_offset(std::uint32_t address) const;

protected:
    FlashHil* hil_;
    std::uint32_t base_address_;
    std::uint32_t size_;
};

} // namespace flash
} // namespace rom
} // namespace caliptra
