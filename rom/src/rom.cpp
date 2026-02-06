/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.cpp

Abstract:

    Main ROM implementation.

--*/

#include "rom/rom.hpp"
#include "romtime/romtime.hpp"

#ifdef TARGET_RISCV32
#include "romtime/platform.hpp"
#endif

namespace caliptra {
namespace rom {

// Global fatal error handler
static FatalErrorHandler* g_fatal_error_handler = nullptr;

void set_fatal_error_handler(FatalErrorHandler* handler) {
    g_fatal_error_handler = handler;
}

#ifdef TARGET_RISCV32
/**
 * Exception handler for RISC-V
 * Called from trap vector
 */
[[noreturn]] void exception_handler() {
    std::uint32_t mcause = romtime::read_csr_mcause();
    std::uint32_t mepc = romtime::read_csr_mepc();
    std::uint32_t sp = romtime::read_csr_mscratch();
    
    // Get ra from register (this is approximate as it depends on calling convention)
    std::uint32_t ra = 0;
    asm volatile("addi %0, ra, 0" : "=r"(ra));

    // Print exception info
    romtime::print("EXCEPTION mcause=");
    romtime::HexWord(mcause).print_to(romtime::g_writer);
    romtime::print(" mepc=");
    romtime::HexWord(mepc).print_to(romtime::g_writer);
    romtime::print(" sp=");
    romtime::HexWord(sp).print_to(romtime::g_writer);
    romtime::print(" ra=");
    romtime::HexWord(ra).print_to(romtime::g_writer);
    romtime::println("");

    fatal_error(error::McuError::GENERIC_EXCEPTION);
}
#else
[[noreturn]] void exception_handler() {
    romtime::println("EXCEPTION occurred");
    fatal_error(error::McuError::GENERIC_EXCEPTION);
}
#endif

// Marker function for panic detection
#ifdef TARGET_RISCV32
extern "C" void panic_is_possible() {
    // This symbol's existence indicates panics are possible
    asm volatile("");
}
#endif

[[noreturn]] void rom_panic() {
#ifdef TARGET_RISCV32
    panic_is_possible();
#endif
    fatal_error_raw(0);
}

[[noreturn]] void fatal_error_raw(std::uint32_t code) {
    if (g_fatal_error_handler != nullptr) {
        g_fatal_error_handler->fatal_error(code);
    }
    
    // If no handler set, create minimal environment and set error
    RomEnv env;
    env.mci().set_fw_fatal_error(code);
    
    // Loop forever
    while (true) {
        asm volatile("");
    }
}

[[noreturn]] void fatal_error(error::McuError error) {
    fatal_error_raw(static_cast<std::uint32_t>(error));
}

} // namespace rom
} // namespace caliptra
