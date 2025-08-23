#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./interrupts/irq.h"
#include "./drivers/keyboard.h"
#include "./drivers/pit.h"

extern "C" void kernel_main() {
    screen_init();
    clear_screen();

    set_color(GREEN, BLACK);
    println("Phase 3 - Step 1: Programmable Interval Timer");
    set_color(LIGHT_GRAY, BLACK);

    idt_init();
    pic_remap(0x20, 0x28);
    irq_install_idt_entries();
    pic_set_masks(0xFC, 0xFF);
    keyboard_init();
    pit_init(100);

    asm volatile("sti");
    println("STI done & PIT at 100Hz. Type keys:");

    // Demo: every second print ticks and blink cursor
    uint32_t last = pit_get_ticks();
    while (true) {
        // Poll keyboard and print characters (main thread)
        int sc;
        while ((sc = kb_get_scancode_nonblocking()) != 0) {
            char ch = 0;
            if (sc < 128) ch = scancode_to_ascii[sc];
            if (ch) {
                char s[2] = { ch, 0 };
                print(s);
            }
        }

        // Print tick every 100 ticks (100Hz -> 1 second)
        uint32_t now = pit_get_ticks();
        if (now - last >= 100) {
            last = now;
            set_color(LIGHT_GREEN, BLACK);
            print(" ticks=");
            char buf[32];
            uint32_t v = now;
            int pos = 0;
            if (v == 0) buf[pos++] = '0';
            else {
                char tmp[32]; int t=0;
                while (v) { tmp[t++]= '0' + (v % 10); v/=10; }
                while (t--) buf[pos++]=tmp[t];
            }
            buf[pos]=0;
            print(buf);
            set_color(LIGHT_GRAY, BLACK);
            print("\n");
        }

        asm volatile("hlt");
    }
}
