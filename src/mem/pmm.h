#pragma once
#include <stdint.h>
#include <stddef.h>

// Initialize the physical memory manager in [region_base, region_end)
void pmm_init(uint32_t region_base, uint32_t region_end);
uint32_t alloc_frame(); // Allocate a single 4KiB frame, return 0 if failed
void free_frame(uint32_t paddr); // Free a single 4KiB frame
uint32_t alloc_frames(size_t count); // Allocate 'count' contiguous 4KiB frames, return 0 if failed
void free_frames(uint32_t paddr, size_t count); // Free 'count' contiguous 4KiB frames

// Utils:
uint32_t pmm_total_frames();
uint32_t pmm_free_frames();
