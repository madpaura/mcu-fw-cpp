/*++

Licensed under the Apache-2.0 license.

File Name:

    flash_partition.cpp

Abstract:

    Flash partition implementation.

--*/

#include "rom/flash/flash_partition.hpp"

namespace caliptra {
namespace rom {
namespace flash {

FlashPartition::FlashPartition(const PartitionDescriptor& desc)
    : desc_(desc) {}

} // namespace flash
} // namespace rom
} // namespace caliptra
