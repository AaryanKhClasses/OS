#include "idt.h"
#include <stdint.h>

extern "C" {
    void irq1_stub();

    void isr_0();  void isr_1();  void isr_2();
    void isr_3();  void isr_4();  void isr_5();
    void isr_6();  void isr_7();  void isr_8();
    void isr_9();  void isr_10(); void isr_11();
    void isr_12(); void isr_13(); void isr_14();
    void isr_15(); void isr_16(); void isr_17();
    void isr_18(); void isr_19(); void isr_20();
    void isr_21(); void isr_22(); void isr_23();
    void isr_24(); void isr_25(); void isr_26();
    void isr_27(); void isr_28(); void isr_29();
    void isr_30(); void isr_31();
}

struct __attribute__((packed)) IDTEntry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
};

static IDTEntry idt[256];

static inline void load_idt(void* idt_base, uint16_t size) {
    struct __attribute__((packed)) {
        uint16_t limit;
        uint32_t base;
    } idtr = { (uint16_t)(size), (uint32_t)idt_base };
    asm volatile("lidt %0" : : "m"(idtr));
}

void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low  = (uint16_t)(base & 0xFFFF);
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
    idt[num].base_high = (uint16_t)((base >> 16) & 0xFFFF);
}

void idt_init() {
    // Clear table
    for (int i = 0; i < 256; ++i) idt[i] = {0,0,0,0,0};

    // Exceptions 0..31
    idt_set_gate(0,  (uint32_t)isr_0,  0x08, 0x8E);
    idt_set_gate(1,  (uint32_t)isr_1,  0x08, 0x8E);
    idt_set_gate(2,  (uint32_t)isr_2,  0x08, 0x8E);
    idt_set_gate(3,  (uint32_t)isr_3,  0x08, 0x8E);
    idt_set_gate(4,  (uint32_t)isr_4,  0x08, 0x8E);
    idt_set_gate(5,  (uint32_t)isr_5,  0x08, 0x8E);
    idt_set_gate(6,  (uint32_t)isr_6,  0x08, 0x8E);
    idt_set_gate(7,  (uint32_t)isr_7,  0x08, 0x8E);
    idt_set_gate(8,  (uint32_t)isr_8,  0x08, 0x8E);
    idt_set_gate(9,  (uint32_t)isr_9,  0x08, 0x8E);
    idt_set_gate(10, (uint32_t)isr_10, 0x08, 0x8E);
    idt_set_gate(11, (uint32_t)isr_11, 0x08, 0x8E);
    idt_set_gate(12, (uint32_t)isr_12, 0x08, 0x8E);
    idt_set_gate(13, (uint32_t)isr_13, 0x08, 0x8E);
    idt_set_gate(14, (uint32_t)isr_14, 0x08, 0x8E);
    idt_set_gate(15, (uint32_t)isr_15, 0x08, 0x8E);
    idt_set_gate(16, (uint32_t)isr_16, 0x08, 0x8E);
    idt_set_gate(17, (uint32_t)isr_17, 0x08, 0x8E);
    idt_set_gate(18, (uint32_t)isr_18, 0x08, 0x8E);
    idt_set_gate(19, (uint32_t)isr_19, 0x08, 0x8E);
    idt_set_gate(20, (uint32_t)isr_20, 0x08, 0x8E);
    idt_set_gate(21, (uint32_t)isr_21, 0x08, 0x8E);
    idt_set_gate(22, (uint32_t)isr_22, 0x08, 0x8E);
    idt_set_gate(23, (uint32_t)isr_23, 0x08, 0x8E);
    idt_set_gate(24, (uint32_t)isr_24, 0x08, 0x8E);
    idt_set_gate(25, (uint32_t)isr_25, 0x08, 0x8E);
    idt_set_gate(26, (uint32_t)isr_26, 0x08, 0x8E);
    idt_set_gate(27, (uint32_t)isr_27, 0x08, 0x8E);
    idt_set_gate(28, (uint32_t)isr_28, 0x08, 0x8E);
    idt_set_gate(29, (uint32_t)isr_29, 0x08, 0x8E);
    idt_set_gate(30, (uint32_t)isr_30, 0x08, 0x8E);
    idt_set_gate(31, (uint32_t)isr_31, 0x08, 0x8E);

    idt_set_gate(0x21, (uint32_t)irq1_stub, 0x08, 0x8E);

    // Load IDT with the *address of the table*
    asm volatile("cli");
    load_idt(idt, sizeof(idt) - 1);
}
