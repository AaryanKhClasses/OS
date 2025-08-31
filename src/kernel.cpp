#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./interrupts/irq.h"
#include "./drivers/keyboard.h"
#include "./drivers/pit.h"
#include "./scheduler/scheduler.h"

static void task_blink(void*) {
    uint32_t n = 0;
    while (1) {
        set_color(LIGHT_GREEN, BLACK);
        print("blink ");
        char buf[16]; int p=0; uint32_t v=n++;
        if (v==0) buf[p++]='0'; else { char t[16]; int k=0; while(v){ t[k++]='0'+(v%10); v/=10; } while(k--) buf[p++]=t[k]; }
        buf[p]=0;
        print(buf);
        set_color(LIGHT_GRAY, BLACK);
        print("\n");
        task_sleep_ms(500);
    }
}

static void task_kbd(void*) {
    print("[kbd] ready\n");
    while (1) {
        int sc;
        bool printed = false;
        while ((sc = kb_get_scancode_nonblocking()) != 0) {
            char ch = 0;
            if (sc < 128) ch = scancode_to_ascii[sc];
            if (ch) {
                char s[2] = { ch, 0 };
                print(s);
                printed = true;
            }
        }
        if (!printed) yield();
    }
}

extern "C" void kernel_main() {
    screen_init();
    clear_screen();

    println("Phase 3: Cooperative scheduler demo");

    idt_init();
    pic_remap(0x20, 0x28);
    irq_install_idt_entries();
    pic_set_masks(0xFC, 0xFF);

    keyboard_init();
    pit_init(100); // 100 Hz

    asm volatile("sti");

    // Scheduler
    scheduler_init();
    task_create(task_blink, nullptr, "blinker");
    task_create(task_kbd,   nullptr, "keyboard");
    println("Starting scheduler...");
    scheduler_start(); // never returns
}
