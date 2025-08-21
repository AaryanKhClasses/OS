[bits 32]
extern isr_handler_c

%macro EX 1
global isr_%1
isr_%1:
    pusha
    push ds
    push es
    mov ax, 0x18     ; kernel data selector
    mov ds, ax
    mov es, ax
    push dword %1    ; push vector number
    call isr_handler_c
    add esp, 4
    pop es
    pop ds
    popa
    iret
%endmacro

EX 0   ; #DE
EX 1   ; #DB
EX 2
EX 3   ; #BP
EX 4
EX 5
EX 6   ; #UD
EX 7
EX 8   ; #DF (we won't see this if it triple-faults)
EX 9
EX 10
EX 11
EX 12
EX 13  ; #GP
EX 14  ; #PF
EX 15
EX 16
EX 17
EX 18
EX 19
EX 20
EX 21
EX 22
EX 23
EX 24
EX 25
EX 26
EX 27
EX 28
EX 29
EX 30
EX 31
