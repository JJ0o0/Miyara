global idt_load
global common_exception_handler
global isr_divide_error
global isr_invalid_opcode
global test_invalid_opcode
extern exception_dispatch

section .text
    ; idt_load(IDTR*)
    ; IDTR* idtr (RDI)
    ; return void
    idt_load:
        lidt [rdi]
        ret

    common_exception_handler:
        mov rdi, 0
        mov rsi, rsp

        ; Stores CPUContext address.
        mov rbp, rsp

        ; 16 bytes align
        and rsp, -16

        ; exception_dispatch(u64, CPUContext*)
        call exception_dispatch

        ; Retrieves the start of CPUContext
        mov rsp, rbp
        jmp common_exception_return
    
    common_exception_return:
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

        add rsp, 8

        ; Exception return
        iretq

    ; #DE - Divide Error
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

        mov qword [rsp + 152], 0
        jmp common_exception_handler
    
    ; #UD - Invalid Opcode
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

        mov qword [rsp + 152], 6
        jmp common_exception_handler
    
    test_invalid_opcode:
        ud2
    