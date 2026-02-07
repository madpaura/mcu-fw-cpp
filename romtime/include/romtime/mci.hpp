/*++

Licensed under the Apache-2.0 license.

File Name:

    mci.hpp

Abstract:

    MCI (MCU Control Interface) driver.

--*/

#pragma once

#include <cstdint>
#include <optional>
#include "common/static_ref.hpp"

namespace caliptra {
namespace romtime {

// Forward declarations for register types
namespace mci_regs {
    struct Mci;
}

/**
 * MCU Reset Reason
 */
enum class McuResetReason {
    /// Cold Boot - Power-on reset (no bits set)
    ColdBoot,
    /// Warm Reset - MCU reset while power maintained
    WarmReset,
    /// Firmware Boot Update - First firmware update after MCI reset
    FirmwareBootReset,
    /// Firmware Hitless Update - Second or later firmware update
    FirmwareHitlessUpdate,
    /// Multiple bits set - invalid state
    Invalid,
};

/**
 * Device Lifecycle State
 */
enum class DeviceLifecycleState : std::uint32_t {
    DeviceUnprovisioned = 0,
    DeviceManufacturing = 1,
    DeviceProvisioned = 2,
    DeviceSecurityLocked = 3,
};

/**
 * MCI (MCU Control Interface) driver
 */
class Mci {
public:
    /**
     * Construct MCI driver with register base address
     */
    explicit Mci(std::uintptr_t base_addr);

    /**
     * Get device lifecycle state
     */
    DeviceLifecycleState device_lifecycle_state() const;

    /**
     * Get raw security state register value
     */
    std::uint32_t security_state() const;

    /**
     * Assert Caliptra boot go
     */
    void caliptra_boot_go();

    /**
     * Set flow status register
     */
    void set_flow_status(std::uint32_t status);

    /**
     * Get flow status register
     */
    std::uint32_t flow_status() const;

    /**
     * Set flow checkpoint (preserves milestone)
     */
    void set_flow_checkpoint(std::uint16_t checkpoint);

    /**
     * Get flow checkpoint
     */
    std::uint16_t flow_checkpoint() const;

    /**
     * Set fatal error code
     */
    void set_fw_fatal_error(std::uint32_t code);

    /**
     * Set flow milestone (OR with current milestones)
     */
    void set_flow_milestone(std::uint16_t milestone);

    /**
     * Get flow milestone
     */
    std::uint16_t flow_milestone() const;

    /**
     * Get hardware flow status
     */
    std::uint32_t hw_flow_status() const;

    /**
     * Set NMI vector
     */
    void set_nmi_vector(std::uint32_t nmi_vector);

    /**
     * Configure watchdog timers
     */
    void configure_wdt(std::uint32_t wdt1_timeout, std::uint32_t wdt2_timeout);

    /**
     * Disable watchdog timer
     */
    void disable_wdt();

    /**
     * Get generic input wires value
     * @param index Wire index (0 or 1)
     * @return Wire value
     */
    std::uint32_t generic_input_wires(std::size_t index) const;

    /**
     * Get raw reset reason register value
     */
    std::uint32_t reset_reason() const;

    /**
     * Get reset reason as enum
     */
    McuResetReason reset_reason_enum() const;

    /**
     * Check if cold reset (power-on reset)
     */
    bool is_cold_reset() const;

    /**
     * Check if warm reset
     */
    bool is_warm_reset() const;

    /**
     * Check if firmware boot update reset
     */
    bool is_fw_boot_update_reset() const;

    /**
     * Check if firmware hitless update reset
     */
    bool is_fw_hitless_update_reset() const;

    /**
     * Read notification 0 interrupt trigger register
     */
    std::uint32_t read_notif0_intr_trig_r() const;

    /**
     * Write notification 0 interrupt trigger register
     */
    void write_notif0_intr_trig_r(std::uint32_t value);

    /**
     * Read WDT timer 1 enable
     */
    std::uint32_t read_wdt_timer1_en() const;

    /**
     * Write WDT timer 1 enable
     */
    void write_wdt_timer1_en(std::uint32_t value);

    /**
     * Handle MCI interrupt
     */
    void handle_interrupt();

    /**
     * Trigger warm reset
     */
    void trigger_warm_reset();

    /**
     * Set SS_CONFIG_DONE_STICKY register
     */
    void set_ss_config_done_sticky();

    /**
     * Check if SS_CONFIG_DONE_STICKY is set
     */
    bool is_ss_config_done_sticky() const;

    /**
     * Set SS_CONFIG_DONE register
     */
    void set_ss_config_done();

    /**
     * Check if SS_CONFIG_DONE is set
     */
    bool is_ss_config_done() const;

    /**
     * Read production debug unlock PK hash
     */
    std::optional<std::uint32_t> read_prod_debug_unlock_pk_hash(std::size_t index) const;

    /**
     * Get length of production debug unlock PK hash array
     */
    std::size_t prod_debug_unlock_pk_hash_len() const;

    /**
     * Read MCU mailbox 0 valid AXI user
     */
    std::optional<std::uint32_t> read_mbox0_valid_axi_user(std::size_t index) const;

    /**
     * Read MCU mailbox 0 AXI user lock
     */
    std::optional<bool> read_mbox0_axi_user_lock(std::size_t index) const;

    /**
     * Read MCU mailbox 1 valid AXI user
     */
    std::optional<std::uint32_t> read_mbox1_valid_axi_user(std::size_t index) const;

    /**
     * Read MCU mailbox 1 AXI user lock
     */
    std::optional<bool> read_mbox1_axi_user_lock(std::size_t index) const;

    /**
     * Get length of mailbox AXI user arrays
     */
    std::size_t mbox_axi_user_len() const;

    /**
     * Write MCU mailbox 0 valid AXI user
     */
    bool write_mbox0_valid_axi_user(std::size_t index, std::uint32_t value);

    /**
     * Lock MCU mailbox 0 AXI user
     */
    bool lock_mbox0_axi_user(std::size_t index);

    /**
     * Write MCU mailbox 1 valid AXI user
     */
    bool write_mbox1_valid_axi_user(std::size_t index, std::uint32_t value);

    /**
     * Lock MCU mailbox 1 AXI user
     */
    bool lock_mbox1_axi_user(std::size_t index);

private:
    std::uintptr_t base_addr_;

    // Register access helpers
    volatile std::uint32_t* reg(std::size_t offset) const;
    std::uint32_t read_reg(std::size_t offset) const;
    void write_reg(std::size_t offset, std::uint32_t value);
    void modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value);

    // Register offsets
    static constexpr std::size_t REG_SECURITY_STATE = 0x00;
    static constexpr std::size_t REG_CPTRA_BOOT_GO = 0x04;
    static constexpr std::size_t REG_FW_FLOW_STATUS = 0x08;
    static constexpr std::size_t REG_FW_ERROR_FATAL = 0x0C;
    static constexpr std::size_t REG_HW_FLOW_STATUS = 0x10;
    static constexpr std::size_t REG_MCU_NMI_VECTOR = 0x14;
    static constexpr std::size_t REG_WDT_TIMER1_TIMEOUT_PERIOD_0 = 0x18;
    static constexpr std::size_t REG_WDT_TIMER1_TIMEOUT_PERIOD_1 = 0x1C;
    static constexpr std::size_t REG_WDT_TIMER2_TIMEOUT_PERIOD_0 = 0x20;
    static constexpr std::size_t REG_WDT_TIMER2_TIMEOUT_PERIOD_1 = 0x24;
    static constexpr std::size_t REG_WDT_TIMER1_CTRL = 0x28;
    static constexpr std::size_t REG_WDT_TIMER1_EN = 0x2C;
    static constexpr std::size_t REG_RESET_REASON = 0x30;
    static constexpr std::size_t REG_RESET_REQUEST = 0x34;
    static constexpr std::size_t REG_SS_CONFIG_DONE_STICKY = 0x38;
    static constexpr std::size_t REG_SS_CONFIG_DONE = 0x3C;
    static constexpr std::size_t REG_PROD_DEBUG_UNLOCK_PK_HASH_BASE = 0x40;
    static constexpr std::size_t REG_MBOX0_VALID_AXI_USER_BASE = 0x80;
    static constexpr std::size_t REG_MBOX0_AXI_USER_LOCK_BASE = 0xA0;
    static constexpr std::size_t REG_MBOX1_VALID_AXI_USER_BASE = 0xC0;
    static constexpr std::size_t REG_MBOX1_AXI_USER_LOCK_BASE = 0xE0;
    static constexpr std::size_t REG_NOTIF0_INTR_TRIG_R = 0x100;
    static constexpr std::size_t REG_NOTIF0_INTERNAL_INTR_R = 0x104;

    // Constants
    static constexpr std::size_t PK_HASH_REG_COUNT = 12;
    static constexpr std::size_t MBOX_AXI_USER_COUNT = 8;

    // Bit masks
    static constexpr std::uint32_t RESET_REASON_WARM_RESET = 0x01;
    static constexpr std::uint32_t RESET_REASON_FW_BOOT_UPD_RESET = 0x02;
    static constexpr std::uint32_t RESET_REASON_FW_HITLESS_UPD_RESET = 0x04;
    static constexpr std::uint32_t SS_CONFIG_DONE_BIT = 0x01;
    static constexpr std::uint32_t NOTIF_CPTRA_MCU_RESET_REQ_STS_MASK = 0x02;
    static constexpr std::uint32_t RESET_REQUEST_MCU_REQ = 0x01;
    static constexpr std::uint32_t MBOX_AXI_USER_LOCK_BIT = 0x01;
};

} // namespace romtime
} // namespace caliptra
