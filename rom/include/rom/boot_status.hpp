/*++

Licensed under the Apache-2.0 license.

File Name:

    boot_status.hpp

Abstract:

    MCU ROM boot status codes.

--*/

#pragma once

#include <cstdint>
#include "common/bitflags.hpp"

namespace caliptra {
namespace rom {

// Base values for status code ranges
constexpr std::uint16_t ROM_INITIALIZATION_BASE = 1;
constexpr std::uint16_t LIFECYCLE_MANAGEMENT_BASE = 65;
constexpr std::uint16_t OTP_FUSE_OPERATIONS_BASE = 129;
constexpr std::uint16_t CALIPTRA_SETUP_BASE = 193;
constexpr std::uint16_t FIRMWARE_LOADING_BASE = 257;
constexpr std::uint16_t FIELD_ENTROPY_BASE = 321;
constexpr std::uint16_t BOOT_FLOW_BASE = 385;

/**
 * Status codes used by MCU ROM to log boot progress.
 */
enum class McuRomBootStatus : std::uint16_t {
    // ROM Initialization Statuses
    RomStarted = ROM_INITIALIZATION_BASE,
    McuMemoryMapInitialized = ROM_INITIALIZATION_BASE + 1,
    StrapsLoaded = ROM_INITIALIZATION_BASE + 2,
    McuRegistersInitialized = ROM_INITIALIZATION_BASE + 3,
    SocManagerInitialized = ROM_INITIALIZATION_BASE + 4,
    MciInitialized = ROM_INITIALIZATION_BASE + 5,
    ResetReasonDetected = ROM_INITIALIZATION_BASE + 6,

    // Lifecycle Management Statuses
    LifecycleControllerInitialized = LIFECYCLE_MANAGEMENT_BASE,
    LifecycleTransitionStarted = LIFECYCLE_MANAGEMENT_BASE + 1,
    LifecycleTransitionComplete = LIFECYCLE_MANAGEMENT_BASE + 2,
    LifecycleTokenBurningStarted = LIFECYCLE_MANAGEMENT_BASE + 3,
    LifecycleTokenBurningComplete = LIFECYCLE_MANAGEMENT_BASE + 4,

    // OTP and Fuse Operations
    OtpControllerInitialized = OTP_FUSE_OPERATIONS_BASE,
    WatchdogConfigured = OTP_FUSE_OPERATIONS_BASE + 1,

    // Caliptra Setup Statuses
    CaliptraBootGoAsserted = CALIPTRA_SETUP_BASE,
    I3cInitialized = CALIPTRA_SETUP_BASE + 1,
    CaliptraReadyForFuses = CALIPTRA_SETUP_BASE + 2,
    AxiUsersConfigured = CALIPTRA_SETUP_BASE + 3,
    FusesPopulatedToCaliptra = CALIPTRA_SETUP_BASE + 4,
    McuMboxAxiUsersConfigured = CALIPTRA_SETUP_BASE + 5,
    SsConfigDoneStickySet = CALIPTRA_SETUP_BASE + 6,
    SsConfigDoneSet = CALIPTRA_SETUP_BASE + 7,
    PkHashVerified = CALIPTRA_SETUP_BASE + 8,
    McuMboxAxiUsersVerified = CALIPTRA_SETUP_BASE + 9,
    FuseWriteComplete = CALIPTRA_SETUP_BASE + 10,
    CaliptraReadyForMailbox = CALIPTRA_SETUP_BASE + 11,
    DeviceOwnershipTransferFlashRead = CALIPTRA_SETUP_BASE + 12,
    DeviceOwnershipTransferStarted = CALIPTRA_SETUP_BASE + 13,
    DeviceOwnershipDeriveStableKey = CALIPTRA_SETUP_BASE + 14,
    DeviceOwnershipBurnFuses = CALIPTRA_SETUP_BASE + 15,
    DeviceOwnershipDetermineOwner = CALIPTRA_SETUP_BASE + 16,
    DeviceOwnershipTransferComplete = CALIPTRA_SETUP_BASE + 17,

    // Firmware Loading Statuses
    RiDownloadFirmwareCommandSent = FIRMWARE_LOADING_BASE,
    RiDownloadFirmwareComplete = FIRMWARE_LOADING_BASE + 1,
    FlashRecoveryFlowStarted = FIRMWARE_LOADING_BASE + 2,
    FlashRecoveryFlowComplete = FIRMWARE_LOADING_BASE + 3,
    FirmwareReadyDetected = FIRMWARE_LOADING_BASE + 4,
    FirmwareValidationComplete = FIRMWARE_LOADING_BASE + 5,
    CaliptraRuntimeReady = FIRMWARE_LOADING_BASE + 6,

    // Field Entropy Programming
    FieldEntropyProgrammingStarted = FIELD_ENTROPY_BASE,
    FieldEntropyPartition0Complete = FIELD_ENTROPY_BASE + 1,
    FieldEntropyPartition1Complete = FIELD_ENTROPY_BASE + 2,
    FieldEntropyPartition2Complete = FIELD_ENTROPY_BASE + 3,
    FieldEntropyPartition3Complete = FIELD_ENTROPY_BASE + 4,
    FieldEntropyProgrammingComplete = FIELD_ENTROPY_BASE + 5,

    // Boot Flow Completion
    ColdBootFlowStarted = BOOT_FLOW_BASE,
    ColdBootFlowComplete = BOOT_FLOW_BASE + 1,
    WarmResetFlowStarted = BOOT_FLOW_BASE + 2,
    WarmResetFlowComplete = BOOT_FLOW_BASE + 3,
    FirmwareBootFlowStarted = BOOT_FLOW_BASE + 4,
    FirmwareBootFlowComplete = BOOT_FLOW_BASE + 5,
    HitlessUpdateFlowStarted = BOOT_FLOW_BASE + 6,
    HitlessUpdateFlowComplete = BOOT_FLOW_BASE + 7,
};

/**
 * MCU Boot Milestones bitflags
 */
namespace BootMilestones {
    constexpr std::uint16_t ROM_STARTED = 0x0001;
    constexpr std::uint16_t CPTRA_BOOT_GO_ASSERTED = 0x0002;
    constexpr std::uint16_t CPTRA_FUSES_WRITTEN = 0x0004;
    constexpr std::uint16_t RI_DOWNLOAD_COMPLETED = 0x0008;
    constexpr std::uint16_t FLASH_RECOVERY_FLOW_COMPLETED = 0x0010;
    constexpr std::uint16_t COLD_BOOT_FLOW_COMPLETE = 0x0020;
    constexpr std::uint16_t WARM_RESET_FLOW_COMPLETE = 0x0040;
    constexpr std::uint16_t FIRMWARE_BOOT_FLOW_COMPLETE = 0x0080;
}

// Keep class for bitwise operations if needed
class McuBootMilestones {
public:
    constexpr McuBootMilestones() : value_(0) {}
    constexpr explicit McuBootMilestones(std::uint16_t v) : value_(v) {}
    
    constexpr std::uint16_t bits() const { return value_; }
    
    constexpr McuBootMilestones operator|(McuBootMilestones other) const {
        return McuBootMilestones(value_ | other.value_);
    }
    
    constexpr McuBootMilestones operator&(McuBootMilestones other) const {
        return McuBootMilestones(value_ & other.value_);
    }
    
    constexpr bool contains(std::uint16_t flag) const {
        return (value_ & flag) == flag;
    }
    
private:
    std::uint16_t value_;
};

} // namespace rom
} // namespace caliptra
