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

// Default memory map (to be overridden by platform)
McuMemoryMap g_mcu_memory_map = {
    .lc_offset = 0x30010000,
    .otp_offset = 0x30020000,
    .i3c_offset = 0x30030000,
    .soc_offset = 0x30040000,
    .mci_offset = 0x30050000,
    .mbox_offset = 0x30060000,
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
