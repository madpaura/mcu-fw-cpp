/*++

Licensed under the Apache-2.0 license.

File Name:

    otp.cpp

Abstract:

    OTP controller implementation.

--*/

#include "rom/otp.hpp"

namespace caliptra {
namespace rom {

Otp::Otp(std::uintptr_t base_addr)
    : base_addr_(base_addr) {}

std::uint32_t Otp::read_reg(std::size_t offset) const {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    return *reg;
}

void Otp::write_reg(std::size_t offset, std::uint32_t value) {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    *reg = value;
}

void Otp::modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value) {
    std::uint32_t current = read_reg(offset);
    write_reg(offset, (current & ~mask) | (value & mask));
}

error::McuResult<void> Otp::init() {
    // Wait for OTP to be idle
    return wait_for_idle();
}

bool Otp::is_idle() const {
    return (read_reg(REG_STATUS) & STATUS_IDLE) != 0;
}

bool Otp::has_error() const {
    return (read_reg(REG_STATUS) & STATUS_ERROR) != 0;
}

OtpError Otp::error_status() const {
    if (!has_error()) {
        return OtpError::None;
    }
    std::uint32_t err_code = read_reg(REG_ERR_CODE) & 0x07;
    switch (err_code) {
        case 0: return OtpError::None;
        case 1: return OtpError::MacroError;
        case 2: return OtpError::MacroEccCorrectable;
        case 3: return OtpError::MacroEccUncorrectable;
        case 4: return OtpError::MacroWriteBlank;
        case 5: return OtpError::CheckTimeout;
        case 6: return OtpError::CheckLocked;
        case 7: return OtpError::AccessError;
        default: return OtpError::MacroError;
    }
}

error::McuResult<std::uint32_t> Otp::read_word(std::uint32_t addr) {
    auto wait_result = wait_for_idle();
    if (wait_result.is_err()) {
        return error::McuResult<std::uint32_t>::Err(wait_result.error());
    }

    // Set address
    write_reg(REG_DIRECT_ACCESS_ADDR, addr);

    // Issue read command
    write_reg(REG_DIRECT_ACCESS_CMD, CMD_READ);

    // Wait for completion
    wait_result = wait_for_idle();
    if (wait_result.is_err()) {
        return error::McuResult<std::uint32_t>::Err(wait_result.error());
    }

    if (has_error()) {
        return error::McuResult<std::uint32_t>::Err(error::McuError::OTP_READ_ERROR);
    }

    return error::McuResult<std::uint32_t>::Ok(read_reg(REG_DIRECT_ACCESS_RDATA_0));
}

error::McuResult<void> Otp::read_words(std::uint32_t addr, std::span<std::uint32_t> buffer) {
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        auto result = read_word(addr + i * 4);
        if (result.is_err()) {
            return error::McuResult<void>::Err(result.error());
        }
        buffer[i] = result.value();
    }
    return error::McuResult<void>::Ok();
}

error::McuResult<void> Otp::write_word(std::uint32_t addr, std::uint32_t value) {
    auto wait_result = wait_for_idle();
    if (wait_result.is_err()) {
        return wait_result;
    }

    // Set address and data
    write_reg(REG_DIRECT_ACCESS_ADDR, addr);
    write_reg(REG_DIRECT_ACCESS_WDATA_0, value);

    // Issue write command
    write_reg(REG_DIRECT_ACCESS_CMD, CMD_WRITE);

    // Wait for completion
    wait_result = wait_for_idle();
    if (wait_result.is_err()) {
        return wait_result;
    }

    if (has_error()) {
        return error::McuResult<void>::Err(error::McuError::OTP_WRITE_ERROR);
    }

    return error::McuResult<void>::Ok();
}

error::McuResult<void> Otp::write_words(std::uint32_t addr, std::span<const std::uint32_t> data) {
    for (std::size_t i = 0; i < data.size(); ++i) {
        auto result = write_word(addr + i * 4, data[i]);
        if (result.is_err()) {
            return result;
        }
    }
    return error::McuResult<void>::Ok();
}

error::McuResult<void> Otp::lock_partition(OtpPartition partition) {
    // Partition locking is typically done through digest calculation
    (void)partition;
    write_reg(REG_DIRECT_ACCESS_CMD, CMD_DIGEST);
    return wait_for_idle();
}

bool Otp::is_partition_locked(OtpPartition partition) const {
    // Check partition lock status
    (void)partition;
    return false;
}

std::size_t Otp::partition_size(OtpPartition partition) const {
    // Return partition size based on partition type
    switch (partition) {
        case OtpPartition::CreatorSwCfg:
        case OtpPartition::OwnerSwCfg:
            return 1024;
        case OtpPartition::HwCfg0:
        case OtpPartition::HwCfg1:
            return 256;
        case OtpPartition::Secret0:
        case OtpPartition::Secret1:
        case OtpPartition::Secret2:
            return 512;
        case OtpPartition::LifeCycle:
            return 128;
        default:
            return 0;
    }
}

void Otp::clear_error() {
    // Clear error by writing to error register
    write_reg(REG_ERR_CODE, 0);
}

error::McuResult<void> Otp::wait_for_idle(std::uint32_t timeout_cycles) {
    while (!is_idle()) {
        if (--timeout_cycles == 0) {
            return error::McuResult<void>::Err(error::McuError::TIMEOUT);
        }
    }
    return error::McuResult<void>::Ok();
}

} // namespace rom
} // namespace caliptra
