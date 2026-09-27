ENTRY(_start)

SECTIONS {
    . = 0x00100000;

    kernel_start = .;

    .rodata : { *(.rodata) }
    .text : { *(.text) }

    . = ALIGN(0x1000);
    .bss : { *(.bss) }

    kernel_end = .;
}