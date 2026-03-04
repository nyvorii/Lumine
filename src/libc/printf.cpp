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
                return -1;
            }
            if (!print(format,amount))
                return -1;
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
                return -1;
            }
            if (!print(str, len))
                return -1;
        } else {
            format = format_begun_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                return -1;
            }
            if (!print(format, len))
                return -1;
            written += len;
            format += len;
        }
    }
    va_end(parameters);
    return written;
}