#include "heap.h"
#include "pmm.h"
#include <stdint.h>
#include "../utils/string.h"

void* kmalloc_pages(size_t size) {
    uint32_t p = alloc_frames(size);
    if(p == 0) return nullptr;
    return (void*)(uintptr_t)p;
}

void kfree_pages(void* ptr, size_t size) {
    if(!ptr) return;
    free_frames((uint32_t)(uintptr_t)ptr, size);
}

static uint8_t* heap_base = nullptr;
static size_t heap_used = 0;
static size_t heap_size = 0;

void heap_init(uint32_t base, size_t size) {
    heap_base = (uint8_t*)(uintptr_t)base;
    heap_used = 0;
    heap_size = size;
}

void* kmalloc(size_t size) {
    if(!heap_base) return nullptr;
    size = (size + 7) & ~7; // align to 8 bytes
    if(heap_used + size > heap_size) return nullptr;
    void* ptr = heap_base + heap_used;
    heap_used += size;
    memset(ptr, 0, size);
    return ptr;
}
