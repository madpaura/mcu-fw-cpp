/*++

Licensed under the Apache-2.0 license.

File Name:

    flash_partition.hpp

Abstract:

    Flash partition interface.

--*/

#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include "common/error.hpp"

namespace caliptra {
namespace rom {
namespace flash {

/**
 * Flash partition type
 */
enum class PartitionType {
    Boot,
    Firmware,
    FirmwareBackup,
    Config,
    Log,
    Recovery,
    Raw,
};

/**
 * Flash partition descriptor
 */
struct PartitionDescriptor {
    std::uint32_t start_address;
    std::uint32_t size;
    PartitionType type;
    bool is_writable;
    bool is_secure;
};

/**
 * Flash partition interface
 */
class FlashPartition {
public:
    /**
     * Construct partition with descriptor
     */
    explicit FlashPartition(const PartitionDescriptor& desc);

    virtual ~FlashPartition() = default;

    /**
     * Get partition start address
     */
    std::uint32_t start_address() const { return desc_.start_address; }

    /**
     * Get partition size
     */
    std::uint32_t size() const { return desc_.size; }

    /**
     * Get partition type
     */
    PartitionType type() const { return desc_.type; }

    /**
     * Check if partition is writable
     */
    bool is_writable() const { return desc_.is_writable; }

    /**
     * Check if partition is secure
     */
    bool is_secure() const { return desc_.is_secure; }

    /**
     * Read data from partition
     * @param offset Offset within partition
     * @param buffer Buffer to read into
     * @return Number of bytes read
     */
    virtual error::McuResult<std::size_t> 
        read(std::uint32_t offset, std::span<std::uint8_t> buffer) = 0;

    /**
     * Write data to partition
     * @param offset Offset within partition
     * @param data Data to write
     * @return Number of bytes written
     */
    virtual error::McuResult<std::size_t> 
        write(std::uint32_t offset, std::span<const std::uint8_t> data) = 0;

    /**
     * Erase partition (or region within partition)
     * @param offset Start offset
     * @param length Length to erase
     */
    virtual error::McuResult<void> 
        erase(std::uint32_t offset, std::uint32_t length) = 0;

protected:
    PartitionDescriptor desc_;
};

} // namespace flash
} // namespace rom
} // namespace caliptra
