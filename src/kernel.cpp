#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./drivers/keyboard.h"

extern "C" void kernel_main() {
    screen_init();
    clear_screen();
    
    println("Boot OK! Setting Up IDT, PIC, and Keyboard...");
    idt_init();
    pic_remap(0x20, 0x28);
    pic_set_masks(0xFD, 0xFF);
    keyboard_init();

    asm volatile("sti");
    println("STI Done!");
    set_color(GREEN, BLACK);
    println("Press any key: ");
    set_color(LIGHT_GRAY, BLACK);
    
    while (true) {
        int sc;
        while ((sc = kb_get_scancode_nonblocking()) != 0) {
            char out = '?';
            if (sc < 128) {
                out = scancode_to_ascii[sc];
            }
            if (out) {
                char s[2] = { out, 0 };
                print(s);
            }
        }
        asm volatile("hlt");
    }
}
