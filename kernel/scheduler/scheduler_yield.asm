global scheduler_yield_asm
extern scheduler_schedule

section .text
    scheduler_yield_asm:
        ; SS
        push qword 0x10

        ; RSP
        push rsp
        add qword [rsp], 8

        ; RFLAGS
        pushfq

        ; CS
        push qword 0x18

        ; iretq will continue from here
        push qword .resume

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

        ; scheduler_schedule(CPUContext*)
        mov rdi, rsp
        call scheduler_schedule
        mov rsp, rax

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

        ; Error Code
        add rsp, 8

        iretq
    
    .resume:
        ret