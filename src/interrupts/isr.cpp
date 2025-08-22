#include "isr.h"
#include "../utils/screen.h"

static const char* exception_names[32] = {
    "Divide-by-zero",          "Debug",                 "Non-maskable interrupt",
    "Breakpoint",              "Overflow",              "BOUND Range Exceeded",
    "Invalid Opcode",          "Device Not Available",  "Double Fault",
    "Coprocessor Overrun",     "Invalid TSS",           "Segment Not Present",
    "Stack Fault",             "General Protection",    "Page Fault",
    "Reserved (15)",           "x87 Floating-Point",    "Alignment Check",
    "Machine Check",           "SIMD Floating-Point",   "Virtualization",
    "Reserved (21)",           "Reserved (22)",         "Reserved (23)",
    "Reserved (24)",           "Reserved (25)",         "Reserved (26)",
    "Reserved (27)",           "Reserved (28)",         "Reserved (29)",
    "Security Exception",      "Reserved (31)"
};

static void print_hex32(uint32_t v) {
    const char* H = "0123456789ABCDEF";
    char buf[] = "0x00000000";
    buf[2] = H[(v >> 28) & 0xF];
    buf[3] = H[(v >> 24) & 0xF];
    buf[4] = H[(v >> 20) & 0xF];
    buf[5] = H[(v >> 16) & 0xF];
    buf[6] = H[(v >> 12) & 0xF];
    buf[7] = H[(v >> 8)  & 0xF];
    buf[8] = H[(v >> 4)  & 0xF];
    buf[9] = H[(v >> 0)  & 0xF];
    print(buf);
}

extern "C" void isr_handler(Registers* r) {
    set_color(LIGHT_RED, BLACK);
    print("EXCEPTION ");
    print_hex32(r->int_no);
    print(": ");
    println(exception_names[r->int_no < 32 ? r->int_no : 31]);

    set_color(LIGHT_GRAY, BLACK);
    print("err=");
    print_hex32(r->err_code);
    print("  eip=");
    print_hex32(r->eip);
    print("  cs=");
    print_hex32(r->cs);
    print("  eflags=");
    print_hex32(r->eflags);
    println("");

    println("System halted (exceptions installed).");
    while (true) asm volatile("hlt");
}
