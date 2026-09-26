global idt_load
global common_exception_handler
global common_irq_handler
global get_cr2

extern exception_dispatch
extern irq_dispatch

section .text
    ; idt_load(IDTR*)
    ; IDTR* idtr (RDI)
    ; return void
    idt_load:
        lidt [rdi]
        ret

    ; common_exception_handler
    common_exception_handler:
        ; Vector Number
        mov rdi, [rsp]

        ; CPUContext*
        lea rsi, [rsp + 8]

        ; Save exception frame base
        mov rbp, rsp

        ; 16 bytes align
        and rsp, -16

        ; exception_dispatch(u64, CPUContext*)
        cld
        call exception_dispatch

        ; Retrieves the start of CPUContext
        mov rsp, rbp
    
    ; common_exception_return
    common_exception_return:
        ; Skip the Vector number
        add rsp, 8

        ; Restoring registers
        pop r15
        pop r14
        pop r13
        pop r12
        pop r11
        pop r10
        pop r9
        pop r8

        pop rbp
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rbx
        pop rax

        ; Skip the error code
        add rsp, 8

        ; Exception return
        iretq

    ; common_irq_handler
    common_irq_handler:
        mov rdi, [rsp] ; IRQ Number

        ; Save exception frame base
        mov rbp, rsp

        ; 16 bytes align
        and rsp, -16

        ; irq_dispatch(u64)
        cld
        call irq_dispatch

        mov rsp, rbp

    ; common_irq_return
    common_irq_return:
        add rsp, 8

        ; Restoring registers
        pop r15
        pop r14
        pop r13
        pop r12
        pop r11
        pop r10
        pop r9
        pop r8

        pop rbp
        pop rdi
        pop rsi
        pop rdx
        pop rcx
        pop rbx
        pop rax

        iretq

    ; u64 get_cr2(void)
    ; return u64 (RAX)
    get_cr2:
        mov rax, cr2
        ret
