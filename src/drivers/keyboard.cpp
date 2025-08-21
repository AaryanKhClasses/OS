// #include "keyboard.h"
// #include "../utils/io.h"
// #include "../utils/screen.h"
// #include <stdint.h>

// static const char keymap[128] = {
//     /*0x00*/ 0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
//     /*0x0F*/ '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
//     /*0x20*/ 'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
//     /*0x2E*/ 'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
//     /*rest*/ 0
// };

// void keyboard_init() {
//     // extern void pic_set_masks(uint8_t master_mask, uint8_t slave_mask);
//     // pic_set_masks(0xFD, 0xFF);
// }

// extern "C" void irq1_handler() {
//     uint8_t scancode = inb(0x60); // Read scancode from keyboard controller
//     if(scancode & 0x80) return; // Ignore key releases

//     char ch = 0;
//     if(scancode < 128) ch = keymap[scancode];

//     if(ch) {
//         char s[2] = { ch, 0 };
//         print(s);
//     }
// }

// #include "keyboard.h"
// #include "../utils/io.h"
// #include <stdint.h>

// #define KB_BUF_SIZE 256
// static volatile uint8_t kb_buf[KB_BUF_SIZE];
// static volatile int kb_head = 0;
// static volatile int kb_tail = 0;

// // Very small, IRQ-safe handler: only port read + store into small .bss buffer.
// // No function calls, no pointers to large structures, no screen writes.
// extern "C" void irq1_handler() {
//     uint8_t sc = inb(0x60);        // MUST read the controller to acknowledge event

//     // Ignore releases
//     if (sc & 0x80) return;

//     int next = (kb_head + 1) % KB_BUF_SIZE;
//     if (next != kb_tail) {         // avoid buffer overflow
//         kb_buf[kb_head] = sc;      // store scancode (not ASCII)
//         kb_head = next;
//     }
// }

// // Poller for main loop: returns 0 if empty, otherwise returns scancode
// extern "C" int kb_get_scancode_nonblocking() {
//     if (kb_tail == kb_head) return 0;
//     int sc = kb_buf[kb_tail];
//     kb_tail = (kb_tail + 1) % KB_BUF_SIZE;
//     return sc;
// }

// void keyboard_init() {
//     // nothing required for now
// }


#include "keyboard.h"
#include "../utils/io.h"
#include <stdint.h>

#define KB_BUF_SIZE 32
static volatile uint8_t kb_buf[KB_BUF_SIZE];
static volatile int kb_head = 0;
static volatile int kb_tail = 0;

extern "C" void irq1_handler() {
    uint8_t sc = inb(0x60);   // ALWAYS read the port
    if (sc & 0x80) return;    // ignore key release

    int next = (kb_head + 1) % KB_BUF_SIZE;
    if (next != kb_tail) {
        kb_buf[kb_head] = sc;
        kb_head = next;
    }
}

extern "C" int kb_get_scancode_nonblocking() {
    if (kb_tail == kb_head) return 0;
    int sc = kb_buf[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUF_SIZE;
    return sc;
}

void keyboard_init() { }

const char scancode_to_ascii[128] = {
    /*0x00*/ 0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    /*0x0F*/ '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    /*0x20*/ 'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    /*0x2E*/ 'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
    /*rest*/ 0
};
