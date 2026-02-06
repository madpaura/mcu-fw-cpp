// Licensed under the Apache-2.0 license
// VeeR interrupt controller implementation

#include "veer/veer.hpp"

namespace caliptra {
namespace runtime {
namespace kernel {
namespace veer {

void InterruptController::init() {
    // Initialize interrupt controller
}

void InterruptController::enable_interrupts() {
#ifdef TARGET_RISCV32
    asm volatile("csrsi mstatus, 0x8");  // Set MIE bit
#endif
}

void InterruptController::disable_interrupts() {
#ifdef TARGET_RISCV32
    asm volatile("csrci mstatus, 0x8");  // Clear MIE bit
#endif
}

void InterruptController::enable_interrupt(std::uint32_t irq) {
    // Enable specific interrupt in PLIC/CLIC
    (void)irq;
}

void InterruptController::disable_interrupt(std::uint32_t irq) {
    // Disable specific interrupt
    (void)irq;
}

void InterruptController::set_priority(std::uint32_t irq, std::uint8_t priority) {
    // Set interrupt priority
    (void)irq;
    (void)priority;
}

void InterruptController::acknowledge(std::uint32_t irq) {
    // Acknowledge interrupt
    (void)irq;
}

} // namespace veer
} // namespace kernel
} // namespace runtime
} // namespace caliptra
