#include "scheduler.h"
#include "../utils/screen.h"
#include "../drivers/pit.h"
#include "../drivers/keyboard.h"

extern "C" void ctx_switch(uint32_t* save_esp_ptr, uint32_t new_esp);

#define MAX_TASKS 8
#define STACK_SIZE 4096 // 4KiB per task

enum TaskState {
    TASK_UNUSED = 0,
    TASK_RUNNABLE,
    TASK_SLEEPING,
    TASK_ZOMBIE
};

struct Task {
    uint32_t esp;
    uint8_t stack[STACK_SIZE];
    TaskState state;
    uint32_t wake_tick;
    task_fn_t entry;
    void* arg;
    const char* name;
};

static Task tasks[MAX_TASKS];
static int current_task = -1;
static int task_count = 0;

static void task_trampoline();
static int pick_next_runnable();

// Utility function to round up ms to ticks
static inline uint32_t ms_to_ticks(uint32_t ms) {
    return (ms + 9) / 10; // Assuming PIT frequency is 100Hz (10ms per tick)
}

void scheduler_init() {
    for(int i = 0; i < MAX_TASKS; i++) {
        tasks[i].state = TASK_UNUSED;
        tasks[i].esp = 0;
        tasks[i].wake_tick = 0;
        tasks[i].entry = nullptr;
        tasks[i].arg = nullptr;
        tasks[i].name = nullptr;
    }
    current_task = -1;
    task_count = 0;
}

static void setup_initial_stack(Task& task) {
    uint32_t* sp = (uint32_t*)(task.stack + STACK_SIZE);
    // Layout: [edi][esi][ebx][ebp][return_eip]
    *--sp = 0; // edi
    *--sp = 0; // esi
    *--sp = 0; // ebx
    *--sp = 0; // ebp
    *--sp = (uint32_t)task_trampoline; // return address (e
    task.esp = (uint32_t)sp;
}

int task_create(task_fn_t entry, void* arg, const char* name) {
    if(!entry) return -1;
    int slot = -1;
    for(int i = 0; i < MAX_TASKS; ++i) {
        if(tasks[i].state == TASK_UNUSED) {
            slot = i;
            break;
        }
    }
    if(slot < 0) return -1; // No free slot

    Task& task = tasks[slot];
    task.state = TASK_RUNNABLE;
    task.wake_tick = 0;
    task.entry = entry;
    task.arg = arg;
    task.name = name;

    setup_initial_stack(task);
    ++task_count;
    return slot;
}

static void task_exit() {
    Task& curr = tasks[current_task];
    curr.state = TASK_ZOMBIE;

    set_color(LIGHT_RED, BLACK);
    print("[scheduler] Task Finished: ");
    print(curr.name ? curr.name : "unnamed");
    print("\n");
    set_color(LIGHT_GRAY, BLACK);

    int next = pick_next_runnable();
    if(next < 0) {
        print("[scheduler] No more runnable tasks. Halting.\n");
        while(1) asm volatile("hlt");
    }

    uint32_t* save_esp = &curr.esp;
    uint32_t new_esp = tasks[next].esp;
    current_task = next;
    ctx_switch(save_esp, new_esp);
    while(1) asm volatile("hlt");
}

// DEBUG
static inline void vga_put(char c, int r, int col) {
    volatile uint8_t* v = (uint8_t*)0xB8000;
    int idx = (r * 80 + col) * 2;
    v[idx] = (uint8_t)c;
    v[idx+1] = 0x0E; // color
}

static void task_trampoline() {
    vga_put('X', 4, 5); // DEBUG
    asm volatile("sti");
    Task& curr = tasks[current_task];
    if(curr.entry) curr.entry(curr.arg);
    task_exit();
}

static int pick_next_runnable() {
    if(task_count == 0) return -1;
    int start = current_task;
    for(int step = 0; step < MAX_TASKS; ++step) {
        int i = (start + 1 + step) % MAX_TASKS;
        if(tasks[i].state == TASK_RUNNABLE) return i;
        if(tasks[i].state == TASK_SLEEPING) {
            if(pit_get_ticks() >= tasks[i].wake_tick) {
                tasks[i].state = TASK_RUNNABLE;
                return i;
            }
        }
    }
    return -1;
}

void yield() {
    int next = pick_next_runnable();
    if(next < 0 || next == current_task) return;

    Task& curr = tasks[current_task];
    Task& nxt = tasks[next];

    uint32_t* save_esp = &curr.esp;
    uint32_t new_esp = nxt.esp;
    current_task = next;
    ctx_switch(save_esp, new_esp);
}

void task_sleep_ms(uint32_t ms) {
    Task& curr = tasks[current_task];
    curr.wake_tick = pit_get_ticks() + ms_to_ticks(ms);
    curr.state = TASK_SLEEPING;
    yield();
}

void scheduler_start() {
    int first = -1;
    for(int i = 0; i < MAX_TASKS; ++i) {
        if(tasks[i].state == TASK_RUNNABLE) {
            first = i;
            break;
        }
    }
    
    if(first < 0) {
        set_color(LIGHT_RED, BLACK);
        print("[scheduler] No runnable tasks to start.\n");
        set_color(LIGHT_GRAY, BLACK);
        while(1) asm volatile("hlt");
    }
    
    current_task = first;

    // DEBUG
    // char buf[32];
    // uint32_t spval = tasks[first].esp;
    // const char* hex = "0123456789ABCDEF";
    // buf[0] = '0'; buf[1] = 'x';
    // for (int i=0;i<8;i++) {
    //     buf[2+i] = hex[(spval >> (28 - 4*i)) & 0xF];
    // }
    // buf[10] = 0;
    // print("About to switch to esp=");
    // print(buf);
    // print("\n");
    // END DEBUG
    
    asm volatile("cli");
    uint32_t dummy_save = 0;
    ctx_switch(&dummy_save, tasks[first].esp);
    while(1) asm volatile("hlt");
}
