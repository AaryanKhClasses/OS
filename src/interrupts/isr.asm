[bits 32]

%assign i 0
%rep 32
    %xdefine isr_name isr_%+i
    global isr_name
%assign i i+1
%endrep

extern isr_handler

%macro ISR_NOERR 2
%1:
    push dword 0    ; fake error code
    push dword %2   ; interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERR 2
%1:
    push dword %2   ; interrupt number
    jmp isr_common
%endmacro

isr_common:
    pusha           ; Save all general-purpose registers
    ; Save segment registers
    push ds
    push es
    push fs
    push gs

    mov ax, 0x18    ; Load the kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push eax
    call isr_handler
    add esp, 4

    ; Restore all segment & registers
    pop gs
    pop fs
    pop es
    pop ds
    popa

    add esp, 8
    iret

; Non Error ISRs
ISR_NOERR isr_0,  0     ; #DE: Divide Error
ISR_NOERR isr_1,  1     ; #DB: Debug Exception
ISR_NOERR isr_2,  2     ; #NMI: Non-maskable Interrupt
ISR_NOERR isr_3,  3     ; #BP: Breakpoint Exception
ISR_NOERR isr_4,  4     ; #OF: Overflow Exception
ISR_NOERR isr_5,  5     ; #BR: Bound Range Exceeded
ISR_NOERR isr_6,  6     ; #UD: Undefined Opcode
ISR_NOERR isr_7,  7     ; #NM: Device Not Available
ISR_NOERR isr_9,  9     ; #MF: Core processor segment overrun
ISR_NOERR isr_16, 16    ; #MF: x87 Floating Point Exception
ISR_NOERR isr_18, 18    ; #MC: Machine Check Exception
ISR_NOERR isr_19, 19    ; #XF: SIMD Floating Point Exception
ISR_NOERR isr_20, 20    ; #VE: Virtualization Exception
ISR_NOERR isr_30, 30    ; #SX: Security Exception

; Error ISRs
ISR_ERR isr_8,  8       ; #DF: Double Fault
ISR_ERR isr_10, 10      ; #TS: Invalid TSS
ISR_ERR isr_11, 11      ; #NP: Segment Not Present
ISR_ERR isr_12, 12      ; #SS: Stack Segment Fault
ISR_ERR isr_13, 13      ; #GP: General Protection Fault
ISR_ERR isr_14, 14      ; #PF: Page Fault
ISR_ERR isr_17, 17      ; #AC: Alignment Check Exception

; Reserved ISRs
ISR_NOERR isr_15, 15
ISR_NOERR isr_21, 21
ISR_NOERR isr_22, 22
ISR_NOERR isr_23, 23
ISR_NOERR isr_24, 24
ISR_NOERR isr_25, 25
ISR_NOERR isr_26, 26
ISR_NOERR isr_27, 27
ISR_NOERR isr_28, 28
ISR_NOERR isr_29, 29
ISR_NOERR isr_31, 31
