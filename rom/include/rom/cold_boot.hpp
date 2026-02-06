/*++

Licensed under the Apache-2.0 license.

File Name:

    cold_boot.hpp

Abstract:

    Cold boot flow handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Cold boot handler class
 */
class ColdBoot {
public:
    /**
     * Execute cold boot flow
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute(RomEnv& env);

private:
    /**
     * Initialize memory map
     */
    static error::McuResult<void> init_memory_map(RomEnv& env);

    /**
     * Load straps configuration
     */
    static error::McuResult<void> load_straps(RomEnv& env);

    /**
     * Initialize peripherals
     */
    static error::McuResult<void> init_peripherals(RomEnv& env);

    /**
     * Configure Caliptra boot
     */
    static error::McuResult<void> configure_caliptra(RomEnv& env);

    /**
     * Populate fuses to Caliptra
     */
    static error::McuResult<void> populate_fuses(RomEnv& env);

    /**
     * Wait for Caliptra ready
     */
    static error::McuResult<void> wait_caliptra_ready(RomEnv& env);

    /**
     * Execute device ownership transfer if needed
     */
    static error::McuResult<void> device_ownership_transfer(RomEnv& env);

    /**
     * Load and verify firmware
     */
    static error::McuResult<void> load_firmware(RomEnv& env);
};

} // namespace rom
} // namespace caliptra
