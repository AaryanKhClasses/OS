#pragma once
#include <stdint.h>
#include <stddef.h>

void* kmalloc_pages(size_t size);
void kfree_pages(void* ptr, size_t size);
void* kmalloc(size_t size);
void heap_init(uint32_t heap_base, size_t heap_size);
