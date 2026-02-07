/*++

Licensed under the Apache-2.0 license.

File Name:

    rom_env.cpp

Abstract:

    ROM Environment implementation.

--*/

#include "rom/rom_env.hpp"

namespace caliptra {
namespace rom {

// Memory map matching the emulator configuration (mcu-config-emulator)
McuMemoryMap g_mcu_memory_map = {
    .lc_offset = 0x70000400,   // LC controller
    .otp_offset = 0x70000000,  // OTP controller
    .i3c_offset = 0x20004000,  // I3C controller
    .soc_offset = 0x30030000,  // SoC interface
    .mci_offset = 0x21000000,  // MCI (Main CPU Interface)
    .mbox_offset = 0x30020000, // Mailbox
};

// Default straps (to be overridden by platform)
McuStraps g_mcu_straps = {
    .value = 0,
};

RomEnv::RomEnv()
    : mci_(g_mcu_memory_map.mci_offset)
    , lc_(g_mcu_memory_map.lc_offset)
    , otp_(g_mcu_memory_map.otp_offset)
    , i3c_(g_mcu_memory_map.i3c_offset)
    , soc_manager_(g_mcu_memory_map.soc_offset,
                   g_mcu_memory_map.soc_offset,
                   g_mcu_memory_map.mbox_offset)
    , straps_(g_mcu_straps)
{}

} // namespace rom
} // namespace caliptra
