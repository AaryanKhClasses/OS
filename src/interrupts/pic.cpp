#include "pic.h"
#include "../utils/io.h"

#define PIC1        0x20
#define PIC2        0xA0
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1

#define ICW1_INIT   0x10
#define ICW1_ICW4   0x01
#define ICW4_8086   0x01

static inline void io_wait() { outb(0x80, 0); }

void pic_remap(uint8_t master_offset, uint8_t slave_offset) {
    uint8_t a1 = inb(PIC1_DATA);
    uint8_t a2 = inb(PIC2_DATA);

    outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, master_offset); // ICW2: Master PIC vector offset
    io_wait();
    outb(PIC2_DATA, slave_offset);  // ICW2: Slave PIC vector offset
    io_wait();

    outb(PIC1_DATA, 0x04); // ICW3: tell Master that Slave is at IRQ2 (0000 0100)
    io_wait();
    outb(PIC2_DATA, 0x02); // ICW3: tell Slave its cascade identity (0000 0010)
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // Restore saved masks
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void pic_set_masks(uint8_t master_mask, uint8_t slave_mask) {
    outb(PIC1_DATA, master_mask);
    outb(PIC2_DATA, slave_mask);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) outb(PIC2_CMD, 0x20);
    outb(PIC1_CMD, 0x20);
}
