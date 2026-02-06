/*++

Licensed under the Apache-2.0 license.

File Name:

    veer.hpp

Abstract:

    VeeR RISC-V core support header.

--*/

#pragma once

#include <cstdint>

namespace caliptra {
namespace runtime {
namespace kernel {
namespace veer {

/**
 * VeeR interrupt controller
 */
class InterruptController {
public:
    /**
     * Initialize interrupt controller
     */
    static void init();

    /**
     * Enable interrupts globally
     */
    static void enable_interrupts();

    /**
     * Disable interrupts globally
     */
    static void disable_interrupts();

    /**
     * Enable specific interrupt
     */
    static void enable_interrupt(std::uint32_t irq);

    /**
     * Disable specific interrupt
     */
    static void disable_interrupt(std::uint32_t irq);

    /**
     * Set interrupt priority
     */
    static void set_priority(std::uint32_t irq, std::uint8_t priority);

    /**
     * Acknowledge interrupt
     */
    static void acknowledge(std::uint32_t irq);
};

/**
 * VeeR chip configuration
 */
class Chip {
public:
    /**
     * Initialize chip
     */
    static void init();

    /**
     * Get chip ID
     */
    static std::uint32_t chip_id();

    /**
     * Configure watchdog
     */
    static void configure_watchdog(std::uint32_t timeout);

    /**
     * Pet watchdog
     */
    static void pet_watchdog();
};

} // namespace veer
} // namespace kernel
} // namespace runtime
} // namespace caliptra
