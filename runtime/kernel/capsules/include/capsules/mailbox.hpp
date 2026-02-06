/*++

Licensed under the Apache-2.0 license.

File Name:

    mailbox.hpp

Abstract:

    Mailbox capsule for Caliptra mailbox access.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include "common/error.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

/**
 * Mailbox capsule
 * Provides system call interface for Caliptra mailbox operations
 */
class MailboxCapsule {
public:
    MailboxCapsule();
    ~MailboxCapsule() = default;

    /**
     * Send command via mailbox
     */
    error::McuResult<void> syscall_send_command(std::uint32_t cmd,
                                                 const std::uint32_t* data,
                                                 std::size_t data_len);

    /**
     * Receive response from mailbox
     */
    error::McuResult<std::size_t> syscall_receive_response(std::uint32_t* buffer,
                                                            std::size_t buffer_len);

    /**
     * Check if mailbox is ready
     */
    error::McuResult<bool> syscall_is_ready();
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
