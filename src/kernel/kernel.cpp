#include "io.h"
#include "idt.h"
#include "pic.h"
#include <cstdint>
#include "printf.h"
#include "shell.h"
#include "timer.h"
#include "logger.h"

extern volatile uint64_t timer_ticks;
extern "C" void timer_isr();


static bool left_shift_pressed = false;
static bool right_shift_pressed = false;

unsigned short* video_memory = (unsigned short*)0xb8000;
int cursor_pos = 0;
bool caps_lock = false;

#define SHELL_BUFFER_SIZE 256
char shell_buffer[SHELL_BUFFER_SIZE];
int shell_buffer_index = 0;

const char scancode_to_ascii[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',   
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,   
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

const char scancode_to_ascii_shifted[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~',  0,
  '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',  0,
   ' ',   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,
    0,    0,   0, '-',   0,   0,   0, '+',   0,   0,   0,   0,   0,  0,
};



extern "C" void keyboard_isr();

extern "C" void keyboard_handler() {
    unsigned char scancode = inb(0x60);

    if (scancode == 0x2A) left_shift_pressed = true;
    else if (scancode == 0xAA) left_shift_pressed = false;
    else if (scancode == 0x36) right_shift_pressed = true;
    else if (scancode == 0xB6) right_shift_pressed = false;

    else if (!(scancode & 0x80)) {
        bool is_shifted = left_shift_pressed || right_shift_pressed;
        char c = is_shifted ? scancode_to_ascii_shifted[scancode] : scancode_to_ascii[scancode];

        if (c == '\n') {
            printf("\n");

            shell_buffer[shell_buffer_index] = '\0'; 
            execute_command(shell_buffer);
            shell_buffer_index = 0; 
            printf("Lumine> ");
        }
        else if (c == '\b') {
            if (shell_buffer_index > 0) {
                shell_buffer_index--;
                printf("\b \b");
            }
        }
        else if (c != 0) {
            if (shell_buffer_index < SHELL_BUFFER_SIZE - 1) {
                shell_buffer[shell_buffer_index++] = c;
                printf("%c", c);
            }
        }
    }

    outb(0x20, 0x20);
}

extern "C" void timer_handler(){
    timer_ticks++;
    outb(0x20, 0x20);
}

extern "C" void dummy_isr(); 

extern "C" void kernel_main(void) {
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i] = (unsigned short)' ' | (0x0F << 8);
    }
    
    remap_pic(); 

    outb(0x21, 0xFC); 
    outb(0xA1, 0xFF);

    for (int i = 0; i < 256; i++) {
        set_idt_gate(i, (uint64_t)&dummy_isr);
    }
    
    set_idt_gate(33, (uint64_t)&keyboard_isr);
    set_idt_gate(32, (uint64_t)&timer_isr);
    set_idt();
    
    timer_init(1000);

    outb(0x21, 0xFC); 
    outb(0xA1, 0xFF);
    
    asm volatile("sti");

    clear_screen();
    printf("Welcome to Lumine OS!\n");
    printf("Type 'help' for a list of commands.\n");
    printf("Wskaznik VGA znajduje sie pod adresem: %x\n", 0xB8000); 
    printf("Wskaznik VGA znajduje sie pod adresem: %x\n", video_memory);
    printf("Wskaznik VGA znajduje sie pod adresem: %x\n", &video_memory);
    klog(INFO, "Lumine OS is starting...");
    klog(DEBUG, "Testing timer interrupts...");
    klog(WARNING, "This is a warning message.");
    klog(ERROR, "This is an error message.");
    printf("Lumine> ");

    while (1) {
        asm volatile("hlt"); 
    }
}
