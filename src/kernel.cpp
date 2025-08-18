#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./drivers/keyboard.h"

extern "C" int kb_getchar_nonblocking();

extern "C" void kernel_main() {
    screen_init();
    clear_screen();
    
    println("Boot OK! Setting Up IDT, PIC, and Keyboard...");
    idt_init();
    pic_remap(0x20, 0x28);
    pic_set_masks(0xFF, 0xFF);
    keyboard_init();

    pic_set_masks(0xFD, 0xFF);

    asm volatile("sti");
    println("STI Done!");
    set_color(GREEN, BLACK);
    println("Type something:");
    set_color(LIGHT_GRAY, BLACK);

    while(true) {
        int ch = kb_getchar_nonblocking();
        while(ch != 0) {
            char str[2] = { (char)ch, 0 };
            print(str);
            ch = kb_getchar_nonblocking();
        }
        asm volatile("hlt");
    }
}
