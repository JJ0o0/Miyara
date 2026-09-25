global isr_divide_error
global isr_invalid_opcode
global isr_general_protection
global isr_page_fault

global isr_timer

extern common_exception_handler
extern irq_dispatch

section .text
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
    
    isr_timer:
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

        mov rbp, rsp
        and rsp, -16

        mov rdi, 0
        call irq_dispatch

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

        iretq
