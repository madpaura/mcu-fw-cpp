/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.ld

Abstract:

    Linker script for MCU ROM ELF.
    Memory map matches mcu-config-emulator.

--*/

OUTPUT_ARCH("riscv")
ENTRY(_start)

/* Memory Map - Matches mcu-config-emulator */
/* See platforms/emulator/config/src/lib.rs */
MEMORY
{
    ROM     (rx)  : ORIGIN = 0x80000000, LENGTH = 64K
    SRAM    (rwx) : ORIGIN = 0x40000000, LENGTH = 512K
    DCCM    (rw)  : ORIGIN = 0x50000000, LENGTH = 16K
}

/* Stack sizes - match Rust ROM (rom_stack_size, rom_estack_size) */
STACK_SIZE = 0x2d00;    /* ~11KB */
ESTACK_SIZE = 0x200;    /* 512 bytes */

/* MRAC value - default for emulator */
MRAC_VALUE = 0xAAAAAAAA;

SECTIONS
{
    /* ROM code section */
    .text : ALIGN(4)
    {
        *(.text.init)       /* Startup code first */
        *(.text .text.*)    /* All other code */
        *(.rodata .rodata.*)    /* Read-only data */
        *(.srodata .srodata.*)
    } > ROM

    /* Store the location of ROM data for copying */
    ROM_DATA_START = .;

    /* RAM initialized data section - placed in SRAM, loaded from ROM */
    .data : ALIGN(4)
    {
        DATA_START = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        DATA_END = .;
    } > SRAM AT > ROM

    /* RAM BSS section */
    .bss (NOLOAD) : ALIGN(4)
    {
        BSS_START = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        BSS_END = .;
    } > SRAM

    /* Global pointer - set to middle of small data sections */
    PROVIDE(GLOBAL_POINTER = DATA_START + 0x800);

    /* Stack - at end of SRAM */
    .stack (NOLOAD) : ALIGN(16)
    {
        STACK_ORIGIN = .;
        . = . + STACK_SIZE;
        STACK_TOP = .;
    } > SRAM

    /* Exception stack - after main stack */
    .estack (NOLOAD) : ALIGN(16)
    {
        ESTACK_ORIGIN = .;
        . = . + ESTACK_SIZE;
        ESTACK_START = .;
    } > SRAM

    /* Discard sections we don't need */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.debug*)
    }
}

/* Verify sections fit */
ASSERT(. <= ORIGIN(ROM) + LENGTH(ROM), "ROM overflow")
