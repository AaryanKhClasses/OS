section .multiboot
align 4
    dd 0x1BADB002       ; Multiboot magic number
    dd 0x0              ; flags
    dd -(0x1BADB002)    ; checksum (magic + flags + checksum = 0)

section .text
global _start
_start:
    extern kernel_main
    call kernel_main
    cli

.hang:
    hlt
    jmp .hang
