#ifndef IDT_H
#define IDT_H
#include <stdint.h>


// Struktura pojedynczego wpisu w tabeli (bramki)
struct idt_gate_t {
    uint16_t low_offset; // Dolna część adresu handlera
    uint16_t selector;        // Segment selektor
    uint8_t ist;
    uint8_t flags;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

// Struktura wskaźnika na całą tabelę (potrzebna dla komendy ładującej 'lidt')
struct idt_register_t {
    uint16_t limit; // Rozmiar tabeli minus 1
    uint64_t base;    // Adres pamięci, gdzie zaczyna się nasza tabela
} __attribute__((packed));

// Deklaracje funkcji
void set_idt_gate(int n, uint64_t handler);
void set_idt();

#endif