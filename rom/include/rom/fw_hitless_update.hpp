/*++

Licensed under the Apache-2.0 license.

File Name:

    fw_hitless_update.hpp

Abstract:

    Firmware hitless update flow handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Firmware hitless update handler class
 */
class FwHitlessUpdate {
public:
    /**
     * Execute hitless update flow
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute(RomEnv& env);

private:
    /**
     * Validate new firmware image
     */
    static error::McuResult<void> validate_new_firmware(RomEnv& env);

    /**
     * Swap firmware images
     */
    static error::McuResult<void> swap_firmware(RomEnv& env);

    /**
     * Commit update
     */
    static error::McuResult<void> commit_update(RomEnv& env);
};

} // namespace rom
} // namespace caliptra
