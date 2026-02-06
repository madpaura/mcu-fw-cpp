/*++

Licensed under the Apache-2.0 license.

File Name:

    hil.hpp

Abstract:

    Flash Hardware Interface Layer (HIL).

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include "common/error.hpp"

namespace caliptra {
namespace rom {
namespace flash {

/**
 * Flash operation type
 */
enum class FlashOp {
    Read,
    Write,
    Erase,
};

/**
 * Flash HIL status
 */
struct FlashStatus {
    bool busy;
    bool write_enabled;
    bool error;
    std::uint8_t error_code;
};

/**
 * Flash Hardware Interface Layer
 * Abstract base class for flash controller implementations
 */
class FlashHil {
public:
    virtual ~FlashHil() = default;

    /**
     * Initialize the flash controller
     */
    virtual error::McuResult<void> init() = 0;

    /**
     * Read bytes from flash
     * @param address Flash address
     * @param buffer Buffer to read into
     * @return Number of bytes read
     */
    virtual error::McuResult<std::size_t> 
        read(std::uint32_t address, std::span<std::uint8_t> buffer) = 0;

    /**
     * Write bytes to flash
     * @param address Flash address
     * @param data Data to write
     * @return Number of bytes written
     */
    virtual error::McuResult<std::size_t> 
        write(std::uint32_t address, std::span<const std::uint8_t> data) = 0;

    /**
     * Erase flash region
     * @param address Start address
     * @param length Number of bytes to erase
     */
    virtual error::McuResult<void> 
        erase(std::uint32_t address, std::uint32_t length) = 0;

    /**
     * Get flash status
     */
    virtual FlashStatus status() const = 0;

    /**
     * Wait for operation to complete
     * @param timeout_cycles Maximum cycles to wait
     */
    virtual error::McuResult<void> 
        wait_idle(std::uint32_t timeout_cycles = 100000) = 0;

    /**
     * Get flash capacity in bytes
     */
    virtual std::uint32_t capacity() const = 0;

    /**
     * Get erase block size
     */
    virtual std::uint32_t erase_block_size() const = 0;

    /**
     * Get page size (write granularity)
     */
    virtual std::uint32_t page_size() const = 0;
};

} // namespace flash
} // namespace rom
} // namespace caliptra
