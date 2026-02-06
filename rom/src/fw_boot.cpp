/*++

Licensed under the Apache-2.0 license.

File Name:

    fw_boot.cpp

Abstract:

    Firmware boot flow implementation.

--*/

#include "rom/fw_boot.hpp"
#include "rom/boot_status.hpp"
#include "romtime/romtime.hpp"

namespace caliptra {
namespace rom {

error::McuResult<void> FwBoot::execute(RomEnv& env) {
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::FirmwareBootFlowStarted));

    romtime::println("[ROM] Firmware boot flow started");

    // Validate firmware image
    auto result = validate_firmware(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::FirmwareValidationComplete));

    env.mci().set_flow_milestone(BootMilestones::FIRMWARE_BOOT_FLOW_COMPLETE);
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::FirmwareBootFlowComplete));

    romtime::println("[ROM] Firmware boot flow complete");

    // This would normally jump to firmware, but we return here for flexibility
    return error::McuResult<void>::Ok();
}

error::McuResult<void> FwBoot::validate_firmware(RomEnv& env) {
    // Validate firmware image
    // Check manifest, verify signatures, check SVN, etc.
    (void)env;
    return error::McuResult<void>::Ok();
}

[[noreturn]] void FwBoot::jump_to_firmware(std::uintptr_t entry_point) {
    // Cast to function pointer and call
    using EntryFunc = void (*)();
    EntryFunc entry = reinterpret_cast<EntryFunc>(entry_point);
    
    // Jump to firmware entry point
    entry();
    
    // Should never return
    while (true) {}
}

} // namespace rom
} // namespace caliptra
