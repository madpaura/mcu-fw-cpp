/*++

Licensed under the Apache-2.0 license.

File Name:

    mci.hpp

Abstract:

    MCI capsule for MCI access from userspace.

--*/

#pragma once

#include <cstdint>
#include "common/error.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

/**
 * MCI capsule
 * Provides system call interface for MCI operations
 */
class MciCapsule {
public:
    MciCapsule();
    ~MciCapsule() = default;

    /**
     * Get boot status
     */
    error::McuResult<std::uint32_t> syscall_get_boot_status();

    /**
     * Get flow status
     */
    error::McuResult<std::uint32_t> syscall_get_flow_status();

    /**
     * Trigger warm reset
     */
    error::McuResult<void> syscall_trigger_reset();
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
