/*++

Licensed under the Apache-2.0 license.

File Name:

    i3c.cpp

Abstract:

    I3C controller implementation.

--*/

#include "rom/i3c.hpp"

namespace caliptra {
namespace rom {

I3c::I3c(std::uintptr_t base_addr)
    : base_addr_(base_addr) {}

std::uint32_t I3c::read_reg(std::size_t offset) const {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    return *reg;
}

void I3c::write_reg(std::size_t offset, std::uint32_t value) {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    *reg = value;
}

void I3c::modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value) {
    std::uint32_t current = read_reg(offset);
    write_reg(offset, (current & ~mask) | (value & mask));
}

error::McuResult<void> I3c::init() {
    // Reset FIFOs
    clear_tx_fifo();
    clear_rx_fifo();

    // Enable controller
    enable();

    return error::McuResult<void>::Ok();
}

bool I3c::is_enabled() const {
    return (read_reg(REG_CTRL) & CTRL_ENABLE) != 0;
}

void I3c::enable() {
    modify_reg(REG_CTRL, CTRL_ENABLE, CTRL_ENABLE);
}

void I3c::disable() {
    modify_reg(REG_CTRL, CTRL_ENABLE, 0);
}

void I3c::set_dynamic_address(std::uint8_t addr) {
    write_reg(REG_DYNAMIC_ADDR, static_cast<std::uint32_t>(addr));
}

std::uint8_t I3c::dynamic_address() const {
    return static_cast<std::uint8_t>(read_reg(REG_DYNAMIC_ADDR) & 0x7F);
}

bool I3c::is_transfer_pending() const {
    return (read_reg(REG_STATUS) & STATUS_TRANSFER_PENDING) != 0;
}

I3cTransferResult I3c::write(std::span<const std::uint8_t> data) {
    I3cTransferResult result = {0, true, 0};

    for (std::size_t i = 0; i < data.size(); ++i) {
        // Wait for TX FIFO space
        while (tx_fifo_level() >= 16) {
            // FIFO full, wait
        }

        // Write byte to TX FIFO
        write_reg(REG_TX_FIFO, static_cast<std::uint32_t>(data[i]));
        result.bytes_transferred++;
    }

    return result;
}

I3cTransferResult I3c::read(std::span<std::uint8_t> buffer) {
    I3cTransferResult result = {0, true, 0};

    for (std::size_t i = 0; i < buffer.size(); ++i) {
        // Wait for RX data
        while (rx_fifo_level() == 0) {
            // FIFO empty, wait (add timeout in practice)
        }

        // Read byte from RX FIFO
        buffer[i] = static_cast<std::uint8_t>(read_reg(REG_RX_FIFO) & 0xFF);
        result.bytes_transferred++;
    }

    return result;
}

std::uint32_t I3c::tx_fifo_level() const {
    return read_reg(REG_TX_FIFO_STATUS) & 0x1F;
}

std::uint32_t I3c::rx_fifo_level() const {
    return read_reg(REG_RX_FIFO_STATUS) & 0x1F;
}

void I3c::clear_tx_fifo() {
    modify_reg(REG_CTRL, CTRL_TX_FIFO_RST, CTRL_TX_FIFO_RST);
    // Clear the reset bit
    modify_reg(REG_CTRL, CTRL_TX_FIFO_RST, 0);
}

void I3c::clear_rx_fifo() {
    modify_reg(REG_CTRL, CTRL_RX_FIFO_RST, CTRL_RX_FIFO_RST);
    // Clear the reset bit
    modify_reg(REG_CTRL, CTRL_RX_FIFO_RST, 0);
}

void I3c::handle_interrupt() {
    std::uint32_t intr_status = read_reg(REG_INTR_STATUS);

    // Clear handled interrupts
    write_reg(REG_INTR_STATUS, intr_status);

    // Process interrupts as needed
}

} // namespace rom
} // namespace caliptra
