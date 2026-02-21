#include "io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

void remap_pic() {
    unsigned char a1, a2;

    // Zapisujemy obecne maski przerwań
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // ICW1: Rozpoczynamy inicjalizację obu układów PIC
    outb(PIC1_COMMAND, 0x11);
    io_wait();
    outb(PIC2_COMMAND, 0x11);
    io_wait();

    // ICW2: Przesunięcie wektorów (Kluczowy moment!)
    // PIC1 zaczyna się od 32 (0x20) - omijamy błędy procesora
    outb(PIC1_DATA, 0x20);
    io_wait();
    // PIC2 zaczyna się od 40 (0x28)
    outb(PIC2_DATA, 0x28);
    io_wait();

    // ICW3: Kaskadowanie (mówimy PIC1, że PIC2 jest do niego podpięty)
    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    // ICW4: Tryb pracy (8086/88)
    outb(PIC1_DATA, 0x01);
    io_wait();
    outb(PIC2_DATA, 0x01);
    io_wait();

    // Przywracamy maski
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}