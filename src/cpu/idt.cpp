#include "idt.h"

idt_gate_t idt[256];
idt_register_t idt_reg;

unsigned short get_cs() {
    unsigned short cs;
    asm volatile("mov %%cs, %0" : "=r" (cs));
    return cs;
}

void set_idt_gate(int n, unsigned int handler) {
    idt[n].low_offset = (unsigned short)(handler & 0xFFFF);
    
    idt[n].sel = get_cs(); 
    
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; 
    idt[n].high_offset = (unsigned short)((handler >> 16) & 0xFFFF);
}

void set_idt() {
    idt_reg.base = (unsigned int)&idt;
    idt_reg.limit = 256 * sizeof(idt_gate_t) - 1;
    asm volatile("lidt %0" : : "m" (idt_reg));
}