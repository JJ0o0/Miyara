global idt_load
global isr_divide_error
extern exception_handler

section .text
    ; idt_load(IDTR*)
    ; IDTR* idtr (RDI)
    ; return void
    idt_load:
        lidt [rdi]
        ret

    ; #DE - Divide Error
    isr_divide_error:
        ; Save registers
        push rax
        push rbx
        push rcx
        push rdx
        push rsi
        push rdi
        push rbp
        push r8
        push r9
        push r10
        push r11
        push r12
        push r13
        push r14
        push r15

        ; RSP points to CPUContext
        mov rdi, rsp

        ; Stores CPUContext address.
        mov rbp, rsp

        ; 16 bytes align
        and rsp, -16

        ; exception_handler(CPUContext*)
        call exception_handler

        ; Retrieves the start of CPUContext
        mov rsp, rbp

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

        ; Exception return
        iretq