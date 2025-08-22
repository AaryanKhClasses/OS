[bits 32]
extern irq_handler

%macro IRQ_STUB 2
global %1
%1:
    push dword 0    ; fake error code
    push dword %2   ; push interrupt number
    jmp irq_common
%endmacro

IRQ_STUB irq_32, 0x20
IRQ_STUB irq_33, 0x21
IRQ_STUB irq_34, 0x22
IRQ_STUB irq_35, 0x23
IRQ_STUB irq_36, 0x24
IRQ_STUB irq_37, 0x25
IRQ_STUB irq_38, 0x26
IRQ_STUB irq_39, 0x27
IRQ_STUB irq_40, 0x28
IRQ_STUB irq_41, 0x29
IRQ_STUB irq_42, 0x2A
IRQ_STUB irq_43, 0x2B
IRQ_STUB irq_44, 0x2C
IRQ_STUB irq_45, 0x2D
IRQ_STUB irq_46, 0x2E
IRQ_STUB irq_47, 0x2F

irq_common:
    pusha
    push ds
    push es
    push fs
    push gs

    mov ax, 0x18
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax
    call irq_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
