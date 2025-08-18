#include "pic.h"
#include "../utils/io.h"

void pic_remap(uint8_t offset1, uint8_t offset2) {
    uint8_t a1 = inb(0x21); // Save old mask for master
    uint8_t a2 = inb(0xA1); // Save old mask for slave

    outb(0x20, 0x11); // Start initialization sequence for master PIC
    outb(0xA0, 0x11); // Start initialization sequence for slave PIC

    outb(0x21, offset1); // Set vector offset for master PIC
    outb(0xA1, offset2); // Set vector offset for slave PIC

    outb(0x21, 0x04); // Tell master PIC that there is a slave PIC at IRQ2
    outb(0xA1, 0x02); // Tell slave PIC its cascade identity

    outb(0x21, 0x01); // Set master PIC to 8086/88 mode
    outb(0xA1, 0x01); // Set slave PIC to 8086/88 mode

    outb(0x21, a1); // Restore master mask
    outb(0xA1, a2); // Restore slave mask
}

void pic_set_masks(uint8_t master_mask, uint8_t slave_mask) {
    outb(0x21, master_mask);
    outb(0xA1, slave_mask);
}
