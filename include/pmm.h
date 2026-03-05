#pragma once
#include <stdint.h>

void pmm_init(uint64_t bitmap_addr, uint64_t total_memory_bytes);
void pmm_init_region(uint64_t base, uint64_t size);
void pmm_deinit_region(uint64_t base, uint64_t size);
void* pmm_alloc_frame();
