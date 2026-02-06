/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.hpp

Abstract:

    Main ROM header - common libraries for MCU ROM.
    Equivalent to Rust's rom.rs

--*/

#pragma once

#include <cstdint>
#include <array>
#include <optional>
#include "rom/boot_status.hpp"
#include "rom/cold_boot.hpp"
#include "rom/warm_boot.hpp"
#include "rom/fw_boot.hpp"
#include "rom/fw_hitless_update.hpp"
#include "rom/device_ownership_transfer.hpp"
#include "rom/fuses.hpp"
#include "rom/fuse_layout.hpp"
#include "rom/lifecycle.hpp"
#include "rom/otp.hpp"
#include "rom/rom_env.hpp"
#include "rom/i3c.hpp"
#include "rom/recovery.hpp"
#include "rom/image_verifier.hpp"
#include "rom/flash/flash_partition.hpp"
#include "rom/flash/hil.hpp"
#include "rom/flash/memory.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

// Constants from rom.rs
constexpr std::uint8_t LMS_FUSE_VALUE = 1;
constexpr std::uint8_t MLDSA_FUSE_VALUE = 0;
constexpr std::uint8_t MLDSA_CALIPTRA_VALUE = 1;
constexpr std::uint8_t LMS_CALIPTRA_VALUE = 3;
constexpr std::uint32_t OTP_DAI_IDLE_BIT_OFFSET = 22;
constexpr std::uint32_t OTP_DIRECT_ACCESS_CMD_REG_OFFSET = 0x60;

/// Number of users supported by the MCU MBOX ACL mechanism.
constexpr std::size_t MCU_MBOX_USERS = 5;

/**
 * BootFlow interface - trait for different boot flows
 * Equivalent to Rust's BootFlow trait
 */
class BootFlow {
public:
    virtual ~BootFlow() = default;
    
    /**
     * Execute the boot flow
     * @param env ROM environment
     * @param params ROM parameters
     * @note This function should not return (noreturn)
     */
    [[noreturn]] virtual void run(RomEnv& env, struct RomParameters& params) = 0;
};

/**
 * AXI user configuration
 * Equivalent to Rust's AxiUsers struct
 */
struct AxiUsers {
    std::array<std::optional<std::uint32_t>, 5> mbox_users{};
    std::uint32_t fuse_user = 0;
    std::uint32_t trng_user = 0;
    std::uint32_t dma_user = 0;
    
    AxiUsers() = default;
};

/**
 * MCU mailbox AXI user configuration
 * Used for verification after SS_CONFIG_DONE_STICKY is set
 * Equivalent to Rust's McuMboxAxiUserConfig struct
 */
struct McuMboxAxiUserConfig {
    /// Expected values for MBOX0 valid AXI users (nullopt = not configured)
    std::array<std::optional<std::uint32_t>, MCU_MBOX_USERS> mbox0_users{};
    /// Expected values for MBOX1 valid AXI users (nullopt = not configured)
    std::array<std::optional<std::uint32_t>, MCU_MBOX_USERS> mbox1_users{};
    /// Expected lock status for MBOX0 AXI users
    std::array<bool, MCU_MBOX_USERS> mbox0_locks{};
    /// Expected lock status for MBOX1 AXI users
    std::array<bool, MCU_MBOX_USERS> mbox1_locks{};
    
    McuMboxAxiUserConfig() = default;
};

/**
 * Lifecycle token types
 */
enum class LifecycleToken : std::uint32_t {
    TestUnlock,
    TestExit,
    RmaUnlock,
};

/**
 * Lifecycle hashed tokens
 */
struct LifecycleHashedTokens {
    std::array<std::uint32_t, 4> data{};
};

/**
 * Stable key type for DOT operations
 */
enum class CmStableKeyType : std::uint8_t {
    IDevID = 0,
    LDevID = 1,
};

/**
 * ROM parameters for boot flow configuration
 * Equivalent to Rust's RomParameters struct
 */
struct RomParameters {
    /// Lifecycle transition (state, token) if requested
    std::optional<std::pair<LifecycleControllerState, LifecycleToken>> lifecycle_transition;
    
    /// Lifecycle tokens to burn (if applicable)
    std::optional<LifecycleHashedTokens> burn_lifecycle_tokens;
    
    /// Flash partition driver
    flash::FlashPartition* flash_partition_driver = nullptr;
    
    /// Whether to program field entropy after booting Caliptra runtime firmware
    std::array<bool, 4> program_field_entropy{};
    
    /// MCU image header size
    std::size_t mcu_image_header_size = 0;
    
    /// MCU image verifier
    const ImageVerifier* mcu_image_verifier = nullptr;
    
    /// The stable key type to use for DOT operations
    std::optional<CmStableKeyType> dot_stable_key_type;
    
    /// Flash storage interface for DOT blob
    const flash::FlashHil* dot_flash = nullptr;
    
    /// OTP integrity check enable
    bool otp_enable_integrity_check = false;
    
    /// OTP consistency check enable
    bool otp_enable_consistency_check = false;
    
    /// OTP check timeout override
    std::optional<std::uint32_t> otp_check_timeout_override;
    
    RomParameters() = default;
};

/**
 * SOC interface for Caliptra SoC registers
 * Equivalent to Rust's Soc struct
 */
class Soc {
public:
    explicit Soc(std::uintptr_t base_addr);
    
    /// Check if Caliptra runtime is ready
    bool ready_for_runtime() const;
    
    /// Check if firmware is ready
    bool fw_ready() const;
    
    /// Get flow status register value
    std::uint32_t flow_status() const;
    
    /// Check if ready for mailbox processing
    bool ready_for_mbox() const;
    
    /// Check if ready for fuses
    bool ready_for_fuses() const;
    
    /// Check if Caliptra has fatal error
    bool cptra_fw_fatal_error() const;
    
    /// Set Caliptra WDT configuration
    void set_cptra_wdt_cfg(std::size_t index, std::uint32_t value);
    
    /// Set Caliptra mailbox valid AXI user
    void set_cptra_mbox_valid_axi_user(std::size_t index, std::uint32_t value);
    
    /// Set Caliptra mailbox AXI user lock
    void set_cptra_mbox_axi_user_lock(std::size_t index, std::uint32_t value);
    
    /// Set Caliptra fuse valid AXI user
    void set_cptra_fuse_valid_axi_user(std::uint32_t value);
    
    /// Set Caliptra fuse AXI user lock
    void set_cptra_fuse_axi_user_lock(std::uint32_t value);
    
    /// Set Caliptra TRNG valid AXI user
    void set_cptra_trng_valid_axi_user(std::uint32_t value);
    
    /// Set Caliptra TRNG AXI user lock
    void set_cptra_trng_axi_user_lock(std::uint32_t value);
    
    /// Set SS Caliptra DMA AXI user
    void set_ss_caliptra_dma_axi_user(std::uint32_t value);
    
    /// Populate fuses from OTP to Caliptra
    void populate_fuses(Otp& otp, const romtime::Mci& mci);
    
    /// Set AXI users from configuration
    void set_axi_users(const AxiUsers& users);
    
    /// Signal fuse write is done
    void fuse_write_done();
    
    /// Wait for firmware ready signal
    void wait_for_firmware_ready(const romtime::Mci& mci);

private:
    std::uintptr_t base_addr_;
    
    // Register offsets for SoC
    static constexpr std::size_t CPTRA_FLOW_STATUS = 0x00;
    static constexpr std::size_t SS_GENERIC_FW_EXEC_CTRL_0 = 0x04;
    static constexpr std::size_t CPTRA_FW_ERROR_FATAL = 0x08;
    static constexpr std::size_t CPTRA_WDT_CFG_BASE = 0x0C;
    static constexpr std::size_t CPTRA_MBOX_VALID_AXI_USER_BASE = 0x20;
    static constexpr std::size_t CPTRA_MBOX_AXI_USER_LOCK_BASE = 0x34;
    static constexpr std::size_t CPTRA_FUSE_VALID_AXI_USER = 0x48;
    static constexpr std::size_t CPTRA_FUSE_AXI_USER_LOCK = 0x4C;
    static constexpr std::size_t CPTRA_TRNG_VALID_AXI_USER = 0x50;
    static constexpr std::size_t CPTRA_TRNG_AXI_USER_LOCK = 0x54;
    static constexpr std::size_t SS_CALIPTRA_DMA_AXI_USER = 0x58;
    static constexpr std::size_t CPTRA_FUSE_WR_DONE = 0x5C;
    static constexpr std::size_t FUSE_VENDOR_PK_HASH_BASE = 0x100;
    static constexpr std::size_t FUSE_PQC_KEY_TYPE = 0x140;
    static constexpr std::size_t FUSE_FMC_KEY_MANIFEST_SVN = 0x144;
    static constexpr std::size_t FUSE_RUNTIME_SVN_BASE = 0x148;
    static constexpr std::size_t SS_UDS_SEED_BASE_ADDR_L = 0x200;
    static constexpr std::size_t SS_UDS_SEED_BASE_ADDR_H = 0x204;
    static constexpr std::size_t SS_STRAP_GENERIC_BASE = 0x208;
    
    // Status bits
    static constexpr std::uint32_t FLOW_STATUS_READY_FOR_RUNTIME = 0x01;
    static constexpr std::uint32_t FLOW_STATUS_READY_FOR_MB = 0x02;
    static constexpr std::uint32_t FLOW_STATUS_READY_FOR_FUSES = 0x04;
    static constexpr std::uint32_t FW_EXEC_CTRL_FW_READY = (1u << 2);
    
    // Helper methods
    std::uint32_t read_reg(std::size_t offset) const;
    void write_reg(std::size_t offset, std::uint32_t value);
};

/**
 * Configure MCU mailbox AXI users in MCI
 * @param mci MCI reference
 * @param mbox0_users MBOX0 AXI users from straps
 * @param mbox1_users MBOX1 AXI users from straps
 * @return Configuration for later verification
 */
McuMboxAxiUserConfig configure_mcu_mbox_axi_users(
    romtime::Mci& mci,
    const std::array<std::uint32_t, MCU_MBOX_USERS>& mbox0_users,
    const std::array<std::uint32_t, MCU_MBOX_USERS>& mbox1_users);

/**
 * Verify production debug unlock PK hash
 * @param mci MCI reference
 * @param otp OTP reference
 * @return Result indicating success or error
 */
error::McuResult<void> verify_prod_debug_unlock_pk_hash(
    const romtime::Mci& mci,
    Otp& otp);

/**
 * Verify MCU mailbox AXI user configuration
 * @param mci MCI reference
 * @param expected Expected configuration
 * @return Result indicating success or error
 */
error::McuResult<void> verify_mcu_mbox_axi_users(
    const romtime::Mci& mci,
    const McuMboxAxiUserConfig& expected);

/**
 * Fatal error handler interface
 */
class FatalErrorHandler {
public:
    virtual ~FatalErrorHandler() = default;
    [[noreturn]] virtual void fatal_error(std::uint32_t code) = 0;
};

/**
 * Set the fatal error handler
 * @note The handler must remain valid for the program duration
 */
void set_fatal_error_handler(FatalErrorHandler* handler);

/**
 * Exception handler for RISC-V
 * Should be invoked from trap vector
 */
[[noreturn]] void exception_handler();

/**
 * Panic handler (minimal, for embedded context)
 */
[[noreturn]] void rom_panic();

/**
 * Trigger fatal error with raw code
 */
[[noreturn]] void fatal_error_raw(std::uint32_t code);

/**
 * Trigger fatal error with error enum
 */
[[noreturn]] void fatal_error(error::McuError error);

/**
 * Main ROM entry point
 * Dispatches to appropriate boot flow based on reset reason
 * Equivalent to Rust's rom_start function
 */
[[noreturn]] void rom_start(RomParameters& params);

/**
 * MCU reset reasons
 */
enum class McuResetReason : std::uint32_t {
    ColdBoot = 0x01,
    WarmReset = 0x02,
    FirmwareBootReset = 0x04,
    FirmwareHitlessUpdate = 0x08,
    Invalid = 0xFFFFFFFF,
};

} // namespace rom
} // namespace caliptra
