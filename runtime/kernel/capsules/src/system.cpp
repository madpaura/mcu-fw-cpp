/*++

Licensed under the Apache-2.0 license.

File Name:

    system.cpp

Abstract:

    System capsule implementation.

--*/

#include "capsules/system.hpp"
#include "romtime/romtime.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

SystemCapsule::SystemCapsule() {}

error::McuResult<std::uint64_t> SystemCapsule::syscall_get_time() {
    return error::McuResult<std::uint64_t>::Ok(romtime::mcycle());
}

error::McuResult<void> SystemCapsule::syscall_delay_us(std::uint32_t us) {
    // Simple busy-wait delay
    // In practice, would use timer or yield
    std::uint64_t start = romtime::mcycle();
    std::uint64_t cycles_to_wait = static_cast<std::uint64_t>(us) * 100; // Assuming ~100MHz
    
    while (romtime::mcycle() - start < cycles_to_wait) {
        // Busy wait
    }
    
    return error::McuResult<void>::Ok();
}

void SystemCapsule::syscall_yield() {
    // TODO: Implement yield to scheduler
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
