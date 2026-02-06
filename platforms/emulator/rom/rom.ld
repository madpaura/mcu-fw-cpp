/*++

Licensed under the Apache-2.0 license.

File Name:

    rom.ld

Abstract:

    Linker script for MCU ROM ELF.
    This is a template - memory addresses should be adjusted for actual hardware.

--*/

OUTPUT_ARCH("riscv")
ENTRY(_start)

/* Memory Map - Emulator defaults */
/* These should match the memory map from mcu-config-emulator */
MEMORY
{
    ROM     (rx)  : ORIGIN = 0x00000000, LENGTH = 64K
    RAM     (rwx) : ORIGIN = 0x40000000, LENGTH = 128K
    STACK   (rw)  : ORIGIN = 0x40020000, LENGTH = 16K
    ESTACK  (rw)  : ORIGIN = 0x40024000, LENGTH = 4K
}

/* Stack sizes */
STACK_SIZE = 16K;
ESTACK_SIZE = 4K;

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

    /* RAM initialized data section */
    .data : ALIGN(4)
    {
        DATA_START = .;
        *(.data .data.*)
        *(.sdata .sdata.*)
        DATA_END = .;
    } > RAM AT > ROM

    /* RAM BSS section */
    .bss (NOLOAD) : ALIGN(4)
    {
        BSS_START = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        BSS_END = .;
    } > RAM

    /* Global pointer - set to middle of small data sections */
    PROVIDE(GLOBAL_POINTER = DATA_START + 0x800);

    /* Stack section */
    .stack (NOLOAD) : ALIGN(16)
    {
        STACK_ORIGIN = .;
        . = . + STACK_SIZE;
        STACK_TOP = .;
    } > STACK

    /* Exception stack section */
    .estack (NOLOAD) : ALIGN(16)
    {
        ESTACK_ORIGIN = .;
        . = . + ESTACK_SIZE;
        ESTACK_START = .;
    } > ESTACK

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
ASSERT(DATA_END <= ORIGIN(RAM) + LENGTH(RAM), "RAM overflow")
