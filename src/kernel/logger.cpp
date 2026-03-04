#include "timer.h"
#include "logger.h"
#include "printf.h"
#include <stdint.h>

extern volatile uint64_t timer_ticks;

void klog(LogLevel level, const char* message) {

    uint64_t sec = timer_ticks / 1000;
    uint64_t ms = timer_ticks % 1000;
    printf("[%d.%d] ", (int)sec, (int)ms);

    const char* level_str;
    switch (level) {
        case INFO:
            set_terminal_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
            level_str = "INFO"; 
            break;
        case WARNING: 
            set_terminal_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
            level_str = "WARNING"; 
            break;
        case ERROR:
            set_terminal_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            level_str = "ERROR"; 
            break;
        case DEBUG: 
            set_terminal_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            level_str = "DEBUG"; 
            break;
    }
   printf("%s %s\n", level_str, message);
   set_terminal_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
}
