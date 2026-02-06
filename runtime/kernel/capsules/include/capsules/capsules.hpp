/*++

Licensed under the Apache-2.0 license.

File Name:

    capsules.hpp

Abstract:

    Runtime kernel capsules - main header for capsule implementations.
    Capsules are Tock OS-style system call handlers.

--*/

#pragma once

#include "capsules/flash_partition.hpp"
#include "capsules/mailbox.hpp"
#include "capsules/mbox_sram.hpp"
#include "capsules/mci.hpp"
#include "capsules/mcu_mbox.hpp"
#include "capsules/system.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace capsules {

// Capsules namespace aggregates all capsule implementations

} // namespace capsules
} // namespace kernel
} // namespace runtime
} // namespace caliptra
