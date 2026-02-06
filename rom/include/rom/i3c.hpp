/*++

Licensed under the Apache-2.0 license.

File Name:

    i3c.hpp

Abstract:

    I3C controller interface.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include "common/error.hpp"
#include "common/static_ref.hpp"

namespace caliptra {
namespace rom {

/**
 * I3C transfer direction
 */
enum class I3cDirection {
    Write = 0,
    Read = 1,
};

/**
 * I3C transfer result
 */
struct I3cTransferResult {
    std::size_t bytes_transferred;
    bool success;
    std::uint8_t error_code;
};

/**
 * I3C controller driver
 */
class I3c {
public:
    /**
     * Construct with register base address
     */
    explicit I3c(std::uintptr_t base_addr);

    /**
     * Initialize I3C controller
     */
    error::McuResult<void> init();

    /**
     * Check if controller is enabled
     */
    bool is_enabled() const;

    /**
     * Enable controller
     */
    void enable();

    /**
     * Disable controller
     */
    void disable();

    /**
     * Set dynamic address
     */
    void set_dynamic_address(std::uint8_t addr);

    /**
     * Get dynamic address
     */
    std::uint8_t dynamic_address() const;

    /**
     * Check if transfer is pending
     */
    bool is_transfer_pending() const;

    /**
     * Write data to I3C bus (target mode)
     */
    I3cTransferResult write(std::span<const std::uint8_t> data);

    /**
     * Read data from I3C bus (target mode)
     */
    I3cTransferResult read(std::span<std::uint8_t> buffer);

    /**
     * Get FIFO status
     */
    std::uint32_t tx_fifo_level() const;
    std::uint32_t rx_fifo_level() const;

    /**
     * Clear FIFOs
     */
    void clear_tx_fifo();
    void clear_rx_fifo();

    /**
     * Handle interrupt
     */
    void handle_interrupt();

private:
    std::uintptr_t base_addr_;

    // Register access helpers
    std::uint32_t read_reg(std::size_t offset) const;
    void write_reg(std::size_t offset, std::uint32_t value);
    void modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value);

    // Register offsets (to be defined based on hardware)
    static constexpr std::size_t REG_CTRL = 0x00;
    static constexpr std::size_t REG_STATUS = 0x04;
    static constexpr std::size_t REG_DYNAMIC_ADDR = 0x08;
    static constexpr std::size_t REG_TX_FIFO = 0x10;
    static constexpr std::size_t REG_RX_FIFO = 0x14;
    static constexpr std::size_t REG_TX_FIFO_STATUS = 0x18;
    static constexpr std::size_t REG_RX_FIFO_STATUS = 0x1C;
    static constexpr std::size_t REG_INTR_STATUS = 0x20;
    static constexpr std::size_t REG_INTR_ENABLE = 0x24;

    // Control bits
    static constexpr std::uint32_t CTRL_ENABLE = 0x01;
    static constexpr std::uint32_t CTRL_TX_FIFO_RST = 0x02;
    static constexpr std::uint32_t CTRL_RX_FIFO_RST = 0x04;

    // Status bits
    static constexpr std::uint32_t STATUS_TRANSFER_PENDING = 0x01;
};

} // namespace rom
} // namespace caliptra
