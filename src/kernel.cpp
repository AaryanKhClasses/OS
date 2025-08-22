#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./interrupts/irq.h"
#include "./drivers/keyboard.h"

extern "C" void kernel_main() {
    screen_init();
    clear_screen();

    set_color(LIGHT_CYAN, BLACK);
    println("Phase 2 - Steps 3..5: PIC + IRQ + Keyboard");
    set_color(LIGHT_GRAY, BLACK);

    // (From Step 2)
    idt_init();
    println("IDT with CPU exception ISRs installed.");

    // PIC remap to 0x20/0x28
    pic_remap(0x20, 0x28);
    println("PIC remapped to 0x20..0x2F.");

    // Install IRQ stubs in IDT
    irq_install_idt_entries();
    println("IRQ stubs 0x20..0x2F installed.");

    // Mask all except IRQ1 (keyboard) => master mask 1111 1101b = 0xFD
    // (Bit=1 masked/disabled; Bit 1 is IRQ1 => set to 0 to enable)
    pic_set_masks(0xFD, 0xFF);
    println("PIC masks set (only keyboard enabled).");

    // Keyboard
    keyboard_init();
    println("Keyboard handler installed (IRQ1). Enabling interrupts...");

    asm volatile("sti");
    println("STI done. Type keys:");

    while (true) {
        int sc;
        while ((sc = kb_get_scancode_nonblocking()) != 0) {
            char out = 0;
            if (sc < 128) out = scancode_to_ascii[sc];
            if (out) {
                char s[2] = { out, 0 };
                print(s);
            }
        }
        asm volatile("hlt");
    }
}
