/*++

Licensed under the Apache-2.0 license.

File Name:

    fw_boot.hpp

Abstract:

    Firmware boot flow handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Firmware boot handler class
 */
class FwBoot {
public:
    /**
     * Execute firmware boot flow
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute(RomEnv& env);

private:
    /**
     * Validate firmware image
     */
    static error::McuResult<void> validate_firmware(RomEnv& env);

    /**
     * Transfer control to firmware
     */
    [[noreturn]] static void jump_to_firmware(std::uintptr_t entry_point);
};

} // namespace rom
} // namespace caliptra
