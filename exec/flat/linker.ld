ENTRY(_start)  /* Entry point symbol */

SECTIONS {
    /* Code section */
    . = 0x100000;  /* Start address of the code section */
    .text : {
        *(.text)  /* Place all code here */
        *(.rodata)  /* Place read-only data (strings) here */
        *(.data)    /* Place other data here */
        *(.bss)     /* Uninitialized data (if needed) */
    }

    /* Ensure overall alignment of the binary */
    . = ALIGN(4096);  /* Align to a 4KB boundary */
}
