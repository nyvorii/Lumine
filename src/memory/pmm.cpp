
/*#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096

uint32_t* bitmap;
size_t bitmap_size;
uint64_t total_memory;

void pmm_set_bit(size_t index) {
    bitmap[index / 32] |= (1 << (index % 32));
}

void pmm_clear_bit(size_t index) {
    bitmap[index / 32] &= ~(1 << (index % 32));
}
bool pmm_test_bit(size_t index) {
    return (bitmap[index / 32] & (1 << (index % 32))) != 0;
}   
*/
