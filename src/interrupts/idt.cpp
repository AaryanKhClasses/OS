#include "idt.h"
#include <stdint.h>

extern "C" void irq1_stub();

struct __attribute__((packed)) IDTEntry {
    uint16_t base_low;
    uint16_t sel;       // GDT (Global Descriptor Table) Selector
    uint8_t always0;
    uint8_t flags;      // Type + Attributes
    uint16_t base_high;
};

struct __attribute__((packed)) IDTPtr {
    uint16_t limit;
    uint32_t base;
};

static IDTEntry idt[256];
static IDTPtr idt_ptr;

static inline void load_idt(void* base, uint16_t size) {
    struct __attribute__((packed)) {
        uint16_t length;
        uint32_t base;
    } IDTR = { size, (uint32_t)base };
    asm volatile("lidt %0" : : "m"(IDTR));
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (uint16_t)(base & 0xFFFF);
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
    idt[num].base_high = (uint16_t)((base >> 16) & 0xFFFF);
}

void idt_init() {
    for(int i = 0; i < 256; i++) idt[i] = {0, 0, 0, 0, 0};
    idt_set_gate(0x21, (uint32_t)irq1_stub, 0x08, 0x8E);
    // 0x21: Keyboard Interrupt
    // 0x08: Code Segment Selector
    // 0x8E: Present, Ring 0, Interrupt Gate

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt[0];
    asm volatile("cli"); // clear interrupts
    load_idt(&idt, sizeof(idt) - 1);
}
