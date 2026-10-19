global tss_load

section .text
    tss_load:
        mov ax, 0x30
        ltr ax
        ret