global _start
extern kernel_main

section .rodata
    ; Multiboot Header
    multiboot_header_start:
        ; Multiboot
        dd 0xE85250D6       ; Magic
        dd 0x00000000       ; Architecture (0 = i386/protected mode)
        dd 0x00000030       ; Header Length (this header, in bytes)
        dd 0x17ADAEFA       ; Checksum (-(magic + architecture + header length))

        ; Framebuffer Tag
        dw 5                ; Type = Framebuffer
        dw 0                ; Flags
        dd 20               ; Size
        dd 1024             ; Preferred Width
        dd 768              ; Preferred Height
        dd 32               ; Preferred Depth
        dd 0                ; Padding (Alinhamento de 8 Bytes)

        ; END tag
        dw 0                ; Type
        dw 0                ; Flags
        dd 8                ; Size (of this tag)
    multiboot_header_end:
    
    ; GDT
    gdt_start:
        ; Selector 0x00. Required null descriptor; never loaded.
        gdt_null:
            dq 0
        
        ; Selector 0x08. 32-bit code segment used by _start before the
        ; jump into protected_mode.
        gdt_code:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x9A     ; Access (present, ring 0, code, executable, readable)
            db 0xFA     ; Limit 19:16 (0xF) + Flags (granularity=4K, 32-bit)
            db 0x00     ; Base  31:24
        
        ; Selector 0x10. 32-bit data segment, loaded into DS/ES/SS.
        gdt_data:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x92     ; Access (present, ring 0, data, writable)
            db 0xFA     ; Limit 19:16 (0xF) + Flags (granularity=4K, 32-bit)
            db 0x00     ; Base  31:24
        
        ; Selector 0x18. 64-bit code segment used once long mode is
        ; entered. Must match the selector idt_set_gate() puts in every
        ; IDT entry (interrupt/idt.c), since ISRs run in this segment.
        gdt_code64:
            dw 0xFFFF   ; Limit 15:0
            dw 0x0000   ; Base  15:0
            db 0x00     ; Base  23:16
            db 0x9A     ; Access (present, ring 0, code, executable, readable)
            db 0xAF     ; Limit 19:16 (0xF) + Flags (granularity=4K, long mode)
            db 0x00     ; Base  31:24
    gdt_end:

    ; GDT Descriptor
    gdt_descriptor:
        dw gdt_end - gdt_start - 1  ; Limit: size of the GDT minus one
        dq gdt_start                ; Base: linear address of the GDT

section .bss
    ; Minimal 4-level page table structures (PML4 -> PDPT -> PD), used to
    ; identity-map the first 2MB of physical memory before enabling
    ; paging. Only the first entry of each table is ever filled in;
    ; the rest stay zeroed (not present).
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

        ; Load the data segment selector into DS/ES/SS. CS is reloaded
        ; separately below via the far jump, since it can't be loaded
        ; with a plain mov.
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov ss, ax

        ; Far jump to reload CS with the new code selector (0x08) and
        ; flush the CPU's stale segment descriptor cache.
        jmp 0x08:protected_mode
    
    protected_mode:
        ; Enable PAE (CR4 bit 5), required before long mode can be enabled
        mov eax, cr4
        or eax, 0x20
        mov cr4, eax

        ; PML4[0] -> PDPT (present, writable)
        mov eax, pdpt
        or eax, 0x3
        mov [pml4], eax

        ; PDPT[0] -> PD (present, writable)
        mov eax, pd
        or eax, 0x3
        mov [pdpt], eax

        ; PD[0] -> identity-mapped 2MB huge page at physical 0x0
        ; (present, writable, PS=1 for a 2MB page)
        mov eax, 0x83
        mov [pd], eax

        ; Point CR3 at the PML4, the root of the page table hierarchy
        mov eax, pml4
        mov cr3, eax

        ; Set EFER.LME (Long Mode Enable), via the EFER MSR (0xC0000080)
        mov ecx, 0xC0000080
        rdmsr

        or eax, 0x100
        wrmsr

        ; Enable paging (CR0.PG). With PAE and LME already set, this is
        ; what actually activates long mode.
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax

        ; Far jump into the 64-bit code segment (0x18), which also
        ; flushes the pipeline/segment cache like the earlier jump did
        jmp 0x18:long_mode
    
    BITS 64
    long_mode:
        mov rsp, stack_top
        mov edi, ebx
        call kernel_main
