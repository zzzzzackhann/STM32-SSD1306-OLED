// Arm Thumb assembly for Cortex-M4 microcontroller
.syntax unified
.cpu cortex-m4 
.thumb 

// Global memory locations
.global vtable
.global reset_handler
.global default_handler

// .text section for code
.section .text
.thumb_func

// Reset handler function
.type reset_handler, %function
reset_handler:

    // Initialize the stack pointer
    ldr r0, =_estack
    mov sp, r0
    
    // Copy .data from FLASH to RAM
    ldr r1, =_sdata
    ldr r2, =_edata
    ldr r3, =_sidata
    cmp r1, r2
    bcs data_is_empty //branch if .data is data_is_empty
copy_data_loop:
    ldr r4, [r3], #4    // load from source (Flash), then incremement r3
    str r4, [r1], #4    //store to destination (RAM), then increment r1
    cmp r1, r2
    bcc copy_data_loop  // branch while (r1 < r2)
data_is_empty:

    // Zero initialize .bss section
    ldr r1, =_sbss
    ldr r2, =_ebss
    mov r3, #0
    cmp r1, r2
    bcs bss_is_empty    // branch if  .bss is bss_is_empty
bss_loop:
    str r3, [r1], #4
    cmp r1, r2
    bcc bss_loop        // branch while (r1 < r2)
bss_is_empty:

bl main

     /* End of main logic -------------------------------------- */

halt:
    b halt

default_hander:
    b default_hander

.size reset_handler, .-reset_handler

//Vector table
.section .isr_vector, "a", %progbits
.align 2    // 4-byte alignment
.type vtable, %object
vtable:
    .word _estack       /* Initial Stack Pointer */
    .word reset_handler /* Reset Handler */
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word default_hander
    .word usart2_handler
    
.size vtable, .-vtable
