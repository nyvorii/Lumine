#pragma once
#include <stdint.h>

// Odczytuje 8 bitów (bajt) z podanego portu sprzętowego
static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    // "%w1" to port (dx), "%b0" to wynik (al)
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Wysyła 8 bitów (bajt) do podanego portu (przyda się np. do PIC EOI)
static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

static inline void io_wait() {
    __asm__ volatile("outb %%al, $0x80" : : "a"(0));
}