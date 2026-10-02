global io_in8
global io_out8
global enable_interrupts
global disable_interrupts
global interrupts_enabled
global interrupts_save_and_disable
global interrupts_restore

section .text
    ; io_in8(u16)
    ; u16 port (RDI)
    ; return u8 (RAX)
    io_in8:
        mov dx, di
        in al, dx
        ret

    ; io_out8(u16, u8)
    ; u16 port (RDI)
    ; u8 value (RSI)
    ; return void
    io_out8:
        mov dx, di
        mov al, sil

        out dx, al
        ret
    
    ; enable_interrupts()
    ; return void
    enable_interrupts:
        sti
        ret
    
    ; disable_interrupts()
    ; return void
    disable_interrupts:
        cli
        ret
    
    ; interrupts_enabled()
    ; return bool (RAX)
    interrupts_enabled:
        pushfq
        pop rax

        test rax, 0x200

        setnz al
        movzx rax, al

        ret
    
    ; interrupts_save_and_disable(bool*)
    ; bool* was_enabled (RDI)
    ; return void
    interrupts_save_and_disable:
        pushfq
        pop rax

        test rax, 0x200

        setnz al
        mov [rdi], al

        cli
        ret
    
    ; interrupts_restore(bool)
    ; bool was_enabled (RDI)
    ; return void
    interrupts_restore:
        test dil, dil
        jz .interrupts_restore_done

        sti
        ret
    
    .interrupts_restore_done:
        cli
        ret