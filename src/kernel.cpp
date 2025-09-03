#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./interrupts/pic.h"
#include "./interrupts/irq.h"
#include "./drivers/keyboard.h"
#include "./drivers/pit.h"
#include "./scheduler/scheduler.h"
#include "./mem/paging.h"
#include "./mem/pmm.h"
#include "./mem/heap.h"

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
    paging_init_identity_16mb();
    pmm_init(0x00400000, 0x01000000);
    heap_init(0x00800000, 0x00100000);

    {
        char buf[64];
        uint32_t total = pmm_total_frames();
        uint32_t freec = pmm_free_frames();
        int pos = 0;
        uint32_t v = total;
        char tmp[16];
        if (v == 0) tmp[pos++]='0';
        else { int t=0; while(v){ tmp[t++]= '0' + (v % 10); v/=10; } while(t--) buf[pos++]=tmp[t]; }
        buf[pos]=0;
        print(" total frames: "); print(buf);
        pos = 0; v = freec;
        if (v == 0) tmp[pos++]='0';
        else { int t=0; while(v){ tmp[t++]= '0' + (v % 10); v/=10; } while(t--) tmp[pos++]=tmp[t]; }
        tmp[pos]=0;
        print(" free frames: "); print(tmp); print("\n");
    }

    void* p = kmalloc_pages(1);
    if (p) {
        print("kmalloc_pages got: ");
        char hex[11] = "0x00000000"; const char* H="0123456789ABCDEF";
        uint32_t addr = (uint32_t)(uintptr_t)p;
        for(int i=0;i<8;i++) hex[2+i]=H[(addr>>(28-4*i))&0xF];
        print(hex); print("\n");
    }

    void* b = kmalloc(64);
    if (b) {
        print("kmalloc(64) -> "); char hex2[11]="0x00000000";
        uint32_t addr2=(uint32_t)(uintptr_t)b;
        const char* H="0123456789ABCDEF";
        for(int i=0;i<8;i++) hex2[2+i]=H[(addr2>>(28-4*i))&0xF];
        print(hex2); print("\n");
    }

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
