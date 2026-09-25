global io_out8
global enable_interrupts

section .text
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