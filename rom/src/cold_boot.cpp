/*++

Licensed under the Apache-2.0 license.

File Name:

    cold_boot.cpp

Abstract:

    Cold boot flow implementation.

--*/

#include "rom/cold_boot.hpp"
#include "rom/boot_status.hpp"
#include "rom/device_ownership_transfer.hpp"
#include "romtime/romtime.hpp"

namespace caliptra {
namespace rom {

error::McuResult<void> ColdBoot::execute(RomEnv& env) {
    // Set boot status
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::ColdBootFlowStarted));
    env.mci().set_flow_milestone(BootMilestones::ROM_STARTED);

    romtime::println("[ROM] Cold boot flow started");

    // Initialize memory map
    auto result = init_memory_map(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::McuMemoryMapInitialized));

    // Load straps
    result = load_straps(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::StrapsLoaded));

    // Initialize peripherals
    result = init_peripherals(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::McuRegistersInitialized));

    // Configure Caliptra
    result = configure_caliptra(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_milestone(BootMilestones::CPTRA_BOOT_GO_ASSERTED);
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::CaliptraBootGoAsserted));

    // Populate fuses to Caliptra
    result = populate_fuses(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_milestone(BootMilestones::CPTRA_FUSES_WRITTEN);
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::FusesPopulatedToCaliptra));

    // Wait for Caliptra ready
    result = wait_caliptra_ready(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::CaliptraReadyForMailbox));

    // Device ownership transfer
    result = device_ownership_transfer(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipTransferComplete));

    // Load firmware
    result = load_firmware(env);
    if (result.is_err()) {
        return result;
    }

    // Mark cold boot complete
    env.mci().set_flow_milestone(BootMilestones::COLD_BOOT_FLOW_COMPLETE);
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::ColdBootFlowComplete));

    romtime::println("[ROM] Cold boot flow complete");

    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::init_memory_map(RomEnv& env) {
    // Memory map initialization
    // Platform-specific: configure memory regions, MPU, etc.
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::load_straps(RomEnv& env) {
    // Load strap configuration
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::init_peripherals(RomEnv& env) {
    // Initialize I3C
    auto result = env.i3c().init();
    if (result.is_err()) {
        return result;
    }

    // Initialize OTP
    result = env.otp().init();
    if (result.is_err()) {
        return result;
    }

    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::configure_caliptra(RomEnv& env) {
    // Assert Caliptra boot go
    env.mci().caliptra_boot_go();
    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::populate_fuses(RomEnv& env) {
    // Populate fuses from OTP to Caliptra
    // This would involve reading fuse values and sending them via mailbox
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::wait_caliptra_ready(RomEnv& env) {
    // Wait for Caliptra to become ready for mailbox commands
    std::uint32_t timeout = 100000;
    while (env.soc_manager().is_mailbox_busy()) {
        if (--timeout == 0) {
            return error::McuResult<void>::Err(error::McuError::TIMEOUT);
        }
        env.soc_manager().delay();
    }
    return error::McuResult<void>::Ok();
}

error::McuResult<void> ColdBoot::device_ownership_transfer(RomEnv& env) {
    // Execute device ownership transfer if needed
    return DeviceOwnershipTransfer::execute(env);
}

error::McuResult<void> ColdBoot::load_firmware(RomEnv& env) {
    // Load and validate firmware
    (void)env;
    return error::McuResult<void>::Ok();
}

} // namespace rom
} // namespace caliptra
