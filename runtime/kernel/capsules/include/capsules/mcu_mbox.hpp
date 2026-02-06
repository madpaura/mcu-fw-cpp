/*++

Licensed under the Apache-2.0 license.

File Name:

    mcu_mbox.hpp

Abstract:

    MCU Mailbox capsule.

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
 * MCU Mailbox capsule
 * Provides system call interface for MCU mailbox operations
 */
class McuMboxCapsule {
public:
    McuMboxCapsule();
    ~McuMboxCapsule() = default;

    /**
     * Send message via MCU mailbox
     */
    error::McuResult<void> syscall_send(std::uint32_t target,
                                         const std::uint8_t* data,
                                         std::size_t length);

    /**
     * Receive message from MCU mailbox
     */
    error::McuResult<std::size_t> syscall_receive(std::uint32_t* source,
                                                   std::uint8_t* buffer,
                                                   std::size_t buffer_len);

    /**
     * Check if message pending
     */
    error::McuResult<bool> syscall_has_pending_message();
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
