#include "./utils/screen.h"
#include "./interrupts/idt.h"
#include "./mem/pmm.h"
#include "./mem/paging.h"
#include "./mem/heap.h"
#include "./utils/string.h"

extern "C" void kernel_main() {
    screen_init();
    clear_screen();
    set_color(LIGHT_CYAN, BLACK);
    println("Phase 4: Kernel Heap Allocator Test");

    // ---- Step 1: Init interrupts + paging ----
    idt_init();
    paging_init_identity_16mb();

    // ---- Step 2: Init PMM (4–16MB usable region) ----
    pmm_init(0x00400000, 0x01000000);
    char buf[64];
    int_to_str(pmm_total_frames(), buf);
    print("PMM: total frames="); println(buf);
    int_to_str(pmm_free_frames(), buf);
    print("PMM: free frames="); println(buf);

    // ---- Step 3: Init Heap ----
    heap_init(0x00800000, 0x00100000);
    println("Heap initialized at 0x00800000 size 1MiB");

    // ---- Step 4: Allocations ----
    void* a = kmalloc(32);
    void* b = kmalloc(128);
    void* c = kmalloc(256);
    print("a="); hex_to_str((uint32_t)a, buf); println(buf);
    print("b="); hex_to_str((uint32_t)b, buf); println(buf);
    print("c="); hex_to_str((uint32_t)c, buf); println(buf);

    // ---- Step 5: Free + reuse ----
    println("Freeing a and c...");
    kfree(a);
    kfree(c);

    void* d = kmalloc(16);
    print("d="); hex_to_str((uint32_t)d, buf); println(buf);

    void* e = kmalloc(200);
    print("e="); hex_to_str((uint32_t)e, buf); println(buf);

    // ---- Step 6: Stress test ----
    println("Allocating many small blocks...");
    for (int i = 0; i < 10; i++) {
        void* p = kmalloc(40);
        print("blk[");
        int_to_str(i, buf);
        print(buf);
        print("]=");
        hex_to_str((uint32_t)p, buf);
        println(buf);
    }

    println("Heap test complete.");
    set_color(LIGHT_GRAY, BLACK);

    while (1) asm volatile("hlt");
}
