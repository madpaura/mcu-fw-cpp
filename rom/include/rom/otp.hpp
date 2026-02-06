/*++

Licensed under the Apache-2.0 license.

File Name:

    otp.hpp

Abstract:

    OTP (One-Time Programmable) controller interface.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <optional>
#include <span>
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * OTP partition identifiers
 */
enum class OtpPartition : std::uint32_t {
    CreatorSwCfg = 0,
    OwnerSwCfg = 1,
    RotCreatorAuthCodesign = 2,
    RotCreatorAuthState = 3,
    HwCfg0 = 4,
    HwCfg1 = 5,
    Secret0 = 6,
    Secret1 = 7,
    Secret2 = 8,
    LifeCycle = 9,
};

/**
 * OTP error types
 */
enum class OtpError {
    None = 0,
    MacroError,
    MacroEccCorrectable,
    MacroEccUncorrectable,
    MacroWriteBlank,
    CheckTimeout,
    CheckLocked,
    AccessError,
};

/**
 * OTP controller driver
 */
class Otp {
public:
    /**
     * Construct with register base address
     */
    explicit Otp(std::uintptr_t base_addr);

    /**
     * Initialize OTP controller
     */
    error::McuResult<void> init();

    /**
     * Check if OTP controller is idle
     */
    bool is_idle() const;

    /**
     * Check if OTP has errors
     */
    bool has_error() const;

    /**
     * Get current error status
     */
    OtpError error_status() const;

    /**
     * Read a word from OTP at given address
     */
    error::McuResult<std::uint32_t> read_word(std::uint32_t addr);

    /**
     * Read multiple words from OTP
     */
    error::McuResult<void> read_words(std::uint32_t addr, std::span<std::uint32_t> buffer);

    /**
     * Write a word to OTP at given address
     */
    error::McuResult<void> write_word(std::uint32_t addr, std::uint32_t value);

    /**
     * Write multiple words to OTP
     */
    error::McuResult<void> write_words(std::uint32_t addr, std::span<const std::uint32_t> data);

    /**
     * Lock a partition
     */
    error::McuResult<void> lock_partition(OtpPartition partition);

    /**
     * Check if partition is locked
     */
    bool is_partition_locked(OtpPartition partition) const;

    /**
     * Get partition size in words
     */
    std::size_t partition_size(OtpPartition partition) const;

    /**
     * Clear error status
     */
    void clear_error();

    /**
     * Wait for OTP operation to complete
     */
    error::McuResult<void> wait_for_idle(std::uint32_t timeout_cycles = 10000);

private:
    std::uintptr_t base_addr_;

    // Register access helpers
    std::uint32_t read_reg(std::size_t offset) const;
    void write_reg(std::size_t offset, std::uint32_t value);
    void modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value);

    // Register offsets
    static constexpr std::size_t REG_STATUS = 0x00;
    static constexpr std::size_t REG_ERR_CODE = 0x04;
    static constexpr std::size_t REG_DIRECT_ACCESS_CMD = 0x08;
    static constexpr std::size_t REG_DIRECT_ACCESS_ADDR = 0x0C;
    static constexpr std::size_t REG_DIRECT_ACCESS_WDATA_0 = 0x10;
    static constexpr std::size_t REG_DIRECT_ACCESS_WDATA_1 = 0x14;
    static constexpr std::size_t REG_DIRECT_ACCESS_RDATA_0 = 0x18;
    static constexpr std::size_t REG_DIRECT_ACCESS_RDATA_1 = 0x1C;
    static constexpr std::size_t REG_CHECK_TRIGGER = 0x20;
    static constexpr std::size_t REG_CHECK_PENDING = 0x24;
    static constexpr std::size_t REG_INTEGRITY_CHECK = 0x28;
    static constexpr std::size_t REG_CONSISTENCY_CHECK = 0x2C;

    // Command values
    static constexpr std::uint32_t CMD_READ = 0x01;
    static constexpr std::uint32_t CMD_WRITE = 0x02;
    static constexpr std::uint32_t CMD_DIGEST = 0x04;

    // Status bits
    static constexpr std::uint32_t STATUS_IDLE = 0x01;
    static constexpr std::uint32_t STATUS_ERROR = 0x02;
};

} // namespace rom
} // namespace caliptra
