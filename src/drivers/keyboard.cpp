#include "keyboard.h"
#include "../utils/io.h"
#include <stdint.h>

#define KB_BUFFER_SIZE 256
static volatile char kb_buffer[KB_BUFFER_SIZE];
static volatile int kb_head = 0;
static volatile int kb_tail = 0;

static const char keymap[128] = {
    /*0x00*/ 0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    /*0x0F*/ '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    /*0x20*/ 'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    /*0x2E*/ 'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
    /*rest*/ 0
};

void keyboard_init() {
    extern void pic_set_masks(uint8_t master_mask, uint8_t slave_mask);
    pic_set_masks(0xFD, 0xFF);
}

extern "C" void irq1_handler() {
    uint8_t scancode = inb(0x60); // Read scancode from keyboard controller
    if(scancode & 0x80) return; // Ignore key releases

    char ch = 0;
    if(scancode < 128) ch = keymap[scancode];

    if(ch) {
        int next = (kb_head + 1) % KB_BUFFER_SIZE;
        if(next != kb_tail) {
            kb_buffer[kb_head] = ch;
            kb_head = next;
        }
    }
}

extern "C" int kb_getchar_nonblocking() {
    if(kb_head == kb_tail) return 0; // Empty
    char ch = kb_buffer[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUFFER_SIZE;
    return (int)ch;
}
