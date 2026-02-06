/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.hpp

Abstract:

    Main ROM header - common libraries for MCU ROM.

--*/

#pragma once

#include "rom/boot_status.hpp"
#include "rom/cold_boot.hpp"
#include "rom/warm_boot.hpp"
#include "rom/fw_boot.hpp"
#include "rom/fw_hitless_update.hpp"
#include "rom/device_ownership_transfer.hpp"
#include "rom/fuses.hpp"
#include "rom/fuse_layout.hpp"
#include "rom/lifecycle.hpp"
#include "rom/otp.hpp"
#include "rom/rom_env.hpp"
#include "rom/i3c.hpp"
#include "rom/recovery.hpp"
#include "rom/image_verifier.hpp"
#include "rom/flash/flash_partition.hpp"
#include "rom/flash/hil.hpp"
#include "rom/flash/memory.hpp"
#include "common/error.hpp"

namespace caliptra {
namespace rom {

/**
 * Fatal error handler interface
 */
class FatalErrorHandler {
public:
    virtual ~FatalErrorHandler() = default;
    [[noreturn]] virtual void fatal_error(std::uint32_t code) = 0;
};

/**
 * Set the fatal error handler
 * @note The handler must remain valid for the program duration
 */
void set_fatal_error_handler(FatalErrorHandler* handler);

/**
 * Exception handler for RISC-V
 * Should be invoked from trap vector
 */
[[noreturn]] void exception_handler();

/**
 * Panic handler (minimal, for embedded context)
 */
[[noreturn]] void rom_panic();

/**
 * Trigger fatal error with raw code
 */
[[noreturn]] void fatal_error_raw(std::uint32_t code);

/**
 * Trigger fatal error with error enum
 */
[[noreturn]] void fatal_error(error::McuError error);

} // namespace rom
} // namespace caliptra
