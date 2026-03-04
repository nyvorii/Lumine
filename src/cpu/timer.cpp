#include "io.h"
#include <stdint.h>

volatile uint64_t timer_ticks = 0;

void timer_init(uint64_t frequency) {
    uint64_t divisor = 1193180 / frequency;

    outb(0x43, 0x36); 
    outb(0x40, divisor & 0xFF); 
    outb(0x40, (divisor >> 8) & 0xFF); 
}
