/*++

Licensed under the Apache-2.0 license.

File Name:

    mbox_sram.hpp

Abstract:

    Mailbox SRAM capsule.

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
 * Mailbox SRAM capsule
 * Provides system call interface for mailbox SRAM operations
 */
class MboxSramCapsule {
public:
    MboxSramCapsule();
    ~MboxSramCapsule() = default;

    /**
     * Read from mailbox SRAM
     */
    error::McuResult<std::size_t> syscall_read(std::uint32_t offset,
                                                std::uint8_t* buffer,
                                                std::size_t length);

    /**
     * Write to mailbox SRAM
     */
    error::McuResult<std::size_t> syscall_write(std::uint32_t offset,
                                                 const std::uint8_t* data,
                                                 std::size_t length);
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
