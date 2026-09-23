global idt_load
global common_exception_handler
global get_cr2

global isr_divide_error
global isr_invalid_opcode
global isr_general_protection
global isr_page_fault

extern exception_dispatch

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

    ; u64 get_cr2(void)
    ; return u64 (RAX)
    get_cr2:
        mov rax, cr2
        ret

    ; #DE - Divide Error (Vector 0)
    isr_divide_error:
        ; Artificial Error Code
        push qword 0

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

        ; Vector number
        push qword 0
        jmp common_exception_handler
    
    ; #UD - Invalid Opcode (Vector 6)
    isr_invalid_opcode:
        ; Artificial Error Code
        push qword 0

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

        ; Vector number
        push qword 6
        jmp common_exception_handler
    
    ; #GP - General Protection Fault (Vector 13)
    isr_general_protection:
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

        ; Vector number
        push qword 13
        jmp common_exception_handler

    ; #PF - Page Fault (Vector 14)
    isr_page_fault:
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

        ; Vector number
        push qword 14
        jmp common_exception_handler
