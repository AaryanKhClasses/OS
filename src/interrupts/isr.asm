[bits 32]
global irq1_stub
extern irq1_handler

irq1_stub:
    pusha                  ; save general regs
    push ds
    push es
    push fs
    push gs

    mov ax, 0x18           ; kernel data segment selector (GDT entry 2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq1_handler

    pop gs
    pop fs
    pop es
    pop ds
    popa

    mov al, 0x20           ; send EOI
    out 0x20, al

    iret
