#include "heap.h"
#include "pmm.h"
#include <stdint.h>
#include "../utils/string.h"

#define ALIGN_UP(x, a) ((((uintptr_t)(x)) + ((a)-1)) & ~((a)-1))

struct Block {
    uint32_t size;
    uint8_t free;
    uint8_t reserved[3]; // padding for alignment
    Block* prev;
    Block* next;
};

static const size_t HEADER_SIZE = ALIGN_UP(sizeof(Block), 16);

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
static Block* free_list_head = nullptr;

static inline Block* ptr_to_block(void* ptr) {
    return (Block*)((uint8_t*)ptr - HEADER_SIZE);
}

static inline void* block_to_ptr(Block* block) {
    return (void*)((uint8_t*)block + HEADER_SIZE);
}

static void insert_free_block(Block* block) {
    block->free = 1;
    block->prev = nullptr;
    block->next = free_list_head;
    if(free_list_head) free_list_head->prev = block;
    free_list_head = block;
}

static void remove_free_block(Block* block) {
    if(block->prev) block->prev->next = block->next;
    if(block->next) block->next->prev = block->prev;
    if(block == free_list_head) free_list_head = block->next;
    block->free = 0;
    block->prev = nullptr;
    block->next = nullptr;
}

static void try_coalesce(Block* block) {
    Block* next = block->next;
    uint8_t* block_end = (uint8_t*)block + block->size;
    for(Block* it = free_list_head; it; it = it->next) {
        if(it == block) continue;
        if((uint8_t*)it == block_end) {
            block->size += it->size;
            remove_free_block(it);
            break;
        }
    }

    for(Block* it = free_list_head; it; it = it->next) {
        if(it == block) continue;
        uint8_t* it_end = (uint8_t*)it + it->size;
        if(it_end == (uint8_t*)block) {
            it->size += block->size;
            remove_free_block(block);
            block = it;
            break;
        }
    }
}

void heap_init(uint32_t base, size_t size) {
    heap_base = (uint8_t*)(uintptr_t)base;
    heap_used = 0;
    heap_size = size;
    if(heap_base && heap_size) {
        memset(heap_base, 0, heap_size);
        Block* block = (Block*)heap_base;
        block->size = (uint32_t)heap_size;
        block->free = 1;
        block->prev = block->next = nullptr;
        free_list_head = block;
    }
}

void* kmalloc(size_t size) {
    if(!heap_base || size == 0) return nullptr;

    size_t need = ALIGN_UP(size, 8);
    size_t total = need + HEADER_SIZE;

    for(Block* it = free_list_head; it; it = it->next) {
        if(it->size >= total) {
            size_t rem = it->size - total;
            if(rem >= HEADER_SIZE + 16) {
                Block* new_block = (Block*)((uint8_t*)it + total);
                new_block->size = (uint32_t)rem;
                new_block->free = 1;
                new_block->prev = it->prev;
                new_block->next = it->next;

                if(new_block->prev) new_block->prev->next = new_block;
                else free_list_head = new_block;
                if(new_block->next) new_block->next->prev = new_block;

                it->size = (uint32_t)total;
                it->free = 0;
                it->prev = it->next = nullptr;
                return block_to_ptr(it);
            } else {
                remove_free_block(it);
                return block_to_ptr(it);
            }
        }
    }
    
    size_t pages_needed = (total + 0xFFF) / 0x1000;
    void* more = kmalloc_pages(pages_needed);
    if(!more) return nullptr;
    kfree_pages(more, pages_needed);
    return nullptr;
}

void kfree(void* ptr) {
    if(!ptr) return;
    Block* block = ptr_to_block(ptr);
    if((uint8_t*)block < heap_base || (uint8_t*)block >= heap_base + heap_size) return;
    insert_free_block(block);
    try_coalesce(block);
}
