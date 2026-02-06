/*++

Licensed under the Apache-2.0 license.

File Name:

    mcu_mbox.cpp

Abstract:

    MCU Mailbox capsule implementation.

--*/

#include "capsules/mcu_mbox.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

McuMboxCapsule::McuMboxCapsule() {}

error::McuResult<void> McuMboxCapsule::syscall_send(
    std::uint32_t target,
    const std::uint8_t* data,
    std::size_t length) {
    
    // TODO: Implement MCU mailbox send
    (void)target;
    (void)data;
    (void)length;
    
    return error::McuResult<void>::Ok();
}

error::McuResult<std::size_t> McuMboxCapsule::syscall_receive(
    std::uint32_t* source,
    std::uint8_t* buffer,
    std::size_t buffer_len) {
    
    // TODO: Implement MCU mailbox receive
    (void)source;
    (void)buffer;
    (void)buffer_len;
    
    return error::McuResult<std::size_t>::Ok(0);
}

error::McuResult<bool> McuMboxCapsule::syscall_has_pending_message() {
    // TODO: Implement pending message check
    return error::McuResult<bool>::Ok(false);
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
