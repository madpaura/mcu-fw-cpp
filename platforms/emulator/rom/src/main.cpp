/*++

Licensed under the Apache-2.0 license.

File Name:

    main.cpp

Abstract:

    Main entry point for MCU ROM.
    Equivalent to platforms/emulator/rom/src/main.rs + riscv.rs

--*/

#include <cstdint>
#include "rom/rom.hpp"
#include "romtime/romtime.hpp"

using namespace caliptra;

namespace {

// Memory map configuration (matches emulator)
[[maybe_unused]] constexpr std::uintptr_t DOT_FLASH_ADDR = 0x8100'0000;
[[maybe_unused]] constexpr std::size_t DOT_FLASH_SIZE = 4 * 1024;

// Platform-specific writer for output
class EmulatorWriter : public romtime::Writer {
public:
    void write(const char* str, std::size_t len) override {
        for (std::size_t i = 0; i < len; ++i) {
            write_char(str[i]);
        }
    }
    
    void write_char(char c) override {
#ifdef TARGET_RISCV32
        // Write to emulator console
        volatile std::uint32_t* emu_output = reinterpret_cast<volatile std::uint32_t*>(0x2000'F004);
        *emu_output = static_cast<std::uint32_t>(c);
#else
        // Write to stdout for host builds
        (void)c;
#endif
    }
};

// Global instances
EmulatorWriter g_emulator_writer;

// Fatal error handler for ROM
class RomFatalErrorHandler : public rom::FatalErrorHandler {
public:
    [[noreturn]] void fatal_error(std::uint32_t code) override {
        romtime::print("[mcu-rom] FATAL ERROR: 0x");
        romtime::HexWord(code).print_to(romtime::g_writer);
        romtime::println("");
        
#ifdef TARGET_RISCV32
        // Set fatal error in MCI and halt
        volatile std::uint32_t* mci_fatal = reinterpret_cast<volatile std::uint32_t*>(0x3000'000C);
        *mci_fatal = code;
        
        // Exit emulator
        volatile std::uint32_t* emu_exit = reinterpret_cast<volatile std::uint32_t*>(0x2000'F000);
        *emu_exit = 0;
#endif
        // Infinite loop
        while (true) {
#ifdef TARGET_RISCV32
            asm volatile("");
#endif
        }
    }
};

RomFatalErrorHandler g_fatal_error_handler;

} // anonymous namespace

// Exception handler - called from assembly
extern "C" [[noreturn]] void exception_handler() {
    rom::exception_handler();
}

#ifdef TARGET_RISCV32
// Main entry point for RISC-V - called from startup assembly
extern "C" void main() {
    // Initialize printer
    romtime::set_printer(&g_emulator_writer);
    
    // Initialize fatal error handler
    rom::set_fatal_error_handler(&g_fatal_error_handler);
    
    // Create ROM parameters
    rom::RomParameters params;
    
    // Start ROM execution (noreturn)
    rom::rom_start(params);
}
#else
// For host builds, provide a regular main
int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    
    // Initialize printer
    romtime::set_printer(&g_emulator_writer);
    romtime::println("[mcu-rom] Host build - nop");
    return 0;
}
#endif
