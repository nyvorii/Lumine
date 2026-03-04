#include "timer.h"
#include "io.h"
#include <stdint.h>

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;

    // Komenda 0x36: Kanał 0, LOBYTE/HIBYTE, Square Wave Mode
    outb(0x43, 0x36);

    // Wysyłamy dzielnik
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

extern volatile uint64_t timer_ticks;

void sleep (uint64_t milliseconds) {
    uint64_t start_ticks = timer_ticks;
    while (timer_ticks < start_ticks + milliseconds){
        asm volatile("sti");
        asm volatile("hlt");
    }
}