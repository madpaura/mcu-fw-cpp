/*++

Licensed under the Apache-2.0 license.

File Name:

    recovery.hpp

Abstract:

    Recovery flow handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Recovery handler class
 */
class Recovery {
public:
    /**
     * Execute recovery flow from flash
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute_flash_recovery(RomEnv& env);

    /**
     * Execute recovery flow from I3C
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute_i3c_recovery(RomEnv& env);

private:
    /**
     * Validate recovery image
     */
    static error::McuResult<void> validate_recovery_image(RomEnv& env);

    /**
     * Load recovery image to RAM
     */
    static error::McuResult<void> load_recovery_image(RomEnv& env);
};

} // namespace rom
} // namespace caliptra
