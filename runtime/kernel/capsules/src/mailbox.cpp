/*++

Licensed under the Apache-2.0 license.

File Name:

    mailbox.cpp

Abstract:

    Mailbox capsule implementation.

--*/

#include "capsules/mailbox.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

MailboxCapsule::MailboxCapsule() {}

error::McuResult<void> MailboxCapsule::syscall_send_command(
    std::uint32_t cmd,
    const std::uint32_t* data,
    std::size_t data_len) {
    
    // TODO: Implement mailbox send command
    (void)cmd;
    (void)data;
    (void)data_len;
    
    return error::McuResult<void>::Ok();
}

error::McuResult<std::size_t> MailboxCapsule::syscall_receive_response(
    std::uint32_t* buffer,
    std::size_t buffer_len) {
    
    // TODO: Implement mailbox receive response
    (void)buffer;
    (void)buffer_len;
    
    return error::McuResult<std::size_t>::Ok(0);
}

error::McuResult<bool> MailboxCapsule::syscall_is_ready() {
    // TODO: Implement mailbox ready check
    return error::McuResult<bool>::Ok(true);
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
