/*++

Licensed under the Apache-2.0 license.

File Name:

    rom_env.hpp

Abstract:

    ROM Environment - Encapsulates all peripherals and managers used by ROM.

--*/

#pragma once

#include <cstdint>
#include "romtime/mci.hpp"
#include "romtime/soc_manager.hpp"
#include "romtime/static_ref.hpp"
#include "rom/lifecycle.hpp"
#include "rom/otp.hpp"
#include "rom/i3c.hpp"

namespace caliptra {
namespace rom {

// Forward declaration for Soc (full definition elsewhere)
class Soc;

/**
 * MCU straps configuration
 */
struct McuStraps {
    std::uint32_t value;
    // Add strap bit definitions as needed
};

/**
 * MCU memory map configuration
 */
struct McuMemoryMap {
    std::uint32_t lc_offset;
    std::uint32_t otp_offset;
    std::uint32_t i3c_offset;
    std::uint32_t soc_offset;
    std::uint32_t mci_offset;
    std::uint32_t mbox_offset;
};

// External declarations (to be provided by platform)
extern McuMemoryMap g_mcu_memory_map;
extern McuStraps g_mcu_straps;

/**
 * ROM Environment containing all peripherals and managers
 */
class RomEnv {
public:
    /**
     * Create a new ROM environment with all peripherals initialized
     */
    RomEnv();

    /**
     * Default destructor
     */
    ~RomEnv() = default;

    // Non-copyable
    RomEnv(const RomEnv&) = delete;
    RomEnv& operator=(const RomEnv&) = delete;

    // Movable
    RomEnv(RomEnv&&) = default;
    RomEnv& operator=(RomEnv&&) = default;

    /**
     * Get MCI instance
     */
    romtime::Mci& mci() { return mci_; }
    const romtime::Mci& mci() const { return mci_; }

    /**
     * Get Lifecycle controller instance
     */
    Lifecycle& lc() { return lc_; }
    const Lifecycle& lc() const { return lc_; }
    
    /**
     * Get Lifecycle controller (alias for lc())
     */
    Lifecycle& lifecycle() { return lc_; }
    const Lifecycle& lifecycle() const { return lc_; }

    /**
     * Get OTP controller instance
     */
    Otp& otp() { return otp_; }
    const Otp& otp() const { return otp_; }

    /**
     * Get I3C instance
     */
    I3c& i3c() { return i3c_; }
    const I3c& i3c() const { return i3c_; }

    /**
     * Get Caliptra SoC manager
     */
    romtime::CaliptraSoC& soc_manager() { return soc_manager_; }
    const romtime::CaliptraSoC& soc_manager() const { return soc_manager_; }

    /**
     * Get straps reference
     */
    const McuStraps& straps() const { return straps_; }

private:
    romtime::Mci mci_;
    Lifecycle lc_;
    Otp otp_;
    I3c i3c_;
    romtime::CaliptraSoC soc_manager_;
    McuStraps straps_;
};

} // namespace rom
} // namespace caliptra
