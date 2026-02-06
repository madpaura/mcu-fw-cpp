/*++

Licensed under the Apache-2.0 license.

File Name:

    flash_partition.hpp

Abstract:

    Flash partition capsule for userspace access to flash.

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
 * Flash partition capsule
 * Provides system call interface for userspace to access flash partitions
 */
class FlashPartitionCapsule {
public:
    FlashPartitionCapsule();
    ~FlashPartitionCapsule() = default;

    /**
     * Handle read system call
     */
    error::McuResult<std::size_t> syscall_read(std::uint32_t partition_id,
                                                std::uint32_t offset,
                                                std::uint8_t* buffer,
                                                std::size_t length);

    /**
     * Handle write system call
     */
    error::McuResult<std::size_t> syscall_write(std::uint32_t partition_id,
                                                 std::uint32_t offset,
                                                 const std::uint8_t* data,
                                                 std::size_t length);

    /**
     * Handle erase system call
     */
    error::McuResult<void> syscall_erase(std::uint32_t partition_id,
                                          std::uint32_t offset,
                                          std::uint32_t length);

    /**
     * Handle partition info system call
     */
    error::McuResult<std::uint32_t> syscall_partition_size(std::uint32_t partition_id);
};

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
