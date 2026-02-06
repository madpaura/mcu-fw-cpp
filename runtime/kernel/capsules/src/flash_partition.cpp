/*++

Licensed under the Apache-2.0 license.

File Name:

    flash_partition.cpp

Abstract:

    Flash partition capsule implementation.

--*/

#include "capsules/flash_partition.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

FlashPartitionCapsule::FlashPartitionCapsule() {}

error::McuResult<std::size_t> FlashPartitionCapsule::syscall_read(
    std::uint32_t partition_id,
    std::uint32_t offset,
    std::uint8_t* buffer,
    std::size_t length) {
    
    // TODO: Implement flash partition read
    (void)partition_id;
    (void)offset;
    (void)buffer;
    (void)length;
    
    return error::McuResult<std::size_t>::Ok(0);
}

error::McuResult<std::size_t> FlashPartitionCapsule::syscall_write(
    std::uint32_t partition_id,
    std::uint32_t offset,
    const std::uint8_t* data,
    std::size_t length) {
    
    // TODO: Implement flash partition write
    (void)partition_id;
    (void)offset;
    (void)data;
    (void)length;
    
    return error::McuResult<std::size_t>::Ok(0);
}

error::McuResult<void> FlashPartitionCapsule::syscall_erase(
    std::uint32_t partition_id,
    std::uint32_t offset,
    std::uint32_t length) {
    
    // TODO: Implement flash partition erase
    (void)partition_id;
    (void)offset;
    (void)length;
    
    return error::McuResult<void>::Ok();
}

error::McuResult<std::uint32_t> FlashPartitionCapsule::syscall_partition_size(
    std::uint32_t partition_id) {
    
    // TODO: Implement partition size query
    (void)partition_id;
    
    return error::McuResult<std::uint32_t>::Ok(0);
}

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
