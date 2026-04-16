#include "idt.h"
#include <cstdint>

idt_gate_t idt[256];
idt_register_t idt_reg;


void set_idt_gate(int n, uint64_t handler) {
    idt[n].offset_low  = (uint16_t)(handler & 0xFFFF);
    
    idt[n].selector    = 0x08; 
    
    idt[n].ist         = 0;
    idt[n].flags       = 0x8E; 
    idt[n].offset_mid  = (uint16_t)((handler >> 16) & 0xFFFF);
    idt[n].offset_high = (uint32_t)((handler >> 32) & 0xFFFFFFFF);
    idt[n].reserved    = 0;
}

void set_idt() {
    // Ładowanie adresu naszej tablicy do struktury rejestru
    idt_reg.base = (uint64_t)&idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    
    // Instrukcja ładująca IDT do procesora
    asm volatile("lidt %0" : : "m" (idt_reg));
}
