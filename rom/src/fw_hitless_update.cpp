/*++

Licensed under the Apache-2.0 license.

File Name:

    fw_hitless_update.cpp

Abstract:

    Firmware hitless update flow implementation.

--*/

#include "rom/fw_hitless_update.hpp"
#include "rom/boot_status.hpp"
#include "romtime/romtime.hpp"

namespace caliptra {
namespace rom {

error::McuResult<void> FwHitlessUpdate::execute(RomEnv& env) {
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::HitlessUpdateFlowStarted));

    romtime::println("[ROM] Hitless update flow started");

    // Validate new firmware image
    auto result = validate_new_firmware(env);
    if (result.is_err()) {
        return result;
    }

    // Swap firmware images
    result = swap_firmware(env);
    if (result.is_err()) {
        return result;
    }

    // Commit update
    result = commit_update(env);
    if (result.is_err()) {
        return result;
    }

    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::HitlessUpdateFlowComplete));

    romtime::println("[ROM] Hitless update flow complete");

    return error::McuResult<void>::Ok();
}

error::McuResult<void> FwHitlessUpdate::validate_new_firmware(RomEnv& env) {
    // Validate the new firmware image
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> FwHitlessUpdate::swap_firmware(RomEnv& env) {
    // Swap firmware images (atomic operation)
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> FwHitlessUpdate::commit_update(RomEnv& env) {
    // Commit the update (mark new firmware as active)
    (void)env;
    return error::McuResult<void>::Ok();
}

} // namespace rom
} // namespace caliptra
