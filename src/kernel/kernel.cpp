#include "io.h"
#include "idt.h"
#include "pic.h"
#include <cstdint>

unsigned short* video_memory = (unsigned short*)0xb8000;
int cursor_pos = 0;
bool caps_lock = false;

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,   
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};


void update_cursor(int pos) {
    outb(0x3D4, 14); 
    outb(0x3D5, (pos >> 8) & 0xFF); 
    outb(0x3D4, 15);
    outb(0x3D5, pos & 0xFF);
}


extern "C" void keyboard_isr();

extern "C" void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    if (!(scancode & 0x80)) {
        
        if (scancode == 0x3A) {
            caps_lock = !caps_lock; 
        } 
        else {
            char c = scancode_to_ascii[scancode];
            
            if (c != 0) {
               
                if (caps_lock && c >= 'a' && c <= 'z') {
                    c -= 32; 
                }

                if (c == '\n') {
                    cursor_pos = (cursor_pos / 80 + 1) * 80;
                } 
                else if (c == '\b') {
                    if (cursor_pos > 0) {
                        cursor_pos--;
                        video_memory[cursor_pos] = (unsigned short)' ' | (0x0F << 8);
                    }
                }
                else {
                    video_memory[cursor_pos] = (unsigned short)c | (0x0F << 8);
                    cursor_pos++;
                }
                
                update_cursor(cursor_pos);
            }
        }
    }

    outb(0x20, 0x20);
}

extern "C" void dummy_isr(); 

extern "C" void kernel_main(void) {
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = (unsigned short)' ' | (0x0F << 8);
    }
    
    remap_pic(); 

    outb(0x21, 0xFD); 
    outb(0xA1, 0xFF);

    for (int i = 0; i < 256; i++) {
        set_idt_gate(i, (uint64_t)&dummy_isr);
    }
    
    set_idt_gate(33, (uint64_t)&keyboard_isr);
    
    set_idt();
    
    outb(0x21, 0xFD); 
    outb(0xA1, 0xFF);
    
    asm volatile("sti");

    const char* str = "Witaj w Lumine Systemie 64bitowy";
    for (int i = 0; str[i] != '\0'; ++i) {
        video_memory[cursor_pos] = (unsigned short)str[i] | (0x0F << 8);
        cursor_pos++;
    }

    while (1) {
        asm volatile("hlt"); 
    }
}
