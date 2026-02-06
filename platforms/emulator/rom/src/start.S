/*++

Licensed under the Apache-2.0 license.

File Name:

    start.S

Abstract:

    Startup code for bare-metal RISCV ROM.
    Port of the Rust start.s

--*/

.option norvc

.section .text.init
.global _start
_start:

.option push
.option norelax
    la gp, GLOBAL_POINTER
.option pop

    # Initialize the stack pointer
    la sp, STACK_TOP

    # Interrupts are disabled within the ROM, so the only possible trap trigger is an exception.
    # As such, configure mtvec with the address of the exception handler in direct mode.
    la t0, _exception_handler
    csrw mtvec, t0

    # Initialize MRAC (Region Access Control Register)
    # MRAC controls cacheability and side effects for 16 memory regions (256MB each)
    # CSR address 0x7c0 = MRAC register
    lui     t0, %hi(MRAC_VALUE)
    addi    t0, t0, %lo(MRAC_VALUE)
    csrw    0x7c0, t0

    # Zero BSS section
    la t0, BSS_START
    la t1, BSS_END
zero_bss:
    bge t0, t1, end_zero_bss
    sw x0, 0(t0)
    addi t0, t0, 4
    j zero_bss
end_zero_bss:

    # Copy ROM data to RAM
    la t0, ROM_DATA_START
    la t1, DATA_START
    la t2, DATA_END
copy_data:
    bge t1, t2, end_copy_data
    lw t3, 0(t0)
    sw t3, 0(t1)
    addi t0, t0, 4
    addi t1, t1, 4
    j copy_data
end_copy_data:

    # Call main entry point
    call main

    # Exit the emulator if we return
    la t0, EMU_CTRL_EXIT
    sw zero, 0(t0)

.section .data
.equ  EMU_CTRL_EXIT, 0x2000F000

.section .text.init
.align 2
_exception_handler:
    # Save the SP to mscratch
    csrw mscratch, sp
    
    # Switch to the exception stack
    la sp, ESTACK_START

    # Call C++ exception handler
    jal exception_handler
