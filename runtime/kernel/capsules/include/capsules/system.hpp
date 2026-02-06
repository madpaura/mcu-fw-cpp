/*++

Licensed under the Apache-2.0 license.

File Name:

    system.hpp

Abstract:

    System capsule for system-level operations.

--*/

#pragma once

#include <cstdint>
#include "common/error.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

/**
 * System capsule
 * Provides system call interface for system-level operations
 */
class SystemCapsule {
public:
    SystemCapsule();
    ~SystemCapsule() = default;

    /**
     * Get system time
     */
    error::McuResult<std::uint64_t> syscall_get_time();

    /**
     * Delay for specified microseconds
     */
    error::McuResult<void> syscall_delay_us(std::uint32_t us);

    /**
     * Yield to scheduler
     */
    void syscall_yield();
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
