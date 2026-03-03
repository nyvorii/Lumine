#include "print.h"
#include <stdarg.h>

extern unsigned short* video_memory;
extern int cursor_pos;
void update_cursor(int pos);

void scroll()
{
    if (cursor_pos >= 80 * 25) {
        for(int i = 0; i < 80 * 24; i++){
            video_memory[i] = video_memory[i+80];
        }
        for(int i = 80 * 24 ; i < 80 * 25; i++){
            video_memory[i] = (unsigned short)' ' | (0x0F << 8);
        }

        cursor_pos = 80 * 24;
    }
}

void print_char(char c)
{
    if (c == '\n') {
        cursor_pos = (cursor_pos / 80 + 1s)
    }
}