[bits 32]

section .text

global keyboard_isr 
global dummy_isr     
extern keyboard_handler 

keyboard_isr:
    pushad
    cld                     
    call keyboard_handler
    popad
    iretd

dummy_isr:
    iretd            