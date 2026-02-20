extern "C" void kernel_main(void) {
    // Adres pamięci karty graficznej w trybie tekstowym
    unsigned short* video_memory = (unsigned short*)0xb8000;
    
    // Twój tekst
    const char* str = "Witaj w moim wlasnym systemie w C++!";
    
    // Rysowanie tekstu na ekranie
    for (int i = 0; str[i] != '\0'; ++i) {
        video_memory[i] = (unsigned short)str[i] | (0x0F << 8); 
    }
}