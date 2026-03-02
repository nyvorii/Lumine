#include "idt.h"
#include <cstdint>

idt_gate_t idt[256];
idt_register_t idt_reg;

// Pobieranie aktualnego selektora segmentu kodu (CS)
uint16_t get_cs() {
    uint16_t cs;
    asm volatile("mov %%cs, %0" : "=r" (cs));
    return cs;
}

// Zauważ zmianę typu handlera na uint64_t!
void set_idt_gate(int n, uint64_t handler) {
    // 1. Dolne 16 bitów adresu (bity 0-15)
    idt[n].offset_mid  = (uint16_t)(handler & 0xFFFF);
    
    // 2. Selektor segmentu kodu
    idt[n].selector    = get_cs(); 
    
    // 3. Opcje bramki (IST na 0, flagi 0x8E oznaczają 64-bitową bramkę przerwań)
    idt[n].ist         = 0;
    idt[n].flags       = 0x8E; 
    
    // 4. Środkowe 16 bitów adresu (bity 16-31)
    idt[n].offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    
    // 5. Górne 32 bity adresu (bity 32-63)
    idt[n].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    
    // 6. Zarezerwowane pole musi być wyzerowane
    idt[n].reserved    = 0;
}

void set_idt() {
    // Ładowanie adresu naszej tablicy do struktury rejestru
    idt_reg.base = (uint64_t)&idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    
    // Instrukcja ładująca IDT do procesora
    asm volatile("lidt %0" : : "m" (idt_reg));
}