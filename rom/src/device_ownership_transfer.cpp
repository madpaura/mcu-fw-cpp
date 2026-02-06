/*++

Licensed under the Apache-2.0 license.

File Name:

    device_ownership_transfer.cpp

Abstract:

    Device ownership transfer implementation.

--*/

#include "rom/device_ownership_transfer.hpp"
#include "rom/boot_status.hpp"
#include "romtime/romtime.hpp"

namespace caliptra {
namespace rom {

error::McuResult<void> DeviceOwnershipTransfer::execute(RomEnv& env) {
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipTransferStarted));

    romtime::println("[ROM] Device ownership transfer started");

    // Read ownership data from flash
    auto result = read_ownership_flash(env);
    if (result.is_err()) {
        return result;
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipTransferFlashRead));

    // Determine current owner
    auto owner_result = determine_owner(env);
    if (owner_result.is_err()) {
        return error::McuResult<void>::Err(owner_result.error());
    }
    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipDetermineOwner));

    OwnershipState state = owner_result.value();

    // If transfer pending, derive stable key and burn fuses
    if (state == OwnershipState::TransferPending) {
        result = derive_stable_key(env);
        if (result.is_err()) {
            return result;
        }
        env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipDeriveStableKey));

        result = burn_fuses(env);
        if (result.is_err()) {
            return result;
        }
        env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipBurnFuses));

        result = complete_transfer(env);
        if (result.is_err()) {
            return result;
        }
    }

    env.mci().set_flow_checkpoint(static_cast<std::uint16_t>(McuRomBootStatus::DeviceOwnershipTransferComplete));

    romtime::println("[ROM] Device ownership transfer complete");

    return error::McuResult<void>::Ok();
}

error::McuResult<void> DeviceOwnershipTransfer::read_ownership_flash(RomEnv& env) {
    // Read ownership data from flash
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> DeviceOwnershipTransfer::derive_stable_key(RomEnv& env) {
    // Derive stable key for ownership
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<void> DeviceOwnershipTransfer::burn_fuses(RomEnv& env) {
    // Burn ownership fuses
    (void)env;
    return error::McuResult<void>::Ok();
}

error::McuResult<OwnershipState> DeviceOwnershipTransfer::determine_owner(RomEnv& env) {
    // Determine current ownership state
    (void)env;
    return error::McuResult<OwnershipState>::Ok(OwnershipState::NoOwner);
}

error::McuResult<void> DeviceOwnershipTransfer::complete_transfer(RomEnv& env) {
    // Complete ownership transfer
    (void)env;
    return error::McuResult<void>::Ok();
}

} // namespace rom
} // namespace caliptra
