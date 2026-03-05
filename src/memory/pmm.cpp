
#include <stdint.h>
#include <stddef.h>
#include "logger.h"
#define PAGE_SIZE 4096

uint32_t* bitmap;
size_t total_blocks;
uint64_t total_used_blocks;

void pmm_set_bit(size_t index) {
    bitmap[index / 32] |= (1 << (index % 32));
}

void pmm_clear_bit(size_t index) {
    bitmap[index / 32] &= ~(1 << (index % 32));
}
bool pmm_test_bit(size_t index) {
    return (bitmap[index / 32] & (1 << (index % 32))) != 0;
}   

void pmm_init_region(uint64_t base , uint64_t size){
    uint64_t align = base/ PAGE_SIZE;
    uint64_t blocks = size / PAGE_SIZE;
    for (; blocks > 0; blocks--){
        pmm_clear_bit(align++);
        total_used_blocks--;
    }
}

void pmm_deinit_region(uint64_t base, uint64_t size) {
    uint64_t align = base/ PAGE_SIZE;
    uint64_t blocks = size / PAGE_SIZE;
    for (; blocks > 0; blocks--){
        pmm_set_bit(align++);
        total_used_blocks++;
    }
}

void pmm_init (uint64_t bitmap_addr, uint64_t total_memory_bytes){
    total_blocks = total_memory_bytes / PAGE_SIZE;
    bitmap = (uint32_t*)bitmap_addr;
    total_used_blocks = total_blocks;

    for (uint64_t i = 0; i < total_blocks / 32; i++){
        bitmap[i] = 0xFFFFFFFF;
    }

    klog(INFO, "PMM structure initialized");
}

void* pmm_alloc_frame() {
    for (uint64_t i = 0; i < total_blocks / 32; i++){
        if (bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++){
                uint64_t frame = i * 32 + j;
                if (!pmm_test_bit(frame)){
                    pmm_set_bit(frame);
                    total_used_blocks++;
                    return (void*)(frame * PAGE_SIZE);
                }
            }
        }
    }
    klog(ERROR, "PMM: OUT OF MEMORY.");
    return nullptr;
}