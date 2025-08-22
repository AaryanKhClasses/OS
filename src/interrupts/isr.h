#pragma once
#include <stdint.h>

struct Registers {
    uint32_t ds, es, fs, gs;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
};

extern "C" void isr_handler(Registers* r);
