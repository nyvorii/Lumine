
#define IDT_H
#include <stdint.h>

// 64-bitowa bramka IDT (16 bajtów)
struct idt_gate_t {
    uint16_t offset_low;   // <--- Kompilator tego szuka!
    uint16_t selector;     
    uint8_t  ist;          
    uint8_t  flags;        
    uint16_t offset_mid;   
    uint32_t offset_high;  
    uint32_t reserved;     
} __attribute__((packed));

struct idt_register_t {
    uint16_t limit;
    uint64_t base; 
} __attribute__((packed));

void set_idt_gate(int n, uint64_t handler);
void set_idt();