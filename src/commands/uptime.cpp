#include "timer.h"
#include "printf.h"
#include <stdint.h>

extern volatile uint64_t timer_ticks;

void uptime() {
    uint64_t seconds = timer_ticks / 1000;
    uint64_t minutes = seconds / 60;
    uint64_t hours = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    printf("\nUptime: hours: %d, minutes: %d, seconds: %d\n", hours, minutes, seconds);
}