/*++

Licensed under the Apache-2.0 license.

File Name:

    lifecycle.hpp

Abstract:

    Lifecycle controller interface.

--*/

#pragma once

#include <cstdint>
#include "common/error.hpp"
#include "common/static_ref.hpp"

namespace caliptra {
namespace rom {

/**
 * Lifecycle controller states (high-level)
 * These map to the controller's state machine
 */
enum class LifecycleControllerState : std::uint32_t {
    RawState = 0,
    TestUnlocked = 1,
    TestLocked = 2,
    Dev = 3,
    Prod = 4,
    ProdEnd = 5,
    Rma = 6,
    Scrap = 7,
    Invalid = 0xFFFFFFFF,
};

/**
 * Lifecycle states (detailed hardware states)
 */
enum class LifecycleState : std::uint32_t {
    Raw = 0,
    TestUnlocked0 = 1,
    TestLocked0 = 2,
    TestUnlocked1 = 3,
    TestLocked1 = 4,
    TestUnlocked2 = 5,
    TestLocked2 = 6,
    TestUnlocked3 = 7,
    Dev = 8,
    Prod = 9,
    ProdEnd = 10,
    Rma = 11,
    Scrap = 12,
    Invalid = 0xFF,
};

/**
 * Lifecycle controller driver
 */
class Lifecycle {
public:
    /**
     * Construct with register base address
     */
    explicit Lifecycle(std::uintptr_t base_addr);

    /**
     * Get current lifecycle state (detailed)
     */
    LifecycleState state() const;
    
    /**
     * Get current lifecycle controller state (high-level)
     * This is the main state used by ROM for flow decisions
     */
    LifecycleControllerState current_state() const;

    /**
     * Check if lifecycle controller is initialized
     */
    bool is_initialized() const;

    /**
     * Get raw status register value
     */
    std::uint32_t status() const;

    /**
     * Check if in manufacturing state
     */
    bool is_manufacturing() const;

    /**
     * Check if in production state
     */
    bool is_production() const;

    /**
     * Check if in RMA state
     */
    bool is_rma() const;

    /**
     * Check if in development state
     */
    bool is_dev() const;

    /**
     * Check if debug is locked
     */
    bool is_debug_locked() const;

    /**
     * Request lifecycle transition
     */
    error::McuResult<void> request_transition(LifecycleState target_state);

    /**
     * Get transition token count
     */
    std::uint32_t transition_token_count() const;

    /**
     * Write transition token
     */
    error::McuResult<void> write_transition_token(std::size_t index, std::uint32_t value);

private:
    std::uintptr_t base_addr_;

    // Register access helpers
    std::uint32_t read_reg(std::size_t offset) const;
    void write_reg(std::size_t offset, std::uint32_t value);

    // Register offsets (to be defined based on hardware)
    static constexpr std::size_t REG_STATE = 0x00;
    static constexpr std::size_t REG_STATUS = 0x04;
    static constexpr std::size_t REG_TRANSITION_CMD = 0x08;
    static constexpr std::size_t REG_TRANSITION_TOKEN_BASE = 0x10;
    static constexpr std::size_t TRANSITION_TOKEN_COUNT = 4;
};

} // namespace rom
} // namespace caliptra
