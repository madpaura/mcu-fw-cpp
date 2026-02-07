/*++

Licensed under the Apache-2.0 license.

File Name:

    mci.cpp

Abstract:

    MCI (MCU Control Interface) driver implementation.

--*/

#include "romtime/mci.hpp"

namespace caliptra {
namespace romtime {

Mci::Mci(std::uintptr_t base_addr) : base_addr_(base_addr) {}

volatile std::uint32_t* Mci::reg(std::size_t offset) const {
    return reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
}

std::uint32_t Mci::read_reg(std::size_t offset) const {
    return *reg(offset);
}

void Mci::write_reg(std::size_t offset, std::uint32_t value) {
    *reg(offset) = value;
}

void Mci::modify_reg(std::size_t offset, std::uint32_t mask, std::uint32_t value) {
    std::uint32_t current = read_reg(offset);
    write_reg(offset, (current & ~mask) | (value & mask));
}

DeviceLifecycleState Mci::device_lifecycle_state() const {
    std::uint32_t state = read_reg(REG_SECURITY_STATE) & 0x03;
    switch (state) {
        case 0: return DeviceLifecycleState::DeviceUnprovisioned;
        case 1: return DeviceLifecycleState::DeviceManufacturing;
        case 2: return DeviceLifecycleState::DeviceProvisioned;
        case 3: return DeviceLifecycleState::DeviceSecurityLocked;
        default: return DeviceLifecycleState::DeviceUnprovisioned;
    }
}

std::uint32_t Mci::security_state() const {
    return read_reg(REG_SECURITY_STATE);
}

void Mci::caliptra_boot_go() {
    write_reg(REG_CPTRA_BOOT_GO, 1);
}

void Mci::set_flow_status(std::uint32_t status) {
    write_reg(REG_FW_FLOW_STATUS, status);
}

std::uint32_t Mci::flow_status() const {
    return read_reg(REG_FW_FLOW_STATUS);
}

void Mci::set_flow_checkpoint(std::uint16_t checkpoint) {
    std::uint32_t milestone = static_cast<std::uint32_t>(flow_milestone()) << 16;
    set_flow_status(milestone | static_cast<std::uint32_t>(checkpoint));
}

std::uint16_t Mci::flow_checkpoint() const {
    return static_cast<std::uint16_t>(flow_status() & 0x0000FFFF);
}

void Mci::set_fw_fatal_error(std::uint32_t code) {
    write_reg(REG_FW_ERROR_FATAL, code);
}

void Mci::set_flow_milestone(std::uint16_t milestone) {
    std::uint32_t milestone_val = static_cast<std::uint32_t>(milestone) << 16;
    set_flow_status(milestone_val | flow_status());
}

std::uint16_t Mci::flow_milestone() const {
    return static_cast<std::uint16_t>(flow_status() >> 16);
}

std::uint32_t Mci::hw_flow_status() const {
    return read_reg(REG_HW_FLOW_STATUS);
}

void Mci::set_nmi_vector(std::uint32_t nmi_vector) {
    write_reg(REG_MCU_NMI_VECTOR, nmi_vector);
}

void Mci::configure_wdt(std::uint32_t wdt1_timeout, std::uint32_t wdt2_timeout) {
    // Set WDT1 period
    write_reg(REG_WDT_TIMER1_TIMEOUT_PERIOD_0, wdt1_timeout);
    write_reg(REG_WDT_TIMER1_TIMEOUT_PERIOD_1, 0);

    // Set WDT2 period
    write_reg(REG_WDT_TIMER2_TIMEOUT_PERIOD_0, wdt2_timeout);
    write_reg(REG_WDT_TIMER2_TIMEOUT_PERIOD_1, 0);

    // Enable WDT1
    write_reg(REG_WDT_TIMER1_CTRL, 1);  // Timer1Restart
    write_reg(REG_WDT_TIMER1_EN, 1);    // Timer1En
}

void Mci::disable_wdt() {
    write_reg(REG_WDT_TIMER1_EN, 0);
}

std::uint32_t Mci::generic_input_wires(std::size_t index) const {
    // Generic input wires registers at offset 0x04 and 0x08 from base
    constexpr std::size_t REG_GENERIC_INPUT_WIRES_BASE = 0x288;  // Match Rust register layout
    if (index >= 2) {
        return 0;
    }
    return read_reg(REG_GENERIC_INPUT_WIRES_BASE + index * 4);
}

std::uint32_t Mci::reset_reason() const {
    return read_reg(REG_RESET_REASON);
}

McuResetReason Mci::reset_reason_enum() const {
    std::uint32_t reason = reset_reason();
    bool warm_reset = (reason & RESET_REASON_WARM_RESET) != 0;
    bool fw_boot_upd = (reason & RESET_REASON_FW_BOOT_UPD_RESET) != 0;
    bool fw_hitless_upd = (reason & RESET_REASON_FW_HITLESS_UPD_RESET) != 0;

    if (!warm_reset && !fw_boot_upd && !fw_hitless_upd) {
        return McuResetReason::ColdBoot;
    } else if (warm_reset && !fw_boot_upd && !fw_hitless_upd) {
        return McuResetReason::WarmReset;
    } else if (!warm_reset && fw_boot_upd && !fw_hitless_upd) {
        return McuResetReason::FirmwareBootReset;
    } else if (!warm_reset && !fw_boot_upd && fw_hitless_upd) {
        return McuResetReason::FirmwareHitlessUpdate;
    } else {
        return McuResetReason::Invalid;
    }
}

bool Mci::is_cold_reset() const {
    return reset_reason_enum() == McuResetReason::ColdBoot;
}

bool Mci::is_warm_reset() const {
    return reset_reason_enum() == McuResetReason::WarmReset;
}

bool Mci::is_fw_boot_update_reset() const {
    return reset_reason_enum() == McuResetReason::FirmwareBootReset;
}

bool Mci::is_fw_hitless_update_reset() const {
    return reset_reason_enum() == McuResetReason::FirmwareHitlessUpdate;
}

std::uint32_t Mci::read_notif0_intr_trig_r() const {
    return read_reg(REG_NOTIF0_INTR_TRIG_R);
}

void Mci::write_notif0_intr_trig_r(std::uint32_t value) {
    write_reg(REG_NOTIF0_INTR_TRIG_R, value);
}

std::uint32_t Mci::read_wdt_timer1_en() const {
    return read_reg(REG_WDT_TIMER1_EN);
}

void Mci::write_wdt_timer1_en(std::uint32_t value) {
    write_reg(REG_WDT_TIMER1_EN, value);
}

void Mci::handle_interrupt() {
    std::uint32_t intr_status = read_reg(REG_NOTIF0_INTERNAL_INTR_R);
    if ((intr_status & NOTIF_CPTRA_MCU_RESET_REQ_STS_MASK) != 0) {
        // Clear interrupt
        modify_reg(REG_NOTIF0_INTERNAL_INTR_R, NOTIF_CPTRA_MCU_RESET_REQ_STS_MASK,
                   NOTIF_CPTRA_MCU_RESET_REQ_STS_MASK);
        // Request MCU reset
        modify_reg(REG_RESET_REQUEST, RESET_REQUEST_MCU_REQ, RESET_REQUEST_MCU_REQ);
    }
}

void Mci::trigger_warm_reset() {
    write_reg(REG_RESET_REQUEST, 1);
}

void Mci::set_ss_config_done_sticky() {
    write_reg(REG_SS_CONFIG_DONE_STICKY, SS_CONFIG_DONE_BIT);
}

bool Mci::is_ss_config_done_sticky() const {
    return (read_reg(REG_SS_CONFIG_DONE_STICKY) & SS_CONFIG_DONE_BIT) != 0;
}

void Mci::set_ss_config_done() {
    write_reg(REG_SS_CONFIG_DONE, SS_CONFIG_DONE_BIT);
}

bool Mci::is_ss_config_done() const {
    return (read_reg(REG_SS_CONFIG_DONE) & SS_CONFIG_DONE_BIT) != 0;
}

std::optional<std::uint32_t> Mci::read_prod_debug_unlock_pk_hash(std::size_t index) const {
    if (index >= PK_HASH_REG_COUNT) {
        return std::nullopt;
    }
    return read_reg(REG_PROD_DEBUG_UNLOCK_PK_HASH_BASE + index * 4);
}

std::size_t Mci::prod_debug_unlock_pk_hash_len() const {
    return PK_HASH_REG_COUNT;
}

std::optional<std::uint32_t> Mci::read_mbox0_valid_axi_user(std::size_t index) const {
    if (index >= MBOX_AXI_USER_COUNT) {
        return std::nullopt;
    }
    return read_reg(REG_MBOX0_VALID_AXI_USER_BASE + index * 4);
}

std::optional<bool> Mci::read_mbox0_axi_user_lock(std::size_t index) const {
    if (index >= MBOX_AXI_USER_COUNT) {
        return std::nullopt;
    }
    return (read_reg(REG_MBOX0_AXI_USER_LOCK_BASE + index * 4) & MBOX_AXI_USER_LOCK_BIT) != 0;
}

std::optional<std::uint32_t> Mci::read_mbox1_valid_axi_user(std::size_t index) const {
    if (index >= MBOX_AXI_USER_COUNT) {
        return std::nullopt;
    }
    return read_reg(REG_MBOX1_VALID_AXI_USER_BASE + index * 4);
}

std::optional<bool> Mci::read_mbox1_axi_user_lock(std::size_t index) const {
    if (index >= MBOX_AXI_USER_COUNT) {
        return std::nullopt;
    }
    return (read_reg(REG_MBOX1_AXI_USER_LOCK_BASE + index * 4) & MBOX_AXI_USER_LOCK_BIT) != 0;
}

std::size_t Mci::mbox_axi_user_len() const {
    return MBOX_AXI_USER_COUNT;
}

bool Mci::write_mbox0_valid_axi_user(std::size_t index, std::uint32_t value) {
    if (index >= MBOX_AXI_USER_COUNT) {
        return false;
    }
    write_reg(REG_MBOX0_VALID_AXI_USER_BASE + index * 4, value);
    return true;
}

bool Mci::lock_mbox0_axi_user(std::size_t index) {
    if (index >= MBOX_AXI_USER_COUNT) {
        return false;
    }
    write_reg(REG_MBOX0_AXI_USER_LOCK_BASE + index * 4, MBOX_AXI_USER_LOCK_BIT);
    return true;
}

bool Mci::write_mbox1_valid_axi_user(std::size_t index, std::uint32_t value) {
    if (index >= MBOX_AXI_USER_COUNT) {
        return false;
    }
    write_reg(REG_MBOX1_VALID_AXI_USER_BASE + index * 4, value);
    return true;
}

bool Mci::lock_mbox1_axi_user(std::size_t index) {
    if (index >= MBOX_AXI_USER_COUNT) {
        return false;
    }
    write_reg(REG_MBOX1_AXI_USER_LOCK_BASE + index * 4, MBOX_AXI_USER_LOCK_BIT);
    return true;
}

} // namespace romtime
} // namespace caliptra
