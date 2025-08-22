#pragma once
#include <stdint.h>

// Remap PIC to master base 0x20, slave base 0x28
void pic_remap(uint8_t master_offset, uint8_t slave_offset);

// Set the PIC interrupt masks (1 = masked/disabled, 0 = unmasked/enabled)
void pic_set_masks(uint8_t master_mask, uint8_t slave_mask);

// Send End-Of-Interrupt to PICs
void pic_send_eoi(uint8_t irq);
