/*++

Licensed under the Apache-2.0 license.

File Name:

    mci.cpp

Abstract:

    MCI capsule implementation.

--*/

#include "capsules/mci.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

MciCapsule::MciCapsule() {}

error::McuResult<std::uint32_t> MciCapsule::syscall_get_boot_status() {
    // TODO: Implement boot status query
    return error::McuResult<std::uint32_t>::Ok(0);
}

error::McuResult<std::uint32_t> MciCapsule::syscall_get_flow_status() {
    // TODO: Implement flow status query
    return error::McuResult<std::uint32_t>::Ok(0);
}

error::McuResult<void> MciCapsule::syscall_trigger_reset() {
    // TODO: Implement reset trigger
    return error::McuResult<void>::Ok();
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
