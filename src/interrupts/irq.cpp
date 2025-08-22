#include "irq.h"
#include "idt.h"
#include "pic.h"
#include "../utils/screen.h"

extern "C" {
    void irq_32(); void irq_33(); void irq_34(); void irq_35();
    void irq_36(); void irq_37(); void irq_38(); void irq_39();
    void irq_40(); void irq_41(); void irq_42(); void irq_43();
    void irq_44(); void irq_45(); void irq_46(); void irq_47();

    void irq_handler_common(Registers* r);
}

static irq_handler_t handlers[16] = {0};

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if(irq < 16) handlers[irq] = handler;
}

extern "C" void irq_handler(Registers* r) {
    uint8_t vector = (uint8_t)r->int_no;
    if(vector < 0x20 || vector > 0x2F) return;
    uint8_t irq = (uint8_t)(vector - 0x20);

    if(handlers[irq]) handlers[irq](r);
    pic_send_eoi(irq);
}

void irq_install_idt_entries() {
    const uint16_t KCODE = 0x10;
    const uint8_t  GATE  = 0x8E;
    idt_set_gate(0x20, (uint32_t)irq_32, KCODE, GATE);
    idt_set_gate(0x21, (uint32_t)irq_33, KCODE, GATE);
    idt_set_gate(0x22, (uint32_t)irq_34, KCODE, GATE);
    idt_set_gate(0x23, (uint32_t)irq_35, KCODE, GATE);
    idt_set_gate(0x24, (uint32_t)irq_36, KCODE, GATE);
    idt_set_gate(0x25, (uint32_t)irq_37, KCODE, GATE);
    idt_set_gate(0x26, (uint32_t)irq_38, KCODE, GATE);
    idt_set_gate(0x27, (uint32_t)irq_39, KCODE, GATE);
    idt_set_gate(0x28, (uint32_t)irq_40, KCODE, GATE);
    idt_set_gate(0x29, (uint32_t)irq_41, KCODE, GATE);
    idt_set_gate(0x2A, (uint32_t)irq_42, KCODE, GATE);
    idt_set_gate(0x2B, (uint32_t)irq_43, KCODE, GATE);
    idt_set_gate(0x2C, (uint32_t)irq_44, KCODE, GATE);
    idt_set_gate(0x2D, (uint32_t)irq_45, KCODE, GATE);
    idt_set_gate(0x2E, (uint32_t)irq_46, KCODE, GATE);
    idt_set_gate(0x2F, (uint32_t)irq_47, KCODE, GATE);
}
