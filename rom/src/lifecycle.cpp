/*++

Licensed under the Apache-2.0 license.

File Name:

    lifecycle.cpp

Abstract:

    Lifecycle controller implementation.

--*/

#include "rom/lifecycle.hpp"

namespace caliptra {
namespace rom {

Lifecycle::Lifecycle(std::uintptr_t base_addr)
    : base_addr_(base_addr) {}

std::uint32_t Lifecycle::read_reg(std::size_t offset) const {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    return *reg;
}

void Lifecycle::write_reg(std::size_t offset, std::uint32_t value) {
    volatile std::uint32_t* reg = reinterpret_cast<volatile std::uint32_t*>(base_addr_ + offset);
    *reg = value;
}

LifecycleState Lifecycle::state() const {
    std::uint32_t state_val = read_reg(REG_STATE) & 0x1F;
    
    switch (state_val) {
        case 0: return LifecycleState::Raw;
        case 1: return LifecycleState::TestUnlocked0;
        case 2: return LifecycleState::TestLocked0;
        case 3: return LifecycleState::TestUnlocked1;
        case 4: return LifecycleState::TestLocked1;
        case 5: return LifecycleState::TestUnlocked2;
        case 6: return LifecycleState::TestLocked2;
        case 7: return LifecycleState::TestUnlocked3;
        case 8: return LifecycleState::Dev;
        case 9: return LifecycleState::Prod;
        case 10: return LifecycleState::ProdEnd;
        case 11: return LifecycleState::Rma;
        case 12: return LifecycleState::Scrap;
        default: return LifecycleState::Invalid;
    }
}

LifecycleControllerState Lifecycle::current_state() const {
    LifecycleState s = state();
    
    switch (s) {
        case LifecycleState::Raw:
            return LifecycleControllerState::RawState;
        case LifecycleState::TestUnlocked0:
        case LifecycleState::TestUnlocked1:
        case LifecycleState::TestUnlocked2:
        case LifecycleState::TestUnlocked3:
            return LifecycleControllerState::TestUnlocked;
        case LifecycleState::TestLocked0:
        case LifecycleState::TestLocked1:
        case LifecycleState::TestLocked2:
            return LifecycleControllerState::TestLocked;
        case LifecycleState::Dev:
            return LifecycleControllerState::Dev;
        case LifecycleState::Prod:
            return LifecycleControllerState::Prod;
        case LifecycleState::ProdEnd:
            return LifecycleControllerState::ProdEnd;
        case LifecycleState::Rma:
            return LifecycleControllerState::Rma;
        case LifecycleState::Scrap:
            return LifecycleControllerState::Scrap;
        default:
            return LifecycleControllerState::Invalid;
    }
}

bool Lifecycle::is_initialized() const {
    return (read_reg(REG_STATUS) & 0x01) != 0;
}

std::uint32_t Lifecycle::status() const {
    return read_reg(REG_STATUS);
}

bool Lifecycle::is_manufacturing() const {
    LifecycleState s = state();
    return s == LifecycleState::Raw || 
           s == LifecycleState::TestUnlocked0 ||
           s == LifecycleState::TestUnlocked1 ||
           s == LifecycleState::TestUnlocked2 ||
           s == LifecycleState::TestUnlocked3 ||
           s == LifecycleState::TestLocked0 ||
           s == LifecycleState::TestLocked1 ||
           s == LifecycleState::TestLocked2;
}

bool Lifecycle::is_production() const {
    LifecycleState s = state();
    return s == LifecycleState::Prod || s == LifecycleState::ProdEnd;
}

bool Lifecycle::is_rma() const {
    return state() == LifecycleState::Rma;
}

bool Lifecycle::is_dev() const {
    return state() == LifecycleState::Dev;
}

bool Lifecycle::is_debug_locked() const {
    LifecycleState s = state();
    return s == LifecycleState::Prod || 
           s == LifecycleState::ProdEnd ||
           s == LifecycleState::TestLocked0 ||
           s == LifecycleState::TestLocked1 ||
           s == LifecycleState::TestLocked2;
}

error::McuResult<void> Lifecycle::request_transition(LifecycleState target_state) {
    // Write transition command
    write_reg(REG_TRANSITION_CMD, static_cast<std::uint32_t>(target_state));
    return error::McuResult<void>::Ok();
}

std::uint32_t Lifecycle::transition_token_count() const {
    return TRANSITION_TOKEN_COUNT;
}

error::McuResult<void> Lifecycle::write_transition_token(std::size_t index, std::uint32_t value) {
    if (index >= TRANSITION_TOKEN_COUNT) {
        return error::McuResult<void>::Err(error::McuError::INVALID_PARAMETER);
    }
    write_reg(REG_TRANSITION_TOKEN_BASE + index * 4, value);
    return error::McuResult<void>::Ok();
}

} // namespace rom
} // namespace caliptra
