#include "paging.h"
#include <stdint.h>

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_tables[4][1024] __attribute__((aligned(4096)));

static inline void write_cr3(uint32_t value) {
    asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
}

static inline uint32_t read_cr0() {
    uint32_t value;
    asm volatile("mov %%cr0, %0" : "=r"(value));
    return value;
}

static inline void write_cr0(uint32_t value) {
    asm volatile("mov %0, %%cr0" :: "r"(value) : "memory");
}

void paging_init_identity_16mb() {
    // Zero Directory and Table
    for(int i = 0; i < 1024; ++i) page_directory[i] = 0x00000002;

    // Setup first page directory entry to point to the first page table
    const uint32_t flags = 0x3; // Present, Read/Write
    for(int t = 0; t < 4; ++t) {
        for(uint32_t i = 0; i < 1024; ++i) {
            uint32_t phys = (t * 0x400000) + (i * 0x1000); // 4KB per page
            page_tables[t][i] = (phys & 0xFFFFF000) | flags;
        }
        page_directory[t] = ((uint32_t)page_tables[t] & 0xFFFFF000) | flags;
    }

    write_cr3((uint32_t)page_directory);
    uint32_t cr0 = read_cr0();
    cr0 |= 0x80000000; // Enable paging
    write_cr0(cr0);
}
