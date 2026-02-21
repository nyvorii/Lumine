#ifndef IDT_H
#define IDT_H

// Struktura pojedynczego wpisu w tabeli (bramki)
struct idt_gate_t {
    unsigned short low_offset;  // Dolne 16 bitów adresu funkcji
    unsigned short sel;         // Selektor segmentu (0x08 to kod kernela)
    unsigned char always0;      // Zawsze 0
    unsigned char flags;        // Flagi (kto może to wywołać, czy wpis jest obecny)
    unsigned short high_offset; // Górne 16 bitów adresu funkcji
} __attribute__((packed));

// Struktura wskaźnika na całą tabelę (potrzebna dla komendy ładującej 'lidt')
struct idt_register_t {
    unsigned short limit; // Rozmiar tabeli minus 1
    unsigned int base;    // Adres pamięci, gdzie zaczyna się nasza tabela
} __attribute__((packed));

// Deklaracje funkcji
void set_idt_gate(int n, unsigned int handler);
void set_idt();

#endif