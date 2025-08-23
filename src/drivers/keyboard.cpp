#include "keyboard.h"
#include "../utils/io.h"
#include "../utils/screen.h"
#include "../interrupts/irq.h"

#define KB_BUF_SIZE 256
static volatile uint8_t buf[KB_BUF_SIZE];
static volatile int head = 0, tail = 0;

static void kb_irq(Registers*) {
    uint8_t sc = inb(0x60);
    if(sc & 0x80) return;
    int next = (head + 1) % KB_BUF_SIZE;
    if(next != tail) {
        buf[head] = sc;
        head = next;
    }
}

int kb_get_scancode_nonblocking() {
    if (tail == head) return 0;
    int sc = buf[tail];
    tail = (tail + 1) % KB_BUF_SIZE;
    return sc;
}

void keyboard_init() {
    irq_register_handler(1, kb_irq);
}

const char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,'\\',
    'z','x','c','v','b','n','m',',','.','/', 0,'*', 0,' ',
    0
};
