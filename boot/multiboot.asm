global _start
extern kernel_main

section .rodata
    ; Multiboot
    dd 0xE85250D6       ; Magic
    dd 0x00000000       ; Architecture
    dd 0x00000018       ; Header Length
    dd 0x17ADAF12       ; Checksum

    ; END tag
    dw 0
    dw 0
    dd 8

    ; GDT
    gdt_start:
        gdt_null:
            dq 0
        gdt_code:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x9A     ; Access
            db 0xFA     ; Limit 19:16 + Flags
            db 0x00     ; Base  31:24
        gdt_data:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x92     ; Access
            db 0xFA     ; Limit 19:16 + Flags
            db 0x00     ; Base  31:24
        gdt_code64:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x9A     ; Access
            db 0xFA     ; Limit 19:16 + Flags
            db 0x00     ; Base  31:24
    gdt_end:

    ; GDT Descriptor
    gdt_descriptor:
        dw 0x1F         ; Limit
        dq gdt_start    ; Base

section .bss
    pml4 : resq 512
    pdpt : resq 512
    pd : resq 512

    stack_bottom:
        resb 16384
    stack_top:

section .text
    BITS 32
    _start:
        ; Load Global Descriptor Table Register
        lgdt [gdt_descriptor]

        ; DATA
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov ss, ax

        ; CODE
        jmp 0x08:protected_mode
    
    protected_mode:
        ; PAE (CR4 bit 5)
        mov eax, cr4
        or eax, 0x20
        mov cr4, eax

        mov eax, pdpt
        or eax, 0x3
        mov [pml4], eax

        mov eax, pd
        or eax, 0x3
        mov [pdpt], eax

        mov eax, 0x83
        mov [pd], eax

        mov eax, pml4
        mov cr3, eax

        mov ecx, 0xC0000080
        rdmsr

        or eax, 0x100
        wrmsr

        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax

        jmp 0x18:long_mode
    
    BITS 64
    long_mode:
        mov rsp, stack_top
        call kernel_main
