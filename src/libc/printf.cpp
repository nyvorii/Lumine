#include "printf.h"
#include "io.h"
#include <stdarg.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>

#define BUFFER_SIZE 256
#define EOF -1

char command_buffer[BUFFER_SIZE];
int buffer_index = 0;

volatile uint16_t* vga_buffer = (uint16_t*)0xB8000;
static size_t terminal_row = 0;
static size_t terminal_column = 0;

void scroll(){
    for (size_t y = 1; y < 25; y++){
        for (size_t x = 0; x < 80; x++){
            const size_t current_index = y * 80 + x;
            const size_t new_index = (y - 1) * 80 + x;
            vga_buffer[new_index] = vga_buffer[current_index];
        }
    }

    for (size_t x = 0; x < 80; x++){
        const size_t index = 24 * 80 + x;
        vga_buffer[index] = (uint16_t) ' ' | (uint16_t)0x0F00;
    }
}

void update_cursor(size_t x, size_t y) {
    uint16_t cursor_location = y * 80 + x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint16_t)(cursor_location & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint16_t)((cursor_location >> 8) & 0xFF));
}

void clear_screen() {
    for (size_t y = 0; y < 25; y++) {
        for (size_t x = 0; x < 80; x++) {
            const size_t index = y * 80 + x;
            vga_buffer[index] = (uint16_t) ' ' | (uint16_t)0x0F00;
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    update_cursor(terminal_column, terminal_row);
}

int putchar(int ic){
    char c = (char)ic;

    if (c == '\b'){
        if (terminal_column > 0){
            terminal_column--;
        } 
        else if (terminal_row > 0){
            terminal_row--;
            terminal_column = 79;
        }
        const size_t index = terminal_row * 80 + terminal_column;
        vga_buffer[index] = (uint16_t) ' ' | (uint16_t)0x0F00;

        update_cursor(terminal_column, terminal_row);
        return ic;
    }

    if (c == '\n'){
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= 25){
            scroll();
            terminal_row = 24;
        }
        update_cursor(terminal_column, terminal_row);
        return ic;
    }


const size_t index = terminal_row * 80 + terminal_column;

vga_buffer[index] = (uint16_t) c | (uint16_t)0x0F00;

terminal_column++;

if (terminal_column >= 80){
    terminal_column = 0;
    terminal_row++;
}

if (terminal_row >= 25){
    scroll();
    terminal_row = 0;
}

update_cursor(terminal_column, terminal_row);

return ic;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}
  
static int itoa(int value, char* buffer, int base) {
    char* ptr = buffer, *ptr1 = buffer, tmp_char;
    int tmp_value;

    if (base < 2 || base > 36) {
        *buffer = '\0';
        return 0;
    }

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);

    if (tmp_value < 0)
        *ptr++ = '-';

    int length = ptr - buffer;
    *ptr-- = '\0';

    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return length;
}

static bool print(const char* data, size_t lenght) {
    const unsigned char* bytes = (const unsigned char*)data;
    for (size_t i = 0; i < lenght; i++)
        if (putchar(bytes[i]) == EOF)
            return false;
    return true;
}

int printf(const char* format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%'){
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount){
                va_end(parameters);
                return -1;
            }
            if (!print(format,amount)){
                va_end(parameters);
                return -1;
            }
            format += amount;
            written += amount;
            continue;
        }

        const char* format_begun_at = format++;

        if (*format == 'c'){
            format++;
            char c = (char) va_arg(parameters,int);
            if (!maxrem) {
                return -1;
            }
            if(!print(&c, sizeof(c)))
                return -1;
            written++;
        } else if (*format == 's') {
            format++;
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str);
            
            if (maxrem < len){
                va_end(parameters);
                return -1;
            }
            if (!print(str, len)){
                va_end(parameters);
                return -1;
            }
        }
        else if (*format == 'd' || *format == 'i'){
            format++;
            int value = va_arg(parameters, int);
            char buffer[32];
            int num_len = itoa(value, buffer, 10);
            if (maxrem < num_len){
                va_end(parameters);
                return -1;
            }
            if (!print(buffer, num_len)){
                    va_end(parameters);
                    return -1;
            }
            written += num_len;
        }
        else if (*format == 'x') {
            format++;
            unsigned int num = va_arg(parameters, unsigned int);
            char buffer[32];
            int num_len = itoa(num, buffer, 16); 
            
            if (maxrem < (size_t)(num_len + 2)) { 
                va_end(parameters);
                return -1;
            }
            
            // 1. dodanie prefiksu "0x"
            if (!print("0x", 2)) {
                va_end(parameters);
                return -1;
            }
            written += 2;

           // zero padding do 8 znaków
            int zeros_to_add = 8 - num_len;
            if (zeros_to_add > 0) {
                for (int i = 0; i < zeros_to_add; i++) {
                    if (!print("0", 1)) {
                        va_end(parameters);
                        return -1;
                    }
                    written++;
                }
            }
            
            // poprawne drukowanie liczby szesnastkowej
            if (!print(buffer, num_len)) {
                va_end(parameters);
                return -1;
            }
            written += num_len;
            
        }
        else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                va_end(parameters);
                return -1;
            }
            if (!print(format, len)){
                va_end(parameters);
                return -1;
            }
            written += len;
            format += len;
        }
    }
    va_end(parameters);
    return written;
}