/* Linker script for NUCLEO-F446RE Board*/

ENTRY(reset_handler)

/* End of RAM = initial stack pointer */
_estack = ORIGIN(RAM) + LENGTH(RAM);

MEMORY
{
    FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    RAM     (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    /* Vector table at start of FLASH */
    .isr_vector :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))
        . = ALIGN(4);
    } > FLASH

    /* Code and read-only data in FLASH */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        . = ALIGN(4);
    } > FLASH

    /* Initialized data */
    /* The startup code will copy these from FLASH to RAM */
    .data :
    {
    . = ALIGN(4);
    _sdata = .;
    *(.data)
    *(.data*)
    . = ALIGN(4);
    _edata = .;
    } > RAM AT > FLASH
    /* Load address of .data section from FLASH */
    _sidata = LOADADDR(.data);

    /* Uninitialized data in RAM (zeroed by startup) */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM
    
} 
