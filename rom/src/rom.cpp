/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.cpp

Abstract:

    Main ROM implementation.
    Equivalent to Rust's rom.rs functionality.

--*/

#include "rom/rom.hpp"
#include "romtime/romtime.hpp"

#ifdef TARGET_RISCV32
#include "romtime/platform.hpp"
#endif

namespace caliptra {
namespace rom {

// =============================================================================
// Global Fatal Error Handler
// =============================================================================

static FatalErrorHandler* g_fatal_error_handler = nullptr;

void set_fatal_error_handler(FatalErrorHandler* handler) {
    g_fatal_error_handler = handler;
}

// =============================================================================
// Exception and Panic Handlers
// =============================================================================

#ifdef TARGET_RISCV32
[[noreturn]] void exception_handler() {
    std::uint32_t mcause = romtime::read_csr_mcause();
    std::uint32_t mepc = romtime::read_csr_mepc();
    std::uint32_t sp = romtime::read_csr_mscratch();
    
    // Get ra from register
    std::uint32_t ra = 0;
    asm volatile("addi %0, ra, 0" : "=r"(ra));

    // Print exception info
    romtime::print("EXCEPTION mcause=");
    romtime::HexWord(mcause).print_to(romtime::g_writer);
    romtime::print(" mepc=");
    romtime::HexWord(mepc).print_to(romtime::g_writer);
    romtime::print(" sp=");
    romtime::HexWord(sp).print_to(romtime::g_writer);
    romtime::print(" ra=");
    romtime::HexWord(ra).print_to(romtime::g_writer);
    romtime::println("");

    fatal_error(error::McuError::GENERIC_EXCEPTION);
}

// Marker function for panic detection
extern "C" void panic_is_possible() {
    asm volatile("");
}
#else
[[noreturn]] void exception_handler() {
    romtime::println("EXCEPTION occurred");
    fatal_error(error::McuError::GENERIC_EXCEPTION);
}
#endif

[[noreturn]] void rom_panic() {
#ifdef TARGET_RISCV32
    panic_is_possible();
#endif
    fatal_error_raw(0);
}

[[noreturn]] void fatal_error_raw(std::uint32_t code) {
    if (g_fatal_error_handler != nullptr) {
        g_fatal_error_handler->fatal_error(code);
    }
    
    // If no handler set, create minimal environment and set error
    RomEnv env;
    env.mci().set_fw_fatal_error(code);
    
    // Loop forever
    while (true) {
#ifdef TARGET_RISCV32
        asm volatile("");
#endif
    }
}

[[noreturn]] void fatal_error(error::McuError error) {
    fatal_error_raw(static_cast<std::uint32_t>(error));
}

// =============================================================================
// Soc Class Implementation
// =============================================================================

Soc::Soc(std::uintptr_t base_addr) : base_addr_(base_addr) {}

std::uint32_t Soc::read_reg(std::size_t offset) const {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    return *reg;
}

void Soc::write_reg(std::size_t offset, std::uint32_t value) {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    *reg = value;
}

bool Soc::ready_for_runtime() const {
    return (read_reg(CPTRA_FLOW_STATUS) & FLOW_STATUS_READY_FOR_RUNTIME) != 0;
}

bool Soc::fw_ready() const {
    return (read_reg(SS_GENERIC_FW_EXEC_CTRL_0) & FW_EXEC_CTRL_FW_READY) != 0;
}

std::uint32_t Soc::flow_status() const {
    return read_reg(CPTRA_FLOW_STATUS);
}

bool Soc::ready_for_mbox() const {
    return (read_reg(CPTRA_FLOW_STATUS) & FLOW_STATUS_READY_FOR_MB) != 0;
}

bool Soc::ready_for_fuses() const {
    return (read_reg(CPTRA_FLOW_STATUS) & FLOW_STATUS_READY_FOR_FUSES) != 0;
}

bool Soc::cptra_fw_fatal_error() const {
    return read_reg(CPTRA_FW_ERROR_FATAL) != 0;
}

void Soc::set_cptra_wdt_cfg(std::size_t index, std::uint32_t value) {
    write_reg(CPTRA_WDT_CFG_BASE + index * 4, value);
}

void Soc::set_cptra_mbox_valid_axi_user(std::size_t index, std::uint32_t value) {
    if (index >= 5) {
        fatal_error(error::McuError::INVALID_PARAMETER);
    }
    write_reg(CPTRA_MBOX_VALID_AXI_USER_BASE + index * 4, value);
}

void Soc::set_cptra_mbox_axi_user_lock(std::size_t index, std::uint32_t value) {
    if (index >= 5) {
        fatal_error(error::McuError::INVALID_PARAMETER);
    }
    write_reg(CPTRA_MBOX_AXI_USER_LOCK_BASE + index * 4, value);
}

void Soc::set_cptra_fuse_valid_axi_user(std::uint32_t value) {
    write_reg(CPTRA_FUSE_VALID_AXI_USER, value);
}

void Soc::set_cptra_fuse_axi_user_lock(std::uint32_t value) {
    write_reg(CPTRA_FUSE_AXI_USER_LOCK, value);
}

void Soc::set_cptra_trng_valid_axi_user(std::uint32_t value) {
    write_reg(CPTRA_TRNG_VALID_AXI_USER, value);
}

void Soc::set_cptra_trng_axi_user_lock(std::uint32_t value) {
    write_reg(CPTRA_TRNG_AXI_USER_LOCK, value);
}

void Soc::set_ss_caliptra_dma_axi_user(std::uint32_t value) {
    write_reg(SS_CALIPTRA_DMA_AXI_USER, value);
}

void Soc::populate_fuses(Otp& otp, const romtime::Mci& mci) {
    // Note: Secret fuses are populated by hardware state machine
    
    // UDS partition base address
    // TODO: Get from registers_generated::fuses::SECRET_MANUF_PARTITION_BYTE_OFFSET
    constexpr std::uint32_t secret_manuf_offset = 0x0;  // Placeholder
    romtime::println("[mcu-fuse-write] Setting UDS/FE base address");
    write_reg(SS_UDS_SEED_BASE_ADDR_L, secret_manuf_offset);
    write_reg(SS_UDS_SEED_BASE_ADDR_H, 0);
    
    // Set DAI idle bit offset and direct access cmd reg offset
    romtime::println("[mcu-fuse-write] Setting DAI idle bit offset and direct access cmd reg offset");
    write_reg(SS_STRAP_GENERIC_BASE, OTP_DAI_IDLE_BIT_OFFSET << 16);
    write_reg(SS_STRAP_GENERIC_BASE + 4, OTP_DIRECT_ACCESS_CMD_REG_OFFSET);
    
    // PQC Key Type
    // TODO: Read from OTP and set appropriately
    // For now, use default MLDSA
    write_reg(FUSE_PQC_KEY_TYPE, MLDSA_CALIPTRA_VALUE);
    romtime::println("[mcu-fuse-write] Setting vendor PQC type");
    
    // FMC Key Manifest SVN
    auto svn_result = otp.read_word(0);  // SVN partition offset
    if (svn_result.is_ok()) {
        write_reg(FUSE_FMC_KEY_MANIFEST_SVN, svn_result.value());
    }
    
    // Note: Additional fuse population (vendor PK hash, runtime SVN, etc.) 
    // would follow the same pattern as the Rust code
    (void)mci;  // Used for MCI register access
}

void Soc::set_axi_users(const AxiUsers& users) {
    // Set mailbox users
    for (std::size_t i = 0; i < users.mbox_users.size(); ++i) {
        if (users.mbox_users[i].has_value()) {
            std::uint32_t user = users.mbox_users[i].value();
            romtime::print("[mcu-rom] Setting Caliptra mailbox user ");
            // TODO: Print index and value
            set_cptra_mbox_valid_axi_user(i, user);
            romtime::println("[mcu-rom] Locking Caliptra mailbox user");
            set_cptra_mbox_axi_user_lock(i, 1);
        }
    }
    
    romtime::println("[mcu-rom] Setting fuse user");
    set_cptra_fuse_valid_axi_user(users.fuse_user);
    romtime::println("[mcu-rom] Locking fuse user");
    set_cptra_fuse_axi_user_lock(1);
    
    romtime::println("[mcu-rom] Setting TRNG user");
    set_cptra_trng_valid_axi_user(users.trng_user);
    romtime::println("[mcu-rom] Locking TRNG user");
    set_cptra_trng_axi_user_lock(1);
    
    romtime::println("[mcu-rom] Setting DMA user");
    set_ss_caliptra_dma_axi_user(users.dma_user);
}

void Soc::fuse_write_done() {
    write_reg(CPTRA_FUSE_WR_DONE, 1);
}

void Soc::wait_for_firmware_ready(const romtime::Mci& mci) {
    // Wait for Caliptra to indicate MCU firmware is ready
    while (!fw_ready()) {
        if (cptra_fw_fatal_error()) {
            romtime::println("[mcu-rom] Caliptra reported a fatal error");
            fatal_error(error::McuError::BOOT_FAILED);
        }
    }
    
    // Clear the reset request interrupt
    // TODO: Modify MCI notification register
    (void)mci;
}

// =============================================================================
// MCU Mailbox Configuration Functions
// =============================================================================

McuMboxAxiUserConfig configure_mcu_mbox_axi_users(
    romtime::Mci& mci,
    const std::array<std::uint32_t, MCU_MBOX_USERS>& mbox0_users,
    const std::array<std::uint32_t, MCU_MBOX_USERS>& mbox1_users) {
    
    McuMboxAxiUserConfig config;
    
    // Configure MBOX0 AXI users
    for (std::size_t i = 0; i < MCU_MBOX_USERS; ++i) {
        if (mbox0_users[i] != 0) {
            romtime::println("[mcu-rom] Setting MCI mailbox 0 user");
            config.mbox0_users[i] = mbox0_users[i];
            config.mbox0_locks[i] = true;
            mci.write_mbox0_valid_axi_user(i, mbox0_users[i]);
            mci.lock_mbox0_axi_user(i);
        }
    }
    
    // Configure MBOX1 AXI users
    for (std::size_t i = 0; i < MCU_MBOX_USERS; ++i) {
        if (mbox1_users[i] != 0) {
            romtime::println("[mcu-rom] Setting MCI mailbox 1 user");
            config.mbox1_users[i] = mbox1_users[i];
            config.mbox1_locks[i] = true;
            mci.write_mbox1_valid_axi_user(i, mbox1_users[i]);
            mci.lock_mbox1_axi_user(i);
        }
    }
    
    return config;
}

// =============================================================================
// Verification Functions
// =============================================================================

error::McuResult<void> verify_prod_debug_unlock_pk_hash(
    const romtime::Mci& mci,
    Otp& otp) {
    
    // Verify length matches: 384 bytes = 96 u32 words
    constexpr std::size_t expected_len = 96;
    std::size_t pk_hash_len = mci.prod_debug_unlock_pk_hash_len();
    
    if (pk_hash_len != expected_len) {
        romtime::println("[mcu-rom] PK hash length mismatch");
        return error::McuResult<void>::Err(error::McuError::OTP_READ_ERROR);
    }
    
    // Compare word-by-word to minimize stack usage
    // Each of the 8 hashes is 48 bytes (12 words)
    bool mismatch = false;
    
    for (std::size_t hash_idx = 0; hash_idx < 8; ++hash_idx) {
        std::uint32_t hash_base_offset = 124 + hash_idx * 48;  // SW_MANUF_PARTITION offset + 124
        
        for (std::size_t word_idx = 0; word_idx < 12; ++word_idx) {
            std::size_t reg_idx = hash_idx * 12 + word_idx;
            
            auto expected_result = otp.read_word(hash_base_offset + word_idx * 4);
            if (expected_result.is_err()) {
                return error::McuResult<void>::Err(error::McuError::OTP_READ_ERROR);
            }
            
            auto actual_result = mci.read_prod_debug_unlock_pk_hash(reg_idx);
            std::uint32_t actual = actual_result.value_or(0);
            
            // Use bitwise OR to accumulate mismatches (constant-time)
            mismatch |= (expected_result.value() != actual);
        }
    }
    
    if (mismatch) {
        romtime::println("[mcu-rom] Prod debug unlock PK hash verification failed");
        return error::McuResult<void>::Err(error::McuError::OTP_READ_ERROR);
    }
    
    romtime::println("[mcu-rom] Prod debug unlock PK hash verification passed");
    return error::McuResult<void>::Ok();
}

error::McuResult<void> verify_mcu_mbox_axi_users(
    const romtime::Mci& mci,
    const McuMboxAxiUserConfig& expected) {
    
    // Verify MBOX0 AXI users and locks
    for (std::size_t i = 0; i < MCU_MBOX_USERS; ++i) {
        // Verify AXI user value if configured
        if (expected.mbox0_users[i].has_value()) {
            std::uint32_t expected_val = expected.mbox0_users[i].value();
            auto actual_result = mci.read_mbox0_valid_axi_user(i);
            std::uint32_t actual_val = actual_result.value_or(0);
            
            if (expected_val != actual_val) {
                romtime::println("[mcu-rom] MCU mailbox 0 user verification failed");
                return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
            }
        }
        
        // Verify lock status
        bool actual_locked = mci.read_mbox0_axi_user_lock(i).value_or(false);
        if (expected.mbox0_locks[i] != actual_locked) {
            romtime::println("[mcu-rom] MCU mailbox 0 user lock verification failed");
            return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
        }
    }
    
    // Verify MBOX1 AXI users and locks
    for (std::size_t i = 0; i < MCU_MBOX_USERS; ++i) {
        if (expected.mbox1_users[i].has_value()) {
            std::uint32_t expected_val = expected.mbox1_users[i].value();
            auto actual_result = mci.read_mbox1_valid_axi_user(i);
            std::uint32_t actual_val = actual_result.value_or(0);
            
            if (expected_val != actual_val) {
                romtime::println("[mcu-rom] MCU mailbox 1 user verification failed");
                return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
            }
        }
        
        bool actual_locked = mci.read_mbox1_axi_user_lock(i).value_or(false);
        if (expected.mbox1_locks[i] != actual_locked) {
            romtime::println("[mcu-rom] MCU mailbox 1 user lock verification failed");
            return error::McuResult<void>::Err(error::McuError::MBOX_INVALID_RESPONSE);
        }
    }
    
    romtime::println("[mcu-rom] MCU mailbox AXI user verification passed");
    return error::McuResult<void>::Ok();
}

// =============================================================================
// ROM Start - Main Entry Point
// =============================================================================

[[noreturn]] void rom_start(RomParameters& params) {
    romtime::println("[mcu-rom] Hello from ROM");
    
    // Create ROM environment with all peripherals
    RomEnv env;
    
    // Store params for later use
    (void)params;  // TODO: Use params in boot flows
    
    // Get MCI reference and set boot milestone
    auto& mci = env.mci();
    mci.set_flow_milestone(BootMilestones::ROM_STARTED);
    
    // Print device lifecycle state
    romtime::print("[mcu-rom] Device lifecycle: ");
    switch (env.lifecycle().current_state()) {
        case LifecycleControllerState::RawState:
            romtime::println("Raw");
            break;
        case LifecycleControllerState::TestUnlocked:
            romtime::println("TestUnlocked");
            break;
        case LifecycleControllerState::Dev:
            romtime::println("Dev");
            break;
        case LifecycleControllerState::Prod:
            romtime::println("Production");
            break;
        case LifecycleControllerState::ProdEnd:
            romtime::println("ProductionEnd");
            break;
        case LifecycleControllerState::Rma:
            romtime::println("RMA");
            break;
        case LifecycleControllerState::Scrap:
            romtime::println("Scrap");
            break;
        default:
            romtime::println("Unknown");
            break;
    }
    
    // Read and print reset reason
    std::uint32_t reset_reason = mci.reset_reason();
    romtime::print("[mcu-rom] MCI RESET_REASON: 0x");
    romtime::HexWord(reset_reason).print_to(romtime::g_writer);
    romtime::println("");
    
    // Handle different reset reasons
    McuResetReason reason = static_cast<McuResetReason>(reset_reason);
    
    switch (reason) {
        case McuResetReason::ColdBoot:
            romtime::println("[mcu-rom] Cold boot detected");
            ColdBoot::execute(env);
            break;
            
        case McuResetReason::WarmReset:
            romtime::println("[mcu-rom] Warm reset detected");
            WarmBoot::execute(env);
            break;
            
        case McuResetReason::FirmwareBootReset:
            romtime::println("[mcu-rom] Firmware boot reset detected");
            FwBoot::execute(env);
            break;
            
        case McuResetReason::FirmwareHitlessUpdate:
            romtime::println("[mcu-rom] Starting firmware hitless update flow");
            FwHitlessUpdate::execute(env);
            break;
            
        default:
            // Invalid - multiple bits set or unknown reason
            romtime::println("[mcu-rom] Invalid reset reason: multiple bits set");
            fatal_error(error::McuError::BOOT_INVALID_RESET_REASON);
    }
    
    // Should never reach here - boot flows are noreturn
    // But we need to satisfy the noreturn attribute
    while (true) {
#ifdef TARGET_RISCV32
        asm volatile("");
#endif
    }
}

} // namespace rom
} // namespace caliptra
