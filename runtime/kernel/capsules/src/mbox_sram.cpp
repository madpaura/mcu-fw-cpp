/*++

Licensed under the Apache-2.0 license.

File Name:

    mbox_sram.cpp

Abstract:

    Mailbox SRAM capsule implementation.

--*/

#include "capsules/mbox_sram.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

MboxSramCapsule::MboxSramCapsule() {}

error::McuResult<std::size_t> MboxSramCapsule::syscall_read(
    std::uint32_t offset,
    std::uint8_t* buffer,
    std::size_t length) {
    
    // TODO: Implement SRAM read
    (void)offset;
    (void)buffer;
    (void)length;
    
    return error::McuResult<std::size_t>::Ok(0);
}

error::McuResult<std::size_t> MboxSramCapsule::syscall_write(
    std::uint32_t offset,
    const std::uint8_t* data,
    std::size_t length) {
    
    // TODO: Implement SRAM write
    (void)offset;
    (void)data;
    (void)length;
    
    return error::McuResult<std::size_t>::Ok(0);
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
