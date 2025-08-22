#pragma once
#include <stdint.h>
#include "isr.h"

void irq_install_idt_entries();

typedef void (*irq_handler_t)(Registers* r);
void irq_register_handler(uint8_t irq, irq_handler_t handler);
