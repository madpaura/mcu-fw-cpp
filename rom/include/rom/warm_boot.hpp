/*++

Licensed under the Apache-2.0 license.

File Name:

    warm_boot.hpp

Abstract:

    Warm boot flow handler.

--*/

#pragma once

#include "rom/rom_env.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Warm boot handler class
 */
class WarmBoot {
public:
    /**
     * Execute warm boot flow
     * @param env ROM environment
     * @return Result indicating success or failure
     */
    static error::McuResult<void> execute(RomEnv& env);

private:
    /**
     * Restore context from saved state
     */
    static error::McuResult<void> restore_context(RomEnv& env);

    /**
     * Verify runtime integrity
     */
    static error::McuResult<void> verify_runtime(RomEnv& env);

    /**
     * Resume runtime execution
     */
    static error::McuResult<void> resume_runtime(RomEnv& env);
};

} // namespace rom
} // namespace caliptra
