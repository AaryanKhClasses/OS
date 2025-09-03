#include "pmm.h"
#include <stdint.h>
#include "../utils/string.h"

#define MAX_FRAMES 4096
#define BITMAP_BYTES (MAX_FRAMES / 8)

static uint8_t pmm_bitmap[BITMAP_BYTES] __attribute__((aligned(4096)));
static uint32_t PMM_BASE = 0x00400000; // 4MiB
static uint32_t PMM_END = 0x01000000; // 16MiB

static uint32_t frame_count = 0;
static uint32_t region_base = 0;
static uint32_t region_end = 0;
static uint32_t first_frame_index = 0;

static inline void bitmap_set(uint32_t index) {
    pmm_bitmap[index >> 3] |= (1 << (index & 7));
}

static inline void bitmap_clear(uint32_t index) {
    pmm_bitmap[index >> 3] &= ~(1 << (index & 7));
}

static inline int bitmap_test(uint32_t index) {
    return (pmm_bitmap[index >> 3] >> (index & 7)) & 1;
}

void pmm_init(uint32_t base, uint32_t end) {
    base = (base + 0xFFF) & ~0xFFFu;
    end &= ~0xFFFu;
    if(end <= base) return;

    region_base = base;
    region_end = end;
    uint32_t bytes = region_end  - region_base;
    frame_count = bytes / 0x1000;
    if(frame_count > MAX_FRAMES) frame_count = MAX_FRAMES;
    memset(pmm_bitmap, 0, BITMAP_BYTES);
}

static void mark_frame_allocated_by_index(uint32_t index) {
    if(index < frame_count) bitmap_set(index);
}

static void mark_frame_free_by_index(uint32_t index) {
    if(index < frame_count) bitmap_clear(index);
}

static inline uint32_t paddr_from_index(uint32_t index) {
    return region_base + (index * 0x1000u);
}

static inline uint32_t index_from_paddr(uint32_t paddr) {
    return (paddr - region_base) / 0x1000u;
}

uint32_t alloc_frame() {
    for(uint32_t i = 0; i < frame_count; ++i) {
        if(!bitmap_test(i)) {
            bitmap_set(i);
            return paddr_from_index(i);
        }
    }
    return 0;
}

void free_frame(uint32_t paddr) {
    if(paddr < region_base || paddr >= region_end) return;
    uint32_t index = index_from_paddr(paddr);
    bitmap_clear(index);
}

uint32_t alloc_frames(size_t count) {
    if(count == 0) return 0;
    if(count == 1) return alloc_frame();
    for(uint32_t i = 0; i <= frame_count - count; ++i) {
        bool found = true;
        for(uint32_t j = 0; j < count; ++j) {
            if(bitmap_test(i + j)) {
                found = false;
                break;
            }
        }
        if(found) {
            for(uint32_t j = 0; j < count; ++j) bitmap_set(i + j);
            return paddr_from_index(i);
        }
    }
    return 0;
}

void free_frames(uint32_t paddr, size_t count) {
    if(count == 0) return;
    if(paddr < region_base || paddr >= region_end) return;
    uint32_t index = index_from_paddr(paddr);
    for(size_t i = 0; i < count && i < frame_count - index; ++i) bitmap_clear(index + i);
}

uint32_t pmm_total_frames() {
    return frame_count;
}

uint32_t pmm_free_frames() {
    uint32_t free_count = 0;
    for(uint32_t i = 0; i < frame_count; ++i) {
        if(!bitmap_test(i)) ++free_count;
    }
    return free_count;
}
