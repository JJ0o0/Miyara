ENTRY(_start)

SECTIONS {
    . = 0x00100000;

    .rodata : { *(.rodata) }
    .text : { *(.text) }

    . = ALIGN(0X1000);
    .bss : { *(.bss) }
}