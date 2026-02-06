/*++

Licensed under the Apache-2.0 license.

File Name:

    device_ownership_transfer.hpp

Abstract:

    Device ownership transfer handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Device ownership state
 */
enum class OwnershipState {
    NoOwner,
    OwnerExists,
    TransferPending,
    TransferComplete,
};

/**
 * Device ownership transfer handler
 */
class DeviceOwnershipTransfer {
public:
    /**
     * Execute ownership transfer flow
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute(RomEnv& env);

    /**
     * Read ownership data from flash
     */
    static error::McuResult<void> read_ownership_flash(RomEnv& env);

    /**
     * Derive stable key for ownership
     */
    static error::McuResult<void> derive_stable_key(RomEnv& env);

    /**
     * Burn ownership fuses
     */
    static error::McuResult<void> burn_fuses(RomEnv& env);

    /**
     * Determine current owner
     */
    static error::McuResult<OwnershipState> determine_owner(RomEnv& env);

    /**
     * Complete ownership transfer
     */
    static error::McuResult<void> complete_transfer(RomEnv& env);
};

} // namespace rom
} // namespace caliptra
