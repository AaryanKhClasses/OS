[bits 32]
global irq1_stub
extern irq1_handler

irq1_stub:
    pusha               ; Save all general-purpose registers
    cld                 ; Clear direction flag

    push ds             ; Save data segment
    push es             ; Save extra segment
    mov ax, 0x10        ; Set data segment to kernel data segment
    mov ds, ax          ; Load data segment
    mov es, ax          ; Load extra segment

    call irq1_handler   ; Call IRQ1 handler

    pop es              ; Restore extra segment
    pop ds              ; Restore data segment

    mov al, 0x20        ; Send EOI (End of Interrupt) to master PIC
    out 0x20, al        ; Acknowledge interrupt

    popa                ; Restore all general-purpose registers
    iret                ; Return from interrupt
